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
 * $Id: log.c,v 1.7 2009/02/20 12:01:06 vtschopp Exp $
 */

#include <stdio.h>
#include <stdarg.h>  /* va_list, va_arg, ... */
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "util/log.h"

// tmp buffer for logging
#define LOG_BUFFER_SIZE 1024
static char LOG_BUFFER[LOG_BUFFER_SIZE];

// output file handler
static FILE * log_out= NULL;
// log level
static log_level_t log_level= LOG_LEVEL_NONE;

// internal prototypes:
static int _log_vfprintf(FILE * out, time_t * epoch, const char * level, const char * fmt, va_list args);

// level prio strings
static const char * LEVEL_EVENTS[]= {"ERROR","WARN","INFO","DEBUG","TRACE"};
// internal log handler function
static int log_handler_default(log_level_t level,const char *fmt, va_list args) {
	time_t epoch;
	time(&epoch);
	return _log_vfprintf(log_out, &epoch, LEVEL_EVENTS[level], fmt, args);
}

// log handler function pointer
static log_handler_func * log_handler= (log_handler_func*)log_handler_default;


int log_sethandler(log_handler_func * handler) {
	log_handler= handler;
	return LOG_OK;
}

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



int log_info(const char *fmt, ...) {
	int rc= LOG_OK;
	if (log_level >= LOG_LEVEL_INFO) {
		va_list args;
		va_start(args,fmt);
		if (log_handler != NULL) {
			rc= log_handler(LOG_LEVEL_INFO,fmt,args);
		}
		va_end(args);
	}
	return rc;
}

int log_warn(const char *fmt, ...) {
	int rc= LOG_OK;
	if (log_level >= LOG_LEVEL_WARN) {
		va_list args;
		va_start(args,fmt);
		if (log_handler != NULL) {
			rc= log_handler(LOG_LEVEL_WARN,fmt,args);
		}
		va_end(args);
	}
	return rc;
}

int log_error(const char *fmt, ...) {
	int rc= LOG_OK;
	if (log_level >= LOG_LEVEL_ERROR) {
		va_list args;
		va_start(args,fmt);
		if (log_handler != NULL) {
			rc= log_handler(LOG_LEVEL_ERROR,fmt,args);
		}
		va_end(args);
	}
	return rc;
}

int log_debug(const char *fmt, ...) {
	int rc= LOG_OK;
	if (log_level >= LOG_LEVEL_DEBUG) {
		va_list args;
		va_start(args,fmt);
		if (log_handler != NULL) {
			rc= log_handler(LOG_LEVEL_DEBUG,fmt,args);
		}
		va_end(args);
	}
	return rc;
}

int log_trace(const char *fmt, ...) {
	int rc= LOG_OK;
	if (log_level >= LOG_LEVEL_TRACE) {
		va_list args;
		va_start(args,fmt);
		if (log_handler != NULL) {
			rc= log_handler(LOG_LEVEL_TRACE,fmt,args);
		}
		va_end(args);
	}
	return rc;
}


// logs in file
// format "YYYY-MM-DD HH:MM:SS <level>: <formated_message>\n"
// uses the LOG_BUFFER to create the complete format string
static int _log_vfprintf(FILE * out, time_t * epoch, const char * level, const char * fmt, va_list args) {
	memset(LOG_BUFFER,0,LOG_BUFFER_SIZE);
	if (out != NULL) {
		struct tm * time= localtime(epoch);
		strftime(LOG_BUFFER,LOG_BUFFER_SIZE,"%F %T ",time);
		size_t size= LOG_BUFFER_SIZE - strlen(LOG_BUFFER);
		if (level != NULL) {
			strncat(LOG_BUFFER,level,size);
			size= size - strlen(LOG_BUFFER);
		}
		strncat(LOG_BUFFER,": ",size);
		size= size - strlen(LOG_BUFFER);
		strncat(LOG_BUFFER,fmt,size);
		size= size - strlen(LOG_BUFFER);
		strncat(LOG_BUFFER,"\n",size);
		vfprintf(out,LOG_BUFFER,args);
	}
	return LOG_OK;
}

