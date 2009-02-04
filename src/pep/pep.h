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
 * $Id: pep.h,v 1.4 2009/02/04 09:51:00 vtschopp Exp $
 */
#ifndef _PEP_H_
#define _PEP_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include "pep/model.h"
#include "pep/error.h"

/** Logging levels */
#define PEP_LOGLEVEL_NONE  0
#define PEP_LOGLEVEL_ERROR 1
#define PEP_LOGLEVEL_WARN  2
#define PEP_LOGLEVEL_INFO  3
#define PEP_LOGLEVEL_DEBUG 4

/*****************************************************************
 * PEP PIP function prototypes and type.
 *
 * These function prototypes allow to implement a PIP. The PIP does
 * pre-processing of the PEP request, before the PEP sends it to the
 * PEP daemon.
 *
 * The PIP functions must return 0 on success or an error code.
 *
 * PIP must be added to the PEP client before sending the request.
 */
typedef int (*pip_init_func) (void);
typedef int (*pip_process_func) (pep_request_t **);
typedef int (*pip_destroy_func) (void);
typedef struct pep_pip {
	char * id; // unique identifier for the PIP
	pip_init_func init; // init the PIP
	pip_process_func process; // pre-process the request
	pip_destroy_func destroy; // destroy the PIP
} pep_pip_t;

/*****************************************************************
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
typedef int (*oh_init_func) (void);
typedef int (*oh_process_func) (pep_request_t **, pep_response_t **);
typedef int (*oh_destroy_func) (void);
typedef struct pep_obligationhandler {
	char * id; // unique identifier for the OH
	oh_init_func init; // init the OH
	oh_process_func process; // post-process the request and response
	oh_destroy_func destroy; // destroy the OH
} pep_obligationhandler_t;

/*****************************************************************
 * PEP client configuration option types.
 *
 * Use pep_setoption(option, ...) to set a configuration option.
 */
typedef enum pep_option {
	PEP_OPTION_LOG_LEVEL,  // Set log level (default no logging 0)
	PEP_OPTION_LOG_STDERR,  // Set log engine file descriptor: stderr, stdout, NULL (default NULL)
	PEP_OPTION_ENDPOINT_URL, // PEP daemon URL: http://localhost:8080/pepd/authz
	PEP_OPTION_ENDPOINT_SSL_VALIDATION, // Enable SSL validation: 0 or 1
	PEP_OPTION_ENDPOINT_SERVER_CERT, // PEP daemon server SSL cert: filename
	PEP_OPTION_ENDPOINT_CLIENT_CERT, // PEP client SSL cert: filename
	PEP_OPTION_ENDPOINT_TIMEOUT, // Timeout for the connection to endpoint URL in second (default 10s)
	PEP_OPTION_ENABLE_PIPS, // Enable PIPs pre-processing: 0 or 1 (default 1)
	PEP_OPTION_ENABLE_OBLIGATIONHANDLERS // Enable OHs post-processing: 0 or 1 (default 1)
} pep_option_t;

/******************************************************************/

/**
 * Initializes the PEP client.
 *
 * @return pep_error_t PEP_OK on success or an error code.
 */
pep_error_t pep_initialize(void);

/**
 * Adds a PIP request pre-processor to the PEP client.
 *
 * @param pep_pip_t * pip pointer to the PIP to add.
 *
 * @return pep_error_t PEP_OK on success or an error code.
 */
pep_error_t pep_addpip(pep_pip_t * pip);

/**
 * Adds an Obligation Handler request, response post-processor to the
 * PEP client.
 *
 * @param pep_obligationhandler * oh pointer to the Obligation handler to add.
 *
 * @return pep_error_t PEP_OK on success or an error code.
 */
pep_error_t pep_addobligationhandler(pep_obligationhandler_t * oh);

/**
 * Sets a PEP client configuration option.
 *
 * @param pep_option_t option the PEP option name.
 * @param ... argument for the PEP option.
 *
 * @return pep_error_t PEP_OK on success or an error code.
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
 * @param pep_request_t ** request address of the pointer to the PEP request to send.
 * @param pep_response_t ** response address of pointer to the PEP response received.
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
