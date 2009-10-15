/*
 * Copyright 2008 Members of the EGEE Collaboration.
 * See http://www.eu-egee.org/partners for details on the copyright holders.
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
 * $Id$
 */
#include <stdlib.h>
#include <string.h>

#include "util/linkedlist.h"
#include "util/log.h"
#include "pep/xacml.h"

struct xacml_resource {
	char * content;
	linkedlist_t * attributes;
};

xacml_resource_t * xacml_resource_create() {
	xacml_resource_t * resource= calloc(1,sizeof(xacml_resource_t));
	if (resource == NULL) {
		log_error("xacml_resource_create: can't allocate xacml_resource_t.");
		return NULL;
	}
	resource->attributes= llist_create();
	if (resource->attributes == NULL) {
		log_error("xacml_resource_create: can't allocate attributes list.");
		free(resource);
		return NULL;
	}
	resource->content= NULL;
	return resource;
}

int xacml_resource_addattribute(xacml_resource_t * resource, xacml_attribute_t * attr) {
	if (resource == NULL || attr == NULL) {
		log_error("xacml_resource_addattribute: NULL resource or attribute.");
		return PEP_XACML_ERROR;
	}
	if (llist_add(resource->attributes,attr) != LLIST_OK) {
		log_error("xacml_resource_addattribute: can't add attribute to list.");
		return PEP_XACML_ERROR;
	}
	else return PEP_XACML_OK;
}

size_t xacml_resource_attributes_length(const xacml_resource_t * resource) {
	if (resource == NULL) {
		log_warn("xacml_resource_attributes_length: NULL resource.");
		return 0;
	}
	return llist_length(resource->attributes);
}

xacml_attribute_t * xacml_resource_getattribute(const xacml_resource_t * resource, int index) {
	if (resource == NULL) {
		log_error("xacml_resource_getattribute: NULL resource.");
		return NULL;
	}
	return llist_get(resource->attributes, index);
}

// if content is NULL, delete existing
int xacml_resource_setcontent(xacml_resource_t * resource, const char * content) {
	if (resource == NULL) {
		log_error("xacml_resource_setcontent: NULL resource pointer.");
		return PEP_XACML_ERROR;
	}
	if (resource->content != NULL) {
		free(resource->content);
	}
	if (content != NULL) {
		size_t size= strlen(content);
		resource->content= calloc(size + 1, sizeof(char));
		if (resource->content == NULL) {
			log_error("xacml_resource_setcontent: can't allocate content (%d bytes).", (int)size);
			return PEP_XACML_ERROR;
		}
		strncpy(resource->content,content,size);
	}
	return PEP_XACML_OK;
}

const char * xacml_resource_getcontent(const xacml_resource_t * resource) {
	if (resource == NULL) {
		log_error("xacml_resource_getcontent: NULL resource.");
		return NULL;
	}
	return resource->content;
}

void xacml_resource_delete(xacml_resource_t * resource) {
	if (resource == NULL) return;
	llist_delete_elements(resource->attributes,(delete_element_func)xacml_attribute_delete);
	llist_delete(resource->attributes);
	if (resource->content != NULL) free(resource->content);
	free(resource);
	resource= NULL;
}
