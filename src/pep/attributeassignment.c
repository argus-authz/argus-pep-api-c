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

#include "util/log.h"
#include "pep/xacml.h"

struct xacml_attributeassignment {
    char * id; /* mandatory */
    char * datatype;
    char * value;
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
    return attr;
}

/**
 * Sets the PEP attribute id. id is mandatory and can't be NULL.
 */
int xacml_attributeassignment_setid(xacml_attributeassignment_t * attr, const char * id) {
    size_t size;
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
    size= strlen(id);
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

/**
 * Sets the AttributeAssignment Datatype, can be NULL
 */
int xacml_attributeassignment_setdatatype(xacml_attributeassignment_t * attr, const char * datatype) {
    if (attr == NULL) {
        log_error("xacml_attributeassignment_setdatatype: NULL attribute.");
        return PEP_XACML_ERROR;
    }

    if (attr->datatype != NULL) {
        free(attr->datatype);
    }

    attr->datatype= NULL;
    if (datatype!=NULL) {
        size_t size= strlen(datatype);
        attr->datatype= calloc(size + 1,sizeof(char));
        if (attr->datatype == NULL) {
            log_error("xacml_attributeassignment_setdatatype: can't allocate datatype (%d bytes).", (int)size);
            return PEP_XACML_ERROR;
        }
        strncpy(attr->datatype,datatype,size);
    }
    return PEP_XACML_OK;
}

/**
 * Returns the attribute assignment attribute datatype.
 */
const char * xacml_attributeassignment_getdatatype(const xacml_attributeassignment_t * attr) {
    if (attr == NULL) {
        log_error("xacml_attributeassignment_getdatatype: NULL attribute.");
        return NULL;
    }
    return attr->datatype;
}


/*
 * Set value at index 0
 */
int xacml_attributeassignment_addvalue(xacml_attributeassignment_t * attr, const char *value) {
    if (attr == NULL || value == NULL) {
        log_error("xacml_attributeassignment_addvalue: NULL attribute.");
        return PEP_XACML_ERROR;
    }
    return xacml_attributeassignment_setvalue(attr,value);
}

int xacml_attributeassignment_setvalue(xacml_attributeassignment_t * attr, const char *value) {
    if (attr == NULL) {
        log_error("xacml_attributeassignment_setvalue: NULL attribute.");
        return PEP_XACML_ERROR;
    }

    if (attr->value != NULL) {
        free(attr->value);
    }

    attr->value= NULL;
    if (value!=NULL) {
        size_t size= strlen(value);
        attr->value= calloc(size + 1,sizeof(char));
        if (attr->value == NULL) {
            log_error("xacml_attributeassignment_setvalue: can't allocate value (%d bytes).", (int)size);
            return PEP_XACML_ERROR;
        }
        strncpy(attr->value,value,size);
    }
    return PEP_XACML_OK;
}


/**
 * Returns the count of PEP attribute assignemnt values.
 */
size_t xacml_attributeassignment_values_length(const xacml_attributeassignment_t * attr) {
    if (attr == NULL) {
        log_warn("xacml_attributeassignment_values_length: NULL attribute.");
        return 0;
    }
    if (attr->value==NULL)
        return 0;
    else
        return 1;
}

/**
 * Always return value
 */
const char * xacml_attributeassignment_getvalue(const xacml_attributeassignment_t * attr,...) {
    if (attr == NULL) {
        log_error("xacml_attributeassignment_getvalue: NULL attribute.");
        return NULL;
    }
    return attr->value;
}



/**
 * Deletes the PEP attribute.
 */
void xacml_attributeassignment_delete(xacml_attributeassignment_t * attr) {
    if (attr == NULL) return;
    if (attr->id != NULL) free(attr->id);
    if (attr->datatype != NULL) free(attr->datatype);
    if (attr->value != NULL) free(attr->value);
    free(attr);
    attr= NULL;
}
