/*************************************************************************
    > File Name: logging.c
    > Author: Kevin
    > Created Time: 2020-08-10
    > Description:
 ************************************************************************/

/* Includes *********************************************************** */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "logging.h"

/* #define LOGGING_DBG */
#ifdef LOGGING_DBG
#define LD(...) printf(__VA_ARGS__)
#else
#define LD(...)
#endif

typedef struct {
  uint8_t time_set;
  int min_level;
  size_t offset;
  char buf[LOGGING_BUF_LENGTH];
}lcfg_t;

#if (LOGGING_CONFIG > LIGHT_WEIGHT)

/* Defines  *********************************************************** */

/* Global Variables *************************************************** */

/* Static Variables *************************************************** */
static lcfg_t lcfg = { 0 };

/* Static Functions Declaractions ************************************* */

#if (TIME_ON != 0)
#include "sl_sleeptimer.h"
/* [2020-12-11 12:11:05] */
#define TIME_SLOT_LEN 21

static int _fill_time(void)
{
  if (lcfg.time_set) {
    sl_sleeptimer_date_t dt = { 0 };
    sl_status_t sl_ret = sl_sleeptimer_get_datetime(&dt);
    if (sl_ret != SL_STATUS_OK) {
      return -1;
    }

    int ret = snprintf(lcfg.buf + lcfg.offset,
                       LOGGING_BUF_LENGTH - lcfg.offset,
                       "[%04u-%02u-%02u %02u:%02u:%02u]",
                       dt.year + 1900,
                       dt.month + 1,
                       dt.month_day,
                       dt.hour,
                       dt.min,
                       dt.sec);
    if (ret != -1) {
      lcfg.offset += ret;
      return 0;
    }
    return -1;
  }
  sl_sleeptimer_timestamp_t t = sl_sleeptimer_get_time();

  int ret = snprintf(lcfg.buf + lcfg.offset,
                     LOGGING_BUF_LENGTH - lcfg.offset,
                     "[RT-%lu:%02lu:%02lu:%02lu]",
                     t / (24 * 60 * 60),
                     (t % (24 * 60 * 60)) / (60 * 60),
                     (t % (60 * 60)) / (60),
                     t % 60);
  if (ret != -1) {
    lcfg.offset += ret;
    return 0;
  }
  return -1;
}

#else // #if (TIME_ON != 0)

#endif // #if (TIME_ON != 0)

#if (LOCATION_ON != 0)
static int _fill_file_line(const char *file_name,
                           unsigned int line)
{
  int ret;
  char *p, *n, *posend;
  char tmp[FILE_NAME_LENGTH] = { 0 };

  if (!file_name) {
    return 0;
  }

  p = lcfg.buf + lcfg.offset;

  n = strrchr(file_name, '/');
  if (!n) {
    n = strrchr(file_name, '\\');
  }
  n = (n ? n + 1 : (char *)file_name);
  posend = strchr(n, '.');

  memcpy(tmp,
         n,
         MIN(FILE_NAME_LENGTH, posend - n));

  ret = snprintf(p,
                 LOGGING_BUF_LENGTH - lcfg.offset - 1,
                 "[%10.10s:%-5d]",
                 tmp,
                 line);

  if (ret != -1) {
    lcfg.offset += ret;
    return 0;
  }
  return -1;
}

#endif

static int _fill_level(int lvl)
{
  const char *flag;
  char *p;
  size_t flaglen;

  switch (lvl) {
    case LOGGING_FATAL:
      flag = FTL_FLAG;
      flaglen = sizeof(FTL_FLAG);
      break;
    case LOGGING_ERROR:
      flag = ERR_FLAG;
      flaglen = sizeof(ERR_FLAG);
      break;
    case LOGGING_WARNING:
      flag = WRN_FLAG;
      flaglen = sizeof(WRN_FLAG);
      break;
    case LOGGING_IMPORTANT_INFO:
      flag = IPM_FLAG;
      flaglen = sizeof(IPM_FLAG);
      break;
    case LOGGING_DEBUG_HIGHTLIGHT:
      flag = DHL_FLAG;
      flaglen = sizeof(DHL_FLAG);
      break;
    case LOGGING_DEBUG:
      flag = DBG_FLAG;
      flaglen = sizeof(DBG_FLAG);
      break;
    default:
      flag = VER_FLAG;
      flaglen = sizeof(VER_FLAG);
      break;
  }
  LD("%d - %lu\n", lvl, flaglen);

  p = lcfg.buf + lcfg.offset;
  /* sizeof contains the '\0' */
  memcpy(p, flag, flaglen - 1);
  lcfg.offset += flaglen - 1;
  return 0;
}

static inline void __logging(const char *str)
{
#if (LOGGING_INTERFACE == SEGGER_RTT)
  SEGGER_RTT_printf(0, "%s", str);
#elif (LOGGING_INTERFACE == VCOM)
  printf("%s", str);
#elif (LOGGING_INTERFACE == INTERFACE_BOTH)
  SEGGER_RTT_printf(0, "%s", str);
  printf("%s", str);
#else
#endif
}

