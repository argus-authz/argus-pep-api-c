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
 * $Id: result.c,v 1.1 2008/12/12 11:34:27 vtschopp Exp $
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "util/linkedlist.h"
#include "pep/model.h"

struct pep_result {
	char * resourceid;
	pep_decision_t decision;
	pep_status_t * status;
	linkedlist_t * obligations; //
};

pep_result_t * pep_result_create() {
	pep_result_t * result= calloc(1,sizeof(pep_result_t));
	if (result == NULL) {
		fprintf(stderr,"ERROR:pep_result_create: can't allocate pep_result_t.\n");
		return NULL;
	}
	result->obligations= llist_create();
	if (result->obligations == NULL) {
		fprintf(stderr,"ERROR:pep_result_create: can't allocate obligations list.\n");
		free(result);
		return NULL;
	}
	result->decision= PEP_DECISION_DENY;
	result->resourceid= NULL;
	result->status= NULL;
	return result;
}

pep_decision_t pep_result_getdecision(const pep_result_t * result) {
	if (result == NULL) {
		fprintf(stderr,"ERROR:pep_result_getdecision: NULL result.\n");
		return -1; // FIXME: define DECISION_INVALID ??
	}
	return result->decision;
}

int pep_result_setdecision(pep_result_t * result, pep_decision_t decision) {
	if (result == NULL) {
		fprintf(stderr,"ERROR:pep_result_setdecision: NULL result.\n");
		return PEP_MODEL_ERROR;
	}
	switch (decision) {
		case PEP_DECISION_DENY:
		case PEP_DECISION_PERMIT:
		case PEP_DECISION_INDETERMINATE:
		case PEP_DECISION_NOT_APPLICABLE:
			result->decision= decision;
			break;
		default:
			fprintf(stderr,"ERROR:pep_result_setdecision: invalid decision: %d.\n", decision);
			return PEP_MODEL_ERROR;
			break;
	}
	return PEP_MODEL_OK;
}

const char * pep_result_getresourceid(const pep_result_t * result) {
	if (result == NULL) {
		fprintf(stderr,"ERROR:pep_result_getresourceid: NULL result.\n");
		return NULL;
	}
	return result->resourceid;
}

int pep_result_setresourceid(pep_result_t * result, const char * resourceid) {
	if (result == NULL) {
		fprintf(stderr,"ERROR:pep_result_setresourceid: NULL result object.\n");
		return PEP_MODEL_ERROR;
	}
	if (result->resourceid != NULL) {
		free(result->resourceid);
		result->resourceid= NULL;
	}
	if (resourceid != NULL) {
		size_t size= strlen(resourceid);
		result->resourceid= calloc(size + 1, sizeof(char));
		if (result->resourceid == NULL) {
			fprintf(stderr,"ERROR:pep_result_setresourceid: can't allocate resourceid (%d bytes).\n",(int)size);
			return PEP_MODEL_ERROR;
		}
		strncpy(result->resourceid,resourceid,size);
	}
	return PEP_MODEL_OK;
}

pep_status_t * pep_result_getstatus(const pep_result_t * result) {
	if (result == NULL) {
		fprintf(stderr,"ERROR:pep_result_getstatus: NULL result.\n");
		return NULL;
	}
	return result->status;
}

int pep_result_setstatus(pep_result_t * result, pep_status_t * status) {
	if (result == NULL || status == NULL) {
		fprintf(stderr,"ERROR:pep_result_setstatus: NULL result or status.\n");
		return PEP_MODEL_ERROR;
	}
	if (result->status != NULL) pep_status_delete(result->status);
	result->status= status;
	return PEP_MODEL_OK;
}

int pep_result_addobligation(pep_result_t * result, pep_obligation_t * obligation) {
	if (result == NULL || obligation == NULL) {
		fprintf(stderr,"ERROR:pep_result_addobligation: NULL result or obligation.\n");
		return PEP_MODEL_ERROR;
	}
	if (llist_add(result->obligations,obligation) != LLIST_OK) {
		fprintf(stderr,"ERROR:pep_result_addobligation: can't add obligation to list.\n");
		return PEP_MODEL_ERROR;
	}
	return PEP_MODEL_OK;
}

size_t pep_result_obligations_length(const pep_result_t * result) {
	if (result == NULL) {
		fprintf(stderr,"ERROR:pep_result_obligations_length: NULL result.\n");
		return PEP_MODEL_ERROR;
	}
	return llist_length(result->obligations);
}

pep_obligation_t * pep_result_getobligation(const pep_result_t * result, int i) {
	if (result == NULL) {
		fprintf(stderr,"ERROR:pep_result_getobligation: NULL result.\n");
		return NULL;
	}
	return llist_get(result->obligations,i);
}

void pep_result_delete(pep_result_t * result) {
	if (result == NULL) return;
	if (result->resourceid != NULL) free(result->resourceid);
	if (result->status != NULL) pep_status_delete(result->status);
	llist_delete_elements(result->obligations,(delete_element_func)pep_obligation_delete);
	llist_delete(result->obligations);
	free(result);
	result= NULL;
}

