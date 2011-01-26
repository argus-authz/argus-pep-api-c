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

struct xacml_response {
    xacml_request_t * request; /* original request */
    linkedlist_t * results; /* list of results */
};

xacml_response_t * xacml_response_create() {
    xacml_response_t * response= calloc(1,sizeof(xacml_response_t));
    if (response == NULL) {
        log_error("xacml_response_create: can't allocate xacml_response_t.");
        return NULL;
    }
    response->results= llist_create();
    if (response->results == NULL) {
        log_error("xacml_response_create: can't create results list.");
        free(response);
        return NULL;
    }
    response->request= NULL;
    return response;
}

int xacml_response_setrequest(xacml_response_t * response, xacml_request_t * request) {
    if (response == NULL || request == NULL) {
        log_error("xacml_response_setrequest: NULL response or request.");
        return PEP_XACML_ERROR;
    }
    if (response->request != NULL) xacml_request_delete(response->request);
    response->request= request;
    return PEP_XACML_OK;
}

xacml_request_t * xacml_response_getrequest(const xacml_response_t * response) {
    if (response == NULL) {
        log_error("xacml_response_getrequest: NULL response.");
        return NULL;
    }
    return response->request;
}

xacml_request_t * xacml_response_relinquishrequest(xacml_response_t * response) {
    xacml_request_t * request;
    if (response == NULL) {
        log_error("xacml_response_getrequest: NULL response.");
        return NULL;
    }
    /* forget about the request, caller is responsible to call xacml_delete_request */
    request= response->request;
    response->request= NULL;
    return request;
}


int xacml_response_addresult(xacml_response_t * response, xacml_result_t * result) {
    if (response == NULL || result == NULL) {
        log_error("xacml_response_addresult: NULL response or result.");
        return PEP_XACML_ERROR;
    }
    if (llist_add(response->results,result) != LLIST_OK) {
        log_error("xacml_response_addresult: can't add result to list.");
        return PEP_XACML_ERROR;
    }
    else return PEP_XACML_OK;
}

size_t xacml_response_results_length(const xacml_response_t * response) {
    if (response == NULL) {
        log_warn("xacml_response_results_length: NULL response.");
        return 0;
    }
    return llist_length(response->results);
}

xacml_result_t * xacml_response_getresult(const xacml_response_t * response, int index) {
    if (response == NULL) {
        log_error("xacml_response_getresult: NULL response.");
        return NULL;
    }
    return llist_get(response->results,index);
}

void xacml_response_delete(xacml_response_t * response) {
    if (response == NULL) return;
    if (response->request != NULL) xacml_request_delete(response->request);
    llist_delete_elements(response->results,(delete_element_func)xacml_result_delete);
    llist_delete(response->results);
    free(response);
    response= NULL;
}
