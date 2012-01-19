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

/* $Id$ */

#include <stdarg.h>  /* va_list, va_arg, ... */
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>

/* from ../util */
#include "linkedlist.h"
#include "buffer.h"
#include "base64.h"
#include "log.h"

#include "pep.h"
#include "io.h"
#include "error.h"


#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define PACKAGE_NAME "argus-pep-api"
#define PACKAGE_VERSION "2.1.0"
#endif

/** internal client counter */
static int n_pep_clients= 0;

/** Default constants */
static const long   DEFAULT_CURL_TIMEOUT= 30L;
static const int    DEFAULT_CURL_SSL_VALIDATION= TRUE;
static const int    DEFAULT_LOG_LEVEL= PEP_LOGLEVEL_NONE;
static const FILE * DEFAULT_LOG_FILE= NULL;
static const int    DEFAULT_PIPS_ENABLED= TRUE;
static const int    DEFAULT_OHS_ENABLED= TRUE;
/* default SSL cipher without ECDH: OpenSSL 1.0 bug */
/*
static const char * DEFAULT_SSL_CIPHER_LIST= "DEFAULT:-ECDH";
*/

/** internal functions prototypes */
static void init_pep_defaults(PEP * pep);
static void init_curl_defaults(PEP * pep);
/* static void init_log_defaults(const PEP * pep); */
static int set_curl_endpoint_url(const PEP * pep);
static int set_curl_connection_timeout(const PEP * pep);
static int set_curl_ssl_validation(const PEP * pep);
static int set_curl_ssl_cipher_list(const PEP * pep);
static int set_curl_server_cert(const PEP * pep);
static int set_curl_server_capath(const PEP * pep);
static int set_curl_client_cert(const PEP * pep);
static int set_curl_client_key(const PEP * pep);
static int set_curl_client_keypassword(const PEP * pep);
static int set_curl_verbose(const PEP * pep);
static int set_curl_stderr(const PEP * pep);
static int set_curl_nosignal(const PEP * pep);
static int set_curl_http_headers(PEP * pep);

/** 
* ADT for PEP client handle.
*
* CURL string parameters must be kept as with libcul <= 7.17.0, strings were not copied. 
* Instead the user was forced keep them available until libcurl no longer needed them. 
*/
struct pep_handle {
    int id;
    CURL * curl;
    struct curl_slist * curl_http_headers;
    pep_linkedlist_t * pips;
    pep_linkedlist_t * ohs;
    char * option_endpoint_url; /* current url */
    pep_linkedlist_t * option_endpoint_urls;
    int option_loglevel;
    FILE * option_logout;
    long option_timeout; 
    char * option_server_cert;
    char * option_server_capath;
    char * option_client_cert;
    char * option_client_key;
    char * option_client_keypassword;
    int option_ssl_validation;
    char * option_ssl_cipher_list;
    int option_pips_enabled;
    int option_ohs_enabled;
    // temporary buffers for pep_authorize
    pep_buffer_t * output;
    pep_buffer_t * b64output;
    pep_buffer_t * input;
    pep_buffer_t * b64input;
};

/* MOVED to ../libargus_pep.c
const char * pep_version(void) {
    if (!VERSION_BUFFER_initialized) {
        snprintf(VERSION_BUFFER,VERSION_BUFFER_SIZE,"%s/%s (%s)",PACKAGE_NAME,PACKAGE_VERSION,curl_version());
        VERSION_BUFFER_initialized= 1;
    }
    return VERSION_BUFFER;
}
*/

/* create and init */
PEP * pep_initialize(void) {

    /* allocate struct */
    PEP * pep= calloc(1,sizeof(struct pep_handle));
    if (pep == NULL) {
        pep_log_error("pep_initialize: can't allocate struct pep_client: %d", sizeof(struct pep_handle));
        return NULL;
    }
    /* set default PEP values */
    init_pep_defaults(pep);
    
    /* create and init curl handle */
    pep->curl= curl_easy_init();
    if (pep->curl == NULL) {
        pep_log_error("pep_initialize: can't create CURL session handle.");
        free(pep);
        return NULL;
    }
    /* set default CURL options */
    init_curl_defaults(pep);
        
    /* create all required lists */
    pep->pips= pep_llist_create();
    if (pep->pips == NULL) {
        pep_log_error("pep_initialize: PIPs list allocation failed.");
        curl_easy_cleanup(pep->curl);
        free(pep);
        return NULL;
    }
    pep->ohs= pep_llist_create();
    if (pep->ohs == NULL) {
        pep_log_error("pep_initialize: OHs list allocation failed.");
        curl_easy_cleanup(pep->curl);
        pep_llist_delete(pep->pips);
        free(pep);
        return NULL;
    }
    
    pep->option_endpoint_urls= pep_llist_create();
    if (pep->option_endpoint_urls == NULL) {
        pep_log_error("pep_initialize: endpoint URLs list allocation failed.");
        curl_easy_cleanup(pep->curl);
        pep_llist_delete(pep->pips);
        pep_llist_delete(pep->ohs);
        free(pep);
        return NULL;
    }
    
    return pep;
}

pep_error_t pep_addpip(PEP * pep, const pep_pip_t * pip) {
    int pip_rc = -1;
    if (pep == NULL) {
        pep_log_error("pep_addpip: NULL pep handle");
        return PEP_ERR_NULL_POINTER;
    }
    if (pip == NULL) {
        pep_log_error("pep_addpip: NULL pip pointer");
        return PEP_ERR_NULL_POINTER;
    }
    if ((pip_rc= pip->init()) != 0) {
        pep_log_error("pep_addpip: PIP[%s] init() failed: %d.",pip->id, pip_rc);
        return PEP_ERR_PIP_INIT;
    }
    if (pep_llist_add(pep->pips,(pep_pip_t *)pip) != LLIST_OK) {
        pep_log_error("pep_addpip: failed to add initialized PIP[%s] into PEP#%d list.",pip->id,pep->id);
        return PEP_ERR_LLIST;
    }
    return PEP_OK;
}

