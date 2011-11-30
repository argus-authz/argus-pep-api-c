/*
 * Copyright (c) Members of the EGEE Collaboration. 2006-2010.
 * See http://www.eu-egee.org/partners/ for details on the copyright holders.
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
 */

#include <stdio.h>
#include <stdarg.h>  /* va_list, va_arg, ... */
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "log.h"

/* tmp buffer for logging */
#define LOG_BUFFER_SIZE 1024

/* output file handler */
static FILE * log_out= NULL;

/* log level */
static pep_log_level_t log_level= LOG_LEVEL_NONE;

/* internal prototypes: */
static int _log_vfprintf(FILE * out, time_t * epoch, const char * level, const char * fmt, va_list args);

/* level prio strings */
static const char * LEVEL_EVENTS[]= {"ERROR"," WARN"," INFO","DEBUG","TRACE"};

/* internal log handler function */
static int default_log_handler(pep_log_level_t level,const char *fmt, va_list args) {
    time_t epoch;
    time(&epoch);
    return _log_vfprintf(log_out, &epoch, LEVEL_EVENTS[level], fmt, args);
}

/* log handler function pointer */
static pep_log_handler_func * log_handler= (pep_log_handler_func*)default_log_handler;


int pep_log_sethandler(pep_log_handler_func * handler) {
    log_handler= handler;
    return LOG_OK;
}

int pep_log_setlevel(pep_log_level_t level) {
    log_level= level;
    return LOG_OK;
}

pep_log_level_t pep_log_getlevel(void) {
    return log_level;
}

int pep_log_setout(FILE * file) {
    log_out= file;
    return LOG_OK;
}

FILE * pep_log_getout(void) {
    return log_out;
}



int pep_log_info(const char *fmt, ...) {
    int rc= LOG_OK;
    va_list args;
    if (log_level >= LOG_LEVEL_INFO) {
        va_start(args,fmt);
        if (log_handler != NULL) {
            rc= log_handler(LOG_LEVEL_INFO,fmt,args);
        }
        va_end(args);
    }
    return rc;
}

int pep_log_warn(const char *fmt, ...) {
    int rc= LOG_OK;
    va_list args;
    if (log_level >= LOG_LEVEL_WARN) {
        va_start(args,fmt);
        if (log_handler != NULL) {
            rc= log_handler(LOG_LEVEL_WARN,fmt,args);
        }
        va_end(args);
    }
    return rc;
}

int pep_log_error(const char *fmt, ...) {
    int rc= LOG_OK;
    va_list args;
    if (log_level >= LOG_LEVEL_ERROR) {
        va_start(args,fmt);
        if (log_handler != NULL) {
            rc= log_handler(LOG_LEVEL_ERROR,fmt,args);
        }
        va_end(args);
    }
    return rc;
}

int pep_log_debug(const char *fmt, ...) {
    int rc= LOG_OK;
    va_list args;
    if (log_level >= LOG_LEVEL_DEBUG) {
        va_start(args,fmt);
        if (log_handler != NULL) {
            rc= log_handler(LOG_LEVEL_DEBUG,fmt,args);
        }
        va_end(args);
    }
    return rc;
}

int pep_log_trace(const char *fmt, ...) {
    int rc= LOG_OK;
    va_list args;
    if (log_level >= LOG_LEVEL_TRACE) {
        va_start(args,fmt);
        if (log_handler != NULL) {
            rc= log_handler(LOG_LEVEL_TRACE,fmt,args);
        }
        va_end(args);
    }
    return rc;
}


/* 
 * logs into file out
 * format "YYYY-MM-DD HH:MM:SS <level>: <formated_message>\n"
 */
static int _log_vfprintf(FILE * out, time_t * epoch, const char * level, const char * fmt, va_list args) {
    struct tm * time;
    size_t size;
    char * buffer;
    if (out != NULL) {
        /* alloc tmp buffer */
        buffer= calloc(LOG_BUFFER_SIZE + 1, sizeof(char));
        if (buffer==NULL) return LOG_ERROR;
        /* format log message into buffer */
        time= localtime(epoch);
        strftime(buffer,LOG_BUFFER_SIZE,"%Y-%m-%d %H:%M:%S ",time);
        size= LOG_BUFFER_SIZE - strlen(buffer);
        if (level != NULL) {
            strncat(buffer,level,size);
            size= LOG_BUFFER_SIZE - strlen(buffer);
        }
        strncat(buffer,": ",size);
        size= LOG_BUFFER_SIZE - strlen(buffer);
        strncat(buffer,fmt,size);
        size= LOG_BUFFER_SIZE - strlen(buffer);
        strncat(buffer,"\n",size);
        /* write buffer to out file */
        vfprintf(out,buffer,args);
        /* flush it */
        fflush(out);
        /* free tmp buffer */
        free(buffer);
    }
    return LOG_OK;
}

