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
 * $Id$
 */
#include <string.h>
#include <stdio.h>
#include <stdarg.h>  /* va_list, va_arg, ... */

#include "pep/error.h"

/*
typedef enum pep_error {
	PEP_OK,                         // 0
	PEP_ERR_MEMORY,                 // 1
	PEP_ERR_NULL_POINTER,           // 2
	PEP_ERR_INIT_LISTS,             // 3
	PEP_ERR_INIT_CURL,              // 4
	PEP_ERR_INIT_PIP,               // 5
	PEP_ERR_INIT_OH,                // 6
	PEP_ERR_OPTION_INVALID,         // 7
	PEP_ERR_AUTHZ_PIP_PROCESS,      // 8
	PEP_ERR_AUTHZ_CURL,             // 9
	PEP_ERR_AUTHZ_CURL_PROCESS,     // 10
	PEP_ERR_AUTHZ_REQUEST,          // 11
	PEP_ERR_AUTHZ_OH_PROCESS,       // 12
	PEP_ERR_MARSHALLING_HESSIAN,    // 13
	PEP_ERR_MARSHALLING_IO,         // 14
	PEP_ERR_UNMARSHALLING_HESSIAN,  // 15
	PEP_ERR_UNMARSHALLING_IO,       // 16
} pep_error_t;
 */

// TODO: find meaningful error messages, that admin (human) can understand
static const char * ERROR_STRINGS[]= {
		"OK",
		"Memory allocation error",
		"NULL pointer error",
		"init: linkedlist error",
		"init: curl error",
		"addpip: PIP init error",
		"addoh: OH init error",
		"setoption: invalid option",
		"authorize: PIP process error",
		"authorize: curl error",
		"authorize: curl process error",
		"authorize: processing error",
		"authorize: OH process error",
		"marshalling: Hessian error",
		"marshalling: IO error",
		"unmarshalling: Hessian error",
		"unmarshalling: IO error",
		NULL
};

// tmp buffer for error
#define ERR_BUFFER_SIZE 1024
static char ERR_BUFFER[ERR_BUFFER_SIZE];
static char ERR_MSG_BUFFER[ERR_BUFFER_SIZE];
static int err_msg_length= 0;

void pep_errmsg(const char * format, ...) {
	va_list args;
	va_start(args,format);
	memset(ERR_MSG_BUFFER,0,ERR_BUFFER_SIZE);
	vsnprintf(ERR_MSG_BUFFER,ERR_BUFFER_SIZE,format,args);
	err_msg_length= strlen(ERR_MSG_BUFFER);
	va_end(args);
}

void pep_clearerr(void) {
	memset(ERR_MSG_BUFFER,0,ERR_BUFFER_SIZE);
	err_msg_length= 0;
}

const char * pep_strerror(pep_error_t pep_errno) {
	//return "pep_strerror(...) not yet implemented.";
	memset(ERR_BUFFER,0,ERR_BUFFER_SIZE);
	if (err_msg_length > 0) {
		snprintf(ERR_BUFFER,ERR_BUFFER_SIZE,"[%d]: %s: %s", (int)pep_errno,ERROR_STRINGS[pep_errno],ERR_MSG_BUFFER);
	}
	else {
		snprintf(ERR_BUFFER,ERR_BUFFER_SIZE,"[%d]: %s", (int)pep_errno,ERROR_STRINGS[pep_errno]);
	}
	return ERR_BUFFER;
}