pep_error_t pep_addobligationhandler(PEP * pep, const pep_obligationhandler_t * oh) {
    int oh_rc= -1;
    if (pep == NULL) {
        pep_log_error("pep_addobligationhandler: NULL pep handle");
        return PEP_ERR_NULL_POINTER;
    }
    if (oh == NULL) {
        pep_log_error("pep_addobligationhandler: NULL oh pointer");
        return PEP_ERR_NULL_POINTER;
    }
    if ((oh_rc= oh->init()) != 0) {
        pep_log_error("pep_addobligationhandler: OH[%s] init() failed: %d",oh->id, oh_rc);
        return PEP_ERR_OH_INIT;
    }
    if (pep_llist_add(pep->ohs,(pep_obligationhandler_t *)oh) != LLIST_OK) {
        pep_log_error("pep_addobligationhandler: failed to add initialized OH[%s] into PEP#%d list.",oh->id,pep->id);
        return PEP_ERR_LLIST;
    }
    return PEP_OK;
}

pep_error_t pep_setoption(PEP * pep, pep_option_t option, ... ) {
    pep_error_t rc= PEP_OK;
    va_list args;
    char * str= NULL;
    size_t str_l= 0;
    int value= -1;
    FILE * file= NULL;
    pep_log_handler_callback * log_handler= NULL;
    if (pep == NULL) {
        pep_log_error("pep_setoption: NULL pep handle");
        return PEP_ERR_NULL_POINTER;
    }
    va_start(args,option);
    switch (option) {
        case PEP_OPTION_ENDPOINT_URL:
            str= va_arg(args,char *);
            if (str == NULL) {
                pep_log_error("pep_setoption: PEP#%d PEP_OPTION_ENDPOINT_URL argument is NULL.", pep->id);
                rc= PEP_ERR_OPTION_INVALID;
                break;
            }
            /* copy url */
            if (pep->option_endpoint_url != NULL) { 
                pep_log_debug("pep_setoption: PEP#%d option_endpoint_url already set to '%s', freeing...",pep->id,pep->option_endpoint_url);
                free(pep->option_endpoint_url); 
            }
            str_l= strlen(str);
            pep->option_endpoint_url= calloc(str_l + 1, sizeof(char));
            if (pep->option_endpoint_url == NULL) {
                pep_log_error("pep_setoption: PEP#%d can't allocate option_endpoint_url: %s.",pep->id,str);
                rc= PEP_ERR_MEMORY;
                break;
            }
            strncpy(pep->option_endpoint_url,str,str_l);
            pep_log_debug("pep_setoption: PEP#%d PEP_OPTION_ENDPOINT_URL: %s",pep->id,pep->option_endpoint_url);
            set_curl_endpoint_url(pep);
            break;
        case PEP_OPTION_ENDPOINT_TIMEOUT:
            value= va_arg(args,int);
            if (value > 0) {
                pep->option_timeout= (long)value;
            }
            pep_log_debug("pep_setoption: PEP#%d PEP_OPTION_ENDPOINT_TIMEOUT: %d",pep->id,(int)(pep->option_timeout));
            set_curl_connection_timeout(pep);
            break;
        case PEP_OPTION_ENDPOINT_SSL_VALIDATION:
            value= va_arg(args,int);
            if (value == 1) {
                pep->option_ssl_validation= TRUE;
            }
            else {
                pep->option_ssl_validation= FALSE;
            }
            pep_log_debug("pep_setoption: PEP#%d PEP_OPTION_ENDPOINT_SSL_VALIDATION: %s",pep->id,(pep->option_ssl_validation == TRUE) ? "TRUE" : "FALSE");
            set_curl_ssl_validation(pep);
            break;
        case PEP_OPTION_ENDPOINT_SSL_CIPHER_LIST:
            str= va_arg(args,char *);
            if (str == NULL) {
                pep_log_error("pep_setoption: PEP#%d PEP_OPTION_ENDPOINT_SSL_CIPHER_LIST argument is NULL.",pep->id);
                rc= PEP_ERR_OPTION_INVALID;
                break;
            }
            /* copy cipher list string */
            if (pep->option_ssl_cipher_list != NULL) { 
                pep_log_debug("pep_setoption: PEP#%d option_ssl_cipher_list already set to '%s', freeing...",pep->id,pep->option_ssl_cipher_list);
                free(pep->option_ssl_cipher_list); 
            }
            str_l= strlen(str);
            pep->option_ssl_cipher_list= calloc(str_l + 1, sizeof(char));
            if (pep->option_ssl_cipher_list == NULL) {
                pep_log_error("pep_setoption: PEP#%d can't allocate option_ssl_cipher_list: %s.",pep->id,str);
                rc= PEP_ERR_MEMORY;
                break;
            }
            strncpy(pep->option_ssl_cipher_list,str,str_l);
            pep_log_debug("pep_setoption: PEP#%d PEP_OPTION_ENDPOINT_SSL_CIPHER_LIST: %s",pep->id,pep->option_ssl_cipher_list);
            set_curl_ssl_cipher_list(pep);
            break;
            
        case PEP_OPTION_ENDPOINT_SERVER_CERT:
            str= va_arg(args,char *);
            if (str == NULL) {
                pep_log_error("pep_setoption: PEP#%d PEP_OPTION_ENDPOINT_SERVER_CERT argument is NULL.", pep->id);
                rc= PEP_ERR_OPTION_INVALID;
                break;
            }
            /* copy client_cert */
            if (pep->option_server_cert != NULL) { 
                pep_log_debug("pep_setoption: PEP#%d option_server_cert already set to '%s', freeing...",pep->id,pep->option_server_cert);
                free(pep->option_server_cert);
            }
            str_l= strlen(str);
            pep->option_server_cert= calloc(str_l + 1, sizeof(char));
            if (pep->option_server_cert == NULL) {
                pep_log_error("pep_setoption: PEP#%d can't allocate option_server_cert: %s.", pep->id,str);
                rc= PEP_ERR_MEMORY;
                break;
            }
            strncpy(pep->option_server_cert,str,str_l);
            pep_log_debug("pep_setoption: PEP#%d PEP_OPTION_ENDPOINT_SERVER_CERT: %s",pep->id,pep->option_server_cert);
            set_curl_server_cert(pep);
            break;
        case PEP_OPTION_ENDPOINT_SERVER_CAPATH:
            str= va_arg(args,char *);
            if (str == NULL) {
                pep_log_error("pep_setoption: PEP#%d PEP_OPTION_ENDPOINT_SERVER_CAPATH argument is NULL.",pep->id);
                rc= PEP_ERR_OPTION_INVALID;
                break;
            }
            /* copy client_cert */
            if (pep->option_server_capath != NULL) { 
                pep_log_debug("pep_setoption: PEP#%d option_server_capath already set to '%s', freeing...",pep->id,pep->option_server_capath);
                free(pep->option_server_capath); 
            }
            str_l= strlen(str);
            pep->option_server_capath= calloc(str_l + 1, sizeof(char));
            if (pep->option_server_capath == NULL) {
                pep_log_error("pep_setoption: PEP#%d can't allocate option_server_capath: %s.",pep->id, str);
                rc= PEP_ERR_MEMORY;
                break;
            }
            strncpy(pep->option_server_capath,str,str_l);
            pep_log_debug("pep_setoption: PEP#%d PEP_OPTION_ENDPOINT_SERVER_CAPATH: %s",pep->id,pep->option_server_capath);
            set_curl_server_capath(pep);
            break;
        case PEP_OPTION_ENDPOINT_CLIENT_CERT:
            str= va_arg(args,char *);
            if (str == NULL) {
                pep_log_error("pep_setoption: PEP#%d PEP_OPTION_ENDPOINT_CLIENT_CERT argument is NULL.",pep->id);
                rc= PEP_ERR_OPTION_INVALID;
                break;
            }
            /* copy client_cert */
            if (pep->option_client_cert != NULL) { 
                pep_log_debug("pep_setoption: PEP#%d option_client_cert already set to '%s', freeing...",pep->id,pep->option_client_cert);
                free(pep->option_client_cert); 
            }
            str_l= strlen(str);
            pep->option_client_cert= calloc(str_l + 1, sizeof(char));
            if (pep->option_client_cert == NULL) {
                pep_log_error("pep_setoption: PEP#%d can't allocate option_client_cert: %s.", pep->id,str);
                rc= PEP_ERR_MEMORY;
                break;
            }
            strncpy(pep->option_client_cert,str,str_l);
            pep_log_debug("pep_setoption: PEP#%d PEP_OPTION_ENDPOINT_CLIENT_CERT: %s",pep->id,pep->option_client_cert);
            set_curl_client_cert(pep);
            break;
        case PEP_OPTION_ENDPOINT_CLIENT_KEY:
            str= va_arg(args,char *);
            if (str == NULL) {
                pep_log_error("pep_setoption: PEP#%d PEP_OPTION_ENDPOINT_CLIENT_KEY argument is NULL.",pep->id);
                rc= PEP_ERR_OPTION_INVALID;
                break;
            }
            /* copy client_key */
            if (pep->option_client_key != NULL) { 
                pep_log_debug("pep_setoption: PEP#%d option_client_key already set to '%s', freeing...",pep->id,pep->option_client_key);
                free(pep->option_client_key); 
            }
            str_l= strlen(str);
            pep->option_client_key= calloc(str_l + 1, sizeof(char));
            if (pep->option_client_key == NULL) {
                pep_log_error("pep_setoption: PEP#%d can't allocate option_client_key: %s.",pep->id,str);
                rc= PEP_ERR_MEMORY;
                break;
            }
            strncpy(pep->option_client_key,str,str_l);
            pep_log_debug("pep_setoption: PEP#%d PEP_OPTION_ENDPOINT_CLIENT_KEY: %s",pep->id,pep->option_client_key);
            set_curl_client_key(pep);
            break;
        case PEP_OPTION_ENDPOINT_CLIENT_KEYPASSWORD:
            str= va_arg(args,char *);
            if (str == NULL) {
                pep_log_error("pep_setoption: PEP#%d PEP_OPTION_ENDPOINT_CLIENT_KEYPASSWORD argument is NULL.",pep->id);
                rc= PEP_ERR_OPTION_INVALID;
                break;
            }
            /* copy client_key */
            if (pep->option_client_keypassword != NULL) { 
                pep_log_debug("pep_setoption: PEP#%d option_client_keypassword already set to '%s', freeing...",pep->id,pep->option_client_keypassword);
                memset(pep->option_client_keypassword,'\0',strlen(pep->option_client_keypassword));
                free(pep->option_client_keypassword); 
            }
            str_l= strlen(str);
            pep->option_client_keypassword= calloc(str_l + 1, sizeof(char));
            if (pep->option_client_keypassword == NULL) {
                pep_log_error("pep_setoption: PEP#%d can't allocate option_client_keypassword: %s.",pep->id, str);
                rc= PEP_ERR_MEMORY;
                break;
            }
            strncpy(pep->option_client_keypassword,str,str_l);
            pep_log_debug("pep_setoption: PEP#%d PEP_OPTION_ENDPOINT_CLIENT_KEYPASSWORD: %d char long",pep->id,(int)strlen(pep->option_client_keypassword));
            set_curl_client_keypassword(pep);
            break;
        case PEP_OPTION_ENABLE_PIPS:
            value= va_arg(args,int);
            if (value == 1) {
                pep->option_pips_enabled= TRUE;
            }
            else {
                pep->option_pips_enabled= FALSE;
            }
            pep_log_debug("pep_setoption: PEP#%d PEP_OPTION_ENABLE_PIPS: %s",pep->id,(pep->option_pips_enabled == TRUE) ? "TRUE" : "FALSE");
            break;
        case PEP_OPTION_ENABLE_OBLIGATIONHANDLERS:
            value= va_arg(args,int);
            if (value == 1) {
                pep->option_ohs_enabled= TRUE;
            }
            else {
                pep->option_ohs_enabled= FALSE;
            }
            pep_log_debug("pep_setoption: PEP#%d PEP_OPTION_ENABLE_OBLIGATIONHANDLERS: %s",pep->id,(pep->option_ohs_enabled == TRUE) ? "TRUE" : "FALSE");
            break;
        case PEP_OPTION_LOG_LEVEL:
            value= va_arg(args,int);
            if (PEP_LOGLEVEL_NONE <= value && value <= PEP_LOGLEVEL_DEBUG) {
                pep->option_loglevel= value;
                pep_log_setlevel(pep->option_loglevel);
            }
            pep_log_debug("pep_setoption: PEP#%d PEP_OPTION_LOG_LEVEL: %d",pep->id,pep->option_loglevel);
            set_curl_verbose(pep);
            break;
        case PEP_OPTION_LOG_STDERR:
            file= va_arg(args,FILE *);
            pep->option_logout= file;
            pep_log_setout(pep->option_logout);
            pep_log_debug("pep_setoption: PEP#%d PEP_OPTION_LOG_STDERR: %p",pep->id,pep->option_logout);
            set_curl_stderr(pep);
            break;
        case PEP_OPTION_LOG_HANDLER:
            log_handler= va_arg(args,pep_log_handler_callback *);
            pep_log_sethandler(log_handler);
            pep_log_debug("pep_setoption: PEP_OPTION_LOG_HANDLER: %p",log_handler);
            break;
        default:
            pep_log_error("pep_setoption: PEP#%d invalid option: %d",pep->id,option);
            rc= PEP_ERR_OPTION_INVALID;
            break;
    }
    va_end(args);
    return rc;
}