void logging_plain(const char *fmt, ...)
{
  va_list valist;

  lcfg.offset = 0;
  memset(lcfg.buf, 0, LOGGING_BUF_LENGTH);
  va_start(valist, fmt);
  vsnprintf(lcfg.buf + lcfg.offset,
            LOGGING_BUF_LENGTH - lcfg.offset,
            fmt,
            valist);

  __logging(lcfg.buf);
}

int __log(const char *file_name, unsigned int line,
          int lvl,
          const char *fmt, ...)
{
  va_list valist;

  if (lvl > (int)lcfg.min_level) {
    return 0;
  }

  lcfg.offset = 0;
  memset(lcfg.buf, 0, LOGGING_BUF_LENGTH);

#if (TIME_ON != 0)
  if (0 != _fill_time()) {
    return -1;
  }
#endif

#if (LOCATION_ON != 0)
  if (0 != _fill_file_line(file_name, line)) {
    return -1;
  }
#endif

  _fill_level(lvl);

  /* fill whatever other modules here */

  lcfg.buf[lcfg.offset++] = ':';
  lcfg.buf[lcfg.offset++] = ' ';

  va_start(valist, fmt);
  vsnprintf(lcfg.buf + lcfg.offset,
            LOGGING_BUF_LENGTH - lcfg.offset,
            fmt,
            valist);

  __logging(lcfg.buf);
  return 0;
}

void log_n(void)
{
  __logging("\n");
}

static void __logging_welcome(void)
{
  char buf[100] = { 0 };

  snprintf(buf, 100,
           RTT_CTRL_BG_BRIGHT_BLUE
           "Project Boots, Compiled at %s - %s"
           RTT_CTRL_RESET
           "\n"
           ,
           __DATE__,
           __TIME__);
  __logging(buf);
}

void logging_init(uint8_t level_threshold)
{
  memset(&lcfg, 0, sizeof(lcfg_t));
  lcfg.min_level = MIN(level_threshold, LOGGING_VERBOSE);

#if (TIME_ON != 0)
  if (SL_STATUS_OK != sl_sleeptimer_init()) {
    return;
  }
#endif
#if (LOGGING_INTERFACE == SEGGER_RTT)
  SEGGER_RTT_Init();
#elif (LOGGING_INTERFACE == VCOM)
#elif (LOGGING_INTERFACE == INTERFACE_BOTH)
  SEGGER_RTT_Init();
#else
#endif
  __logging_welcome();
}

void logging_level_threshold_set(uint8_t l)
{
  /* Fatal cannot be disabled */
  lcfg.min_level = MIN(l, LOGGING_VERBOSE);
}

void hex_dump(const uint8_t *array_base,
              size_t len,
              uint8_t align,
              uint8_t reverse)
{
  if (!align) {
    align = 16;
  }

  lcfg.offset = 0;
  memset(lcfg.buf, 0, LOGGING_BUF_LENGTH);
  for (size_t i = 0; i < len; i++) {
    int r = snprintf(lcfg.buf + lcfg.offset,
                     LOGGING_BUF_LENGTH - lcfg.offset,
                     (i + 1) % align ? "%02X " : "%02X\n",
                     reverse ? array_base[len - i - 1] : array_base[i]);
    if (r == -1) {
      return;
    }
    lcfg.offset += r;
  }
  __logging(lcfg.buf);
  log_n();
}
#endif // #if (LOGGING_CONFIG > LIGHT_WEIGHT)

void logging_demo(void)
{
  const char *msg[] = {
    "This is a  fatal message",
    "This is an error message",
    "This is a  warning message",
    "This is an important info message",
    "This is a  debug highlight message",
    "This is a  debug message",
    "This is a  verbose message",
  };
  LOGF("%s\n", msg[0]);
  LOGE("%s\n", msg[1]);
  LOGW("%s\n", msg[2]);
  LOGI("%s\n", msg[3]);
  LOGH("%s\n", msg[4]);
  LOGD("%s\n", msg[5]);
  LOGV("%s\n", msg[6]);
}

#define TEST_HEX_DUMP_BUF_LEN (32 * 4 + 11)
void test_hex_dump(void)
{
  uint8_t buf[TEST_HEX_DUMP_BUF_LEN];

  for (int i = 0; i < TEST_HEX_DUMP_BUF_LEN; i++) {
    buf[i] = i;
  }

  LOGH("Hex Dump with 8-byte alignment\n");
  HEX_DUMP_8(buf, TEST_HEX_DUMP_BUF_LEN);

  LOGI("Hex Dump with 16-byte alignment\n");
  HEX_DUMP_16(buf, TEST_HEX_DUMP_BUF_LEN);

  LOGW("Hex Dump with 32-byte alignment\n");
  HEX_DUMP_32(buf, TEST_HEX_DUMP_BUF_LEN);
}
