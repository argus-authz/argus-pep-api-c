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
 * $Id: log.c,v 1.3 2009/01/29 14:51:16 vtschopp Exp $
 */

#include <stdio.h>
#include <stdarg.h>  /* va_list, va_arg, ... */
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "util/log.h"

// tmp buffer for logging
#define BUFFER_SIZE 1024
static char BUFFER[BUFFER_SIZE];

// output file handler
static FILE * log_out= NULL;
// log level
static log_level_t log_level= LOG_LEVEL_NONE;

int log_setlevel(log_level_t level) {
	log_level= level;
	return LOG_OK;
}

log_level_t log_getlevel(void) {
	return log_level;
}

int log_setout(FILE * file) {
	log_out= file;
	return LOG_OK;
}

FILE * log_getout(void) {
	return log_out;
}

// prototype
static int log_vfprintf(FILE * out, time_t * epoch, const char * level, const char * fmt, va_list args);

int log_info(const char *fmt, ...) {
	int rc= LOG_OK;
	if (log_level >= LOG_LEVEL_INFO) {
		if (log_out != NULL) {
			time_t epoch;
			time(&epoch);
			va_list args;
			va_start(args,fmt);
			rc= log_vfprintf(log_out, &epoch, "INFO", fmt, args);
			va_end(args);
		}
	}
	return rc;
}

int log_warn(const char *fmt, ...) {
	int rc= LOG_OK;
	if (log_level >= LOG_LEVEL_WARN) {
		if (log_out != NULL) {
			time_t epoch;
			time(&epoch);
			va_list args;
			va_start(args,fmt);
			rc= log_vfprintf(log_out, &epoch, "WARN", fmt, args);
			va_end(args);
		}
	}
	return rc;
}

int log_error(const char *fmt, ...) {
	int rc= LOG_OK;
	if (log_level >= LOG_LEVEL_ERROR) {
		if (log_out != NULL) {
			time_t epoch;
			time(&epoch);
			va_list args;
			va_start(args,fmt);
			rc= log_vfprintf(log_out, &epoch, "ERROR", fmt, args);
			va_end(args);
		}
	}
	return rc;
}

int log_debug(const char *fmt, ...) {
	int rc= LOG_OK;
	if (log_level >= LOG_LEVEL_DEBUG) {
		if (log_out != NULL) {
			time_t epoch;
			time(&epoch);
			va_list args;
			va_start(args,fmt);
			rc= log_vfprintf(log_out, &epoch, "DEBUG", fmt, args);
			va_end(args);
		}
	}
	return rc;
}



static int log_vfprintf(FILE * out, time_t * epoch, const char * level, const char * fmt, va_list args) {
	struct tm * time= localtime(epoch);
	strftime(BUFFER,BUFFER_SIZE,"%F %T ",time);
	size_t size= BUFFER_SIZE - strlen(BUFFER);
	strncat(BUFFER,level,size);
	size= size - strlen(BUFFER);
	strncat(BUFFER,": ",size);
	size= size - strlen(BUFFER);
	strncat(BUFFER,fmt,size);
	vfprintf(out,BUFFER,args);
	memset(BUFFER,0,BUFFER_SIZE);
	return LOG_OK;
}
