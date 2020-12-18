/*************************************************************************
 *  @file logging.h
 *  @author Kevin
 *  @date 2020-08-10
 *  @note
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

#include "em_core.h"

#if defined(WDOG_ENABLED) && (WDOG_ENABLED == 1)
#include "hal/wdog/wdog.h"
#define WDOG_STOP()                   wdog_set_state(0)
#else
#define WDOG_STOP()
#endif

/* UTILS */
#ifndef   MIN
  #define MIN(a, b)                   (((a) < (b)) ? (a) : (b))
#endif

#ifndef   MAX
  #define MAX(a, b)                   (((a) > (b)) ? (a) : (b))
#endif

#if (FATAL_ABORT == 0)
#define ABORT()
#else
#define ABORT()                       abort()
#endif

#if (ERROR_ABORT == 0)
#define ERR_ABORT()
#else
#define ERR_ABORT()                   abort()
#endif

#if (LOGGING_CONFIG == LIGHT_WEIGHT)
/* Light weight mode start */

#if (TIME_ON != 0)
#include "sl_sleeptimer.h"
#define TIME_INIT()                   sl_sleeptimer_init()
#define TIME_GET()                    sl_sleeptimer_get_time()
#else
#define TIME_INIT()
#define TIME_GET()                    0
#endif

#if (LOGGING_INTERFACE == SEGGER_RTT)
#define LOG_PLAIN(...)                SEGGER_RTT_printf(0, __VA_ARGS__)

#define INIT_LOG(x)                                                    \
  do{                                                                  \
    SEGGER_RTT_Init();                                                 \
    TIME_INIT();                                                       \
    LOGI("Project Boots, Compiled at %s - %s.\n", __DATE__, __TIME__); \
  }while (0)

#elif (LOGGING_INTERFACE == VCOM)
#define LOG_PLAIN(...)                printf(__VA_ARGS__)

#define INIT_LOG(x)                                                    \
  do{                                                                  \
    TIME_INIT();                                                       \
    LOGI("Project Boots, Compiled at %s - %s.\n", __DATE__, __TIME__); \
  }while (0)
#elif (LOGGING_INTERFACE == INTERFACE_BOTH)
#define LOG_PLAIN(...)                 \
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
#define LOG_PLAIN(...)
#define INIT_LOG(x)
#endif

