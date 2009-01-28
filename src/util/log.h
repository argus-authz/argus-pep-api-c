/*
 * Copyright 2008 Members of the EGEE Collaboration.
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
 * $Id: log.h,v 1.1 2009/01/28 16:15:15 vtschopp Exp $
 */
#ifndef _LOG_H_
#define _LOG_H_

#ifdef  __cplusplus
extern "C" {
#endif

/** function return codes */
#define LOG_OK 0
#define LOG_ERROR -1

/** Log levels */
typedef enum {
	LOG_LEVEL_NONE =0,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_WARN,
	LOG_LEVEL_INFO,
	LOG_LEVEL_DEBUG
} log_level_t;

/**
 * Sets the log level to level.
 */
int log_setlevel(log_level_t level);

/**
 * Sets the file descriptor fd as logging file descriptor. NULL for no logging.
 */
int log_setfout(FILE * fd);

int log_info(const char *, ...);
int log_warn(const char *, ...);
int log_error(const char *, ...);
int log_debug(const char *, ...);


#ifdef  __cplusplus
}
#endif


#endif // _LOG_H_
