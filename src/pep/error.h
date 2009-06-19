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
 */
/*
 * $Id$
 * $Name:  $
 * @author Valery Tschopp <valery.tschopp@switch.ch>
 * @version 1.0
 */
#ifndef _PEP_ERROR_H_
#define _PEP_ERROR_H_

#ifdef  __cplusplus
extern "C" {
#endif

/** @defgroup Error Error Reporting
 * Error codes and error reporting functions.
 *
 * Example:
 * @code
 * ...
 * pep_error_t rc= pep_initialize();
 * if (rc != PEP_OK) {
 *    fprintf(stderr,"ERROR: %s\n",pep_strerror(rc));
 *    pep_destroy();
 *    exit(1);
 * }
 * ...
 * @endcode
 *
 * @see pep_error_t
 * @see pep_strerror(pep_error_t errno)
 *
 * @{
 */
/**
 * PEP client error codes.
 *
 * Use pep_strerror(pep_error_t errno) to retrieve the error description.
 *
 * @see pep_strerror(pep_error_t errno)
 */
typedef enum pep_error {
	PEP_OK = 0, /**< OK, No error */
	PEP_ERR_MEMORY, /**< Memory allocation error */
	PEP_ERR_NULL_POINTER, /**< NULL pointer exception */
	PEP_ERR_INIT_LISTS, /**< List allocation error in pep_initialize() */
	PEP_ERR_INIT_CURL, /**< Curl initialization error in pep_initialize() */
	PEP_ERR_INIT_PIP, /**< PIP pip_init_func() error in pep_addpip(pep_pip_t *) */
	PEP_ERR_INIT_OH, /**< Obligation Hanlder oh_init_func() error in pep_addobligationhandler(pep_obligationhandler_t *) */
	PEP_ERR_OPTION_INVALID, /**< PEP client option invalid in pep_setoption(pep_option_t,args) */
	PEP_ERR_AUTHZ_PIP_PROCESS, /**< PIP process(pep_request_t) error in pep_authorize(pep_request_t **,pep_response_t **) */
	PEP_ERR_AUTHZ_CURL, /**< Curl generic error in in pep_authorize(pep_request_t **,pep_response_t **) */
	PEP_ERR_AUTHZ_CURL_PROCESS, /**< Curl POST error in pep_authorize(pep_request_t **,pep_response_t **) */
	PEP_ERR_AUTHZ_REQUEST, /**< HTTP Request failed (status not 200) in pep_authorize(pep_request_t **,pep_response_t **) */
	PEP_ERR_AUTHZ_OH_PROCESS, /**< Obligation Handler oh_process_func() error in pep_authorize(pep_request_t **,pep_response_t **) */
	PEP_ERR_MARSHALLING_HESSIAN, /**< Hessian marshalling error in pep_authorize(pep_request_t **,pep_response_t **) */
	PEP_ERR_MARSHALLING_IO, /**< IO error in pep_authorize(pep_request_t **,pep_response_t **) */
	PEP_ERR_UNMARSHALLING_HESSIAN, /**< Hessian unmarshalling error in pep_authorize(pep_request_t **,pep_response_t **) */
	PEP_ERR_UNMARSHALLING_IO, /**< IO error in pep_authorize(pep_request_t **,pep_response_t **) */
} pep_error_t;

/**
 * Gets the string describing the error number errno.
 *
 * @param pep_errno the error code returned by a function.
 * @return const char * the error message describing the error.
 */
const char * pep_strerror(pep_error_t pep_errno);

/** @internal
 * Sets an optional error message.
 */
void pep_errmsg(const char * format, ...);

/** @internal
 * Reset all the error messages previously set.
 */
void pep_clearerr(void);

/** @} */

#ifdef  __cplusplus
}
#endif

#endif // _PEP_ERROR_H_