pep_error_t pep_authorize(PEP * pep, xacml_request_t ** request, xacml_response_t ** response) {
    int i= 0;
    int pip_rc, oh_rc;
    //pep_buffer_t * output, * b64output, * b64input, * input;
    size_t output_l, b64output_l;
    pep_error_t marshal_rc, unmarshal_rc;
    CURLcode curl_rc;
    long http_code= 0;
    xacml_request_t * effective_request;
    if (pep == NULL) {
        pep_log_error("pep_authorize: NULL pep handle");
        return PEP_ERR_NULL_POINTER;
    }
    if (pep->option_endpoint_url == NULL) {
        pep_log_error("pep_authorize: NULL mandatory option PEP_OPTION_ENDPOINT_URL");
        return PEP_ERR_NULL_POINTER;
    }
    if (request == NULL || *request == NULL) {
        pep_log_error("pep_authorize: PEP#%d NULL request pointer",pep->id);
        return PEP_ERR_NULL_POINTER;
    }
    
    /* apply pips if enabled and any */
    if (pep->option_pips_enabled && pep_llist_length(pep->pips) > 0) {
        size_t pips_l= pep_llist_length(pep->pips);
        pep_log_info("pep_authorize: PEP#%d %d PIPs available, processing...",pep->id, (int)pips_l);
        for (i= 0; i<pips_l; i++) {
            pep_pip_t * pip= pep_llist_get(pep->pips,i);
            if (pip != NULL) {
                pep_log_debug("pep_authorize: PEP#%d calling pip[%s]->process(request)...",pep->id,pip->id);
                pip_rc= pip->process(request);
                if (pip_rc != 0) {
                    pep_log_error("pep_authorize: PIP[%s] process(request) failed: %d", pip->id, pip_rc);
                    return PEP_ERR_PIP_PROCESS;
                }
            }
        }
    }

    /* marshal the authorization request into output buffer */
    pep->output= pep_buffer_create(512);
    if (pep->output == NULL) {
        pep_log_error("pep_authorize: PEP#%d can't create output buffer (512 bytes).",pep->id);
        return PEP_ERR_MEMORY;
    }
    marshal_rc= xacml_request_marshalling(*request,pep->output);
    if ( marshal_rc != PEP_OK ) {
        pep_log_error("pep_authorize: PEP#%d can't marshal XACML request: %s.",pep->id,pep_strerror(marshal_rc));
        pep_buffer_delete(pep->output);
        return marshal_rc;
    }

    /* base64 encode the output buffer */
    output_l= pep_buffer_length(pep->output);
    pep->b64output= pep_buffer_create( output_l );
    if (pep->b64output == NULL) {
        pep_log_error("pep_authorize: PEP#%d can't create base64 output buffer (%d bytes).",pep->id,(int)output_l);
        pep_buffer_delete(pep->output);
        return PEP_ERR_MEMORY;
    }
    
    pep_log_debug("pep_authorize: PEP#%d: encoding base64 output...",pep->id);
    pep_base64_encode_buffer_l(pep->output,pep->b64output,BASE64_DEFAULT_LINE_SIZE);

    /* output buffer not needed anymore. */
    pep_buffer_delete(pep->output);

    /* configure curl handler to POST the base64 encoded marshalled PEP request buffer */
    curl_rc= curl_easy_setopt(pep->curl, CURLOPT_POST, 1L);
    if (curl_rc != CURLE_OK) {
        pep_log_error("pep_authorize: PEP#%d curl_easy_setopt(curl,CURLOPT_POST,1) failed: %s.",pep->id,curl_easy_strerror(curl_rc));
        pep_buffer_delete(pep->b64output);
        return PEP_ERR_CURL + curl_rc;
    }
    b64output_l= pep_buffer_length(pep->b64output);
    curl_rc= curl_easy_setopt(pep->curl, CURLOPT_POSTFIELDSIZE, (long)b64output_l);
    if (curl_rc != CURLE_OK) {
        pep_log_error("pep_authorize: PEP#%d curl_easy_setopt(curl,CURLOPT_POSTFIELDSIZE,%d) failed: %s.",pep->id,(int)b64output_l,curl_easy_strerror(curl_rc));
        pep_buffer_delete(pep->b64output);
        return PEP_ERR_CURL + curl_rc;
    }

    curl_rc= curl_easy_setopt(pep->curl, CURLOPT_READDATA, pep->b64output);
    if (curl_rc != CURLE_OK) {
        pep_log_error("pep_authorize: PEP#%d curl_easy_setopt(curl,CURLOPT_READDATA,b64output) failed: %s.",pep->id,curl_easy_strerror(curl_rc));
        pep_buffer_delete(pep->b64output);
        return PEP_ERR_CURL + curl_rc;
    }

    curl_rc= curl_easy_setopt(pep->curl, CURLOPT_READFUNCTION, pep_buffer_read);
    if (curl_rc != CURLE_OK) {
        pep_log_error("pep_authorize: PEP#%d curl_easy_setopt(curl,CURLOPT_READFUNCTION,buffer_read) failed: %s.",pep->id,curl_easy_strerror(curl_rc));
        pep_buffer_delete(pep->b64output);
        return PEP_ERR_CURL + curl_rc;
    }


    /* configure curl handler to read the base64 encoded HTTP response */
    pep->b64input= pep_buffer_create(1024);
    if (pep->b64input == NULL) {
        pep_log_error("pep_authorize: PEP#%d can't create base64 input buffer.",pep->id);
        pep_buffer_delete(pep->b64output);
        return PEP_ERR_MEMORY;
    }

    curl_rc= curl_easy_setopt(pep->curl, CURLOPT_WRITEDATA, pep->b64input);
    if (curl_rc != CURLE_OK) {
        pep_log_error("pep_authorize: PEP#%d curl_easy_setopt(curl,CURLOPT_WRITEDATA,b64input) failed: %s.",pep->id,curl_easy_strerror(curl_rc));
        pep_buffer_delete(pep->b64output);
        pep_buffer_delete(pep->b64input);
        return PEP_ERR_CURL + curl_rc;
    }
    curl_rc= curl_easy_setopt(pep->curl, CURLOPT_WRITEFUNCTION, pep_buffer_write);
    if (curl_rc != CURLE_OK) {
        pep_log_error("pep_authorize: PEP#%d curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,buffer_write) failed: %s.",pep->id,curl_easy_strerror(curl_rc));
        pep_buffer_delete(pep->b64output);
        pep_buffer_delete(pep->b64input);
        return PEP_ERR_CURL + curl_rc;
    }


    /* send the request */
    pep_log_info("pep_authorize: PEP#%d sending XACML request to: %s",pep->id,pep->option_endpoint_url);
    curl_rc= curl_easy_perform(pep->curl);
    if (curl_rc != CURLE_OK) {
        pep_log_error("pep_authorize: PEP#%d sending XACML request to %s failed: curl[%d] %s.",pep->id,pep->option_endpoint_url,(int)curl_rc,curl_easy_strerror(curl_rc));
        pep_buffer_delete(pep->b64output);
        pep_buffer_delete(pep->b64input);
        return PEP_ERR_CURL + curl_rc;
    }

    /* check for HTTP 200 response code */
    http_code= 0;
    curl_rc= curl_easy_getinfo(pep->curl,CURLINFO_RESPONSE_CODE,&http_code);
    if (curl_rc != CURLE_OK) {
        pep_log_error("pep_authorize: PEP#%d curl_easy_getinfo(pep->curl,CURLINFO_RESPONSE_CODE,&http_code) failed: %s.",pep->id,curl_easy_strerror(curl_rc));
        pep_buffer_delete(pep->b64output);
        pep_buffer_delete(pep->b64input);
        return PEP_ERR_CURL + curl_rc;
    }
    if (http_code != 200) {
        pep_log_error("pep_authorize: PEP#%d: HTTP status code: %d.",pep->id,(int)http_code);
        pep_buffer_delete(pep->b64output);
        pep_buffer_delete(pep->b64input);
        return PEP_ERR_AUTHZ_REQUEST;
    }

    /* not required anymore */
    pep_buffer_delete(pep->b64output);

    pep_log_debug("pep_authorize: PEP#%d: HTTP status code: %d.",pep->id,(int)http_code);

    /* create the Hessian input buffer */
    pep->input= pep_buffer_create(1024);
    if (pep->input == NULL) {
        pep_log_error("pep_authorize: PEP#%d can't create input buffer.",pep->id);
        pep_buffer_delete(pep->b64input);
        return PEP_ERR_MEMORY;
    }

    /* base64 decode the input buffer into the Hessian buffer. */
    pep_log_debug("pep_authorize: PEP#%d: decoding base64 input...",pep->id);
    pep_base64_decode_buffer(pep->b64input,pep->input);

    /* unmarshal the PEP response */
    unmarshal_rc= xacml_response_unmarshalling(response,pep->input);
    if ( unmarshal_rc != PEP_OK) {
        pep_log_error("pep_authorize: PEP#%d can't unmarshal the XACML response: %s.", pep->id, pep_strerror(unmarshal_rc));
        pep_buffer_delete(pep->b64input);
        pep_buffer_delete(pep->input);
        return unmarshal_rc;
    }

    pep_log_info("pep_authorize: PEP#%d XACML Response decoded and deserialized.",pep->id);

    /* not required anymore */
    pep_buffer_delete(pep->b64input);
    pep_buffer_delete(pep->input);


    /* get effective response */
    effective_request= xacml_response_getrequest(*response);
    if (effective_request != NULL) {
        pep_log_debug("pep_authorize: PEP#%d effective request received",pep->id);
        /* delete original */
        xacml_request_delete(*request);
        /* and replace by effective one */
        *request= xacml_response_relinquishrequest(*response);
    }

    /* apply obligation handlers if enabled and any */
    if (pep->option_ohs_enabled && pep_llist_length(pep->ohs) > 0) {
        size_t ohs_l= pep_llist_length(pep->ohs);
        pep_log_info("pep_authorize: PEP#%d %d OHs available, processing...",pep->id,(int)ohs_l);
        for (i= 0; i<ohs_l; i++) {
            pep_obligationhandler_t * oh= pep_llist_get(pep->ohs,i);
            if (oh != NULL) {
                pep_log_debug("pep_authorize: PEP#%d calling OH[%s]->process(request,response)...",pep->id,oh->id);
                oh_rc = oh->process(request,response);
                if (oh_rc != 0) {
                    pep_log_error("pep_authorize: PEP#%d OH[%s] process(request,response) failed: %d.",pep->id,oh->id,oh_rc);
                    return PEP_ERR_OH_PROCESS;
                }
            }
        }
    }
    
    return PEP_OK;
}

