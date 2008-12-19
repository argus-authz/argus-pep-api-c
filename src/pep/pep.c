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
 * $Id: pep.c,v 1.3 2008/12/19 11:44:40 vtschopp Exp $
 */
#include <stdarg.h>  /* va_list, va_arg, ... */
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>

#include "util/linkedlist.h"
#include "util/buffer.h"
#include "util/base64.h"

#include "pep/pep.h"
#include "pep/io.h"

#ifdef HAVE_CONFIG_H
#include "config.h"  /* PACKAGE_NAME and PACKAGE_VERSION const */
#else
#define PACKAGE_NAME "PEP-C"
#define PACKAGE_VERSION "1.0.0"
#endif

/** List of PIPs */
static linkedlist_t * pips;

/** List of ObligationHandlers */
static linkedlist_t * ohs;

/** CURL handler */
static CURL * curl= NULL;

/** Options */
static int option_loglevel= 0;
static char * option_logengine= NULL;
static char * option_url= NULL;
static long option_timeout= 10L;

static int option_pips_enabled= 0;
static int option_ohs_enabled= 0;

pep_error_t pep_initialize(void) {
	pips= llist_create();
	if (pips == NULL) {
		fprintf(stderr,"ERROR:pep_initialize: PIPs list allocation failed.\n");
		return PEP_ERR_INIT_LISTS;
	}
	ohs= llist_create();
	if (ohs == NULL) {
		llist_delete(pips);
		fprintf(stderr,"ERROR:pep_initialize: OHs list allocation failed.\n");
		return PEP_ERR_INIT_LISTS;
	}

	CURLcode curl_rc= curl_global_init(CURL_GLOBAL_ALL);
	if (curl_rc != CURLE_OK) {
		fprintf(stderr,"ERROR:pep_initialize: CURL global initialization failed: %s\n", curl_easy_strerror(curl_rc));
		llist_delete(pips);
		llist_delete(ohs);
		return PEP_ERR_INIT_CURL;
	}
	curl= curl_easy_init();
	if (curl == NULL) {
		fprintf(stderr,"ERROR:pep_initialize: can't create CURL session handler.\n");
		llist_delete(pips);
		llist_delete(ohs);
		return PEP_ERR_INIT_CURL;
	}
	return PEP_OK;
}

pep_error_t pep_addpip(pep_pip_t * pip) {
	if (pip == NULL) {
		fprintf(stderr,"ERROR:pep_addpip: NULL pip pointer\n");
		return PEP_ERR_NULL_POINTER;
	}
	int pip_rc = -1;
	if ((pip_rc= pip->init()) != 0) {
		fprintf(stderr,"ERROR:pep_addpip: PIP[%s] init() failed: %d.\n",pip->id, pip_rc);
		return PEP_ERR_INIT_PIP;
	}
	if (llist_add(pips,pip) != LLIST_OK) {
		fprintf(stderr,"ERROR:pep_addpip: failed to add initialized PIP[%s] in list.\n",pip->id);
		return PEP_ERR_INIT_LISTS;
	}
	return PEP_OK;
}

pep_error_t pep_addobligationhandler(pep_obligationhandler_t * oh) {
	if (oh == NULL) {
		fprintf(stderr,"ERROR:pep_addobligationhandler: NULL oh pointer\n");
		return PEP_ERR_NULL_POINTER;
	}
	int oh_rc= -1;
	if ((oh_rc= oh->init()) != 0) {
		fprintf(stderr,"ERROR:pep_addobligationhandler: OH[%s] init() failed: %d\n",oh->id, oh_rc);
		return PEP_ERR_INIT_OH;
	}
	if (llist_add(ohs,oh) != LLIST_OK) {
		fprintf(stderr,"ERROR:pep_addobligationhandler: failed to add initialized OH[%s] in list.\n", oh->id);
		return PEP_ERR_INIT_LISTS;
	}
	return PEP_OK;
}

// FIXME: implement all options
pep_error_t pep_setoption(pep_option_t option, ... ) {
	pep_error_t rc= PEP_OK;
	va_list args;
	va_start(args,option);
	char * str;
	switch (option) {
		case PEP_OPTION_ENDPOINT_URL:
			str= va_arg(args,char *);
			if (str == NULL) {
				fprintf(stderr,"ERROR:pep_setoption: PEP_OPTION_ENDPOINT_URL argument is NULL.\n");
				rc= PEP_ERR_OPTION_INVALID;
				break;
			}
			if (option_url != NULL) {
				free(option_url);
				option_url= NULL;
			}
			size_t size= strlen(str);
			option_url= calloc(size + 1, sizeof(char));
			if (option_url == NULL) {
				fprintf(stderr,"ERROR:pep_setoption: can't allocate option_url: %s.\n", str);
				rc= PEP_ERR_MEMORY;
				break;
			}
			strncpy(option_url,str,size);
			break;
		default:
			fprintf(stderr,"TODO:pep_setoption: %d option NOT YET IMPLEMENTED.\n", option);
			break;
	}
	va_end(args);
	return rc;
}

