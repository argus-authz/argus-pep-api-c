/*
 * Copyright 2008-2009 Members of the EGEE Collaboration.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * $Id$
 */
#ifndef _LOG_H_
#define _LOG_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include <stdio.h> /* FILE */
#include <stdarg.h>  /* va_list, va_arg, ... */


/** log function return codes */
#define LOG_OK 0
#define LOG_ERROR -1

/** Log levels */
typedef enum {
	LOG_LEVEL_NONE = -1,
	LOG_LEVEL_ERROR = 0,
	LOG_LEVEL_WARN = 1,
	LOG_LEVEL_INFO = 2,
	LOG_LEVEL_DEBUG = 3,
	LOG_LEVEL_TRACE = 4
} log_level_t;

/**
 * Optional log_handler function prototype
 *
 * @param level the level to log
 * @param fmt the format string
 * @param args the variable arguments list
 * @return int 0 on success or an error code
 */
typedef int log_handler_func(int level,const char * fmt, va_list args);

/**
 * Sets the optional log handler function.
 *
 * @param handler function pointer to the optional log handler
 * @return {@link #LOG_OK} or {@link #LOG_ERROR} on error
 *
 * @note If the optional log handler function is set, the log messages will
 * be send to the handler and not written to the logging output anymore.
 */
int log_sethandler(log_handler_func * handler);

/**
 * Sets the log level to level.
 * @return {@link #LOG_OK} or {@link #LOG_ERROR} on error
 */
int log_setlevel(log_level_t level);

/**
 * Returns the current log level
 * @return log_level_t the current log level.
 */
log_level_t log_getlevel(void);

/**
 * Sets the file descriptor fd as logging file descriptor. NULL for no logging.
 * @return {@link #LOG_OK} or {@link #LOG_ERROR} on error
 */
int log_setout(FILE * fd);

/**
 * Returns the current output stream for logging.
 * @return FILE * pointer to the log file or @a NULL if not enabled.
 */
FILE * log_getout(void);

/**
 * Logs message at LOG_LEVEL_INFO level.
 * @return {@link #LOG_OK} or {@link #LOG_ERROR} on error
 */
int log_info(const char *, ...);
/**
 * Logs message at LOG_LEVEL_WARN level.
 * @return {@link #LOG_OK} or {@link #LOG_ERROR} on error
 */
int log_warn(const char *, ...);
/**
 * Logs message at LOG_LEVEL_ERROR level.
 * @return {@link #LOG_OK} or {@link #LOG_ERROR} on error
 */
int log_error(const char *, ...);

/**
 * Logs message at LOG_LEVEL_DEBUG level.
 * @return {@link #LOG_OK} or {@link #LOG_ERROR} on error
 */
int log_debug(const char *, ...);

/**
 * Logs message at LOG_LEVEL_TRACE level.
 * @return {@link #LOG_OK} or {@link #LOG_ERROR} on error
 */
int log_trace(const char *, ...);

#ifdef  __cplusplus
}
#endif


#endif // _LOG_H_