/* no return code, not useful */
void pep_destroy(PEP * pep) {
    int pips_destroy_rc= 0;
    int ohs_destroy_rc= 0;
    
    if (pep == NULL) return;

    /* release curl http headers */
    if (pep->curl_http_headers != NULL) {
        curl_slist_free_all(pep->curl_http_headers);
        pep->curl_http_headers= NULL;
    }

    /* release curl */
    if (pep->curl != NULL) {
        curl_easy_cleanup(pep->curl);
        pep->curl= NULL;
    }
    
    /* free options... */
    if (pep->option_endpoint_url != NULL) {
        free(pep->option_endpoint_url);
        pep->option_endpoint_url= NULL;
    }
    if (pep->option_ssl_cipher_list != NULL) {
        free(pep->option_ssl_cipher_list);
        pep->option_ssl_cipher_list= NULL;
    }
    if (pep->option_server_cert != NULL) {
        free(pep->option_server_cert);
        pep->option_server_cert= NULL;
    }
    if (pep->option_server_capath != NULL) {
        free(pep->option_server_capath);
        pep->option_server_capath= NULL;
    }
    if (pep->option_client_cert != NULL) {
        free(pep->option_client_cert);
        pep->option_client_cert= NULL;
    }
    if (pep->option_client_key != NULL) {
        free(pep->option_client_key);
        pep->option_client_key= NULL;
    }
    if (pep->option_client_keypassword != NULL) {
        memset(pep->option_client_keypassword,'\0',strlen(pep->option_client_keypassword));
        free(pep->option_client_keypassword);
        pep->option_client_keypassword= NULL;
    }

    /* destroy all pips if any */
    while (pep_llist_length(pep->pips) > 0) {
        pep_pip_t * pip= pep_llist_remove(pep->pips,0);
        if (pip != NULL) {
            pips_destroy_rc += pip->destroy();
        }
    }
    pep_llist_delete(pep->pips);
    if (pips_destroy_rc > 0) {
        pep_log_warn("pep_destroy: some PIP->destroy() failed...");
    }

    /* destroy all obligation handlers if any */
    while (pep_llist_length(pep->ohs) > 0) {
        pep_obligationhandler_t * oh= pep_llist_remove(pep->ohs,0);
        if (oh != NULL) {
            ohs_destroy_rc += oh->destroy();
        }
    }
    pep_llist_delete(pep->ohs);
    if (ohs_destroy_rc > 0) {
        pep_log_warn("pep_destroy: some OH->destroy() failed...");
    }

    free(pep);
}


