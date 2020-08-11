/*************************************************************************
    > File Name: logging.h
    > Author: Kevin
    > Created Time: 2020-08-10
    > Description:
 ************************************************************************/

#ifndef LOGGING_H
#define LOGGING_H
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "logging_config.h"
#include "logging_color_def.h"

/* UTILS */
#ifndef   MIN
  #define MIN(a, b)         (((a) < (b)) ? (a) : (b))
#endif

#ifndef   MAX
  #define MAX(a, b)         (((a) > (b)) ? (a) : (b))
#endif

#if (FATAL_ABORT == 0)
#define ABORT()
#else
#define ABORT() abort()
#endif

#if (LOGGING_CONFIG == LIGHT_WEIGHT)
/* Light weight mode start */

#if (TIME_ON != 0)
#include "sl_sleeptimer.h"
#define TIME_INIT() sl_sleeptimer_init()
#define TIME_GET()  sl_sleeptimer_get_time()
#else
#define TIME_INIT()
#define TIME_GET()  0
#endif

#if (LOGGING_INTERFACE == SEGGER_RTT)
#define __LOG(...)                SEGGER_RTT_printf(0, __VA_ARGS__)

#define INIT_LOG(x)                                                    \
  do{                                                                  \
    SEGGER_RTT_Init();                                                 \
    TIME_INIT();                                                       \
    LOGI("Project Boots, Compiled at %s - %s.\n", __DATE__, __TIME__); \
  }while (0)

#elif (LOGGING_INTERFACE == VCOM)
#define __LOG(...)                printf(__VA_ARGS__)

#define INIT_LOG(x)                                                    \
  do{                                                                  \
    TIME_INIT();                                                       \
    LOGI("Project Boots, Compiled at %s - %s.\n", __DATE__, __TIME__); \
  }while (0)
#elif (LOGGING_INTERFACE == INTERFACE_BOTH)
#define __LOG(...)                       \
  do {                                 \
    printf(__VA_ARGS__);               \
    SEGGER_RTT_printf(0, __VA_ARGS__); \
  } while (0)

#define INIT_LOG(x)                                                    \
  do{                                                                  \
    SEGGER_RTT_Init();                                                 \
    TIME_INIT();                                                       \
    LOGI("Project Boots, Compiled at %s - %s.\n", __DATE__, __TIME__); \
  }while (0)
#else
#define __LOG(...)
#define INIT_LOG(x)
#endif

static inline void __fill_file_line(char *in,
                                    uint8_t in_len,
                                    const char *file_name,
                                    unsigned int line)
{
  char *p, *n, *posend;
  char tmp[FILE_NAME_LENGTH] = { 0 };

  if (!file_name || !in) {
    return;
  }

  p = in;

  n = strrchr(file_name, '/');
  if (!n) {
    n = strrchr(file_name, '\\');
  }
  n = (n ? n + 1 : (char *)file_name);
  posend = strchr(n, '.');

  memcpy(tmp,
         n,
         MIN(FILE_NAME_LENGTH, posend - n));

  snprintf(p,
           in_len,
           "[%10.10s:%-5d]",
           tmp,
           line);
  return;
}

#define HEX_DUMP(array_base, array_size, align, reverse)                                                                 \
  do {                                                                                                                   \
    for (int i_log_exlusive = 0; i_log_exlusive < (array_size); i_log_exlusive++) {                                      \
      __LOG(((i_log_exlusive + 1) % (align)) ? "%02X " : "%02X\n",                                                         \
          (reverse) ? ((char*)(array_base))[array_size - i_log_exlusive - 1] : ((char*)(array_base))[i_log_exlusive]); } \
    __LOG("\n");                                                                                                           \
  } while (0)

#define HEX_DUMP_8(array_base, len)  HEX_DUMP((array_base), (len), 8, 0)
#define HEX_DUMP_16(array_base, len)  HEX_DUMP((array_base), (len), 16, 0)
#define HEX_DUMP_32(array_base, len)  HEX_DUMP((array_base), (len), 32, 0)

#define __LOG_FILL_HEADER(flag)                                          \
  char exclusive_buf__[PREF_LEN] = { 0 };                                \
  char exclusive_buf__1[FILE_LINE_LEN] = { 0 };                          \
  uint32_t t = TIME_GET();                                               \
  __fill_file_line(exclusive_buf__1, FILE_LINE_LEN, __FILE__, __LINE__); \
  sprintf(exclusive_buf__, FMT_PREF,                                     \
          t / (24 * 60 * 60),                                            \
          (t % (24 * 60 * 60)) / (60 * 60),                              \
          (t % (60 * 60)) / (60),                                        \
          t % 60,                                                        \
          exclusive_buf__1,                                              \
          (flag)                                                         \
          );                                                             \

