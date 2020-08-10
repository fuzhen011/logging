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

#include <stdint.h>
#include "logging_config.h"
#include "logging_color_def.h"

#if (LOGGING_CONFIG == LIGHT_WEIGHT)

#define INIT_LOG()

#if (LOGGING_INTERFACE == SEGGER_RTT)
#define LOG(...)                SEGGER_RTT_printf(0, __VA_ARGS__)
#elif (LOGGING_INTERFACE == VCOM)
#define LOG(...)                printf(__VA_ARGS__)
#elif (LOGGING_INTERFACE == INTERFACE_BOTH)
#define LOG(...)                       \
  do {                                 \
    printf(__VA_ARGS__);               \
    SEGGER_RTT_printf(0, __VA_ARGS__); \
  } while (0)
#endif

#else // #if (LOGGING_CONFIG == LIGHT_WEIGHT)
void logging_init(uint8_t level_threshold);
void logging_demo(void);
void log_n(void);

int __log(const char *file_name, unsigned int line,
          int lvl,
          const char *fmt, ...);

#define INIT_LOG(x) logging_init(x)

#define LOG(lvl, fmt, ...) __log(__FILE__, __LINE__, (lvl), (fmt), ##__VA_ARGS__)
#define LOGN() log_n()

/*
 * Below 7 LOGx macros are used for logging data in specific level.
 */
#define LOGF(fmt, ...) \
  do { LOG(LOGGING_FATAL, (fmt), ##__VA_ARGS__); abort(); } while (0)
#define LOGE(fmt, ...) LOG(LOGGING_ERROR, (fmt), ##__VA_ARGS__)
#define LOGW(fmt, ...) LOG(LOGGING_WARNING, (fmt), ##__VA_ARGS__)
#define LOGI(fmt, ...) LOG(LOGGING_IMPORTANT_INFO, (fmt), ##__VA_ARGS__)
#define LOGH(fmt, ...) LOG(LOGGING_DEBUG_HIGHTLIGHT, (fmt), ##__VA_ARGS__)
#define LOGD(fmt, ...) LOG(LOGGING_DEBUG, (fmt), ##__VA_ARGS__)
#define LOGV(fmt, ...) LOG(LOGGING_VERBOSE, (fmt), ##__VA_ARGS__)

#define LOGBGE(what, err) LOGE(what " returns Error[0x%04x]\n", (err))

#endif // #if (LOGGING_CONFIG > LIGHT_WEIGHT)

/* UTILS */
#ifndef   MIN
  #define MIN(a, b)         (((a) < (b)) ? (a) : (b))
#endif

#ifndef   MAX
  #define MAX(a, b)         (((a) > (b)) ? (a) : (b))
#endif

#ifdef __cplusplus
}
#endif
#endif //LOGGING_H
