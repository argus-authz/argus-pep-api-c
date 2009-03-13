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
 * $Id: attributeassignment.c,v 1.3 2009/03/13 12:02:17 vtschopp Exp $
 */
#include <stdlib.h>
#include <string.h>

#include "util/linkedlist.h"
#include "util/log.h"
#include "pep/xacml.h"

struct xacml_attributeassignment {
	char * id; // mandatory
	linkedlist_t * values; // string list
};

/**
 * Creates a PEP attribute assignment with the given id. id can be NULL, but not recommended.
 */
xacml_attributeassignment_t * xacml_attributeassignment_create(const char * id) {
	xacml_attributeassignment_t * attr= calloc(1,sizeof(xacml_attributeassignment_t));
	if (attr == NULL) {
		log_error("xacml_attributeassignment_create: can't allocate xacml_attributeassignment_t.");
		return NULL;
	}
	attr->id= NULL;
	if (id != NULL) {
		size_t size= strlen(id);
		attr->id= calloc(size + 1,sizeof(char));
		if (attr->id == NULL) {
			log_error("xacml_attributeassignment_create: can't allocate id (%d bytes).",(int)size);
			free(attr);
			return NULL;
		}
		strncpy(attr->id,id,size);
	}
	attr->values= llist_create();
	if (attr->values == NULL) {
		log_error("xacml_attributeassignment_create: can't create values list.");
		free(attr->id);
		free(attr);
		return NULL;
	}
	return attr;
}

/**
 * Sets the PEP attribute id. id is mandatory and can't be NULL.
 */
int xacml_attributeassignment_setid(xacml_attributeassignment_t * attr, const char * id) {
	if (attr == NULL) {
		log_error("xacml_attributeassignment_setid: NULL attribute.");
		return PEP_XACML_ERROR;
	}
	if (id == NULL) {
		log_error("xacml_attributeassignment_setid: NULL id.");
		return PEP_XACML_ERROR;
	}
	if (attr->id != NULL) {
		free(attr->id);
	}
	size_t size= strlen(id);
	attr->id= calloc(size + 1,sizeof(char));
	if (attr->id == NULL) {
		log_error("xacml_attributeassignment_setid: can't allocate id (%d bytes).", (int)size);
		return PEP_XACML_ERROR;
	}
	strncpy(attr->id,id,size);
	return PEP_XACML_OK;
}

/**
 * Returns the PEP attribute assignment attribute id.
 */
const char * xacml_attributeassignment_getid(const xacml_attributeassignment_t * attr) {
	if (attr == NULL) {
		log_error("xacml_attributeassignment_getid: NULL attribute.");
		return NULL;
	}
	return attr->id;
}

int xacml_attributeassignment_addvalue(xacml_attributeassignment_t * attr, const char *value) {
	if (attr == NULL || value == NULL) {
		log_error("xacml_attributeassignment_addvalue: NULL attribute or value.");
		return PEP_XACML_ERROR;
	}
	// copy the const value
	size_t size= strlen(value);
	char * v= calloc(size + 1, sizeof(char));
	if (v == NULL) {
		log_error("xacml_attributeassignment_addvalue: can't allocate value (%d bytes).", (int)size);
		return PEP_XACML_ERROR;
	}
	strncpy(v,value,size);
	if (llist_add(attr->values,v) != LLIST_OK) {
		log_error("xacml_attributeassignment_addvalue: can't add value to list.");
		return PEP_XACML_ERROR;
	}
	else return PEP_XACML_OK;
}
/**
 * Returns the count of PEP attribute assignemnt values.
 */
size_t xacml_attributeassignment_values_length(const xacml_attributeassignment_t * attr) {
	if (attr == NULL) {
		log_error("xacml_attributeassignment_values_length: NULL attribute.");
		return PEP_XACML_ERROR;
	}
	return llist_length(attr->values);
}

/**
 * Returns the PEP attribute assignment value at index i
 */
const char * xacml_attributeassignment_getvalue(const xacml_attributeassignment_t * attr,int i) {
	if (attr == NULL) {
		log_error("xacml_attributeassignment_getvalue: NULL attribute.");
		return NULL;
	}
	return llist_get(attr->values,i);
}



/**
 * Deletes the PEP attribute.
 */
void xacml_attributeassignment_delete(xacml_attributeassignment_t * attr) {
	if (attr == NULL) return;
	free(attr->id);
	llist_delete_elements(attr->values,(delete_element_func)free);
	llist_delete(attr->values);
	free(attr);
	attr= NULL;
}