#define LOGF(__fmt__, ...)                             \
  do {                                                 \
    __LOG_FILL_HEADER(FTL_FLAG);                       \
    __LOG("%s" __fmt__, exclusive_buf__, ##__VA_ARGS__); \
    ABORT();                                           \
  } while (0)

#define LOGE(__fmt__, ...)                               \
  do {                                                   \
    if (LOGGING_LEVEL >=  LOGGING_ERROR) {               \
      __LOG_FILL_HEADER(ERR_FLAG);                       \
      __LOG("%s" __fmt__, exclusive_buf__, ##__VA_ARGS__); \
    }                                                    \
  } while (0)

#define LOGW(__fmt__, ...)                               \
  do {                                                   \
    if (LOGGING_LEVEL >=  LOGGING_WARNING) {             \
      __LOG_FILL_HEADER(WRN_FLAG);                       \
      __LOG("%s" __fmt__, exclusive_buf__, ##__VA_ARGS__); \
    }                                                    \
  } while (0)

#define LOGI(__fmt__, ...)                               \
  do {                                                   \
    if (LOGGING_LEVEL >=  LOGGING_IMPORTANT_INFO) {      \
      __LOG_FILL_HEADER(IPM_FLAG);                       \
      __LOG("%s" __fmt__, exclusive_buf__, ##__VA_ARGS__); \
    }                                                    \
  } while (0)

#define LOGH(__fmt__, ...)                               \
  do {                                                   \
    if (LOGGING_LEVEL >=  LOGGING_DEBUG_HIGHTLIGHT) {    \
      __LOG_FILL_HEADER(DHL_FLAG);                       \
      __LOG("%s" __fmt__, exclusive_buf__, ##__VA_ARGS__); \
    }                                                    \
  } while (0)

#define LOGD(__fmt__, ...)                               \
  do {                                                   \
    if (LOGGING_LEVEL >=  LOGGING_DEBUG) {               \
      __LOG_FILL_HEADER(DBG_FLAG);                       \
      __LOG("%s" __fmt__, exclusive_buf__, ##__VA_ARGS__); \
    }                                                    \
  } while (0)

#define LOGV(__fmt__, ...)                               \
  do {                                                   \
    if (LOGGING_LEVEL >=  LOGGING_VERBOSE) {             \
      __LOG_FILL_HEADER(VER_FLAG);                       \
      __LOG("%s" __fmt__, exclusive_buf__, ##__VA_ARGS__); \
    }                                                    \
  } while (0)

/* Light weight mode end */
#else // #if (LOGGING_CONFIG == LIGHT_WEIGHT)
void logging_init(uint8_t level_threshold);
void log_n(void);
void logging_level_threshold_set(uint8_t l);
int __log(const char *file_name, unsigned int line,
          int lvl,
          const char *fmt, ...);
void hex_dump(const uint8_t *array_base,
              size_t len,
              uint8_t align,
              uint8_t reverse);

#define INIT_LOG(x) logging_init(x)

#define __LOG(lvl, fmt, ...) __log(__FILE__, __LINE__, (lvl), (fmt), ##__VA_ARGS__)
#define LOGN() log_n()

#define HEX_DUMP_8(array_base, len)  hex_dump((array_base), (len), 8, 0)
#define HEX_DUMP_16(array_base, len)  hex_dump((array_base), (len), 16, 0)
#define HEX_DUMP_32(array_base, len)  hex_dump((array_base), (len), 32, 0)

/*
 * Below 7 LOGx macros are used for logging data in specific level.
 */
#define LOGF(fmt, ...) \
  do { __LOG(LOGGING_FATAL, (fmt), ##__VA_ARGS__); ABORT(); } while (0)
#define LOGE(fmt, ...) __LOG(LOGGING_ERROR, (fmt), ##__VA_ARGS__)
#define LOGW(fmt, ...) __LOG(LOGGING_WARNING, (fmt), ##__VA_ARGS__)
#define LOGI(fmt, ...) __LOG(LOGGING_IMPORTANT_INFO, (fmt), ##__VA_ARGS__)
#define LOGH(fmt, ...) __LOG(LOGGING_DEBUG_HIGHTLIGHT, (fmt), ##__VA_ARGS__)
#define LOGD(fmt, ...) __LOG(LOGGING_DEBUG, (fmt), ##__VA_ARGS__)
#define LOGV(fmt, ...) __LOG(LOGGING_VERBOSE, (fmt), ##__VA_ARGS__)
#define LOG(lvl, fmt, ...) __LOG(lvl, (fmt), ##__VA_ARGS__)

#define LOGBGE(what, err) LOGE(what " returns Error[0x%04x]\n", (err))

#endif // #if (LOGGING_CONFIG > LIGHT_WEIGHT)

/*
 * For demostration or testing
 */
void logging_demo(void);
void test_hex_dump(void);
#ifdef __cplusplus
}
#endif
#endif //LOGGING_H
