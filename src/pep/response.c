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
 * $Id: response.c,v 1.1 2008/12/12 11:34:27 vtschopp Exp $
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "util/linkedlist.h"
#include "pep/model.h"

struct pep_response {
	pep_request_t * request; // original request
	linkedlist_t * results; // list of results
};

pep_response_t * pep_response_create() {
	pep_response_t * response= calloc(1,sizeof(pep_response_t));
	if (response == NULL) {
		fprintf(stderr,"ERROR:pep_response_create: can't allocate pep_response_t.\n");
		return NULL;
	}
	response->results= llist_create();
	if (response->results == NULL) {
		fprintf(stderr,"ERROR:pep_response_create: can't create results list.\n");
		free(response);
		return NULL;
	}
	response->request= NULL;
	return response;
}

int pep_response_setrequest(pep_response_t * response, pep_request_t * request) {
	if (response == NULL || request == NULL) {
		fprintf(stderr,"ERROR:pep_response_setrequest: NULL response or request.\n");
		return PEP_MODEL_ERROR;
	}
	if (response->request != NULL) pep_request_delete(response->request);
	response->request= request;
	return PEP_MODEL_OK;
}

pep_request_t * pep_response_getrequest(const pep_response_t * response) {
	if (response == NULL) {
		fprintf(stderr,"ERROR:pep_response_getrequest: NULL response.\n");
		return NULL;
	}
	return response->request;
}

int pep_response_addresult(pep_response_t * response, pep_result_t * result) {
	if (response == NULL || result == NULL) {
		fprintf(stderr,"ERROR:pep_response_addresult: NULL response or result.\n");
		return PEP_MODEL_ERROR;
	}
	if (llist_add(response->results,result) != LLIST_OK) {
		fprintf(stderr,"ERROR:pep_response_addresult: can't add result to list.\n");
		return PEP_MODEL_ERROR;
	}
	else return PEP_MODEL_OK;
}

size_t pep_response_results_length(const pep_response_t * response) {
	if (response == NULL) {
		fprintf(stderr,"ERROR:pep_response_results_length: NULL response.\n");
		return PEP_MODEL_ERROR;
	}
	return llist_length(response->results);
}

pep_result_t * pep_response_getresult(const pep_response_t * response, int index) {
	if (response == NULL) {
		fprintf(stderr,"ERROR:pep_response_getresult: NULL response.\n");
		return NULL;
	}
	return llist_get(response->results,index);
}

void pep_response_delete(pep_response_t * response) {
	if (response == NULL) return;
	if (response->request != NULL) pep_request_delete(response->request);
	llist_delete_elements(response->results,(delete_element_func)pep_result_delete);
	llist_delete(response->results);
	free(response);
	response= NULL;
}