static inline void __fill_file_line(char *in,
                                    uint8_t      in_len,
                                    const char   *file_name,
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
  n      = (n ? n + 1 : (char *)file_name);
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

#define HEX_DUMP(array_base, array_size, align, reverse)                                                                       \
  do {                                                                                                                         \
    for (int i_log_exlusive = 0; i_log_exlusive < (array_size); i_log_exlusive++) {                                            \
      LOG_PLAIN(((i_log_exlusive + 1) % (align)) ? "%02X " : "%02X\n",                                                         \
                (reverse) ? ((char*)(array_base))[array_size - i_log_exlusive - 1] : ((char*)(array_base))[i_log_exlusive]); } \
    LOG_PLAIN("\n");                                                                                                           \
  } while (0)

#define HEX_DUMP_8(array_base, len)   HEX_DUMP((array_base), (len), 8, 0)
#define HEX_DUMP_16(array_base, len)  HEX_DUMP((array_base), (len), 16, 0)
#define HEX_DUMP_32(array_base, len)  HEX_DUMP((array_base), (len), 32, 0)

#define LOG_FILL_HEADER(flag)                                            \
  char exclusive_buf__[PREF_LEN] = { 0 };                                \
  char     exclusive_buf__1[FILE_LINE_LEN] = { 0 };                      \
  uint32_t t                               = TIME_GET();                 \
  __fill_file_line(exclusive_buf__1, FILE_LINE_LEN, __FILE__, __LINE__); \
  sprintf(exclusive_buf__, FMT_PREF,                                     \
          t / (24 * 60 * 60),                                            \
          (t % (24 * 60 * 60)) / (60 * 60),                              \
          (t % (60 * 60)) / (60),                                        \
          t % 60,                                                        \
          exclusive_buf__1,                                              \
          (flag)                                                         \
          );                                                             \

#define LOG(lvl, fmt, ...)                                           \
  do {                                                               \
    if (LOGGING_LEVEL > (lvl)) {                                     \
      LOG_FILL_HEADER((lvl) == LOGGING_FATAL ? FTL_FLAG              \
                      : (lvl) == LOGGING_ERROR ? ERR_FLAG            \
                      : (lvl) == LOGGING_WARNING ? WRN_FLAG          \
                      : (lvl) == LOGGING_IMPORTANT_INFO ? IPM_FLAG   \
                      : (lvl) == LOGGING_DEBUG_HIGHTLIGHT ? DHL_FLAG \
                      : (lvl) == LOGGING_DEBUG ? DBG_FLAG            \
                      : VER_FLAG);                                   \
      LOG_PLAIN("%s" __fmt__, exclusive_buf__, ##__VA_ARGS__);       \
    }                                                                \
  }while(0)

#define LOGF(__fmt__, ...)                                   \
  do {                                                       \
    LOG_FILL_HEADER(FTL_FLAG);                               \
    LOG_PLAIN("%s" __fmt__, exclusive_buf__, ##__VA_ARGS__); \
    ABORT();                                                 \
  } while (0)

#define LOGE(__fmt__, ...)                                     \
  do {                                                         \
    if (LOGGING_LEVEL >=  LOGGING_ERROR) {                     \
      LOG_FILL_HEADER(ERR_FLAG);                               \
      LOG_PLAIN("%s" __fmt__, exclusive_buf__, ##__VA_ARGS__); \
      ERR_ABORT();
} \
  } while (0)

#define LOGW(__fmt__, ...)                                     \
  do {                                                         \
    if (LOGGING_LEVEL >=  LOGGING_WARNING) {                   \
      LOG_FILL_HEADER(WRN_FLAG);                               \
      LOG_PLAIN("%s" __fmt__, exclusive_buf__, ##__VA_ARGS__); \
    }                                                          \
  } while (0)

#define LOGI(__fmt__, ...)                                     \
  do {                                                         \
    if (LOGGING_LEVEL >=  LOGGING_IMPORTANT_INFO) {            \
      LOG_FILL_HEADER(IPM_FLAG);                               \
      LOG_PLAIN("%s" __fmt__, exclusive_buf__, ##__VA_ARGS__); \
    }                                                          \
  } while (0)

#define LOGH(__fmt__, ...)                                     \
  do {                                                         \
    if (LOGGING_LEVEL >=  LOGGING_DEBUG_HIGHTLIGHT) {          \
      LOG_FILL_HEADER(DHL_FLAG);                               \
      LOG_PLAIN("%s" __fmt__, exclusive_buf__, ##__VA_ARGS__); \
    }                                                          \
  } while (0)

#define LOGD(__fmt__, ...)                                     \
  do {                                                         \
    if (LOGGING_LEVEL >=  LOGGING_DEBUG) {                     \
      LOG_FILL_HEADER(DBG_FLAG);                               \
      LOG_PLAIN("%s" __fmt__, exclusive_buf__, ##__VA_ARGS__); \
    }                                                          \
  } while (0)

#define LOGV(__fmt__, ...)                                     \
  do {                                                         \
    if (LOGGING_LEVEL >=  LOGGING_VERBOSE) {                   \
      LOG_FILL_HEADER(VER_FLAG);                               \
      LOG_PLAIN("%s" __fmt__, exclusive_buf__, ##__VA_ARGS__); \
    }                                                          \
  } while (0)

/* Light weight mode end */
#else // #if (LOGGING_CONFIG == LIGHT_WEIGHT)
#if defined(TIME_ON) && (TIME_ON == 1)
#include "sl_sleeptimer.h"
#endif

/**
 * ******************************************************************
 * @defgroup logging_func
 * @brief logging functions for the full featured mode.
 *
 ******************************************************************
 * @{ */

/**
 * @brief logging_init initialization for logging functionality
 *
 * @param level_threshold - minimal logging level threshold
 */
void logging_init(uint8_t level_threshold);

/**
 * @brief log_n function to output a new line
 */
void log_n(void);

/**
 * @brief logging_level_threshold_set set logging level threshold at runtime
 *
 * @param l - new level
 */
void logging_level_threshold_set(uint8_t l);

/**
 * @brief __log function to wrap a logging message with all prefix tags and put
 * them altogether to the logging buffer.
 *
 * @param file_name - file name (location) information
 * @param line - line (location) information
 * @param lvl - logging message level information
 * @param fmt - format string
 * @param ... - parameters
 *
 * @return 0 on success, -1 otherwise
 */
int  __log(const char   *file_name,
           unsigned int line,
           int          lvl,
           const char   *fmt,
           ...);

/**
 * @brief hex_dump function to dump an array of content which is not printable
 * string.
 *
 * @param array_base - base pointer of the array
 * @param len - array length in bytes
 * @param align - line alignment deciding how many bytes will show in a single
 * line
 * @param reverse - 0 - Little indian, 1 otherwise
 */
void hex_dump(const uint8_t *array_base,
              size_t        len,
              uint8_t       align,
              uint8_t       reverse);

/**
 * @brief logging_plain function to output the logging message without the
 * prefixed tags
 *
 * @param fmt - format string
 * @param ... - parameters
 */
void logging_plain(const char *fmt,
                   ...);
int  logging_set_time(sl_sleeptimer_date_t *dt);

/**
 * @brief logging_demo function for fine-tuning the logging colors, flags etc.
 *
 * @param lvl - which level message to output, 0xff to output all in sequence
 */
void logging_demo(uint8_t lvl);

/**
 * @brief test_hex_dump function for testing the HEX_DUMP* macros
 */
void test_hex_dump(void);
/**  @} logging_func */

#define INIT_LOG(x)                   logging_init(x)

#define LOG(lvl, fmt, ...)            __log(__FILE__, __LINE__, (lvl), (fmt), ##__VA_ARGS__)
#define LOGN()                        log_n()

#define HEX_DUMP_8(array_base, len)   hex_dump((array_base), (len), 8, 0)
#define HEX_DUMP_16(array_base, len)  hex_dump((array_base), (len), 16, 0)
#define HEX_DUMP_32(array_base, len)  hex_dump((array_base), (len), 32, 0)

/*
 * Below 7 LOGx macros are used for logging data in specific level.
 */
#define LOGF(fmt, ...) \
  do { LOG(LOGGING_FATAL, (fmt), ##__VA_ARGS__); ABORT(); } while (0)
#define LOGE(fmt, ...) \
  do { LOG(LOGGING_ERROR, (fmt), ##__VA_ARGS__); ERR_ABORT(); } while(0)
#define LOGW(fmt, ...)                LOG(LOGGING_WARNING, (fmt), ##__VA_ARGS__)
#define LOGI(fmt, ...)                LOG(LOGGING_IMPORTANT_INFO, (fmt), ##__VA_ARGS__)
#define LOGH(fmt, ...)                LOG(LOGGING_DEBUG_HIGHTLIGHT, (fmt), ##__VA_ARGS__)
#define LOGD(fmt, ...)                LOG(LOGGING_DEBUG, (fmt), ##__VA_ARGS__)
#define LOGV(fmt, ...)                LOG(LOGGING_VERBOSE, (fmt), ##__VA_ARGS__)
#define LOG_PLAIN(fmt, ...)           logging_plain((fmt), ##__VA_ARGS__)

#define LOGBGE(what, err)             LOGE(what " returns Error[0x%04x]\n", (err))

#endif // #if (LOGGING_CONFIG > LIGHT_WEIGHT)

#define LOG_ASSERT(exp)                   \
  do{                                     \
    if (!(exp)) {                         \
      CORE_irqState_t irqState;           \
      CORE_CRITICAL_IRQ_DISABLE();        \
      WDOG_STOP();                        \
      (void)irqState;                     \
      LOGF("Assert in [%s]\n", __func__); \
    }                                     \
  }while (0)

#ifdef ASSERT
#undef ASSERT
#endif
#define ASSERT      LOG_ASSERT

#ifdef ASSERT_MSG
#undef ASSERT_MSG
#endif
#define ASSERT_MSG  LOG_ASSERT_MSG

#define LOG_ASSERT_MSG(exp, fmt, ...)                         \
  do{                                                         \
    if (!(exp)) {                                             \
      CORE_irqState_t irqState;                               \
      CORE_CRITICAL_IRQ_DISABLE();                            \
      WDOG_STOP();                                            \
      (void)irqState;                                         \
      LOGF("Assert in [%s] - " fmt, __func__, ##__VA_ARGS__); \
    }                                                         \
  }while(0)

/**
 * ******************************************************************
 * @defgroup se_call
 * @brief secure calls for BGAPI commands, basically the macros check the return
 * value of the BGAPI calls and assert the return value to be normal, used only
 * for Bluetooth application
 *
 ******************************************************************
 * @{ */

/**
 * @brief SE_CALL - Check BGAPI call return value, if not bg_err_success, assert,
 * dedicated for GSDK version 2.x
 *
 * @param x - BGAPI function
 *
 */
#define SE_CALL(x)                                                     \
  do {                                                                 \
    uint16_t se_call_exclusive_returned_value = (x)->result;           \
    LOG_ASSERT_MSG(se_call_exclusive_returned_value == bg_err_success, \
                   "BGAPI Call Failed, Error Code: 0x%04x\n",          \
                   se_call_exclusive_returned_value);                  \
  }while(0)

/**
 * @brief SE_CALL - Check BGAPI call return value, if not bg_err_success, assert,
 * dedicated for GSDK version 3.x
 *
 * @param x - BGAPI function
 *
 */
#define SE_CALL3(x)                                                  \
  do {                                                               \
    uint32_t se_call_exclusive_returned_value = (x);                 \
    LOG_ASSERT_MSG(se_call_exclusive_returned_value == SL_STATUS_OK, \
                   "BGAPI Call Failed, Error Code: 0x%04x\n",        \
                   se_call_exclusive_returned_value);                \
  }while(0)

/**  @} se_call */

#ifdef __cplusplus
}
#endif
#endif //LOGGING_H
