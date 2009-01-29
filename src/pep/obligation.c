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
 * $Id: obligation.c,v 1.2 2009/01/29 17:16:36 vtschopp Exp $
 */
#include <stdlib.h>
#include <string.h>

#include "util/linkedlist.h"
#include "util/log.h"
#include "pep/model.h"

struct pep_obligation {
	char * id; // mandatory
	pep_fulfillon_t fulfillon; // optional
	linkedlist_t * assignments; // AttributeAssignments list
};

// id can be NULL
pep_obligation_t * pep_obligation_create(const char * id) {
	pep_obligation_t * obligation= calloc(1,sizeof(pep_obligation_t));
	if (obligation == NULL) {
		log_error("pep_obligation_create: can't allocate pep_obligation_t.");
		return NULL;
	}
	obligation->id= NULL;
	if (id != NULL) {
		size_t size= strlen(id);
		obligation->id= calloc(size + 1,sizeof(char));
		if (obligation->id == NULL) {
			log_error("pep_obligation_create: can't allocate id (%d bytes).",(int)size);
			free(obligation);
			return NULL;
		}
		strncpy(obligation->id,id,size);
	}
	obligation->assignments= llist_create();
	if (obligation->assignments == NULL) {
		log_error("pep_obligation_create: can't create assignments list.");
		free(obligation->id);
		free(obligation);
		return NULL;
	}
	obligation->fulfillon= PEP_FULFILLON_DENY;
	return obligation;
}

// id can't be NULL
int pep_obligation_setid(pep_obligation_t * obligation, const char * id) {
	if (obligation == NULL) {
		log_error("pep_obligation_setid: NULL obligation.");
		return PEP_MODEL_ERROR;
	}
	if (id == NULL) {
		log_error("pep_obligation_setid: NULL id.");
		return PEP_MODEL_ERROR;
	}
	if (obligation->id != NULL) {
		free(obligation->id);
	}
	size_t size= strlen(id);
	obligation->id= calloc(size + 1,sizeof(char));
	if (obligation->id == NULL) {
		log_error("pep_obligation_setid: can't allocate id (%d bytes).", (int)size);
		return PEP_MODEL_ERROR;
	}
	strncpy(obligation->id,id,size);
	return PEP_MODEL_OK;

}
const char * pep_obligation_getid(const pep_obligation_t * obligation) {
	if (obligation == NULL) {
		log_error("pep_obligation_getfulfillon: NULL obligation.");
		return NULL;
	}
	return obligation->id;
}


pep_fulfillon_t pep_obligation_getfulfillon(const pep_obligation_t * obligation) {
	if (obligation == NULL) {
		log_error("pep_obligation_getfulfillon: NULL obligation.");
		return PEP_MODEL_ERROR;
	}
	return obligation->fulfillon;
}

int pep_obligation_setfulfillon(pep_obligation_t * obligation, pep_fulfillon_t fulfillon) {
	if (obligation == NULL) {
		log_error("pep_obligation_setfulfillon: NULL obligation.");
		return PEP_MODEL_ERROR;
	}
	switch (fulfillon) {
		case PEP_FULFILLON_DENY:
		case PEP_FULFILLON_PERMIT:
			obligation->fulfillon= fulfillon;
			break;
		default:
			log_error("pep_obligation_setfulfillon: invalid fulfillon: %d.", fulfillon);
			return PEP_MODEL_ERROR;
			break;
	}
	return PEP_MODEL_OK;
}

int pep_obligation_addattributeassignment(pep_obligation_t * obligation, pep_attribute_assignment_t * attr) {
	if (obligation == NULL) {
		log_error("pep_obligation_addattributeassignment: NULL obligation.");
		return PEP_MODEL_ERROR;
	}
	if (attr == NULL) {
		log_error("pep_obligation_addattributeassignment: NULL attribute assignment.");
		return PEP_MODEL_ERROR;
	}
	if (llist_add(obligation->assignments,attr) != LLIST_OK) {
		log_error("pep_obligation_addattributeassignment: can't add attribute assignment to list.");
		return PEP_MODEL_ERROR;

	}
	return PEP_MODEL_OK;
}

size_t pep_obligation_attributeassignments_length(const pep_obligation_t * obligation) {
	if (obligation == NULL) {
		log_error("pep_obligation_attributeassignments_length: NULL obligation.");
		return PEP_MODEL_ERROR;
	}
	return llist_length(obligation->assignments);
}

pep_attribute_assignment_t * pep_obligation_getattributeassignment(const pep_obligation_t * obligation,int i) {
	if (obligation == NULL) {
		log_error("pep_obligation_getattributeassignment: NULL obligation.");
		return NULL;
	}
	return llist_get(obligation->assignments,i);
}

void pep_obligation_delete(pep_obligation_t * obligation) {
	if (obligation == NULL) return;
	if (obligation->id != NULL) free(obligation->id);
	llist_delete_elements(obligation->assignments,(delete_element_func)pep_attribute_assignment_delete);
	llist_delete(obligation->assignments);
	free(obligation);
	obligation= NULL;
}
