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

/*
 * $Id: error.h 1299 2009-10-15 15:29:34Z vtschopp $
 * $Name:  $
 * @author Valery Tschopp <valery.tschopp@switch.ch>
 */
#ifndef _PEP_PIP_H_
#define _PEP_PIP_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include "pep/xacml.h"

/** @defgroup PIP Policy Information Point (PIP)
 * PEP client PIP function prototypes and type.
 *
 * These function prototypes allow to implement a PIP. The PIP
 * pre-processes the PEP request, before the PEP client sends it to the
 * PEP daemon.
 *
 * The PIP functions must return 0 on success or an error code.
 *
 * PIP must be added to the PEP client before sending the request.
 * @{
 */
/**
 * PIP init function prototype.
 *
 * The init() function is called when the PIP is added to the PEP client.
 *
 * @return 0 on success or an error code.
 * @see pep_addpip(pep_pip_t * pip)
 */
typedef int pip_init_func(void);

/**
 * PIP process function prototype.
 *
 * The process(request) function is called before the PEP client
 * submit the authorization request to the PEP daemon.
 *
 * @param xacml_request_t ** address of the pointer to the PEP request
 * @return 0 on success or an error code.
 * @see pep_authorize(xacml_request_t **, xacml_response_t **)
 */
typedef int pip_process_func(xacml_request_t **);

/**
 * PIP destroy function prototype.
 *
 * The destroy() function is called when the PEP client is destroyed.
 *
 * @return 0 on success or an error code.
 * @see pep_destroy()
 */
typedef int pip_destroy_func(void);

/**
 * PIP type.
 */
typedef struct pep_pip {
	char * id; /**< unique identifier for the PIP */
	pip_init_func * init; /**< pointer to the PIP init function */
	pip_process_func * process; /**< pointer to PIP process function */
	pip_destroy_func * destroy; /**< pointer to the PIP destroy function */
} pep_pip_t;

/** @} */


#ifdef  __cplusplus
}
#endif

#endif
