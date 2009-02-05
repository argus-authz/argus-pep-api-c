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
 * $Id: pep.h,v 1.5 2009/02/05 16:00:55 vtschopp Exp $
 */
#ifndef _PEP_H_
#define _PEP_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include "pep/model.h"
#include "pep/error.h"

/*
 * PEP log levels
 */
#define PEP_LOGLEVEL_NONE  0 /**< No logging at all */
#define PEP_LOGLEVEL_ERROR 1 /**< Log only ERROR messages */
#define PEP_LOGLEVEL_WARN  2 /**< Log ERROR and WARN messages */
#define PEP_LOGLEVEL_INFO  3 /**< Log ERROR, WARN and INFO messages */
#define PEP_LOGLEVEL_DEBUG 4 /**< Log ERROR, WARN, INFO and DEBUG messages */

/*
 * PEP PIP function prototypes and type.
 *
 * These function prototypes allow to implement a PIP. The PIP
 * pre-processes the PEP request, before the PEP client sends it to the
 * PEP daemon.
 *
 * The PIP functions must return 0 on success or an error code.
 *
 * PIP must be added to the PEP client before sending the request.
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
 * @return 0 on success or an error code.
 * @see pep_authorize(pep_request_t **, pep_response_t **)
 */
typedef int pip_process_func(pep_request_t **);

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
typedef struct {
	char * id; /**< unique identifier for the PIP */
	pip_init_func * init; /**< pointer to the PIP init function */
	pip_process_func * process; /**< pointer to PIP process function */
	pip_destroy_func * destroy; /**< pointer to the PIP destroy function */
} pep_pip_t;

/*
 * PEP Obligation handler function prototypes and type.
 *
 * The function prototypes allow to implement a Obligation Handler (OH).
 * The OH does the post-processing of the PEP request and response, after
 * the PEP have send the request and receive the response.
 *
 * The OH functions must return 0 on success or an error code.
 *
 * OH must be added to the PEP client before sending the request.
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
 * The process(request,response) function is called after the PEP client
 * receives the PEP response back from PEP daemon.
 *
 * @param pep_request_t ** address of the pointer to the PEP request
 * @param pep_response_t ** address of the pointer to the PEP response
 *
 * @return 0 on success or an error code.
 * @see pep_authorize(pep_request_t **, pep_response_t **)
 */
typedef int oh_process_func(pep_request_t **, pep_response_t **);

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
 * ObligationHandler type.
 */
typedef struct {
	char * id; /**< unique identifier for the OH */
	oh_init_func * init; /**< pointer to the OH init function */
	oh_process_func * process; /**< pointer to the OH process function */
	oh_destroy_func * destroy; /**< pointer to the OH destroy function */
} pep_obligationhandler_t;

/**
 * PEP client configuration options.
 *
 * @see pep_setoption(option, ...) to set a configuration option.
 */
typedef enum {
	PEP_OPTION_LOG_LEVEL,  /**< Set log level (default {@link #PEP_LOGLEVEL_NONE}) */
	PEP_OPTION_LOG_STDERR,  /**< Set log engine file descriptor: stderr, stdout, NULL (default NULL) */
	PEP_OPTION_ENDPOINT_URL, /**< PEP daemon URL. Example: http://localhost:8080/pepd/authz */
	PEP_OPTION_ENDPOINT_SSL_VALIDATION, /**< Enable SSL validation: 0 or 1 (default 1) */
	PEP_OPTION_ENDPOINT_SERVER_CERT, /**< PEP daemon server SSL certificate: absolute filename */
	PEP_OPTION_ENDPOINT_CLIENT_CERT, /**< PEP client SSL certificate for client authn: absolute filename */
	PEP_OPTION_ENDPOINT_TIMEOUT, /**< Timeout for the connection to endpoint URL in second (default 10s) */
	PEP_OPTION_ENABLE_PIPS, /**< Enable PIPs pre-processing: 0 or 1 (default 1) */
	PEP_OPTION_ENABLE_OBLIGATIONHANDLERS /**< Enable OHs post-processing: 0 or 1 (default 1) */
} pep_option_t;

/**
 * Initializes the PEP client.
 *
 * @return pep_error_t PEP_OK on success or an error code.
 */
pep_error_t pep_initialize(void);

/**
 * Adds a PIP request pre-processor to the PEP client.
 *
 * @param pip pointer to the {@link pep_pip_t} to add.
 *
 * @return pep_error_t PEP_OK on success or an error code.
 */
pep_error_t pep_addpip(pep_pip_t * pip);

/**
 * Adds an Obligation Handler post-processor to the
 * PEP client.
 *
 * @param oh pointer to the {@link pep_obligationhandler_t} to add.
 *
 * @return pep_error_t PEP_OK on success or an error code.
 */
pep_error_t pep_addobligationhandler(pep_obligationhandler_t * oh);

/**
 * Sets a PEP client configuration option.
 *
 * @param option the PEP client option to set.
 * @param ... argument(s) for the PEP option.
 *
 * @return pep_error_t PEP_OK on success or an error code.
 * @see pep_option_t
 *
 * Available options:
 *   pep_setoption(PEP_OPTION_ENDPOINT_URL, "https://pep.switch.ch:8080/authz");
 *   pep_setoption(PEP_OPTION_LOG_STDERR, mylogfile);
 *   pep_setoption(PEP_OPTION_LOG_LEVEL, PEP_LOGLEVEL_WARN);
 *   pep_setoption(PEP_OPTION_ENABLE_PIPS,0); // disable PIP processing
 *   pep_setoption(PEP_OPTION_ENABLE_OBLIGATIONHANDLERS,1); // enable OH processing (default is enabled)
 *
 */
pep_error_t pep_setoption(pep_option_t option, ... );

/**
 * Sends the PEP request to the PEP daemon and returns the PEP response.
 *
 * If the PIPs are enabled, they will be applied to the request before submitting it.
 * If the ObligationHandlers are enabled, they will be applied to the response after
 * the response is received and returned from the method.
 *
 * @param request address of the pointer to the {@link #pep_request_t} to send.
 * @param response address of pointer to the {@link #pep_response_t} received.
 *
 * @return pep_error_t PEP_OK on success or an error code.
 */
pep_error_t pep_authorize(pep_request_t ** request, pep_response_t ** response);

/**
 * Cleanups and destroys the PEP client.
 *
 * @return pep_error_t PEP_OK on success or an error code.
 */
pep_error_t pep_destroy(void);

#ifdef  __cplusplus
}
#endif

#endif