/**************************/
/*** INTERNAL FUNCTIONS ***/
/**************************/

/** set the pep handle default values */
static void init_pep_defaults(PEP * pep) {
    if (pep==NULL) return;
    /* increase client counter */
    pep->id= n_pep_clients++;
    pep->curl_http_headers= NULL;
    /* set default options */
    pep->option_endpoint_url= NULL;
    pep->option_loglevel= DEFAULT_LOG_LEVEL;
    pep->option_logout= (FILE *)DEFAULT_LOG_FILE;
    pep->option_timeout= (long)DEFAULT_CURL_TIMEOUT; 
    pep->option_server_cert= NULL;
    pep->option_server_capath= NULL;
    pep->option_client_cert= NULL;
    pep->option_client_key= NULL;
    pep->option_client_keypassword= NULL;
    pep->option_ssl_validation= DEFAULT_CURL_SSL_VALIDATION;
    pep->option_ssl_cipher_list= NULL;
    pep->option_pips_enabled= DEFAULT_PIPS_ENABLED;
    pep->option_ohs_enabled= DEFAULT_OHS_ENABLED;
}

/** set some curl default value */
static void init_curl_defaults(PEP * pep) {
/*
    CURLcode curl_rc;
*/
    /* set default http headers */
    set_curl_http_headers(pep);
    /* set default timeout */
    set_curl_connection_timeout(pep);
    /* set default ssl validation */
    set_curl_ssl_validation(pep);
    /* disable signal for multi-threading */
    set_curl_nosignal(pep);
    /* OpenSSL 1.0 bug fix: will disable ECDH ciphers, see DEFAULT_SSL_CIPHER_LIST */
/* DON'T DO THIS ANYMORE: Argus >= 1.3.1 disable it on the server!!!!
#ifndef HAVE_LIBCURL_NSS
    pep_log_debug("init_curl_defaults: PEP#%d DEFAULT_SSL_CIPHER_LIST: %s",pep->id,DEFAULT_SSL_CIPHER_LIST);    
    curl_rc= curl_easy_setopt(pep->curl,CURLOPT_SSL_CIPHER_LIST,DEFAULT_SSL_CIPHER_LIST);
    if (curl_rc != CURLE_OK) {
        pep_log_warn("init_curl_defaults: PEP#%d curl_easy_setopt(curl,CURLOPT_SSL_CIPHER_LIST,%s) failed: %s",pep->id,DEFAULT_SSL_CIPHER_LIST,curl_easy_strerror(curl_rc));
    }
#endif
*/
}

