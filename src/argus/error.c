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

#include "argus/error.h"

/*
typedef enum {
    PEP_OK                          = 0,
    PEP_ERR_MEMORY                  = 1,
    PEP_ERR_NULL_POINTER            = 2,
    PEP_ERR_LLIST                   = 3,
    PEP_ERR_CURL                    = 4,
    PEP_ERR_PIP_INIT                = 5,
    PEP_ERR_OH_INIT                 = 6,
    PEP_ERR_OPTION_INVALID          = 7,
    PEP_ERR_PIP_PROCESS             = 8,
    PEP_ERR_CURL_PROCESS            = 9,
    PEP_ERR_AUTHZ_REQUEST           = 10,
    PEP_ERR_OH_PROCESS              = 11, 
    PEP_ERR_MARSHALLING_HESSIAN     = 12,
    PEP_ERR_MARSHALLING_IO          = 13,
    PEP_ERR_UNMARSHALLING_HESSIAN   = 14,
    PEP_ERR_UNMARSHALLING_IO        = 15
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
        
    case PEP_ERR_CURL:
        return "CURL error";
        
    case PEP_ERR_PIP_INIT:
        return "PIP init error";
        
    case PEP_ERR_OH_INIT:
        return "OH init error";
        
    case PEP_ERR_OPTION_INVALID:
        return "invalid option";
        
    case PEP_ERR_PIP_PROCESS:
        return "PIP process error";
        
    case PEP_ERR_CURL_PERFORM:
        return "CURL processing error";
        
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
        return "Unkown error";
    }
}
