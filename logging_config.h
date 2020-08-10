/*************************************************************************
    > File Name: logging_config.h
    > Author: Kevin
    > Created Time: 2020-08-10
    > Description:
 ************************************************************************/

#ifndef LOGGING_CONFIG_H
#define LOGGING_CONFIG_H
#ifdef __cplusplus
extern "C"
{
#endif
#include "logging_color_def.h"

#ifndef LOGGING_BUF_LENGTH
#define LOGGING_BUF_LENGTH  1024
#endif

#define SEGGER_RTT  1
#define VCOM  2
#define INTERFACE_BOTH  (SEGGER_RTT | VCOM)

#ifndef LOGGING_INTERFACE
#define LOGGING_INTERFACE INTERFACE_BOTH
#endif

#define LIGHT_WEIGHT  0
#define FULL_FEATURES 1
#define CUSTOMIZED  2

#ifndef LOGGING_CONFIG
#define LOGGING_CONFIG FULL_FEATURES
#endif

#ifndef FILE_NAME_LENGTH
#define FILE_NAME_LENGTH  10
#endif
#ifndef LINE_NAME_LENGTH
#define LINE_NAME_LENGTH  5
#endif
#define FILE_LINE_LENGTH  (FILE_NAME_LENGTH + LINE_NAME_LENGTH + 3)

/*
 * Logging Levels
 */
enum {
  LOGGING_FATAL,
  LOGGING_ERROR,
  LOGGING_WARNING,
  LOGGING_IMPORTANT_INFO,
  LOGGING_DEBUG_HIGHTLIGHT,
  LOGGING_DEBUG,
  LOGGING_VERBOSE
};

/*
 * Configuration Items:
 *   TIME_ON - If to add time information to the logging piece
 *   LOCATION_ON - If to add location (file:line) information to the logging piece
 */

#if (LOGGING_CONFIG == FULL_FEATURES)

#define TIME_ON 1
#define LOCATION_ON 1

#elif (LOGGING_CONFIG == LIGHT_WEIGHT)

#define TIME_ON 1
#ifndef LOGGING_LEVEL
#define LOGGING_LEVEL LOGGING_VERBOSE
#endif

#elif (LOGGING_CONFIG == CUSTOMIZED)
#else
#error "LOGGING_CONFIG value invalid"
#endif

#if !defined(TIME_ON)
#error "TIME_ON NOT Defined"
#endif

#define FTL_FLAG  "[" RTT_CTRL_BG_BRIGHT_RED "FTL" RTT_CTRL_RESET "]"
#define ERR_FLAG  "[" RTT_CTRL_TEXT_BRIGHT_RED "ERR" RTT_CTRL_RESET "]"
#define WRN_FLAG  "[" RTT_CTRL_BG_BRIGHT_YELLOW "WRN" RTT_CTRL_RESET "]"
#define IPM_FLAG  "[" RTT_CTRL_BG_BRIGHT_MAGENTA "IPM" RTT_CTRL_RESET "]"
#define DHL_FLAG  "[" RTT_CTRL_BG_BRIGHT_CYAN "DHL" RTT_CTRL_RESET "]"
#define DBG_FLAG  "[" RTT_CTRL_TEXT_BRIGHT_GREEN "DBG" RTT_CTRL_RESET "]"
#define VER_FLAG  "[VER]"

#define PREF_LEN  (64)
#define FILE_LINE_LEN (20)
#define FMT_PREF  "[RT-%4lu:%02lu:%02lu:%02lu]%s%s: "

#ifdef __cplusplus
}
#endif
#endif //LOGGING_CONFIG_H