/** 
 * set curl http headers:
 * - disable 'Expect: 100-continue' HTTP 1.1 header in POST
 * - set 'User-Agent: <value>' header
 */
static int set_curl_http_headers(PEP * pep) {
    CURLcode curl_rc;
    /* disable 'Expect: 100-continue' HTTP 1.1 header in POST */
    pep->curl_http_headers= curl_slist_append(pep->curl_http_headers, "Expect:");  
    pep_log_debug("set_curl_http_headers: PEP#%d curl_http_headers: 'Expect:'",pep->id);    
    /* set 'User-Agent:' header */
    pep->curl_http_headers= curl_slist_append(pep->curl_http_headers, "User-Agent: " PACKAGE_NAME "/" PACKAGE_VERSION );  
    pep_log_debug("set_curl_http_headers: PEP#%d curl_http_headers: 'User-Agent: " PACKAGE_NAME "/" PACKAGE_VERSION "'",pep->id);    
    curl_rc= curl_easy_setopt(pep->curl, CURLOPT_HTTPHEADER, pep->curl_http_headers);
    if (curl_rc != CURLE_OK) {
        pep_log_warn("set_curl_http_headers: PEP#%d curl_easy_setopt(curl,CURLOPT_HTTPHEADER,curl_http_headers) failed: %s.",pep->id,curl_easy_strerror(curl_rc));
        return 1;
    }
    return 0;
}

