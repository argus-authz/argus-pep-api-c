/*
 * Copyright 2008 Members of the EGEE Collaboration.
 * See http://www.eu-egee.org/partners for details on the copyright holders.
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
 * $Id: error.h 1299 2009-10-15 15:29:34Z vtschopp $
 * $Name:  $
 * @author Valery Tschopp <valery.tschopp@switch.ch>
 */
#ifndef _PEP_OH_H_
#define _PEP_OH_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include "pep/xacml.h"

/** @defgroup ObligationHandler Obligation Handler (OH)
 * PEP client Obligation handler function prototypes and type.
 *
 * The OH function prototypes allow to implement a Obligation Handler (OH).
 * The Obligation Handler does the post-processing of the PEP request and response, after
 * the PEP client have send the request and receive the response.
 *
 * The OH functions must return 0 on success or an error code.
 *
 * OH must be added to the PEP client before sending the request.
 * @{
 */
/**
 * Obligation handler init function prototype.
 *
 * The init() function is called when the OH is added to the PEP client.
 *
 * @return 0 on success or an error code.
 * @see pep_addobligationhandler(pep_obligationhandler_t * oh)
 */
typedef int oh_init_func(void);

/**
 * Obligation handler process function prototype.
 *
 * The process(&request,&response) function will be called by the pep_authorize(...) function, just
 * after the XACML response is received back from PEP daemon.
 *
 * @param xacml_request_t ** address of the pointer to the PEP request
 * @param xacml_response_t ** address of the pointer to the PEP response
 *
 * @return 0 on success or an error code.
 * @see pep_authorize(xacml_request_t **, xacml_response_t **)
 */
typedef int oh_process_func(xacml_request_t **, xacml_response_t **);

/**
 * Obligation handler destroy function prototype.
 *
 * The destroy() function is called when the PEP client is destroyed.
 *
 * @return 0 on success or an error code.
 * @see pep_destroy()
 */
typedef int oh_destroy_func(void);

/**
 * Obligation Handler type.
 */
typedef struct pep_obligationhandler {
	char * id; /**< unique identifier for the OH */
	oh_init_func * init; /**< pointer to the OH init function */
	oh_process_func * process; /**< pointer to the OH process function */
	oh_destroy_func * destroy; /**< pointer to the OH destroy function */
} pep_obligationhandler_t;
/** @} */


#ifdef  __cplusplus
}
#endif

#endif // _PEP_OH_H_
