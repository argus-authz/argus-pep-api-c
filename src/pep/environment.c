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
 * $Id: environment.c,v 1.2 2009/01/29 17:16:36 vtschopp Exp $
 */
#include <stdlib.h>

#include "util/linkedlist.h"
#include "util/log.h"
#include "pep/model.h"

struct pep_environment {
	linkedlist_t * attributes;
};

pep_environment_t * pep_environment_create() {
	pep_environment_t * env= calloc(1,sizeof(pep_environment_t));
	if (env == NULL) {
		log_error("pep_environment_create: can't allocate pep_environment_t.");
		return NULL;
	}
	env->attributes= llist_create();
	if (env->attributes == NULL) {
		log_error("pep_environment_create: can't create attributes list.");
		free(env);
		return NULL;
	}
	return env;
}

int pep_environment_addattribute(pep_environment_t * env, pep_attribute_t * attr) {
	if (env == NULL || attr == NULL) {
		log_error("pep_environment_addattribute: NULL environment or attribute.");
		return PEP_MODEL_ERROR;
	}
	if (llist_add(env->attributes,attr) != LLIST_OK) {
		log_error("pep_environment_addattribute: can't add attribute to list.");
		return PEP_MODEL_ERROR;
	}
	else return PEP_MODEL_OK;
}

size_t pep_environment_attributes_length(const pep_environment_t * env) {
	if (env == NULL) {
		log_error("pep_environment_attributes_length: NULL environment.");
		return PEP_MODEL_ERROR;
	}
	return llist_length(env->attributes);

}

pep_attribute_t * pep_environment_getattribute(const pep_environment_t * env, int index) {
	if (env == NULL) {
		log_error("pep_environment_getattribute: NULL environment.");
		return NULL;
	}
	return llist_get(env->attributes, index);

}

void pep_environment_delete(pep_environment_t * env) {
	if (env == NULL) return;
	llist_delete_elements(env->attributes,(delete_element_func)pep_attribute_delete);
	llist_delete(env->attributes);
	free(env);
	env= NULL;
}

