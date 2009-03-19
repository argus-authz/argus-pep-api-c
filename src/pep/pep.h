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
 * $Id: pep.h,v 1.14 2009/03/19 14:20:41 vtschopp Exp $
 * $Name:  $
 * @author Valery Tschopp <valery.tschopp@switch.ch>
 * @version 1.0
 */
#ifndef _PEP_H_
#define _PEP_H_

#ifdef  __cplusplus
extern "C" {
#endif

/** @mainpage PEP client C library
 *
 * This is the PEP client library for C, used to talk with the PEP daemon.
 *
 */

/** @defgroup PEPClient PEP-C Client */
/** @defgroup PIP Policy Information Point (PIP) */
/** @defgroup ObligationHandler Obligation Handler (OH) */
/** @defgroup Error Error Reporting */
/** @defgroup Logging Log Level and Output */
/** @defgroup XACML PEP XACML Objects Model */

#include <stdarg.h> // va_list
#include "pep/xacml.h"
#include "pep/error.h"

/** @addtogroup Logging
 *
 * Sets PEP client log levels and log output.
 *
 * By default the log level is {@link #PEP_LOGLEVEL_NONE} and the log output is NULL, therefore,
 * the PEP client doesn't log anything.
 *
 * See @ref Error for example how to handle error in your code.
 *
 * Example to debug in a log file:
 * @code
 * ...
 * // set log output to mylogfile
 * rc= pep_setoption(PEP_OPTION_LOG_STDERR, my_logfile);
 * if (rc != PEP_OK) {
 *    fprintf(stderr,"ERROR: %s\n",pep_strerror(rc));
 * }
 * // set log level to DEBUG
 * rc= pep_setoption(PEP_OPTION_LOG_LEVEL, PEP_LOGLEVEL_DEBUG);
 * if (rc != PEP_OK) {
 *    fprintf(stderr,"ERROR: %s\n",pep_strerror(rc));
 * }
 * ...
 * @endcode
 *
 * @{
 */
#define PEP_LOGLEVEL_NONE  -1 /**< No logging at all */
#define PEP_LOGLEVEL_ERROR  0 /**< Logs only ERROR messages */
#define PEP_LOGLEVEL_WARN   1 /**< Logs ERROR and WARN messages */
#define PEP_LOGLEVEL_INFO   2 /**< Logs ERROR, WARN and INFO messages */
#define PEP_LOGLEVEL_DEBUG  3 /**< Logs ERROR, WARN, INFO and DEBUG messages */

/**
 * Optional log handler function callback prototype.
 * You can implement your own callback function to @b replace the default log handler.
 * The PEP log message are @b not terminated with a "\n".
 *
 * @param level The log level to log
 * @param format The format string
 * @param args The variable arguments list
 * @return int 0 or an error code.
 *
 * Example to use your own logging callback function:
 * @code
 * ...
 * // functions to log messages to my log file
 * int my_vlog(int level, const char *fmt, va_list args) {
 *    if (level >= my_log_level) {
 *       vfprintf(my_logfile,fmt,args);
 *    }
 *    return 0;
 * }
 * int my_log_error(const char *fmt, ...) {
 *     va_list args;
 *     va_start(args,fmt);
 *     int rc= my_vlog(MY_LOG_ERROR,fmt,args);
 *     va_end(args);
 *     return rc;
 * }
 * ...
 * // set my log function as log handler callback function
 * rc= pep_setoption(PEP_OPTION_LOG_HANDLER, (pep_log_handler_callback)my_vlog);
 * if (rc != PEP_OK) {
 *    my_log_error("pep_setoption(PEP_OPTION_LOG_HANDLER,...) failed: %s\n",pep_strerror(rc));
 * }
 * ...
 * @endcode
 *
 * @see pep_setoption(pep_option_t option,...)
 * @see pep_log_handler_callback function prototype
 */
typedef int pep_log_handler_callback(int level, const char * format, va_list args);

/** @} */

/** @addtogroup PIP
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

/** @addtogroup ObligationHandler
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
 * The process(request,response) function is called after the PEP client
 * receives the PEP response back from PEP daemon.
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
 * ObligationHandler type.
 */
typedef struct pep_obligationhandler {
	char * id; /**< unique identifier for the OH */
	oh_init_func * init; /**< pointer to the OH init function */
	oh_process_func * process; /**< pointer to the OH process function */
	oh_destroy_func * destroy; /**< pointer to the OH destroy function */
} pep_obligationhandler_t;
/** @} */

/** @addtogroup PEPClient
 * PEP client used to send PEP request to the PEP daemon and receive PEP response back.
 * @{
 */

/**
 * PEP client configuration options.
 *
 * @see pep_setoption(option, ...) to set a configuration option.
 */
typedef enum pep_option {
	PEP_OPTION_LOG_LEVEL,  /**< Set log level (default {@link #PEP_LOGLEVEL_NONE}) */
	PEP_OPTION_LOG_STDERR,  /**< Set log engine file descriptor: @c stderr, @c stdout, @c NULL (default @c NULL) */
	PEP_OPTION_LOG_HANDLER,  /**< Set the optional log handler callback function pointer (default @c NULL) */
	PEP_OPTION_ENDPOINT_URL, /**< Set the @b mandatory PEP daemon URL. You can set failover URLs by setting this option many times. */
	PEP_OPTION_ENDPOINT_SSL_VALIDATION, /**< Enable SSL validation: 0 or 1 (default 1) */
	PEP_OPTION_ENDPOINT_SERVER_CERT, /**< PEP daemon server SSL certificate (PEM format): absolute filename */
	PEP_OPTION_ENDPOINT_CLIENT_CERT, /**< PEP client SSL certificate (PEM format) for client authN: absolute filename */
	PEP_OPTION_ENDPOINT_CLIENT_KEY, /**< PEP client SSL private key (PEM format) for client authN: absolute filename */
	PEP_OPTION_ENDPOINT_CLIENT_KEYPASSWORD, /**< PEP client SSL private key password for client authN: string */
	PEP_OPTION_ENDPOINT_TIMEOUT, /**< Timeout for the connection to endpoint URL in second (default 10s) */
	PEP_OPTION_ENABLE_PIPS, /**< Enable PIPs pre-processing: 0 or 1 (default 1) */
	PEP_OPTION_ENABLE_OBLIGATIONHANDLERS /**< Enable OHs post-processing: 0 or 1 (default 1) */
} pep_option_t;

/**
 * Initializes the PEP client.
 *
 * @return {@link #pep_error_t} PEP_OK on success or an error code.
 */
pep_error_t pep_initialize(void);

/**
 * Adds a PIP request pre-processor to the PEP client.
 *
 * @param pip pointer to the {@link #pep_pip_t} to add. See @ref PIP for more info.
 *
 * @return {@link #pep_error_t} PEP_OK on success or an error code.
 */
pep_error_t pep_addpip(pep_pip_t * pip);

/**
 * Adds an Obligation Handler post-processor to the
 * PEP client. See @ref ObligationHandler for more info.
 *
 * @param oh pointer to the {@link #pep_obligationhandler_t} to add.
 *
 * @return {@link #pep_error_t} PEP_OK on success or an error code.
 */
pep_error_t pep_addobligationhandler(pep_obligationhandler_t * oh);

/**
 * Sets a PEP client configuration option.
 *
 * @param option the PEP client option to set.
 * @param ... argument(s) for the PEP client option.
 *
 * @return {@link #pep_error_t} PEP_OK on success or an error code.
 * @see pep_option for available options.
 *
 * Option {@link #PEP_OPTION_ENDPOINT_URL} @c const @c char @c * argument:
 * @code
 *   // set the PEP daemon endpoint URL (with failover URLs)
 *   pep_setoption(PEP_OPTION_ENDPOINT_URL, (const char *)"https://pepd.switch.ch:8080/authz");
 *   pep_setoption(PEP_OPTION_ENDPOINT_URL, (const char *)"https://pepd-backup.switch.ch:8080/authz");
 *   pep_setoption(PEP_OPTION_ENDPOINT_URL, (const char *)"https://pepd.example.ch/authz");
 * @endcode
 * Option {@link #PEP_OPTION_LOG_LEVEL} @c int argument:
 * @code
 *   // set logging level to WARN (only ERROR + WARN messages shown)
 *   pep_setoption(PEP_OPTION_LOG_LEVEL, (int)PEP_LOGLEVEL_WARN);
 * @endcode
 * Option {@link #PEP_OPTION_LOG_STDERR} @c FILE @c * argument:
 * @code
 *   // set logging output to stderr
 *   pep_setoption(PEP_OPTION_LOG_STDERR, (FILE *)stdout);
 * @endcode
 * Option {@link #PEP_OPTION_LOG_HANDLER} {@link #pep_log_handler_callback} @c * argument:
 * @code
 *   // override default logging handler with own logging callback function
 *   pep_setoption(PEP_OPTION_LOG_HANDLER, (pep_log_handler_callback *)my_logging_callback);
 * @endcode
 * Option {@link #PEP_OPTION_ENABLE_PIPS} @c int (@a FALSE or @a TRUE) argument:
 * @code
 *   // disable PIPs processing
 *   pep_setoption(PEP_OPTION_ENABLE_PIPS, (int)0);
 * @endcode
 * Option {@link #PEP_OPTION_ENABLE_OBLIGATIONHANDLERS} @c int (@a FALSE or @a TRUE) argument:
 * @code
 * 	 // already enabled by default, only for example purpose
 *   pep_setoption(PEP_OPTION_ENABLE_OBLIGATIONHANDLERS, (int)1);
 * @endcode
 *
 */
pep_error_t pep_setoption(pep_option_t option, ... );

/**
 * Sends the XACML request to the PEP daemon and returns the XACML response.
 *
 * If some PIPs are present, they will be applied to the XACML request before submitting
 * it to the PEPd.
 * If some ObligationHandlers are present, they will be applied to the XACML response after
 * the response is received from the PEPd.
 *
 * After the call, the @c request parameter is the @b effective XACML request, as processed by the PEPd.
 *
 * @param request address of the pointer to the {@link #xacml_request_t} to send.
 * @param response address of pointer to the {@link #xacml_response_t} received.
 *
 * @return {@link #pep_error_t} PEP_OK on success or an error code.
 */
pep_error_t pep_authorize(xacml_request_t ** request, xacml_response_t ** response);

/**
 * Cleanups and destroys the PEP client.
 *
 * @return {@link #pep_error_t} PEP_OK on success or an error code.
 */
pep_error_t pep_destroy(void);

/** @example test_pep.c
 * This is an example how to use the PEP client.
 */

/** @} */

#ifdef  __cplusplus
}
#endif

#endif
