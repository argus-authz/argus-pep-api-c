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
 * Argus PEP client API
 *
 * $Id$
 * @author Valery Tschopp <valery.tschopp@switch.ch>
 * @version 2.0
 */
#ifndef _PEP_H_
#define _PEP_H_

#ifdef  __cplusplus
extern "C" {
#endif

/** @mainpage Argus Authorization Service PEP client API for C
 *
 * <h2>Description</h2>
 * The PEP client API for C is used to authorize requests with the Argus PEP Server, and
 * receive authorization decisions. 
 * <p>
 * <h2>Functionalities</h2>
 * 
 * 
 * <p>
 * <h2>Multi-threading</h2>
 * Version 2.X of the PEP client C API is multi-thread friendly. However, you are not allowed 
 * to share a PEP handle among multiple threads:
 * <ul> 
 * <li>Each thread must create its own PEP handle 
 * <li>If your threads are object (OO programming, ...), it is recommended you to 
 * create (pep_initialize) the PEP handle in the constructor, and release it (pep_destroy) 
 * in the destructor. 
 * </ul> 
 */

/** @defgroup PEPClient PEP client API */
/** @defgroup Logging Log Level and Output */

#include <stdarg.h> /* va_list */
#include "xacml.h"
#include "profiles.h"
#include "pip.h"
#include "oh.h"
#include "error.h"



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
 * rc= pep_setoption(pep,PEP_OPTION_LOG_STDERR, my_logfile);
 * if (rc != PEP_OK) {
 *    fprintf(stderr,"ERROR: %s\n",pep_strerror(rc));
 * }
 * // set log level to DEBUG
 * rc= pep_setoption(pep,PEP_OPTION_LOG_LEVEL, PEP_LOGLEVEL_DEBUG);
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
 * rc= pep_setoption(pep,PEP_OPTION_LOG_HANDLER, (pep_log_handler_callback)my_vlog);
 * if (rc != PEP_OK) {
 *    my_log_error("pep_setoption(PEP_OPTION_LOG_HANDLER,...) failed: %s\n",pep_strerror(rc));
 * }
 * ...
 * @endcode
 *
 * @see pep_setoption(pep,pep_option_t option,...)
 * @see pep_log_handler_callback function prototype
 */
typedef int pep_log_handler_callback(int level, const char * format, va_list args);

/** @} */


/** @addtogroup PEPClient
 * PEP client used to send authorization request to the PEP daemon and receive authorization response with decision back.
 * @{
 */


/**
 * PEP client @b handle
 */
typedef struct pep_handle PEP;

/**
 * PEP client configuration options.
 *
 * @see pep_setoption(pep,option, ...) to set a configuration option.
 */
typedef enum pep_option {
    PEP_OPTION_LOG_LEVEL,  /**< Set log level (default {@link #PEP_LOGLEVEL_NONE}) */
    PEP_OPTION_LOG_STDERR,  /**< Set log engine file descriptor: @c stderr, @c stdout, @c NULL (default @c NULL) */
    PEP_OPTION_LOG_HANDLER,  /**< Set the optional log handler callback function pointer (default @c NULL) */
    PEP_OPTION_ENDPOINT_URL, /**< Set the @b mandatory PEP daemon endpoint URL. */
    PEP_OPTION_ENDPOINT_SSL_VALIDATION, /**< Enable SSL validation: 0 or 1 (default 1) */
    PEP_OPTION_ENDPOINT_SERVER_CERT, /**< PEP daemon server SSL certificate (PEM format): absolute filename */
    PEP_OPTION_ENDPOINT_SERVER_CAPATH, /**< Directory holding CA certificates (hashed filenames in PEM format) to verify the PEP daemon: absolute directory name */
    PEP_OPTION_ENDPOINT_CLIENT_CERT, /**< PEP client SSL certificate (PEM format) for client authN: absolute filename */
    PEP_OPTION_ENDPOINT_CLIENT_KEY, /**< PEP client SSL private key (PEM format) for client authN: absolute filename */
    PEP_OPTION_ENDPOINT_CLIENT_KEYPASSWORD, /**< PEP client SSL private key password for client authN: string */
    PEP_OPTION_ENDPOINT_TIMEOUT, /**< Timeout for the connection to endpoint URL in second (default 30s) */
    PEP_OPTION_ENABLE_PIPS, /**< Enable PIPs pre-processing: 0 or 1 (default 1) */
    PEP_OPTION_ENABLE_OBLIGATIONHANDLERS, /**< Enable OHs post-processing: 0 or 1 (default 1) */
    PEP_OPTION_ENDPOINT_SSL_CIPHER_LIST /**< PEP client list of ciphers to use for the SSL connection: string */
} pep_option_t;

/**
 * Returns a human readable string with the version number of the PEP client API and some of its important components (like libcurl version).
 * @return a null terminated string. e.g. "libargus-pep-api/2.0.0 ..."
 */
const char * pep_version(void);

/**
 * Creates and initializes a new PEP client @b handle. This function must be the first function 
 * to call, and it returns a PEP client handle that you must use as input to other PEP client 
 * functions.
 *
 * @return the PEP client @b handle or null on error.
 */
PEP * pep_initialize(void);

/**
 * Adds a PIP request pre-processor to the PEP client. The PIP init() function
 * is called in this method.
 *
 * See @ref PIP and @ref ProfilesAdapters for more info.
 *
 * @param pep pointer to the @b handle of the PEP client.
 * @param pip pointer to the {@link #pep_pip_t} to add.
 *
 * @return {@link #pep_error_t} PEP_OK on success or an error code.
 */
pep_error_t pep_addpip(PEP * pep, const pep_pip_t * pip);

/**
 * Adds an Obligation Handler post-processor to the
 * PEP client. The OH init() function is be called in this method.
 *
 * See @ref ObligationHandler and @ref ProfilesAdapters for more info.
 *
 * @param pep pointer to the @b handle of the PEP client.
 * @param oh pointer to the {@link #pep_obligationhandler_t} to add.
 *
 * @return {@link #pep_error_t} PEP_OK on success or an error code.
 */
pep_error_t pep_addobligationhandler(PEP * pep, const pep_obligationhandler_t * oh);

/**
 * Sets a PEP client configuration option.
 *
 * @param pep pointer to the @b handle of the PEP client.
 * @param option the PEP client option to set.
 * @param ... argument(s) for the PEP client option.
 *
 * @return {@link #pep_error_t} PEP_OK on success or an error code.
 * @see pep_option for available options.
 *
 * Option {@link #PEP_OPTION_ENDPOINT_URL} @c const @c char @c * argument:
 * @code
 *   // set the PEP daemon endpoint URL
 *   pep_setoption(pep,PEP_OPTION_ENDPOINT_URL, (const char *)"https://pepd.switch.ch:8154/authz");
 * @endcode
 * Option {@link #PEP_OPTION_ENDPOINT_SERVER_CAPATH} @c const @c char * argument:
 * @code
 *   // set the PEP daemon server CA directory for SSL/TLS validation
 *   pep_setoption(pep,PEP_OPTION_ENDPOINT_SERVER_CAPATH, (const char *)"/etc/grid-security/certificates");
 * @endcode 
 * Option {@link #PEP_OPTION_ENDPOINT_CLIENT_CERT} @c const @c char * argument:
 * @code
 *   // set the PEP client certificate for SSL/TLS client authentication
 *   pep_setoption(pep,PEP_OPTION_ENDPOINT_CLIENT_CERT, (const char *)"/tmp/x509up_u5000");
 * @endcode 
 * Option {@link #PEP_OPTION_ENDPOINT_CLIENT_KEY} @c const @c char * argument:
 * @code
 *   // set the PEP client private key for SSL/TLS client authentication
 *   pep_setoption(pep,PEP_OPTION_ENDPOINT_CLIENT_KEY, (const char *)"/tmp/x509up_u5000");
 * @endcode 
 * Option {@link #PEP_OPTION_LOG_LEVEL} @c int argument:
 * @code
 *   // set logging level to WARN (only ERROR + WARN messages shown)
 *   pep_setoption(pep,PEP_OPTION_LOG_LEVEL, (int)PEP_LOGLEVEL_WARN);
 * @endcode
 * Option {@link #PEP_OPTION_LOG_STDERR} @c FILE @c * argument:
 * @code
 *   // set logging output to stderr
 *   pep_setoption(pep,PEP_OPTION_LOG_STDERR, (FILE *)stdout);
 * @endcode
 * Option {@link #PEP_OPTION_LOG_HANDLER} {@link #pep_log_handler_callback} @c * argument:
 * @code
 *   // override default logging handler with own logging callback function
 *   pep_setoption(pep,PEP_OPTION_LOG_HANDLER, (pep_log_handler_callback *)my_logging_callback);
 * @endcode
 * Option {@link #PEP_OPTION_ENABLE_PIPS} @c int (@a FALSE or @a TRUE) argument:
 * @code
 *   // disable PIPs processing
 *   pep_setoption(pep,PEP_OPTION_ENABLE_PIPS, (int)0);
 * @endcode
 * Option {@link #PEP_OPTION_ENABLE_OBLIGATIONHANDLERS} @c int (@a FALSE or @a TRUE) argument:
 * @code
 *   // already enabled by default, only for example purpose
 *   pep_setoption(pep,PEP_OPTION_ENABLE_OBLIGATIONHANDLERS, (int)1);
 * @endcode
 *
 */
pep_error_t pep_setoption(PEP * pep, pep_option_t option, ... );

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
 * @param pep pointer to the @b handle of the PEP client.
 * @param request address of the pointer to the {@link #xacml_request_t} to send.
 * @param response address of pointer to the {@link #xacml_response_t} received.
 *
 * @return {@link #pep_error_t} PEP_OK on success or an error code.
 */
pep_error_t pep_authorize(PEP * pep, xacml_request_t ** request, xacml_response_t ** response);

/**
 * Cleanups and destroys the PEP client. Any uses of the @b handle after this function has been called are illegal. 
 *
 * @param pep pointer to the @b handle of the PEP client.
 *
 * @return none
 */
void pep_destroy(PEP * pep);

/** @example pep_client_example.c
 * This is an example how to use the PEP client.
 */

/** @} */

#ifdef  __cplusplus
}
#endif

#endif
