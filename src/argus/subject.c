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
#include <string.h>

#include "util/linkedlist.h"
#include "util/log.h"
#include "argus/xacml.h"

struct xacml_subject {
	char * category;
	linkedlist_t * attributes;
};

xacml_subject_t * xacml_subject_create() {
	xacml_subject_t * subject= calloc(1,sizeof(xacml_subject_t));
	if (subject == NULL) {
		log_error("xacml_subject_create: can't allocate xacml_subject_t.");
		return NULL;
	}
	subject->attributes= llist_create();
	if (subject->attributes == NULL) {
		log_error("xacml_subject_create: can't allocate attributes list.");
		free(subject);
		return NULL;
	}
	subject->category= NULL;
	return subject;
}

/* category == NULL will delete the existing one. */
int xacml_subject_setcategory(xacml_subject_t * subject, const char * category) {
	if (subject == NULL) {
		log_error("xacml_subject_setcategory: NULL subject.");
		return PEP_XACML_ERROR;
	}
	if (subject->category != NULL) {
		free(subject->category);
	}
	if (category != NULL) {
		size_t size= strlen(category);
		subject->category= calloc(size + 1, sizeof(char));
		if (subject->category == NULL) {
			log_error("xacml_subject_setcategory: can't allocate category (%d bytes).", (int)size);
			return PEP_XACML_ERROR;
		}
		strncpy(subject->category,category,size);
	}
	return PEP_XACML_OK;
}

const char * xacml_subject_getcategory(const xacml_subject_t * subject) {
	if (subject == NULL) {
		log_error("xacml_subject_getcategory: NULL subject.");
		return NULL;
	}
	return subject->category;
}

int xacml_subject_addattribute(xacml_subject_t * subject, xacml_attribute_t * attr) {
	if (subject == NULL || attr == NULL) {
		log_error("xacml_subject_addattribute: NULL subject or attribute.");
		return PEP_XACML_ERROR;
	}
	if (llist_add(subject->attributes,attr) != LLIST_OK) {
		log_error("xacml_subject_addattribute: can't add attribute to list.");
		return PEP_XACML_ERROR;
	}
	else return PEP_XACML_OK;
}

size_t xacml_subject_attributes_length(const xacml_subject_t * subject) {
	if (subject == NULL) {
		log_warn("xacml_subject_attributes_length: NULL subject.");
		return 0;
	}
	return llist_length(subject->attributes);
}

xacml_attribute_t * xacml_subject_getattribute(const xacml_subject_t * subject, int index) {
	if (subject == NULL) {
		log_error("xacml_subject_getattribute: NULL subject.");
		return NULL;
	}
	return llist_get(subject->attributes, index);
}

void xacml_subject_delete(xacml_subject_t * subject) {
	if (subject == NULL) return;
	llist_delete_elements(subject->attributes,(delete_element_func)xacml_attribute_delete);
	llist_delete(subject->attributes);
	if (subject->category != NULL) {
		free(subject->category);
	}
	free(subject);
	subject= NULL;
}

