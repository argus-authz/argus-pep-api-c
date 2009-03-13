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
 * $Id: request.c,v 1.4 2009/03/13 12:02:17 vtschopp Exp $
 */
#include <stdlib.h>

#include "util/linkedlist.h"
#include "util/log.h"
#include "pep/xacml.h"

struct xacml_request {
	linkedlist_t * subjects;
	linkedlist_t * resources;
	xacml_action_t * action;
	xacml_environment_t * environment;
};

/**
 * Creates an empty PEP request.
 */
xacml_request_t * xacml_request_create() {
	xacml_request_t * request= calloc(1,sizeof(xacml_request_t));
	if (request == NULL) {
		log_error("xacml_request_create: can't allocate xacml_request_t.");
		return NULL;
	}
	request->subjects= llist_create();
	if (request->subjects == NULL) {
		log_error("xacml_request_create: can't create subjects list.");
		free(request);
		return NULL;
	}
	request->resources= llist_create();
	if (request->resources == NULL) {
		log_error("xacml_request_create: can't create resources list.");
		llist_delete(request->subjects);
		free(request);
		return NULL;
	}
	request->action= NULL;
	request->environment= NULL;
	return request;
}

int xacml_request_addsubject(xacml_request_t * request, xacml_subject_t * subject) {
	if (request == NULL || subject == NULL) {
		log_error("xacml_request_addsubject: NULL request or subject.");
		return PEP_XACML_ERROR;
	}
	if (llist_add(request->subjects,subject) != LLIST_OK) {
		log_error("xacml_request_addsubject: can't add subject to list.");
		return PEP_XACML_ERROR;
	}
	else return PEP_XACML_OK;
}

size_t xacml_request_subjects_length(const xacml_request_t * request) {
	if (request == NULL) {
		log_error("xacml_request_subjects_length: NULL request.");
		return PEP_XACML_ERROR;
	}
	return llist_length(request->subjects);
}

xacml_subject_t * xacml_request_getsubject(const xacml_request_t * request, int index) {
	if (request == NULL) {
		log_error("xacml_request_getsubject: NULL request.");
		return NULL;
	}
	return llist_get(request->subjects,index);
}

int xacml_request_addresource(xacml_request_t * request, xacml_resource_t * resource) {
	if (request == NULL || resource == NULL) {
		log_error("xacml_request_addresource: NULL request or resource.");
		return PEP_XACML_ERROR;
	}
	if (llist_add(request->resources,resource) != LLIST_OK) {
		log_error("xacml_request_addresource: can't add resource to list.");
		return PEP_XACML_ERROR;
	}
	else return PEP_XACML_OK;
}

size_t xacml_request_resources_length(const xacml_request_t * request) {
	if (request == NULL) {
		log_error("xacml_request_resources_length: NULL request.");
		return PEP_XACML_ERROR;
	}
	return llist_length(request->resources);
}

xacml_resource_t * xacml_request_getresource(const xacml_request_t * request, int index) {
	if (request == NULL) {
		log_error("xacml_request_getresource: NULL request.");
		return NULL;
	}
	return llist_get(request->resources,index);
}

int xacml_request_setaction(xacml_request_t * request, xacml_action_t * action) {
	if (request == NULL) {
		log_error("xacml_request_setaction: NULL request.");
		return PEP_XACML_ERROR;
	}
	if (request->action != NULL) xacml_action_delete(request->action);
	request->action= action;
	return PEP_XACML_OK;
}

xacml_action_t * xacml_request_getaction(const xacml_request_t * request) {
	if (request == NULL) {
		log_error("xacml_request_getaction: NULL request.");
		return NULL;
	}
	return request->action;
}

int xacml_request_setenvironment(xacml_request_t * request, xacml_environment_t * env) {
	if (request == NULL) {
		log_error("xacml_request_setenvironment: NULL request.");
		return PEP_XACML_ERROR;
	}
	if (request->environment != NULL) xacml_environment_delete(request->environment);
	request->environment= env;
	return PEP_XACML_OK;
}

xacml_environment_t * xacml_request_getenvironment(const xacml_request_t * request) {
	if (request == NULL) {
		log_error("xacml_request_getenvironment: NULL request.");
		return NULL;
	}
	return request->environment;
}

/**
 * Delete the given PEP request.
 */
void xacml_request_delete(xacml_request_t * request) {
	if (request == NULL) return;
	llist_delete_elements(request->subjects,(delete_element_func)xacml_subject_delete);
	llist_delete(request->subjects);
	llist_delete_elements(request->resources,(delete_element_func)xacml_resource_delete);
	llist_delete(request->resources);
	if (request->action != NULL) xacml_action_delete(request->action);
	if (request->environment != NULL) xacml_environment_delete(request->environment);
	free(request);
	request= NULL;
}

