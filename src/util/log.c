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
 * $Id: log.c,v 1.1 2009/01/28 16:15:15 vtschopp Exp $
 */

#include <stdio.h>
#include <stdarg.h>  /* va_list, va_arg, ... */
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "util/log.h"

static FILE * log_out= NULL;
static log_level_t log_level= LOG_LEVEL_NONE;

int log_setlevel(log_level_t level) {
	log_level= level;
	return LOG_OK;
}

int log_setout(FILE * file) {
	log_out= file;
	return 0;
}

static int log_fprintf(FILE * out, time_t * epoch, const char * level, const char * fmt, va_list args);

int log_info(const char *fmt, ...) {
	int rc= LOG_OK;
	if (log_level >= LOG_LEVEL_INFO) {
		if (log_out != NULL) {
			time_t epoch;
			time(&epoch);
			va_list args;
			va_start(args,fmt);
			rc= log_fprintf(log_out, &epoch, "INFO", fmt, args);
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
			rc= log_fprintf(log_out, &epoch, "WARN", fmt, args);
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
			rc= log_fprintf(log_out, &epoch, "ERROR", fmt, args);
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
			rc= log_fprintf(log_out, &epoch, "DEBUG", fmt, args);
			va_end(args);
		}
	}
	return rc;
}



static int log_fprintf(FILE * out, time_t * epoch, const char * level, const char * fmt, va_list args) {
	size_t size= strlen(level) + strlen(fmt) + 1024;
	char * new_fmt= calloc(size + 1,sizeof(char));
	if (new_fmt == NULL) {
		// error
		return LOG_ERROR;
	}
	// TODO: add timestamp like log4j %d{ISO8601}
	// TODO: use snprintf(buf, size, "%s %s %s", timestamp, level, fmt)
	snprintf(new_fmt,size,"%s %s %s","ISO8601",level,fmt);
	int rc= vfprintf(out,new_fmt,args);
	free(new_fmt);
	return LOG_OK;
}
