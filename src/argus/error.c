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

#include <string.h>
#include <stdio.h>
#include <stdarg.h>  /* va_list, va_arg, ... */
#include <curl/curl.h> /* curl_easy_strerror */

#include "error.h"

/*
typedef enum {
    PEP_OK                          = 0,
    PEP_ERR_MEMORY,
    PEP_ERR_NULL_POINTER,
    PEP_ERR_LLIST,
    PEP_ERR_PIP_INIT,
    PEP_ERR_OH_INIT,
    PEP_ERR_OPTION_INVALID,
    PEP_ERR_PIP_PROCESS,
    PEP_ERR_AUTHZ_REQUEST,
    PEP_ERR_OH_PROCESS, 
    PEP_ERR_MARSHALLING_HESSIAN,
    PEP_ERR_MARSHALLING_IO,
    PEP_ERR_UNMARSHALLING_HESSIAN,
    PEP_ERR_UNMARSHALLING_IO,
    PEP_ERR_CURL                    = 1024,
} pep_error_t;
*/

const char * pep_strerror(pep_error_t pep_errno) {
    switch(pep_errno) {
    case PEP_OK:
        return "No error";
        
    case PEP_ERR_MEMORY:
        return "Memory allocation error";
        
    case PEP_ERR_NULL_POINTER:
        return "NULL pointer error";
        
    case PEP_ERR_LLIST:
        return "linkedlist error";
        
    case PEP_ERR_PIP_INIT:
        return "PIP init error";
        
    case PEP_ERR_OH_INIT:
        return "OH init error";
        
    case PEP_ERR_OPTION_INVALID:
        return "invalid option";
        
    case PEP_ERR_PIP_PROCESS:
        return "PIP process error";
        
    case PEP_ERR_AUTHZ_REQUEST:
        return "authorize request error";
        
    case PEP_ERR_OH_PROCESS:
        return "OH process error";
        
    case PEP_ERR_MARSHALLING_HESSIAN:
        return "Hessian marshalling error";
        
    case PEP_ERR_MARSHALLING_IO:
        return "Marshalling IO error";
        
    case PEP_ERR_UNMARSHALLING_HESSIAN:
        return "Hessian unmarshalling error";
        
    case PEP_ERR_UNMARSHALLING_IO:
        return "Unmarshalling IO error";
        
    default:
        /* should be PEP_ERR_CURL. curl_easy_strerror returns "Unkown error" if no match */
        return curl_easy_strerror(pep_errno - PEP_ERR_CURL);
    }
}
