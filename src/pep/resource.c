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
 * $Id: resource.c,v 1.1 2008/12/12 11:34:27 vtschopp Exp $
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "util/linkedlist.h"
#include "pep/model.h"

struct pep_resource {
	char * content;
	linkedlist_t * attributes;
};

pep_resource_t * pep_resource_create() {
	pep_resource_t * resource= calloc(1,sizeof(pep_resource_t));
	if (resource == NULL) {
		fprintf(stderr,"ERROR:pep_resource_create: can't allocate pep_resource_t.\n");
		return NULL;
	}
	resource->attributes= llist_create();
	if (resource->attributes == NULL) {
		fprintf(stderr,"ERROR:pep_resource_create: can't allocate attributes list.\n");
		free(resource);
		return NULL;
	}
	resource->content= NULL;
	return resource;
}

int pep_resource_addattribute(pep_resource_t * resource, pep_attribute_t * attr) {
	if (resource == NULL || attr == NULL) {
		fprintf(stderr,"ERROR:pep_resource_addattribute: NULL resource or attribute.\n");
		return PEP_MODEL_ERROR;
	}
	if (llist_add(resource->attributes,attr) != LLIST_OK) {
		fprintf(stderr,"ERROR:pep_resource_addattribute: can't add attribute to list.\n");
		return PEP_MODEL_ERROR;
	}
	else return PEP_MODEL_OK;
}

size_t pep_resource_attributes_length(const pep_resource_t * resource) {
	if (resource == NULL) {
		fprintf(stderr,"ERROR:pep_resource_attributes_length: NULL resource.\n");
		return PEP_MODEL_ERROR;
	}
	return llist_length(resource->attributes);
}

pep_attribute_t * pep_resource_getattribute(const pep_resource_t * resource, int index) {
	if (resource == NULL) {
		fprintf(stderr,"ERROR:pep_resource_getattribute: NULL resource.\n");
		return NULL;
	}
	return llist_get(resource->attributes, index);
}

int pep_resource_setcontent(pep_resource_t * resource, const char * content) {
	if (resource == NULL || content == NULL) {
		fprintf(stderr,"ERROR:pep_resource_setcontent: NULL resource or content.\n");
		return PEP_MODEL_ERROR;
	}
	if (resource->content != NULL) {
		free(resource->content);
	}
	size_t size= strlen(content);
	resource->content= calloc(size + 1, sizeof(char));
	if (resource->content == NULL) {
		fprintf(stderr,"ERROR:pep_resource_setcontent: can't allocate content (%d bytes).\n", (int)size);
		return PEP_MODEL_ERROR;
	}
	strncpy(resource->content,content,size);
	return PEP_MODEL_OK;
}

const char * pep_resource_getcontent(const pep_resource_t * resource) {
	if (resource == NULL) {
		fprintf(stderr,"ERROR:pep_resource_getcontent: NULL resource.\n");
		return NULL;
	}
	return resource->content;
}

void pep_resource_delete(pep_resource_t * resource) {
	if (resource == NULL) return;
	llist_delete_elements(resource->attributes,(delete_element_func)pep_attribute_delete);
	llist_delete(resource->attributes);
	if (resource->content != NULL) free(resource->content);
	free(resource);
	resource= NULL;
}
