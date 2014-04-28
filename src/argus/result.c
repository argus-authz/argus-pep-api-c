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

/* from ../util */
#include "linkedlist.h"
#include "log.h"

#include "xacml.h"

struct xacml_result {
    char * resourceid;
    xacml_decision_t decision;
    xacml_status_t * status;
    pep_linkedlist_t * obligations; /* */
};

xacml_result_t * xacml_result_create() {
    xacml_result_t * result= calloc(1,sizeof(struct xacml_result));
    if (result == NULL) {
        pep_log_error("xacml_result_create: can't allocate xacml_result_t.");
        return NULL;
    }
    result->obligations= pep_llist_create();
    if (result->obligations == NULL) {
        pep_log_error("xacml_result_create: can't allocate obligations list.");
        free(result);
        return NULL;
    }
    result->decision= XACML_DECISION_DENY;
    result->resourceid= NULL;
    result->status= NULL;
    return result;
}

xacml_decision_t xacml_result_getdecision(const xacml_result_t * result) {
    if (result == NULL) {
        pep_log_error("xacml_result_getdecision: NULL result.");
        return -1; /* FIXME: define DECISION_INVALID ?? */
    }
    return result->decision;
}

int xacml_result_setdecision(xacml_result_t * result, xacml_decision_t decision) {
    if (result == NULL) {
        pep_log_error("xacml_result_setdecision: NULL result.");
        return PEP_XACML_ERROR;
    }
    switch (decision) {
        case XACML_DECISION_DENY:
        case XACML_DECISION_PERMIT:
        case XACML_DECISION_INDETERMINATE:
        case XACML_DECISION_NOT_APPLICABLE:
            result->decision= decision;
            break;
        default:
            pep_log_error("xacml_result_setdecision: invalid decision: %d.", decision);
            return PEP_XACML_ERROR;
            break;
    }
    return PEP_XACML_OK;
}

const char * xacml_result_getresourceid(const xacml_result_t * result) {
    if (result == NULL) {
        pep_log_error("xacml_result_getresourceid: NULL result.");
        return NULL;
    }
    return result->resourceid;
}

int xacml_result_setresourceid(xacml_result_t * result, const char * resourceid) {
    if (result == NULL) {
        pep_log_error("xacml_result_setresourceid: NULL result object.");
        return PEP_XACML_ERROR;
    }
    if (result->resourceid != NULL) {
        free(result->resourceid);
        result->resourceid= NULL;
    }
    if (resourceid != NULL) {
        size_t size= strlen(resourceid);
        result->resourceid= calloc(size + 1, sizeof(char));
        if (result->resourceid == NULL) {
            pep_log_error("xacml_result_setresourceid: can't allocate resourceid (%d bytes).",(int)size);
            return PEP_XACML_ERROR;
        }
        strncpy(result->resourceid,resourceid,size);
    }
    return PEP_XACML_OK;
}

xacml_status_t * xacml_result_getstatus(const xacml_result_t * result) {
    if (result == NULL) {
        pep_log_error("xacml_result_getstatus: NULL result.");
        return NULL;
    }
    return result->status;
}

int xacml_result_setstatus(xacml_result_t * result, xacml_status_t * status) {
    if (result == NULL || status == NULL) {
        pep_log_error("xacml_result_setstatus: NULL result or status.");
        return PEP_XACML_ERROR;
    }
    if (result->status != NULL) xacml_status_delete(result->status);
    result->status= status;
    return PEP_XACML_OK;
}

int xacml_result_addobligation(xacml_result_t * result, xacml_obligation_t * obligation) {
    if (result == NULL || obligation == NULL) {
        pep_log_error("xacml_result_addobligation: NULL result or obligation.");
        return PEP_XACML_ERROR;
    }
    if (pep_llist_add(result->obligations,obligation) != LLIST_OK) {
        pep_log_error("xacml_result_addobligation: can't add obligation to list.");
        return PEP_XACML_ERROR;
    }
    return PEP_XACML_OK;
}

size_t xacml_result_obligations_length(const xacml_result_t * result) {
    if (result == NULL) {
        pep_log_warn("xacml_result_obligations_length: NULL result.");
        return 0;
    }
    return pep_llist_length(result->obligations);
}

xacml_obligation_t * xacml_result_getobligation(const xacml_result_t * result, int i) {
    if (result == NULL) {
        pep_log_error("xacml_result_getobligation: NULL result.");
        return NULL;
    }
    return pep_llist_get(result->obligations,i);
}

int xacml_result_removeobligation(xacml_result_t * result, int i) {
    xacml_obligation_t * obligation;
    if (result == NULL)	{
        pep_log_error("xacml_result_removeobligation: NULL result.");
        return PEP_XACML_ERROR;
    }
    obligation = pep_llist_remove(result->obligations,i);
    if (obligation == NULL) {
        pep_log_error("xacml_result_removeobligation: failed to remove obligation from list.");
        return PEP_XACML_ERROR;
    }

    xacml_obligation_delete(obligation);
    return PEP_XACML_OK;
}

void xacml_result_delete(xacml_result_t * result) {
    if (result == NULL) return;
    if (result->resourceid != NULL) free(result->resourceid);
    if (result->status != NULL) xacml_status_delete(result->status);
    pep_llist_delete_elements(result->obligations,(pep_llist_delete_elt_f)xacml_obligation_delete);
    pep_llist_delete(result->obligations);
    free(result);
    result= NULL;
}

