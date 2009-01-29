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
 * $Id: action.c,v 1.2 2009/01/29 17:16:36 vtschopp Exp $
 */
#include <stdlib.h>

#include "util/linkedlist.h"
#include "util/log.h"
#include "pep/model.h"

struct pep_action {
	linkedlist_t * attributes;
};

pep_action_t * pep_action_create() {
	pep_action_t * action= calloc(1,sizeof(pep_action_t));
	if (action == NULL) {
		log_error("pep_action_create: can't allocate pep_action_t.");
		return NULL;
	}
	action->attributes= llist_create();
	if (action->attributes == NULL) {
		log_error("pep_action_create: can't create attributes list.");
		free(action);
		return NULL;
	}
	return action;
}

int pep_action_addattribute(pep_action_t * action, pep_attribute_t * attr) {
	if (action == NULL || attr == NULL) {
		log_error("pep_action_addattribute: NULL action or attribute.");
		return PEP_MODEL_ERROR;
	}
	if (llist_add(action->attributes,attr) != LLIST_OK) {
		log_error("pep_action_addattribute: can't add attribute to list.");
		return PEP_MODEL_ERROR;
	}
	else return PEP_MODEL_OK;
}

void pep_action_delete(pep_action_t * action) {
	if (action == NULL) return;
	llist_delete_elements(action->attributes,(delete_element_func)pep_attribute_delete);
	llist_delete(action->attributes);
	free(action);
	action= NULL;
}

size_t pep_action_attributes_length(const pep_action_t * action) {
	if (action == NULL) {
		log_error("pep_action_attributes_length: NULL action.");
		return PEP_MODEL_ERROR;
	}
	return llist_length(action->attributes);
}

pep_attribute_t * pep_action_getattribute(const pep_action_t * action, int index) {
	if (action == NULL) {
		log_error("pep_action_getattribute: NULL action.");
		return NULL;
	}
	return llist_get(action->attributes, index);
}

