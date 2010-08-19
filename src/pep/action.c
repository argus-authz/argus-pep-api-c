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

#include "util/linkedlist.h"
#include "util/log.h"
#include "pep/xacml.h"

struct xacml_action {
    linkedlist_t * attributes;
};

xacml_action_t * xacml_action_create() {
    xacml_action_t * action= calloc(1,sizeof(xacml_action_t));
    if (action == NULL) {
        log_error("xacml_action_create: can't allocate xacml_action_t.");
        return NULL;
    }
    action->attributes= llist_create();
    if (action->attributes == NULL) {
        log_error("xacml_action_create: can't create attributes list.");
        free(action);
        return NULL;
    }
    return action;
}

int xacml_action_addattribute(xacml_action_t * action, xacml_attribute_t * attr) {
    if (action == NULL || attr == NULL) {
        log_error("xacml_action_addattribute: NULL action or attribute.");
        return PEP_XACML_ERROR;
    }
    if (llist_add(action->attributes,attr) != LLIST_OK) {
        log_error("xacml_action_addattribute: can't add attribute to list.");
        return PEP_XACML_ERROR;
    }
    else return PEP_XACML_OK;
}

void xacml_action_delete(xacml_action_t * action) {
    if (action == NULL) return;
    llist_delete_elements(action->attributes,(delete_element_func)xacml_attribute_delete);
    llist_delete(action->attributes);
    free(action);
    action= NULL;
}

size_t xacml_action_attributes_length(const xacml_action_t * action) {
    if (action == NULL) {
        log_warn("xacml_action_attributes_length: NULL action.");
        return 0;
    }
    return llist_length(action->attributes);
}

xacml_attribute_t * xacml_action_getattribute(const xacml_action_t * action, int index) {
    if (action == NULL) {
        log_error("xacml_action_getattribute: NULL action.");
        return NULL;
    }
    return llist_get(action->attributes, index);
}

