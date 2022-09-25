#ifndef _WDM_LOG_H_
#define _WDM_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /** Black */
#define RED     "\033[31m"      /** Red */
#define GREEN   "\033[32m"      /** Green */
#define YELLOW  "\033[33m"      /** Yellow */
#define BLUE    "\033[34m"      /** Blue */
#define MAGENTA "\033[35m"      /** 品红色 */
#define CYAN    "\033[36m"      /** 蓝绿色 */
#define WHITE   "\033[37m"      /** White */
#define BOLD_BLACK   "\033[1m\033[30m"      /** Bold Black */
#define BOLD_RED     "\033[1m\033[31m"      /** Bold Red */
#define BOLD_GREEN   "\033[1m\033[32m"      /** Bold Green */
#define BOLD_YELLOW  "\033[1m\033[33m"      /** Bold Yellow */
#define BOLD_BLUE    "\033[1m\033[34m"      /** Bold Blue */
#define BOLD_MAGENTA "\033[1m\033[35m"      /** Bold Magenta */
#define BOLD_CYAN    "\033[1m\033[36m"      /** Bold Cyan */
#define BOLD_WHITE   "\033[1m\033[37m"      /** Bold White */

#define LOG_COLOR_TRACE GREEN
#define LOG_COLOR_DEBUG CYAN
#define LOG_COLOR_INFO WHITE
#define LOG_COLOR_WARN YELLOW
#define LOG_COLOR_ERROR RED
#define LOG_COLOR_FATAL BOLD_RED

#define LOG_LEVEL_TRACE     5
#define LOG_LEVEL_DEBUG     4
#define LOG_LEVEL_INFO      3
#define LOG_LEVEL_WARN      2
#define LOG_LEVEL_ERROR     1
#define LOG_LEVEL_FATAL     0

#ifndef WDM_SOURCE_ROOT_LENGTH
#define WDM_SOURCE_ROOT_LENGTH 0
#endif

#ifndef WDM_LOG_LEVEL
#if WDM_DEBUG
#define LOG_LEVEL LOG_LEVEL_TRACE
#else
#define LOG_LEVEL LOG_LEVEL_INFO
#endif
#else
#define LOG_LEVEL WDM_LOG_LEVEL
#endif

#if LOG_LEVEL >= LOG_LEVEL_TRACE
#define LOG_DATA(str, data, size, max, split)   \
    do{ \
        wdm_log_data(&__FILE__[WDM_SOURCE_ROOT_LENGTH], __FUNCTION__, __LINE__, str, data, size, max, split); \
    }while(0)
#else
#define LOG_DATA(str, data, size, max, split)
#endif

#if LOG_LEVEL >= LOG_LEVEL_TRACE
#define LOG_TRACE(fmt, ...)   \
    do{ \
        printf(LOG_COLOR_TRACE"[TRACE][%s][%s][%d]:"fmt"\n"RESET, &__FILE__[WDM_SOURCE_ROOT_LENGTH], __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    }while(0)
#else
#define LOG_TRACE(fmt, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_DEBUG
#define LOG_DEBUG(fmt, ...)   \
    do{ \
        printf(LOG_COLOR_DEBUG"[DEBUG][%s][%s][%d]:"fmt"\n"RESET, &__FILE__[WDM_SOURCE_ROOT_LENGTH], __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    }while(0)
#else
#define LOG_DEBUG(fmt, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_INFO
#define LOG_INFO(fmt, ...)   \
    do{ \
        printf(LOG_COLOR_INFO"[INFO ][%s][%s][%d]:"fmt"\n"RESET, &__FILE__[WDM_SOURCE_ROOT_LENGTH], __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    }while(0)
#else
#define LOG_INFO(fmt, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_WARN
#define LOG_WARN(fmt, ...)   \
    do{ \
        printf(LOG_COLOR_WARN"[WARN ][%s][%s][%d]:"fmt"\n"RESET, &__FILE__[WDM_SOURCE_ROOT_LENGTH], __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    }while(0)
#else
#define LOG_WARN(fmt, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_ERROR
#define LOG_ERROR(fmt, ...)   \
    do{ \
        printf(LOG_COLOR_ERROR"[ERROR][%s][%s][%d]:"fmt"\n"RESET, &__FILE__[WDM_SOURCE_ROOT_LENGTH], __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    }while(0)
#else
#define LOG_ERROR(fmt, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_FATAL
#define LOG_FATAL(fmt, ...)   \
    do{ \
        printf(LOG_COLOR_FATAL"[FATAL][%s][%s][%d]:"fmt"\n"RESET, &__FILE__[WDM_SOURCE_ROOT_LENGTH], __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    }while(0)
#else
#define LOG_FATAL(fmt, ...)
#endif

static inline void
wdm_log_data(const char *file, const char *function, int line, const char *str, const void *data, int size, int max,
             int split) {
    if (data == NULL || size <= 0) {
        return;
    }
    if (max > size || max <= 0) {
        max = size;
    }
    printf(LOG_COLOR_TRACE"[FATAL][%s][%s][%d]:%s", file, function, line, str);
    for (int i = 0; i < max; ++i) {
        if (i % split == 0) {
            printf("\n");
        }
        uint8_t c = ((uint8_t *) data)[i];
        printf("%02X ", c);
    }
    printf(RESET "\n");
}

#ifdef __cplusplus
}
#endif

#endif