/** disable signal for multi-threading */
static int set_curl_nosignal(const PEP * pep) {
    CURLcode curl_rc;
    pep_log_debug("set_curl_nosignal: PEP#%d curl_easy_setopt(curl,CURLOPT_NOSIGNAL,1)",pep->id);
    curl_rc= curl_easy_setopt(pep->curl, CURLOPT_NOSIGNAL, 1);
    if (curl_rc != CURLE_OK) {
        pep_log_warn("set_curl_nosignal: PEP#%d curl_easy_setopt(curl,CURLOPT_NOSIGNAL,1) failed: %s.",pep->id,curl_easy_strerror(curl_rc));
        return 1;
    }
    return 0;
}

/** set libcurl CURLOPT_URL */
static int set_curl_endpoint_url(const PEP * pep) {
    CURLcode curl_rc;
    pep_log_debug("set_curl_endpoint_url: PEP#%d option_endpoint_url: %s",pep->id,pep->option_endpoint_url);
    curl_rc= curl_easy_setopt(pep->curl, CURLOPT_URL, pep->option_endpoint_url);
    if (curl_rc != CURLE_OK) {
        pep_log_error("set_curl_endpoint_url: PEP#%d curl_easy_setopt(curl,CURLOPT_URL,%s) failed: %s.",pep->id,pep->option_endpoint_url,curl_easy_strerror(curl_rc));
        return 1;
    }
    return 0;
}


/* set libcurl CURLOPT_TIMEOUT */
static int set_curl_connection_timeout(const PEP * pep) {
    CURLcode curl_rc;
    pep_log_debug("set_curl_connection_timeout: PEP#%d option_timeout: %d",pep->id,(int)(pep->option_timeout));
    curl_rc= curl_easy_setopt(pep->curl, CURLOPT_TIMEOUT, pep->option_timeout);
    if (curl_rc != CURLE_OK) {
        pep_log_error("set_curl_connection_timeout: PEP#%d curl_easy_setopt(curl,CURLOPT_TIMEOUT,%d) failed: %s",pep->id, (int)(pep->option_timeout),curl_easy_strerror(curl_rc));
        return 1;
    }
    return 0;
}

/* set libcurl CURLOPT_SSL_VERIFYPEER */
static int set_curl_ssl_validation(const PEP * pep) {
    CURLcode curl_rc;
    pep_log_debug("set_curl_ssl_validation: PEP#%d option_ssl_validation: %s",pep->id,pep->option_ssl_validation ? "TRUE" : "FALSE");
    curl_rc= curl_easy_setopt(pep->curl,CURLOPT_SSL_VERIFYPEER,pep->option_ssl_validation);
    if (curl_rc != CURLE_OK) {
        pep_log_error("set_curl_ssl_validation: PEP#%d curl_easy_setopt(curl,CURLOPT_SSL_VERIFYPEER,%d) failed: %s",pep->option_ssl_validation,curl_easy_strerror(curl_rc));
        return 1;
    }
    return 0;
}

/* set libcurl CURLOPT_SSL_CIPHER_LIST iff option_ssl_cipher_list not NULL */
static int set_curl_ssl_cipher_list(const PEP * pep) {
    CURLcode curl_rc;
    pep_log_debug("set_curl_ssl_cipher_list: PEP#%d option_ssl_cipher_list: %s",pep->id,pep->option_ssl_cipher_list);
    if (pep->option_ssl_cipher_list != NULL) {
        curl_rc= curl_easy_setopt(pep->curl,CURLOPT_SSL_CIPHER_LIST,pep->option_ssl_cipher_list);
        if (curl_rc != CURLE_OK) {
            pep_log_error("set_curl_ssl_cipher_list: PEP#%d curl_easy_setopt(curl,CURLOPT_SSL_CIPHER_LIST,%s) failed: %s",pep->id,pep->option_ssl_cipher_list,curl_easy_strerror(curl_rc));
            return 1;
        }
    }
    return 0;
}

