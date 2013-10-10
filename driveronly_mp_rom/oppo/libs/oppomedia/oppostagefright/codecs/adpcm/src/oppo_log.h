/******************************************************************************
** Copyright (C), 2011-2013, OPPO Mobile Comm Corp., Ltd
** VENDOR_EDIT 
** FileName: oppo_log.h
** Author: luodexiang
** Create Date: 2012-6-19
** Description: LOG header  file .
** Version: 1.0
** ------------------Revision History: ------------------------
** <author>       <time>    <version >    <desc>
** luodexiang 2012-6-19  Revision 2.1   create this moudle  
******************************************************************************/

#ifndef OPPO_UTIL_LOG_H
#define OPPO_UTIL_LOG_H

#include <stdarg.h>

/* av_log API */

#define AV_LOG_QUIET    -8

/**
 * Something went really wrong and we will crash now.
 */
#define AV_LOG_PANIC     0

/**
 * Something went wrong and recovery is not possible.
 * For example, no header was found for a format which depends
 * on headers or an illegal combination of parameters is used.
 */
#define AV_LOG_FATAL     8

/**
 * Something went wrong and cannot losslessly be recovered.
 * However, not all future data is affected.
 */
#define AV_LOG_ERROR    16

/**
 * Something somehow does not look correct. This may or may not
 * lead to problems. An example would be the use of '-vstrict -2'.
 */
#define AV_LOG_WARNING  24

#define AV_LOG_INFO     32
#define AV_LOG_VERBOSE  40

/**
 * Stuff which is only useful for libav* developers.
 */
#define AV_LOG_DEBUG    48

/**
 * Sends the specified message to the log if the level is less than or equal
 * to the current av_log_level. By default, all logging messages are sent to
 * stderr. This behavior can be altered by setting a different av_vlog callback
 * function.
 *
 * @param avcl A pointer to an arbitrary struct of which the first field is a
 * pointer to an AVClass struct.
 * @param level The importance level of the message, lower values signifying
 * higher importance.
 * @param fmt The format string (printf-compatible) that specifies how
 * subsequent arguments are converted to output.
 * @see av_vlog
 */

void av_log(void*, int level, const char *fmt, ...);

void av_vlog(void*, int level, const char *fmt, va_list);
int av_log_get_level(void);
void av_log_set_level(int);
void av_log_set_callback(void (*)(void*, int, const char*, va_list));
void av_log_default_callback(void* ptr, int level, const char* fmt, va_list vl);

#endif /* OPPO_UTIL_LOG_H */
