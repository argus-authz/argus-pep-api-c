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

#include "linkedlist.h" /* ../util/linkedlist.h */
#include "log.h" /* ../util/log.h */
#include "xacml.h"

struct xacml_obligation {
    char * id; /* mandatory */
    xacml_fulfillon_t fulfillon; /* optional */
    linkedlist_t * assignments; /* AttributeAssignments list */
};

/* id can be NULL */
xacml_obligation_t * xacml_obligation_create(const char * id) {
    xacml_obligation_t * obligation= calloc(1,sizeof(xacml_obligation_t));
    if (obligation == NULL) {
        log_error("xacml_obligation_create: can't allocate xacml_obligation_t.");
        return NULL;
    }
    obligation->id= NULL;
    if (id != NULL) {
        size_t size= strlen(id);
        obligation->id= calloc(size + 1,sizeof(char));
        if (obligation->id == NULL) {
            log_error("xacml_obligation_create: can't allocate id (%d bytes).",(int)size);
            free(obligation);
            return NULL;
        }
        strncpy(obligation->id,id,size);
    }
    obligation->assignments= llist_create();
    if (obligation->assignments == NULL) {
        log_error("xacml_obligation_create: can't create assignments list.");
        free(obligation->id);
        free(obligation);
        return NULL;
    }
    obligation->fulfillon= XACML_FULFILLON_DENY;
    return obligation;
}

/* id can't be NULL */
int xacml_obligation_setid(xacml_obligation_t * obligation, const char * id) {
    size_t size;
    if (obligation == NULL) {
        log_error("xacml_obligation_setid: NULL obligation.");
        return PEP_XACML_ERROR;
    }
    if (id == NULL) {
        log_error("xacml_obligation_setid: NULL id.");
        return PEP_XACML_ERROR;
    }
    if (obligation->id != NULL) {
        free(obligation->id);
    }
    size= strlen(id);
    obligation->id= calloc(size + 1,sizeof(char));
    if (obligation->id == NULL) {
        log_error("xacml_obligation_setid: can't allocate id (%d bytes).", (int)size);
        return PEP_XACML_ERROR;
    }
    strncpy(obligation->id,id,size);
    return PEP_XACML_OK;

}
const char * xacml_obligation_getid(const xacml_obligation_t * obligation) {
    if (obligation == NULL) {
        log_error("xacml_obligation_getfulfillon: NULL obligation.");
        return NULL;
    }
    return obligation->id;
}


xacml_fulfillon_t xacml_obligation_getfulfillon(const xacml_obligation_t * obligation) {
    if (obligation == NULL) {
        log_error("xacml_obligation_getfulfillon: NULL obligation.");
        return PEP_XACML_ERROR;
    }
    return obligation->fulfillon;
}

int xacml_obligation_setfulfillon(xacml_obligation_t * obligation, xacml_fulfillon_t fulfillon) {
    if (obligation == NULL) {
        log_error("xacml_obligation_setfulfillon: NULL obligation.");
        return PEP_XACML_ERROR;
    }
    switch (fulfillon) {
        case XACML_FULFILLON_DENY:
        case XACML_FULFILLON_PERMIT:
            obligation->fulfillon= fulfillon;
            break;
        default:
            log_error("xacml_obligation_setfulfillon: invalid fulfillon: %d.", fulfillon);
            return PEP_XACML_ERROR;
            break;
    }
    return PEP_XACML_OK;
}

int xacml_obligation_addattributeassignment(xacml_obligation_t * obligation, xacml_attributeassignment_t * attr) {
    if (obligation == NULL) {
        log_error("xacml_obligation_addattributeassignment: NULL obligation.");
        return PEP_XACML_ERROR;
    }
    if (attr == NULL) {
        log_error("xacml_obligation_addattributeassignment: NULL attribute assignment.");
        return PEP_XACML_ERROR;
    }
    if (llist_add(obligation->assignments,attr) != LLIST_OK) {
        log_error("xacml_obligation_addattributeassignment: can't add attribute assignment to list.");
        return PEP_XACML_ERROR;

    }
    return PEP_XACML_OK;
}

size_t xacml_obligation_attributeassignments_length(const xacml_obligation_t * obligation) {
    if (obligation == NULL) {
        log_warn("xacml_obligation_attributeassignments_length: NULL obligation.");
        return 0;
    }
    return llist_length(obligation->assignments);
}

xacml_attributeassignment_t * xacml_obligation_getattributeassignment(const xacml_obligation_t * obligation,int i) {
    if (obligation == NULL) {
        log_error("xacml_obligation_getattributeassignment: NULL obligation.");
        return NULL;
    }
    return llist_get(obligation->assignments,i);
}

void xacml_obligation_delete(xacml_obligation_t * obligation) {
    if (obligation == NULL) return;
    if (obligation->id != NULL) free(obligation->id);
    llist_delete_elements(obligation->assignments,(delete_element_func)xacml_attributeassignment_delete);
    llist_delete(obligation->assignments);
    free(obligation);
    obligation= NULL;
}
