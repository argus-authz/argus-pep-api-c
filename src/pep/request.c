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
 * $Id: request.c,v 1.1 2008/12/12 11:34:27 vtschopp Exp $
 */
#include <stdlib.h>
#include <stdio.h>

#include "util/linkedlist.h"
#include "pep/model.h"

struct pep_request {
	linkedlist_t * subjects;
	linkedlist_t * resources;
	pep_action_t * action;
	pep_environment_t * environment;
};

/**
 * Creates an empty PEP request.
 */
pep_request_t * pep_request_create() {
	pep_request_t * request= calloc(1,sizeof(pep_request_t));
	if (request == NULL) {
		fprintf(stderr,"ERROR:pep_request_create: can't allocate pep_request_t.\n");
		return NULL;
	}
	request->subjects= llist_create();
	if (request->subjects == NULL) {
		fprintf(stderr,"ERROR:pep_request_create: can't create subjects list.\n");
		free(request);
		return NULL;
	}
	request->resources= llist_create();
	if (request->resources == NULL) {
		fprintf(stderr,"ERROR:pep_request_create: can't create resources list.\n");
		llist_delete(request->subjects);
		free(request);
		return NULL;
	}
	request->action= NULL;
	request->environment= NULL;
	return request;
}

int pep_request_addsubject(pep_request_t * request, pep_subject_t * subject) {
	if (request == NULL || subject == NULL) {
		fprintf(stderr,"ERROR:pep_request_addsubject: NULL request or subject.\n");
		return PEP_MODEL_ERROR;
	}
	if (llist_add(request->subjects,subject) != LLIST_OK) {
		fprintf(stderr,"ERROR:pep_request_addsubject: can't add subject to list.\n");
		return PEP_MODEL_ERROR;
	}
	else return PEP_MODEL_OK;
}

size_t pep_request_subjects_length(const pep_request_t * request) {
	if (request == NULL) {
		fprintf(stderr,"ERROR:pep_request_subjects_length: NULL request.\n");
		return PEP_MODEL_ERROR;
	}
	return llist_length(request->subjects);
}

pep_subject_t * pep_request_getsubject(const pep_request_t * request, int index) {
	if (request == NULL) {
		fprintf(stderr,"ERROR:pep_request_getsubject: NULL request.\n");
		return NULL;
	}
	return llist_get(request->subjects,index);
}

int pep_request_addresource(pep_request_t * request, pep_resource_t * resource) {
	if (request == NULL || resource == NULL) {
		fprintf(stderr,"ERROR:pep_request_addresource: NULL request or resource.\n");
		return PEP_MODEL_ERROR;
	}
	if (llist_add(request->resources,resource) != LLIST_OK) {
		fprintf(stderr,"ERROR:pep_request_addresource: can't add resource to list.\n");
		return PEP_MODEL_ERROR;
	}
	else return PEP_MODEL_OK;
}

size_t pep_request_resources_length(const pep_request_t * request) {
	if (request == NULL) {
		fprintf(stderr,"ERROR:pep_request_resources_length: NULL request.\n");
		return PEP_MODEL_ERROR;
	}
	return llist_length(request->resources);
}

pep_resource_t * pep_request_getresource(const pep_request_t * request, int index) {
	if (request == NULL) {
		fprintf(stderr,"ERROR:pep_request_getresource: NULL request.\n");
		return NULL;
	}
	return llist_get(request->resources,index);
}

int pep_request_setaction(pep_request_t * request, pep_action_t * action) {
	if (request == NULL || action == NULL) {
		fprintf(stderr,"ERROR:pep_request_setaction: NULL request or action.\n");
		return PEP_MODEL_ERROR;
	}
	if (request->action != NULL) pep_action_delete(request->action);
	request->action= action;
	return PEP_MODEL_OK;
}

pep_action_t * pep_request_getaction(const pep_request_t * request) {
	if (request == NULL) {
		fprintf(stderr,"ERROR:pep_request_getaction: NULL request.\n");
		return NULL;
	}
	return request->action;
}

int pep_request_setenvironment(pep_request_t * request, pep_environment_t * env) {
	if (request == NULL || env == NULL) {
		fprintf(stderr,"ERROR:pep_request_setenvironment: NULL request or env.\n");
		return PEP_MODEL_ERROR;
	}
	if (request->environment != NULL) pep_environment_delete(request->environment);
	request->environment= env;
	return PEP_MODEL_OK;
}

pep_environment_t * pep_request_getenvironment(const pep_request_t * request) {
	if (request == NULL) {
		fprintf(stderr,"ERROR:pep_request_getenvironment: NULL request.\n");
		return NULL;
	}
	return request->environment;
}

/**
 * Delete the given PEP request.
 */
void pep_request_delete(pep_request_t * request) {
	if (request == NULL) return;
	llist_delete_elements(request->subjects,(delete_element_func)pep_subject_delete);
	llist_delete(request->subjects);
	llist_delete_elements(request->resources,(delete_element_func)pep_resource_delete);
	llist_delete(request->resources);
	if (request->action != NULL) pep_action_delete(request->action);
	if (request->environment != NULL) pep_environment_delete(request->environment);
	free(request);
	request= NULL;
}

