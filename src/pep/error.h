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
 * $Id: error.h,v 1.1 2008/12/12 11:34:27 vtschopp Exp $
 */
#ifndef _PEP_ERROR_H_
#define _PEP_ERROR_H_

#ifdef  __cplusplus
extern "C" {
#endif

/*****************************************************************
 * PEP client error codes.
 *
 * Use pep_strerror(errno) to retrieve the error description.
 */
typedef enum pep_error {
	PEP_OK = 0,
	PEP_ERR_MEMORY,
	PEP_ERR_NULL_POINTER,
	PEP_ERR_INIT_LISTS,
	PEP_ERR_INIT_CURL,
	PEP_ERR_INIT_PIP,
	PEP_ERR_INIT_OH,
	PEP_ERR_OPTION_INVALID,
	PEP_ERR_AUTHZ_PIP_PROCESS,
	PEP_ERR_AUTHZ_CURL,
	PEP_ERR_AUTHZ_REQUEST,
	PEP_ERR_AUTHZ_OH_PROCESS,
	PEP_ERR_MARSHALLING_HESSIAN,
	PEP_ERR_MARSHALLING_IO,
	PEP_ERR_UNMARSHALLING_HESSIAN,
	PEP_ERR_UNMARSHALLING_IO,
} pep_error_t;

/**
 * Returns the string describing the error number errno.
 */
const char * pep_strerror(pep_error_t errno);

#ifdef  __cplusplus
}
#endif

#endif // _PEP_ERROR_H_