/** set libcurl CURLOPT_CAINFO iff option_server_cert not NULL */
static int set_curl_server_cert(const PEP * pep) {
    CURLcode curl_rc;
    pep_log_debug("set_curl_server_cert: PEP#%d option_server_cert: %s",pep->id,pep->option_server_cert);
    if (pep->option_server_cert != NULL) {
        curl_rc= curl_easy_setopt(pep->curl,CURLOPT_CAINFO,pep->option_server_cert);
        if (curl_rc != CURLE_OK) {
            pep_log_error("set_curl_server_cert: PEP#%d curl_easy_setopt(curl,CURLOPT_CAINFO,%s) failed: %s",pep->id,pep->option_server_cert,curl_easy_strerror(curl_rc));
            return 1;
        }
    }
    return 0;
}

/** set libcurl CURLOPT_CAPATH iff option_server_capath not NULL */
static int set_curl_server_capath(const PEP * pep) {
    CURLcode curl_rc;
    pep_log_debug("set_curl_server_capath: PEP#%d option_server_capath: %s",pep->id,pep->option_server_capath);
    if (pep->option_server_capath != NULL) {
        curl_rc= curl_easy_setopt(pep->curl,CURLOPT_CAPATH,pep->option_server_capath);
        if (curl_rc != CURLE_OK) {
            pep_log_error("set_curl_server_capath: PEP#%d curl_easy_setopt(curl,CURLOPT_CAPATH,%s) failed: %s",pep->id,pep->option_server_capath,curl_easy_strerror(curl_rc));
            return 1;
        }
    }
    return 0;
}

/** set libcurl CURLOPT_SSLCERT iff option_client_cert not NULL */
static int set_curl_client_cert(const PEP * pep) {
    CURLcode curl_rc;
    pep_log_debug("set_curl_client_cert: PEP#%d option_client_cert: %s",pep->id,pep->option_client_cert);
    if (pep->option_client_cert != NULL) {
        curl_rc= curl_easy_setopt(pep->curl,CURLOPT_SSLCERT,pep->option_client_cert);
        if (curl_rc != CURLE_OK) {
            pep_log_error("set_curl_client_cert: PEP#%d curl_easy_setopt(curl,CURLOPT_SSLCERT,%s) failed: %s",pep->id,pep->option_client_cert,curl_easy_strerror(curl_rc));
            return 1;
        }
    }
    return 0;
}


/** set libcurl CURLOPT_SSLKEY iff option_client_key not NULL */
static int set_curl_client_key(const PEP * pep) {
    CURLcode curl_rc;
    pep_log_debug("set_curl_client_key: PEP#%d option_client_key: %s",pep->id,pep->option_client_key);
    if (pep->option_client_key != NULL) {
        curl_rc= curl_easy_setopt(pep->curl,CURLOPT_SSLKEY,pep->option_client_key);
        if (curl_rc != CURLE_OK) {
            pep_log_error("set_curl_client_key: PEP#%d curl_easy_setopt(curl,CURLOPT_SSLKEY,%s) failed: %s",pep->id,pep->option_client_key,curl_easy_strerror(curl_rc));
            return 1;
        }
    }
    return 0;
}


/** set libcurl CURLOPT_SSLKEYPASSWD iff option_client_keypassword not NULL */
static int set_curl_client_keypassword(const PEP * pep) {
    CURLcode curl_rc;
    if (pep->option_client_keypassword != NULL) {
        pep_log_debug("set_curl_client_keypassword: PEP#%d option_client_keypassword: %d char long",pep->id,(int)strlen(pep->option_client_keypassword));
        curl_rc= curl_easy_setopt(pep->curl,CURLOPT_SSLKEYPASSWD,pep->option_client_keypassword);
        if (curl_rc != CURLE_OK) {
            pep_log_error("set_curl_client_keypassword: PEP#%d curl_easy_setopt(curl,CURLOPT_SSLKEYPASSWD,%s) failed: %s",pep->id,pep->option_client_keypassword,curl_easy_strerror(curl_rc));
            return 1;
        }
    }
    return 0;
}

/** set libcurl CURLOPT_VERBOSE to true if option_loglevel >= DEBUG, false otherwise */
static int set_curl_verbose(const PEP * pep) {
    CURLcode curl_rc;
    long enabled= 0L;
    pep_log_debug("set_curl_verbose: PEP#%d option_loglevel: %d",pep->id,pep->option_loglevel);
    if (pep->option_loglevel >= PEP_LOGLEVEL_DEBUG) {
        enabled= 1L;
    }
    curl_rc= curl_easy_setopt(pep->curl,CURLOPT_VERBOSE,enabled);
    if (curl_rc != CURLE_OK) {
        pep_log_error("set_curl_verbose: PEP#%d curl_easy_setopt(curl,CURLOPT_VERBOSE,%d) failed: %s",pep->id,(int)enabled,curl_easy_strerror(curl_rc));
        return 1;
    }
    return 0;
}

/** set libcurl CURLOPT_STDERR */
static int set_curl_stderr(const PEP * pep) {
    CURLcode curl_rc;
    pep_log_debug("set_curl_stderr: PEP#%d option_logout: %p",pep->id,pep->option_logout);
    curl_rc= curl_easy_setopt(pep->curl,CURLOPT_STDERR,pep->option_logout);
    if (curl_rc != CURLE_OK) {
        pep_log_error("set_curl_stderr: PEP#%d curl_easy_setopt(curl,CURLOPT_STDERR,%p) failed: %s",pep->id,pep->option_logout,curl_easy_strerror(curl_rc));
        return 1;
    }
    return 0;
}


