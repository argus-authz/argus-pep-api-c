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

struct xacml_attribute {
	char * id; // mandatory
	char * datatype; // optional
	char * issuer; // optional
	linkedlist_t * values; // string list
};

/**
 * Creates a PEP attribute with the given id.
 */
xacml_attribute_t * xacml_attribute_create(const char * id) {
	xacml_attribute_t * attr= calloc(1,sizeof(xacml_attribute_t));
	if (attr == NULL) {
		log_error("xacml_attribute_create: can't allocate xacml_attribute_t.");
		return NULL;
	}
	attr->id= NULL;
	if (id != NULL) {
		size_t size= strlen(id);
		attr->id= calloc(size + 1,sizeof(char));
		if (attr->id == NULL) {
			log_error("xacml_attribute_create: can't allocate id (%d bytes).",(int)size);
			free(attr);
			return NULL;
		}
		strncpy(attr->id,id,size);
	}
	attr->datatype= NULL;
	attr->issuer= NULL;
	attr->values= llist_create();
	if (attr->values == NULL) {
		log_error("xacml_attribute_create: can't create values list.");
		free(attr->id);
		free(attr);
		return NULL;
	}
	return attr;
}

/**
 * Sets the PEP attribute id. id is mandatory and can't be NULL.
 */
int xacml_attribute_setid(xacml_attribute_t * attr, const char * id) {
	if (attr == NULL) {
		log_error("xacml_attribute_setid: NULL attribute.");
		return PEP_XACML_ERROR;
	}
	if (id == NULL) {
		log_error("xacml_attribute_setid: NULL id.");
		return PEP_XACML_ERROR;
	}
	if (attr->id != NULL) {
		free(attr->id);
	}
	size_t size= strlen(id);
	attr->id= calloc(size + 1,sizeof(char));
	if (attr->id == NULL) {
		log_error("xacml_attribute_setid: can't allocate id (%d bytes).", (int)size);
		return PEP_XACML_ERROR;
	}
	strncpy(attr->id,id,size);
	return PEP_XACML_OK;
}

const char * xacml_attribute_getid(const xacml_attribute_t * attr) {
	if (attr == NULL) {
		log_error("xacml_attribute_getid: NULL attribute.");
		return NULL;
	}
	return attr->id;
}

/**
 * Sets the PEP attribute data type. NULL to delete existing datatype.
 */
int xacml_attribute_setdatatype(xacml_attribute_t * attr, const char * datatype) {
	if (attr == NULL) {
		log_error("xacml_attribute_setdatatype: NULL attribute.");
		return PEP_XACML_ERROR;
	}
	if (attr->datatype != NULL) {
		free(attr->datatype);
	}
	attr->datatype= NULL;
	if (datatype != NULL) {
		size_t size= strlen(datatype);
		attr->datatype= calloc(size + 1,sizeof(char));
		if (attr->datatype == NULL) {
			log_error("xacml_attribute_setdatatype: can't allocate datatype (%d bytes).", (int)size);
			return PEP_XACML_ERROR;
		}
		strncpy(attr->datatype,datatype,size);
	}
	return PEP_XACML_OK;
}

const char * xacml_attribute_getdatatype(const xacml_attribute_t * attr) {
	if (attr == NULL) {
		log_error("xacml_attribute_getdatatype: NULL attribute.");
		return NULL;
	}
	return attr->datatype;
}


/**
 * Sets the PEP attribute issuer. NULL to delete existing id.
 */
int xacml_attribute_setissuer(xacml_attribute_t * attr, const char * issuer) {
	if (attr == NULL) {
		log_error("xacml_attribute_setissuer: NULL attribute.");
		return PEP_XACML_ERROR;
	}
	if (attr->issuer != NULL) {
		free(attr->issuer);
	}
	attr->issuer= NULL;
	if (issuer != NULL) {
		size_t size= strlen(issuer);
		attr->issuer= calloc(size + 1,sizeof(char));
		if (attr->issuer == NULL) {
			log_error("xacml_attribute_setissuer: can't allocate issuer (%d bytes).", (int)size);
			return PEP_XACML_ERROR;
		}
		strncpy(attr->issuer,issuer,size);
	}
	return PEP_XACML_OK;

}

const char * xacml_attribute_getissuer(const xacml_attribute_t * attr) {
	if (attr == NULL) {
		log_error("xacml_attribute_getissuer: NULL attribute.");
		return NULL;
	}
	return attr->issuer;
}

/**
 * Adds a value to the PEP attribute.
 */
int xacml_attribute_addvalue(xacml_attribute_t * attr, const char *value) {
	if (attr == NULL || value == NULL) {
		log_error("xacml_attribute_addvalue: NULL attribute or value.");
		return PEP_XACML_ERROR;
	}
	// copy the const value
	size_t size= strlen(value);
	char * v= calloc(size + 1, sizeof(char));
	if (v == NULL) {
		log_error("xacml_attribute_addvalue: can't allocate value (%d bytes).", (int)size);
		return PEP_XACML_ERROR;
	}
	strncpy(v,value,size);
	if (llist_add(attr->values,v) != LLIST_OK) {
		log_error("xacml_attribute_addvalue: can't add value to list.");
		return PEP_XACML_ERROR;
	}
	else return PEP_XACML_OK;
}

size_t xacml_attribute_values_length(const xacml_attribute_t * attr) {
	if (attr == NULL) {
		log_warn("xacml_attribute_values_length: NULL attribute.");
		return 0;
	}
	return llist_length(attr->values);
}

const char * xacml_attribute_getvalue(const xacml_attribute_t * attr,int index) {
	if (attr == NULL) {
		log_error("xacml_attribute_getvalue: NULL attribute.");
		return NULL;
	}
	return llist_get(attr->values,index);
}

/**
 * Deletes the PEP attribute.
 */
void xacml_attribute_delete(xacml_attribute_t * attr) {
	if (attr == NULL) return;
	if (attr->id != NULL) free(attr->id);
	if (attr->datatype != NULL) free(attr->datatype);
	if (attr->issuer != NULL) free(attr->issuer);
	llist_delete_elements(attr->values,(delete_element_func)free);
	llist_delete(attr->values);
	free(attr);
	attr= NULL;
}

