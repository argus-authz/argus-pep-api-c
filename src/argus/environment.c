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

#include <stdlib.h>

#include "util/linkedlist.h"
#include "util/log.h"
#include "argus/xacml.h"

struct xacml_environment {
	linkedlist_t * attributes;
};

xacml_environment_t * xacml_environment_create() {
	xacml_environment_t * env= calloc(1,sizeof(xacml_environment_t));
	if (env == NULL) {
		log_error("xacml_environment_create: can't allocate xacml_environment_t.");
		return NULL;
	}
	env->attributes= llist_create();
	if (env->attributes == NULL) {
		log_error("xacml_environment_create: can't create attributes list.");
		free(env);
		return NULL;
	}
	return env;
}

int xacml_environment_addattribute(xacml_environment_t * env, xacml_attribute_t * attr) {
	if (env == NULL || attr == NULL) {
		log_error("xacml_environment_addattribute: NULL environment or attribute.");
		return PEP_XACML_ERROR;
	}
	if (llist_add(env->attributes,attr) != LLIST_OK) {
		log_error("xacml_environment_addattribute: can't add attribute to list.");
		return PEP_XACML_ERROR;
	}
	else return PEP_XACML_OK;
}

size_t xacml_environment_attributes_length(const xacml_environment_t * env) {
	if (env == NULL) {
		log_warn("xacml_environment_attributes_length: NULL environment.");
		return 0;
	}
	return llist_length(env->attributes);

}

xacml_attribute_t * xacml_environment_getattribute(const xacml_environment_t * env, int index) {
	if (env == NULL) {
		log_error("xacml_environment_getattribute: NULL environment.");
		return NULL;
	}
	return llist_get(env->attributes, index);

}

void xacml_environment_delete(xacml_environment_t * env) {
	if (env == NULL) return;
	llist_delete_elements(env->attributes,(delete_element_func)xacml_attribute_delete);
	llist_delete(env->attributes);
	free(env);
	env= NULL;
}

