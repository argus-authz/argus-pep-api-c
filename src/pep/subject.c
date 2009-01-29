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
 * $Id: subject.c,v 1.3 2009/01/29 17:16:36 vtschopp Exp $
 */
#include <stdlib.h>
#include <string.h>

#include "util/linkedlist.h"
#include "util/log.h"
#include "pep/model.h"

struct pep_subject {
	char * category;
	linkedlist_t * attributes;
};

pep_subject_t * pep_subject_create() {
	pep_subject_t * subject= calloc(1,sizeof(pep_subject_t));
	if (subject == NULL) {
		log_error("pep_subject_create: can't allocate pep_subject_t.");
		return NULL;
	}
	subject->attributes= llist_create();
	if (subject->attributes == NULL) {
		log_error("pep_subject_create: can't allocate attributes list.");
		free(subject);
		return NULL;
	}
	subject->category= NULL;
	return subject;
}

// category == NULL will delete the existing one.
int pep_subject_setcategory(pep_subject_t * subject, const char * category) {
	if (subject == NULL) {
		log_error("pep_subject_setcategory: NULL subject.");
		return PEP_MODEL_ERROR;
	}
	if (subject->category != NULL) {
		free(subject->category);
	}
	if (category != NULL) {
		size_t size= strlen(category);
		subject->category= calloc(size + 1, sizeof(char));
		if (subject->category == NULL) {
			log_error("pep_subject_setcategory: can't allocate category (%d bytes).", (int)size);
			return PEP_MODEL_ERROR;
		}
		strncpy(subject->category,category,size);
	}
	return PEP_MODEL_OK;
}

const char * pep_subject_getcategory(const pep_subject_t * subject) {
	if (subject == NULL) {
		log_error("pep_subject_getcategory: NULL subject.");
		return NULL;
	}
	return subject->category;
}

int pep_subject_addattribute(pep_subject_t * subject, pep_attribute_t * attr) {
	if (subject == NULL || attr == NULL) {
		log_error("pep_subject_addattribute: NULL subject or attribute.");
		return PEP_MODEL_ERROR;
	}
	if (llist_add(subject->attributes,attr) != LLIST_OK) {
		log_error("pep_subject_addattribute: can't add attribute to list.");
		return PEP_MODEL_ERROR;
	}
	else return PEP_MODEL_OK;
}

size_t pep_subject_attributes_length(const pep_subject_t * subject) {
	if (subject == NULL) {
		log_error("pep_subject_attributes_length: NULL subject.");
		return PEP_MODEL_ERROR;
	}
	return llist_length(subject->attributes);
}

pep_attribute_t * pep_subject_getattribute(const pep_subject_t * subject, int index) {
	if (subject == NULL) {
		log_error("pep_subject_getattribute: NULL subject.");
		return NULL;
	}
	return llist_get(subject->attributes, index);
}

void pep_subject_delete(pep_subject_t * subject) {
	if (subject == NULL) return;
	llist_delete_elements(subject->attributes,(delete_element_func)pep_attribute_delete);
	llist_delete(subject->attributes);
	if (subject->category != NULL) {
		free(subject->category);
	}
	free(subject);
	subject= NULL;
}

