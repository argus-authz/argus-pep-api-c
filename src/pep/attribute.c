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
 * $Id: attribute.c,v 1.1 2008/12/12 11:34:27 vtschopp Exp $
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "util/linkedlist.h"
#include "pep/model.h"

struct pep_attribute {
	char * id; // mandatory
	char * datatype; // optional
	char * issuer; // optional
	linkedlist_t * values; // string list
};

/**
 * Creates a PEP attribute with the given id.
 */
pep_attribute_t * pep_attribute_create(const char * id) {
	pep_attribute_t * attr= calloc(1,sizeof(pep_attribute_t));
	if (attr == NULL) {
		fprintf(stderr,"ERROR:pep_attribute_create: can't allocate pep_attribute_t.\n");
		return NULL;
	}
	attr->id= NULL;
	if (id != NULL) {
		size_t size= strlen(id);
		attr->id= calloc(size + 1,sizeof(char));
		if (attr->id == NULL) {
			fprintf(stderr,"ERROR:pep_attribute_create: can't allocate id (%d bytes).\n",(int)size);
			free(attr);
			return NULL;
		}
		strncpy(attr->id,id,size);
	}
	attr->datatype= NULL;
	attr->issuer= NULL;
	attr->values= llist_create();
	if (attr->values == NULL) {
		fprintf(stderr,"ERROR:pep_attribute_create: can't create values list.\n");
		free(attr->id);
		free(attr);
		return NULL;
	}
	return attr;
}

/**
 * Sets the PEP attribute id. id is mandatory and can't be NULL.
 */
int pep_attribute_setid(pep_attribute_t * attr, const char * id) {
	if (attr == NULL) {
		fprintf(stderr,"ERROR:pep_attribute_setid: NULL attribute.\n");
		return PEP_MODEL_ERROR;
	}
	if (id == NULL) {
		fprintf(stderr,"ERROR:pep_attribute_setid: NULL id.\n");
		return PEP_MODEL_ERROR;
	}
	if (attr->id != NULL) {
		free(attr->id);
	}
	size_t size= strlen(id);
	attr->id= calloc(size + 1,sizeof(char));
	if (attr->id == NULL) {
		fprintf(stderr,"ERROR:pep_attribute_setid: can't allocate id (%d bytes).\n", (int)size);
		return PEP_MODEL_ERROR;
	}
	strncpy(attr->id,id,size);
	return PEP_MODEL_OK;
}

const char * pep_attribute_getid(const pep_attribute_t * attr) {
	if (attr == NULL) {
		fprintf(stderr,"ERROR:pep_attribute_getid: NULL attribute.\n");
		return NULL;
	}
	return attr->id;
}

/**
 * Sets the PEP attribute data type. NULL to delete existing datatype.
 */
int pep_attribute_setdatatype(pep_attribute_t * attr, const char * datatype) {
	if (attr == NULL) {
		fprintf(stderr,"ERROR:pep_attribute_setdatatype: NULL attribute.\n");
		return PEP_MODEL_ERROR;
	}
	if (attr->datatype != NULL) {
		free(attr->datatype);
	}
	attr->datatype= NULL;
	if (datatype != NULL) {
		size_t size= strlen(datatype);
		attr->datatype= calloc(size + 1,sizeof(char));
		if (attr->datatype == NULL) {
			fprintf(stderr,"ERROR:pep_attribute_setdatatype: can't allocate datatype (%d bytes).\n", (int)size);
			return PEP_MODEL_ERROR;
		}
		strncpy(attr->datatype,datatype,size);
	}
	return PEP_MODEL_OK;
}

const char * pep_attribute_getdatatype(const pep_attribute_t * attr) {
	if (attr == NULL) {
		fprintf(stderr,"ERROR:pep_attribute_getdatatype: NULL attribute.\n");
		return NULL;
	}
	return attr->datatype;
}


/**
 * Sets the PEP attribute issuer. NULL to delete existing id.
 */
int pep_attribute_setissuer(pep_attribute_t * attr, const char * issuer) {
	if (attr == NULL) {
		fprintf(stderr,"ERROR:pep_attribute_setissuer: NULL attribute.\n");
		return PEP_MODEL_ERROR;
	}
	if (attr->issuer != NULL) {
		free(attr->issuer);
	}
	attr->issuer= NULL;
	if (issuer != NULL) {
		size_t size= strlen(issuer);
		attr->issuer= calloc(size + 1,sizeof(char));
		if (attr->issuer == NULL) {
			fprintf(stderr,"ERROR:pep_attribute_setissuer: can't allocate issuer (%d bytes).\n", (int)size);
			return PEP_MODEL_ERROR;
		}
		strncpy(attr->issuer,issuer,size);
	}
	return PEP_MODEL_OK;

}

const char * pep_attribute_getissuer(const pep_attribute_t * attr) {
	if (attr == NULL) {
		fprintf(stderr,"ERROR:pep_attribute_getissuer: NULL attribute.\n");
		return NULL;
	}
	return attr->issuer;
}

/**
 * Adds a value to the PEP attribute.
 */
int pep_attribute_addvalue(pep_attribute_t * attr, const char *value) {
	if (attr == NULL || value == NULL) {
		fprintf(stderr,"ERROR:pep_attribute_addvalue: NULL attribute or value.\n");
		return PEP_MODEL_ERROR;
	}
	// copy the const value
	size_t size= strlen(value);
	char * v= calloc(size + 1, sizeof(char));
	if (v == NULL) {
		fprintf(stderr,"ERROR:pep_attribute_addvalue: can't allocate value (%d bytes).\n", (int)size);
		return PEP_MODEL_ERROR;
	}
	strncpy(v,value,size);
	if (llist_add(attr->values,v) != LLIST_OK) {
		fprintf(stderr,"ERROR:pep_attribute_addvalue: can't add value to list.\n");
		return PEP_MODEL_ERROR;
	}
	else return PEP_MODEL_OK;
}

size_t pep_attribute_values_length(const pep_attribute_t * attr) {
	if (attr == NULL) {
		fprintf(stderr,"ERROR:pep_attribute_values_length: NULL attribute.\n");
		return PEP_MODEL_ERROR;
	}
	return llist_length(attr->values);
}

const char * pep_attribute_getvalue(const pep_attribute_t * attr,int index) {
	if (attr == NULL) {
		fprintf(stderr,"ERROR:pep_attribute_getvalue: NULL attribute.\n");
		return NULL;
	}
	return llist_get(attr->values,index);
}

/**
 * Deletes the PEP attribute.
 */
void pep_attribute_delete(pep_attribute_t * attr) {
	if (attr == NULL) return;
	if (attr->id != NULL) free(attr->id);
	if (attr->datatype != NULL) free(attr->datatype);
	if (attr->issuer != NULL) free(attr->issuer);
	llist_delete_elements(attr->values,(delete_element_func)free);
	llist_delete(attr->values);
	free(attr);
	attr= NULL;
}