pep_error_t pep_authorize(pep_request_t ** inout_request, pep_response_t ** out_response) {
	if (inout_request == NULL) {
		fprintf(stderr,"ERROR:pep_authorize: NULL request pointer\n");
		return PEP_ERR_NULL_POINTER;
	}
	pep_request_t * request= *inout_request;
	int i= 0;
	// apply pips if enabled and any
	int pip_rc= -1;
	if (option_pips_enabled && llist_length(pips) > 0) {
		size_t pips_l= llist_length(pips);
		for (i= 0; i<pips_l; i++) {
			pep_pip_t * pip= llist_get(pips,i);
			if (pip != NULL) {
				pip_rc= pip->process(&request);
				if (pip_rc != 0) {
					fprintf(stderr,"ERROR:pep_authorize: PIP[%s] process(request) failed: %d\n", pip->id, pip_rc);
					return PEP_ERR_AUTHZ_PIP_PROCESS;
				}
			}
		}
	}

	// marshal the PEP request in output buffer
	BUFFER * output= buffer_create(512);
	if (output == NULL) {
		fprintf(stderr,"ERROR:pep_authorize: can't create output buffer.\n");
		return PEP_ERR_MEMORY;
	}
	int marshal_rc= pep_request_marshalling(request,output);
	if ( marshal_rc != PEP_OK ) {
		fprintf(stderr,"ERROR:pep_authorize: can't marshal PEP request: %s.\n", pep_strerror(marshal_rc));
		buffer_delete(output);
		return marshal_rc;
	}

	// base64 encode the output buffer
	size_t output_l= buffer_length(output);
	BUFFER * b64output= buffer_create( output_l );
	if (b64output == NULL) {
		fprintf(stderr,"ERROR:pep_authorize: can't create base64 output buffer.\n");
		buffer_delete(output);
		return PEP_ERR_MEMORY;
	}
	base64_encode_l(output,b64output,BASE64_DEFAULT_LINE_SIZE);

	// output buffer not needed anymore.
	buffer_delete(output);

	// set the CURL related options
	//XXX: options debug and timeout
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, PACKAGE_NAME "/" PACKAGE_VERSION);

	curl_easy_setopt(curl, CURLOPT_TIMEOUT, option_timeout);
    curl_easy_setopt(curl, CURLOPT_URL, option_url);



	// configure curl handler to POST the base64 encoded marshalled PEP request buffer
	CURLcode curl_rc= curl_easy_setopt(curl, CURLOPT_POST, 1L);
	if (curl_rc != CURLE_OK) {
		fprintf(stderr,"ERROR:pep_authorize: failed to set CURL option CURLOPT_POST=1: %s.\n",curl_easy_strerror(curl_rc));
		buffer_delete(b64output);
		return PEP_ERR_AUTHZ_CURL;
	}

	long b64output_l= (long)buffer_length(b64output);
	curl_rc= curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, b64output_l);
	if (curl_rc != CURLE_OK) {
		fprintf(stderr,"ERROR:pep_authorize: failed to set CURL option CURLOPT_POSTFIELDSIZE=%d: %s.\n",(int)b64output_l,curl_easy_strerror(curl_rc));
		buffer_delete(b64output);
		return PEP_ERR_AUTHZ_CURL;
	}

	curl_rc= curl_easy_setopt(curl, CURLOPT_READDATA, b64output);
	if (curl_rc != CURLE_OK) {
		fprintf(stderr,"ERROR:pep_authorize: failed to set CURL option CURLOPT_READDATA=b64output: %s.\n",curl_easy_strerror(curl_rc));
		buffer_delete(b64output);
		return PEP_ERR_AUTHZ_CURL;
	}

	curl_rc= curl_easy_setopt(curl, CURLOPT_READFUNCTION, buffer_read);
	if (curl_rc != CURLE_OK) {
		fprintf(stderr,"ERROR:pep_authorize: failed to set CURL option CURLOPT_READFUNCTION=buffer_read: %s.\n",curl_easy_strerror(curl_rc));
		buffer_delete(b64output);
		return PEP_ERR_AUTHZ_CURL;
	}


	// configure curl handler to read the base64 encoded HTTP response
	BUFFER * b64input= buffer_create(1024); // FIXME: optimize size
	if (b64input == NULL) {
		fprintf(stderr,"ERROR:pep_authorize: can't create base64 input buffer.\n");
		buffer_delete(b64output);
		return PEP_ERR_MEMORY;
	}

    curl_rc= curl_easy_setopt(curl, CURLOPT_WRITEDATA, b64input);
	if (curl_rc != CURLE_OK) {
		fprintf(stderr,"ERROR:pep_authorize: failed to set CURL option CURLOPT_WRITEDATA=b64input: %s.\n",curl_easy_strerror(curl_rc));
		buffer_delete(b64output);
		buffer_delete(b64input);
		return PEP_ERR_AUTHZ_CURL;
	}
    curl_rc= curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, buffer_write);
	if (curl_rc != CURLE_OK) {
		fprintf(stderr,"ERROR:pep_authorize: failed to set CURL option CURLOPT_WRITEFUNCTION=buffer_write: %s.\n",curl_easy_strerror(curl_rc));
		buffer_delete(b64output);
		buffer_delete(b64input);
		return PEP_ERR_AUTHZ_CURL;
	}

	// send the request
	curl_rc= curl_easy_perform(curl);
	if (curl_rc != CURLE_OK) {
		fprintf(stderr,"ERROR:pep_authorize: CURL failed to perform the POST operation: %s.\n",curl_easy_strerror(curl_rc));
		buffer_delete(b64output);
		buffer_delete(b64input);
		return PEP_ERR_AUTHZ_CURL;
	}

	// check for HTTP 200 response code
	long http_code= 0;
	curl_rc= curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE, &http_code);
	if (curl_rc != CURLE_OK) {
		fprintf(stderr,"ERROR:pep_authorize: failed to get CURL info CURLINFO_RESPONSE_CODE: %s.\n",curl_easy_strerror(curl_rc));
		buffer_delete(b64output);
		buffer_delete(b64input);
		return PEP_ERR_AUTHZ_CURL;
	}
	if (http_code != 200) {
		fprintf(stderr,"ERROR:pep_authorize: HTTP status code: %ld.\n",http_code);
		buffer_delete(b64output);
		buffer_delete(b64input);
		return PEP_ERR_AUTHZ_REQUEST;
	}

	// not required anymore
	buffer_delete(b64output);

	// base64 decode the input buffer.
	size_t b64input_l= buffer_length(b64input);
	BUFFER * input= buffer_create(b64input_l);
	if (output == NULL) {
		fprintf(stderr,"ERROR:pep_authorize: can't create input buffer.\n");
		buffer_delete(b64input);
		return PEP_ERR_MEMORY;
	}
	base64_decode(b64input,input);

	// unmarshal the PEP response
	pep_response_t * response= NULL;
	int unmarshal_rc= pep_response_unmarshalling(&response,input);
	if ( unmarshal_rc != PEP_OK) {
		fprintf(stderr,"ERROR:pep_authorize: can't unmarshal the PEP response: %s.\n", pep_strerror(unmarshal_rc));
		buffer_delete(b64input);
		buffer_delete(input);
		return unmarshal_rc;
	}
	*out_response= response;

	// apply obligation handlers if enabled and any
	int oh_rc= 0;
	if (option_ohs_enabled && llist_length(ohs) > 0) {
		size_t ohs_l= llist_length(ohs);
		for (i= 0; i<ohs_l; i++) {
			pep_obligationhandler_t * oh= llist_get(ohs,i);
			if (oh != NULL) {
				oh_rc = oh->process(&request,&response);
				if (oh_rc != 0) {
					fprintf(stderr,"ERROR:pep_authorize OH[%s] process(request,response) failed: %d\n.", oh->id,oh_rc);
					return PEP_ERR_AUTHZ_OH_PROCESS;
				}
			}
		}
	}
	return PEP_OK;
}

// TODO: return code...
pep_error_t pep_destroy(void) {
	// free options...
	if (option_logengine) free(option_logengine);
	if (option_url) free(option_url);
	// FIXME: free all char options

	// release curl
	if (curl != NULL) {
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();


	// destroy all pips if any
	int pips_destroy_rc= 0;
	while (llist_length(pips) > 0) {
		pep_pip_t * pip= llist_remove(pips,0);
		if (pip != NULL) {
			pips_destroy_rc += pip->destroy();
		}
	}
	llist_delete(pips);

	// destroy all obligation handlers if any
	int ohs_destroy_rc= 0;
	while (llist_length(ohs) > 0) {
		pep_obligationhandler_t * oh= llist_remove(ohs,0);
		if (oh != NULL) {
			ohs_destroy_rc += oh->destroy();
		}
	}
	llist_delete(ohs);
	return PEP_OK;
}

