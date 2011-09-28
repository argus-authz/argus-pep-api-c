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

#include <string.h>

#include "io.h"
#include "hessian.h" /* ../hessian/hessian.h */
#include "log.h" /* ../util/log.h */

/** functions return codes  */
#define PEP_IO_OK     0
#define PEP_IO_ERROR -1

/**
 * Hessian 1.0 marshalling/unmarshalling prototypes.
 *
 * Returns PEP_IO_OK or PEP_IO_ERROR.
 */
static int xacml_attribute_marshal(const xacml_attribute_t * attr, hessian_object_t ** h_attribute);
static int xacml_attribute_unmarshal(xacml_attribute_t ** attr, const hessian_object_t * h_attribute);
static int xacml_subject_marshal(const xacml_subject_t * subject, hessian_object_t ** h_subject);
static int xacml_subject_unmarshal(xacml_subject_t ** subject, const hessian_object_t * h_subject);
static int xacml_resource_marshal(const xacml_resource_t * resource, hessian_object_t ** h_resource);
static int xacml_resource_unmarshal(xacml_resource_t ** resource, const hessian_object_t * h_resource);
static int xacml_action_marshal(const xacml_action_t * action, hessian_object_t ** h_action);
static int xacml_action_unmarshal(xacml_action_t ** action, const hessian_object_t * h_action);
static int xacml_environment_marshal(const xacml_environment_t * env, hessian_object_t ** h_environment);
static int xacml_environment_unmarshal(xacml_environment_t ** env, const hessian_object_t * h_environment);
static int xacml_request_marshal(const xacml_request_t * request, hessian_object_t ** h_request);
static int xacml_request_unmarshal(xacml_request_t ** request, const hessian_object_t * h_request);
static int xacml_response_unmarshal(xacml_response_t ** response, const hessian_object_t * h_response);
static int xacml_result_unmarshal(xacml_result_t ** result, const hessian_object_t * h_result);
static int xacml_status_unmarshal(xacml_status_t ** status, const hessian_object_t * h_status);
static int xacml_statuscode_unmarshal(xacml_statuscode_t ** statuscode, const hessian_object_t * h_statuscode);
static int xacml_obligation_unmarshal(xacml_obligation_t ** obligation, const hessian_object_t * h_obligation);
static int xacml_attributeassignment_unmarshal(xacml_attributeassignment_t ** attr, const hessian_object_t * h_attribute);

/**
 * Returns the Hessian map for this Action or a Hessian null if the Action is null.
 */
static int xacml_action_marshal(const xacml_action_t * action, hessian_object_t ** h_act) {
    hessian_object_t * h_action, * h_attrs, * h_attrs_key;
    size_t list_l;
    int i;
    if (action == NULL) {
        h_action= hessian_create(HESSIAN_NULL);
        if (h_action == NULL) {
            log_error("xacml_action_marshal: NULL action, but can't create Hessian null.");
            return PEP_IO_ERROR;
        }
        *h_act= h_action;
        return PEP_IO_OK;
    }
    h_action= hessian_create(HESSIAN_MAP,XACML_HESSIAN_ACTION_CLASSNAME);
    if (h_action == NULL) {
        log_error("xacml_action_marshal: can't create Hessian map: %s.", XACML_HESSIAN_ACTION_CLASSNAME);
        return PEP_IO_ERROR;
    }
    /* attributes list */
    h_attrs= hessian_create(HESSIAN_LIST);
    if (h_attrs == NULL) {
        log_error("xacml_action_marshal: can't create Hessian list: %s.", XACML_HESSIAN_ACTION_ATTRIBUTES);
        hessian_delete(h_action);
        return PEP_IO_ERROR;
    }
    list_l= xacml_action_attributes_length(action);
    for (i= 0; i < list_l; i++) {
        xacml_attribute_t * attr= xacml_action_getattribute(action,i);
        hessian_object_t * h_attr= NULL;
        if (xacml_attribute_marshal(attr,&h_attr) != PEP_IO_OK) {
            log_error("xacml_action_marshal: can't marshal attribute at: %d.",i);
            hessian_delete(h_action);
            hessian_delete(h_attrs);
            return PEP_IO_ERROR;
        }
        if (hessian_list_add(h_attrs,h_attr) != HESSIAN_OK) {
            log_error("xacml_action_marshal: can't add Hessian attribute to Hessian list at: %d.",i);
            hessian_delete(h_action);
            hessian_delete(h_attrs);
            hessian_delete(h_attr);
            return PEP_IO_ERROR;
        }
    }
    h_attrs_key= hessian_create(HESSIAN_STRING,XACML_HESSIAN_ACTION_ATTRIBUTES);
    if (h_attrs_key == NULL) {
        log_error("xacml_action_marshal: can't create Hessian map<key>: %s.", XACML_HESSIAN_ACTION_ATTRIBUTES);
        hessian_delete(h_action);
        hessian_delete(h_attrs);
        return PEP_IO_ERROR;
    }
    if (hessian_map_add(h_action,h_attrs_key,h_attrs) != HESSIAN_OK) {
        log_error("xacml_action_marshal: can't add %s Hessian list to action Hessian map.", XACML_HESSIAN_ACTION_ATTRIBUTES);
        hessian_delete(h_action);
        hessian_delete(h_attrs);
        hessian_delete(h_attrs_key);
        return PEP_IO_ERROR;
    }
    *h_act= h_action;
    return PEP_IO_OK;
}

static int xacml_action_unmarshal(xacml_action_t ** act, const hessian_object_t * h_action) {
    const char * map_type;
    xacml_action_t * action;
    size_t map_l;
    int i;
    if (hessian_gettype(h_action) != HESSIAN_MAP) {
        log_error("xacml_action_unmarshal: wrong Hessian type: %d (%s).", hessian_gettype(h_action), hessian_getclassname(h_action));
        return PEP_IO_ERROR;
    }
    map_type= hessian_map_gettype(h_action);
    if (map_type == NULL) {
        log_error("xacml_action_unmarshal: NULL Hessian map type.");
        return PEP_IO_ERROR;
    }
    if (strcmp(XACML_HESSIAN_ACTION_CLASSNAME,map_type) != 0) {
        log_error("xacml_action_unmarshal: wrong Hessian map type: %s.",map_type);
        return PEP_IO_ERROR;
    }
    action= xacml_action_create();
    if (action == NULL) {
        log_error("xacml_action_unmarshal: can't create XACML action.");
        return PEP_IO_ERROR;
    }

    /* parse all map pair<key>s */
    map_l= hessian_map_length(h_action);
    for(i= 0; i<map_l; i++) {
        hessian_object_t * h_map_key= hessian_map_getkey(h_action,i);
        const char * key;
        if (hessian_gettype(h_map_key) != HESSIAN_STRING) {
            log_error("xacml_action_unmarshal: Hessian map<key> is not an Hessian string at: %d.",i);
            xacml_action_delete(action);
            return PEP_IO_ERROR;
        }
        key= hessian_string_getstring(h_map_key);
        if (key == NULL) {
            log_error("xacml_action_unmarshal: Hessian map<key>: NULL string at: %d.",i);
            xacml_action_delete(action);
            return PEP_IO_ERROR;
        }
        if (strcmp(XACML_HESSIAN_ACTION_ATTRIBUTES,key) == 0) {
            hessian_object_t * h_attributes= hessian_map_getvalue(h_action,i);
            size_t h_attributes_l;
            int j;
            if (hessian_gettype(h_attributes) != HESSIAN_LIST) {
                log_error("xacml_action_unmarshal: Hessian map<'%s',value> is not a Hessian list at: %d.",key, i);
                xacml_action_delete(action);
                return PEP_IO_ERROR;
            }
            h_attributes_l= hessian_list_length(h_attributes);
            for(j= 0; j<h_attributes_l; j++) {
                hessian_object_t * h_attr= hessian_list_get(h_attributes,j);
                xacml_attribute_t * attribute= NULL;
                if (xacml_attribute_unmarshal(&attribute,h_attr)) {
                    log_error("xacml_action_unmarshal: can't unmarshal XACML attribute at: %d.",j);
                    xacml_action_delete(action);
                    return PEP_IO_ERROR;
                }
                if (xacml_action_addattribute(action,attribute) != PEP_XACML_OK) {
                    log_error("xacml_action_unmarshal: can't add XACML attribute to XACML action at: %d",j);
                    xacml_action_delete(action);
                    xacml_attribute_delete(attribute);
                    return PEP_IO_ERROR;
                }
            }
        }
        else {
            /* unkown key ??? */
            log_warn("xacml_action_unmarshal: unknown Hessian map<key>: %s at: %d.",key,i);
        }
    }
    *act= action;
    return PEP_IO_OK;
}


/**
 * Returns the Hessian object representing the XACML attribute.
 */
static int xacml_attribute_marshal(const xacml_attribute_t * attr, hessian_object_t ** h_attr) {
    hessian_object_t * h_attribute, * h_value, * h_key, * h_values, * h_values_key;
    const char * attr_id, * attr_dt, * attr_issuer;
    size_t values_l;
    int i;
    if (attr == NULL) {
        log_error("xacml_attribute_marshal: NULL attribute object.");
        return PEP_IO_ERROR;
    }
    h_attribute= hessian_create(HESSIAN_MAP,XACML_HESSIAN_ATTRIBUTE_CLASSNAME);
    if (h_attribute == NULL) {
        log_error("xacml_attribute_marshal: can't create attribute Hessian map: %s", XACML_HESSIAN_ATTRIBUTE_CLASSNAME);
        return PEP_IO_ERROR;
    }

    /* mandatory attribute */
    attr_id= xacml_attribute_getid(attr);
    h_value= hessian_create(HESSIAN_STRING,attr_id);
    if (h_value== NULL) {
        log_error("xacml_attribute_marshal: can't create Hessian string: %s", attr_id);
        hessian_delete(h_attribute);
        return PEP_IO_ERROR;
    }
    h_key= hessian_create(HESSIAN_STRING,XACML_HESSIAN_ATTRIBUTE_ID);
    if (hessian_map_add(h_attribute,h_key,h_value) != HESSIAN_OK) {
        log_error("xacml_attribute_marshal: can't add pair<'%s','%s'> to Hessian map: %s", XACML_HESSIAN_ATTRIBUTE_ID,attr_id,XACML_HESSIAN_ATTRIBUTE_CLASSNAME);
        hessian_delete(h_attribute);
        hessian_delete(h_key);
        hessian_delete(h_value);
        return PEP_IO_ERROR;
    }
    /* optional datatype */
    attr_dt= xacml_attribute_getdatatype(attr);
    if (attr_dt != NULL) {
        h_key= hessian_create(HESSIAN_STRING,XACML_HESSIAN_ATTRIBUTE_DATATYPE);
        h_value= hessian_create(HESSIAN_STRING,attr_dt);
        if (hessian_map_add(h_attribute,h_key,h_value) != HESSIAN_OK) {
            log_error("xacml_attribute_marshal: can't add pair<'%s','%s'> to Hessian map: %s", XACML_HESSIAN_ATTRIBUTE_DATATYPE,attr_dt,XACML_HESSIAN_ATTRIBUTE_CLASSNAME);
            hessian_delete(h_attribute);
            hessian_delete(h_key);
            hessian_delete(h_value);
            return PEP_IO_ERROR;
        }
    }
    /* optional issuer */
    attr_issuer= xacml_attribute_getissuer(attr);
    if (attr_issuer != NULL) {
        h_key= hessian_create(HESSIAN_STRING,XACML_HESSIAN_ATTRIBUTE_ISSUER);
        h_value= hessian_create(HESSIAN_STRING,attr_issuer);
        if (hessian_map_add(h_attribute,h_key,h_value) != HESSIAN_OK) {
            log_error("xacml_attribute_marshal: can't add pair<'%s','%s'> to Hessian map: %s", XACML_HESSIAN_ATTRIBUTE_ISSUER,attr_issuer,XACML_HESSIAN_ATTRIBUTE_CLASSNAME);
            hessian_delete(h_attribute);
            hessian_delete(h_key);
            hessian_delete(h_value);
            return PEP_IO_ERROR;
        }
    }
    /* values list */
    h_values= hessian_create(HESSIAN_LIST);
    if (h_values == NULL) {
        log_error("xacml_attribute_marshal: can't create %s Hessian list.", XACML_HESSIAN_ATTRIBUTE_VALUES);
        hessian_delete(h_attribute);
        return PEP_IO_ERROR;
    }
    values_l= xacml_attribute_values_length(attr);
    for (i= 0; i < values_l; i++) {
        const char * value= xacml_attribute_getvalue(attr,i);
        h_value= hessian_create(HESSIAN_STRING,value);
        if (h_value == NULL) {
            log_error("xacml_attribute_marshal: can't create Hessian string: %s at: %d.", value, i);
            hessian_delete(h_attribute);
            hessian_delete(h_values);
            return PEP_IO_ERROR;
        }
        if (hessian_list_add(h_values,h_value) != HESSIAN_OK) {
            log_error("xacml_attribute_marshal: can't add Hessian string: %s to Hessian list.", value);
            hessian_delete(h_attribute);
            hessian_delete(h_values);
            hessian_delete(h_value);
            return PEP_IO_ERROR;
        }
    }
    h_values_key= hessian_create(HESSIAN_STRING,XACML_HESSIAN_ATTRIBUTE_VALUES);
    if (hessian_map_add(h_attribute,h_values_key,h_values) != HESSIAN_OK) {
        log_error("xacml_attribute_marshal: can't add attributes Hessian list to attribute Hessian map.");
        hessian_delete(h_attribute);
        hessian_delete(h_values_key);
        hessian_delete(h_values);
        return PEP_IO_ERROR;
    }
    *h_attr= h_attribute;
    return PEP_IO_OK;
}

static int xacml_attribute_unmarshal(xacml_attribute_t ** attr, const hessian_object_t * h_attribute) {
    const char * map_type;
    xacml_attribute_t * attribute;
    size_t map_l;
    int i;
    if (hessian_gettype(h_attribute) != HESSIAN_MAP) {
        log_error("xacml_attribute_unmarshal: wrong Hessian type: %d (%s).", hessian_gettype(h_attribute), hessian_getclassname(h_attribute));
        return PEP_IO_ERROR;
    }
    map_type= hessian_map_gettype(h_attribute);
    if (map_type == NULL) {
        log_error("xacml_attribute_unmarshal: NULL Hessian map type.");
        return PEP_IO_ERROR;
    }
    if (strcmp(XACML_HESSIAN_ATTRIBUTE_CLASSNAME,map_type) != 0) {
        log_error("xacml_attribute_unmarshal: wrong Hessian map type: %s.",map_type);
        return PEP_IO_ERROR;
    }

    attribute= xacml_attribute_create(NULL);
    if (attribute == NULL) {
        log_error("xacml_attribute_unmarshal: can't create XACML attribute.");
        return PEP_IO_ERROR;
    }

    /* parse all map pair<key>s */
    map_l= hessian_map_length(h_attribute);
    for(i= 0; i<map_l; i++) {
        hessian_object_t * h_map_key= hessian_map_getkey(h_attribute,i);
        const char * key;
        if (hessian_gettype(h_map_key) != HESSIAN_STRING) {
            log_error("xacml_attribute_unmarshal: Hessian map<key> is not an Hessian string at: %d.",i);
            xacml_attribute_delete(attribute);
            return PEP_IO_ERROR;
        }
        key= hessian_string_getstring(h_map_key);
        if (key == NULL) {
            log_error("xacml_attribute_unmarshal: Hessian map<key>: NULL string at: %d.",i);
            xacml_attribute_delete(attribute);
            return PEP_IO_ERROR;
        }

        /* id (mandatory) */
        if (strcmp(XACML_HESSIAN_ATTRIBUTE_ID,key) == 0) {
            hessian_object_t * h_string= hessian_map_getvalue(h_attribute,i);
            const char * id;
            if (hessian_gettype(h_string) != HESSIAN_STRING) {
                log_error("xacml_attribute_unmarshal: Hessian map<'%s',value> is not a Hessian string at: %d.",key,i);
                xacml_attribute_delete(attribute);
                return PEP_IO_ERROR;
            }
            id= hessian_string_getstring(h_string);
            if (xacml_attribute_setid(attribute,id) != PEP_XACML_OK) {
                log_error("xacml_attribute_unmarshal: can't set id: %s to XACML attribute at: %d",id,i);
                xacml_attribute_delete(attribute);
                return PEP_IO_ERROR;
            }
        }
        /* datatype (optional) */
        else if (strcmp(XACML_HESSIAN_ATTRIBUTE_DATATYPE,key) == 0) {
            const char * datatype= NULL;
            hessian_object_t * h_string= hessian_map_getvalue(h_attribute,i);
            hessian_t h_string_type= hessian_gettype(h_string);
            if ( h_string_type != HESSIAN_STRING && h_string_type != HESSIAN_NULL) {
                log_error("xacml_attribute_unmarshal: Hessian map<'%s',value> is not a Hessian string or null at: %d.",key,i);
                xacml_attribute_delete(attribute);
                return PEP_IO_ERROR;
            }
            if (h_string_type == HESSIAN_STRING) {
                datatype= hessian_string_getstring(h_string);
            }
            if (xacml_attribute_setdatatype(attribute,datatype) != PEP_XACML_OK) {
                log_error("xacml_attribute_unmarshal: can't set datatype: %s to XACML attribute at: %d",datatype,i);
                xacml_attribute_delete(attribute);
                return PEP_IO_ERROR;
            }

        }
        /* issuer (optional) */
        else if (strcmp(XACML_HESSIAN_ATTRIBUTE_ISSUER,key) == 0) {
            const char * issuer = NULL;
            hessian_object_t * h_string= hessian_map_getvalue(h_attribute,i);
            hessian_t h_string_type= hessian_gettype(h_string);
            if ( h_string_type != HESSIAN_STRING && h_string_type != HESSIAN_NULL) {
                log_error("xacml_attribute_unmarshal: Hessian map<'%s',value> is not a Hessian string or null at: %d.",key,i);
                xacml_attribute_delete(attribute);
                return PEP_IO_ERROR;
            }
            if (h_string_type == HESSIAN_STRING) {
                issuer= hessian_string_getstring(h_string);
            }
            if (xacml_attribute_setissuer(attribute,issuer) != PEP_XACML_OK) {
                log_error("xacml_attribute_unmarshal: can't set issuer: %s to XACML attribute at: %d",issuer,i);
                xacml_attribute_delete(attribute);
                return PEP_IO_ERROR;
            }

        }
        /* values list */
        else if (strcmp(XACML_HESSIAN_ATTRIBUTE_VALUES,key) == 0) {
            hessian_object_t * h_values= hessian_map_getvalue(h_attribute,i);
            size_t h_values_l;
            int j;
            if (hessian_gettype(h_values) != HESSIAN_LIST) {
                log_error("xacml_attribute_unmarshal: Hessian map<'%s',value> is not a Hessian list.",key);
                xacml_attribute_delete(attribute);
                return PEP_IO_ERROR;
            }
            h_values_l= hessian_list_length(h_values);
            for(j= 0; j<h_values_l; j++) {
                const char * value;
                hessian_object_t * h_value= hessian_list_get(h_values,j);
                if (hessian_gettype(h_value) != HESSIAN_STRING) {
                    log_error("xacml_attribute_unmarshal: Hessian map<'%s',value> is not a Hessian string at: %d.",key,i);
                    xacml_attribute_delete(attribute);
                    return PEP_IO_ERROR;
                }
                value= hessian_string_getstring(h_value);
                if (xacml_attribute_addvalue(attribute,value) != PEP_XACML_OK) {
                    log_error("xacml_attribute_unmarshal: can't add value: %s to XACML attribute at: %d",value,j);
                    xacml_attribute_delete(attribute);
                    return PEP_IO_ERROR;
                }
            }

        }
        else {
            log_warn("xacml_attribute_unmarshal: unknown Hessian map<key>: %s at: %d.",key,i);
        }
    }
    *attr= attribute;
    return PEP_IO_OK;
}

/**
 * Returns the Hessian map for this Environment or a Hessian null if the Environment is null.
 */
static int xacml_environment_marshal(const xacml_environment_t * env, hessian_object_t ** h_env) {
    hessian_object_t * h_environment, * h_attrs, * h_attrs_key;
    size_t list_l;
    int i;
    if (env == NULL) {
        hessian_object_t * h_null= hessian_create(HESSIAN_NULL);
        if (h_null == NULL) {
            log_error("xacml_environment_marshal: NULL environment, but can't create Hessian null.");
            return PEP_IO_ERROR;
        }
        *h_env= h_null;
        return PEP_IO_OK;
    }
    h_environment= hessian_create(HESSIAN_MAP,XACML_HESSIAN_ENVIRONMENT_CLASSNAME);
    if (h_environment == NULL) {
        log_error("xacml_environment_marshal: can't create Hessian map: %s.", XACML_HESSIAN_ENVIRONMENT_CLASSNAME);
        return PEP_IO_ERROR;
    }
    /* attributes list */
    h_attrs= hessian_create(HESSIAN_LIST);
    if (h_attrs == NULL) {
        log_error("xacml_environment_marshal: can't create %s Hessian list.", XACML_HESSIAN_ENVIRONMENT_ATTRIBUTES);
        hessian_delete(h_environment);
        return PEP_IO_ERROR;
    }
    list_l= xacml_environment_attributes_length(env);
    for (i= 0; i < list_l; i++) {
        xacml_attribute_t * attr= xacml_environment_getattribute(env,i);
        hessian_object_t * h_attr= NULL;
        if (xacml_attribute_marshal(attr,&h_attr) != PEP_IO_OK) {
            log_error("xacml_environment_marshal: can't marshall XACML attribute at: %d",i);
            hessian_delete(h_environment);
            hessian_delete(h_attrs);
            return PEP_IO_ERROR;
        }
        if (hessian_list_add(h_attrs,h_attr) != HESSIAN_OK) {
            log_error("xacml_environment_marshal: can't add Hessian attribute to Hessian list at: %d",i);
            hessian_delete(h_environment);
            hessian_delete(h_attrs);
            hessian_delete(h_attr);
            return PEP_IO_ERROR;
        }
    }
    h_attrs_key= hessian_create(HESSIAN_STRING,XACML_HESSIAN_ENVIRONMENT_ATTRIBUTES);
    if (hessian_map_add(h_environment,h_attrs_key,h_attrs) != HESSIAN_OK) {
        log_error("xacml_environment_marshal: can't add attributes Hessian list to environment Hessian map.");
        hessian_delete(h_environment);
        hessian_delete(h_attrs);
        hessian_delete(h_attrs_key);
        return PEP_IO_ERROR;
    }
    *h_env= h_environment;
    return PEP_IO_OK;
}


static int xacml_environment_unmarshal(xacml_environment_t ** env, const hessian_object_t * h_environment) {
    const char * map_type;
    xacml_environment_t * environment;
    size_t map_l;
    int i;
    if (hessian_gettype(h_environment) != HESSIAN_MAP) {
        log_error("xacml_environment_unmarshal: wrong Hessian type: %d (%s).", hessian_gettype(h_environment), hessian_getclassname(h_environment));
        return PEP_IO_ERROR;
    }
    map_type= hessian_map_gettype(h_environment);
    if (map_type == NULL) {
        log_error("xacml_environment_unmarshal: NULL Hessian map type.");
        return PEP_IO_ERROR;
    }
    if (strcmp(XACML_HESSIAN_ENVIRONMENT_CLASSNAME,map_type) != 0) {
        log_error("xacml_environment_unmarshal: wrong Hessian map type: %s.",map_type);
        return PEP_IO_ERROR;
    }
    environment= xacml_environment_create();
    if (environment == NULL) {
        log_error("xacml_environment_unmarshal: can't create XACML environment.");
        return PEP_IO_ERROR;
    }

    /* parse all map pair<key>s */
    map_l= hessian_map_length(h_environment);
    for(i= 0; i<map_l; i++) {
        const char * key;
        hessian_object_t * h_map_key= hessian_map_getkey(h_environment,i);
        if (hessian_gettype(h_map_key) != HESSIAN_STRING) {
            log_error("xacml_environment_unmarshal: Hessian map<key> is not an Hessian string at: %d.",i);
            xacml_environment_delete(environment);
            return PEP_IO_ERROR;
        }
        key= hessian_string_getstring(h_map_key);
        if (key == NULL) {
            log_error("xacml_environment_unmarshal: Hessian map<key>: NULL string at: %d.",i);
            xacml_environment_delete(environment);
            return PEP_IO_ERROR;
        }
        if (strcmp(XACML_HESSIAN_ENVIRONMENT_ATTRIBUTES,key) == 0) {
            hessian_object_t * h_attributes= hessian_map_getvalue(h_environment,i);
            size_t h_attributes_l;
            int j;
            if (hessian_gettype(h_attributes) != HESSIAN_LIST) {
                log_error("xacml_environment_unmarshal: Hessian map<'%s',value> is not a Hessian list at: %d.",key, i);
                xacml_environment_delete(environment);
                return PEP_IO_ERROR;
            }
            h_attributes_l= hessian_list_length(h_attributes);
            for(j= 0; j<h_attributes_l; j++) {
                hessian_object_t * h_attr= hessian_list_get(h_attributes,j);
                xacml_attribute_t * attribute= NULL;
                if (xacml_attribute_unmarshal(&attribute,h_attr)) {
                    log_error("xacml_environment_unmarshal: can't unmarshal XACML attribute at: %d.",j);
                    xacml_environment_delete(environment);
                    return PEP_IO_ERROR;
                }
                if (xacml_environment_addattribute(environment,attribute) != PEP_XACML_OK) {
                    log_error("xacml_environment_unmarshal: can't add XACML attribute to XACML environment at: %d",j);
                    xacml_environment_delete(environment);
                    xacml_attribute_delete(attribute);
                    return PEP_IO_ERROR;
                }
            }
        }
        else {
            /* unkown key ??? */
            log_warn("xacml_environment_unmarshal: unknown Hessian map<key>: %s at: %d.",key,i);
        }
    }
    *env= environment;
    return PEP_IO_OK;
}

/**
 * Returns PEP_IO_OK or PEP_IO_ERROR
 */
static int xacml_request_marshal(const xacml_request_t * request, hessian_object_t ** h_req) {
    hessian_object_t * h_request, * h_subjects, * h_subjects_key, * h_resources, * h_resources_key; 
    hessian_object_t * h_action, * h_action_key, * h_environment, * h_environment_key;
    xacml_action_t * action;
    xacml_environment_t * environment;
    size_t list_l;
    int i;
    if (request == NULL) {
        log_error("xacml_request_marshal: NULL request object.");
        return PEP_IO_ERROR;
    }
    /* request as hessian map */
    h_request= hessian_create(HESSIAN_MAP,XACML_HESSIAN_REQUEST_CLASSNAME);
    if (h_request == NULL) {
        log_error("xacml_request_marshal: can't create request Hessian map: %s.",XACML_HESSIAN_REQUEST_CLASSNAME);
        return PEP_IO_ERROR;
    }
    /* subjects list */
    h_subjects= hessian_create(HESSIAN_LIST);
    if (h_subjects == NULL) {
        log_error("xacml_request_marshal: can't create subjects Hessian list.");
        hessian_delete(h_request);
        return PEP_IO_ERROR;
    }
    list_l= xacml_request_subjects_length(request);
    for (i= 0; i < list_l; i++) {
        xacml_subject_t * subject= xacml_request_getsubject(request,i);
        hessian_object_t * h_subject= NULL;
        if (xacml_subject_marshal(subject,&h_subject) != PEP_IO_OK) {
            log_error("xacml_request_marshal: failed to marshal XACML subject at: %d.",i);
            hessian_delete(h_request);
            hessian_delete(h_subjects);
            return PEP_IO_ERROR;
        }
        if (hessian_list_add(h_subjects,h_subject) != HESSIAN_OK) {
            log_error("xacml_request_marshal: can't add Hessian subject %d in Hessian subjects list.",i);
            hessian_delete(h_request);
            hessian_delete(h_subjects);
            hessian_delete(h_subject);
            return PEP_IO_ERROR;
        }
    }
    h_subjects_key= hessian_create(HESSIAN_STRING,XACML_HESSIAN_REQUEST_SUBJECTS);
    if (hessian_map_add(h_request,h_subjects_key,h_subjects) != HESSIAN_OK) {
        log_error("xacml_request_marshal: can't add Hessian subjects list in Hessian request map.");
        hessian_delete(h_request);
        hessian_delete(h_subjects_key);
        hessian_delete(h_subjects);
        return PEP_IO_ERROR;
    }
    /* resources list */
    h_resources= hessian_create(HESSIAN_LIST);
    if (h_resources == NULL) {
        log_error("xacml_request_marshal: can't create resources Hessian list.");
        hessian_delete(h_request);
        return PEP_IO_ERROR;
    }
    list_l= xacml_request_resources_length(request);
    for (i= 0; i < list_l; i++) {
        xacml_resource_t * resource= xacml_request_getresource(request,i);
        hessian_object_t * h_resource= NULL;
        if (xacml_resource_marshal(resource,&h_resource) != PEP_IO_OK) {
            log_error("xacml_request_marshal: failed to marshal XACML resource at: %d.",i);
            hessian_delete(h_request);
            hessian_delete(h_resources);
            return PEP_IO_ERROR;
        }
        if (hessian_list_add(h_resources,h_resource) != HESSIAN_OK) {
            log_error("xacml_request_marshal: can't add Hessian resource %d to Hessian resources list.",i);
            hessian_delete(h_request);
            hessian_delete(h_resources);
            hessian_delete(h_resource);
            return PEP_IO_ERROR;
        }
    }
    h_resources_key= hessian_create(HESSIAN_STRING,XACML_HESSIAN_REQUEST_RESOURCES);
    if (hessian_map_add(h_request,h_resources_key,h_resources) != HESSIAN_OK) {
        log_error("xacml_request_marshal: can't add Hessian resources list to Hessian request map.");
        hessian_delete(h_request);
        hessian_delete(h_resources_key);
        hessian_delete(h_resources);
        return PEP_IO_ERROR;
    }
    /* action */
    action= xacml_request_getaction(request);
    h_action= NULL;
    if (xacml_action_marshal(action,&h_action) != PEP_IO_OK) {
        log_error("xacml_request_marshal: failed to marshal XACML action.");
        hessian_delete(h_request);
        return PEP_IO_ERROR;
    }
    h_action_key= hessian_create(HESSIAN_STRING,XACML_HESSIAN_REQUEST_ACTION);
    if (hessian_map_add(h_request,h_action_key,h_action) != HESSIAN_OK) {
        log_error("xacml_request_marshal: can't add Hessian action to Hessian request.");
        hessian_delete(h_request);
        hessian_delete(h_action_key);
        hessian_delete(h_action);
        return PEP_IO_ERROR;
    }
    /* environment */
    environment= xacml_request_getenvironment(request);
    h_environment= NULL;
    if (xacml_environment_marshal(environment,&h_environment) != PEP_IO_OK) {
        log_error("xacml_request_marshal: failed to marshal XACML environment.");
        hessian_delete(h_request);
        return PEP_IO_ERROR;
    }
    h_environment_key= hessian_create(HESSIAN_STRING,XACML_HESSIAN_REQUEST_ENVIRONMENT);
    if (hessian_map_add(h_request,h_environment_key,h_environment) != HESSIAN_OK) {
        log_error("xacml_request_marshal: can't add Hessian environment to Hessian request.");
        hessian_delete(h_request);
        hessian_delete(h_environment_key);
        hessian_delete(h_environment);
        return PEP_IO_ERROR;
    }
    *h_req= h_request;
    return PEP_IO_OK;
}

static int xacml_request_unmarshal(xacml_request_t ** req, const hessian_object_t * h_request) {
    const char * map_type;
    xacml_request_t * request;
    size_t map_l;
    int i;
    if (hessian_gettype(h_request) != HESSIAN_MAP) {
        log_error("xacml_request_unmarshal: wrong Hessian type: %d (%s).", hessian_gettype(h_request), hessian_getclassname(h_request));
        return PEP_IO_ERROR;
    }
    map_type= hessian_map_gettype(h_request);
    if (map_type == NULL) {
        log_error("xacml_request_unmarshal: NULL Hessian map type.");
        return PEP_IO_ERROR;
    }
    if (strcmp(XACML_HESSIAN_REQUEST_CLASSNAME,map_type) != 0) {
        log_error("xacml_request_unmarshal: wrong Hessian map type: %s.",map_type);
        return PEP_IO_ERROR;
    }

    request= xacml_request_create();
    if (request == NULL) {
        log_error("xacml_request_unmarshal: can't create XACML request.");
        return PEP_IO_ERROR;
    }

    /* parse all map pair<key>s */
    map_l= hessian_map_length(h_request);
    for(i= 0; i<map_l; i++) {
        hessian_object_t * h_map_key= hessian_map_getkey(h_request,i);
        const char * key;
        int j;
        if (hessian_gettype(h_map_key) != HESSIAN_STRING) {
            log_error("xacml_request_unmarshal: Hessian map<key> is not an Hessian string at: %d.",i);
            xacml_request_delete(request);
            return PEP_IO_ERROR;
        }
        key= hessian_string_getstring(h_map_key);
        if (key == NULL) {
            log_error("xacml_request_unmarshal: Hessian map<key>: NULL string at: %d.",i);
            xacml_request_delete(request);
            return PEP_IO_ERROR;
        }
        /* subjects list */
        if (strcmp(XACML_HESSIAN_REQUEST_SUBJECTS,key) == 0) {
            hessian_object_t * h_subjects= hessian_map_getvalue(h_request,i);
            size_t h_subjects_l;
            if (hessian_gettype(h_subjects) != HESSIAN_LIST) {
                log_error("xacml_request_unmarshal: Hessian map<'%s',value> is not a Hessian list at: %d.",key, i);
                xacml_request_delete(request);
                return PEP_IO_ERROR;
            }
            h_subjects_l= hessian_list_length(h_subjects);
            for(j= 0; j<h_subjects_l; j++) {
                hessian_object_t * h_subject= hessian_list_get(h_subjects,j);
                xacml_subject_t * subject= NULL;
                if (xacml_subject_unmarshal(&subject,h_subject) != PEP_IO_OK) {
                    log_error("xacml_request_unmarshal: can't unmarshal XACML subject at: %d.",j);
                    xacml_request_delete(request);
                    return PEP_IO_ERROR;
                }
                if (xacml_request_addsubject(request,subject) != PEP_XACML_OK) {
                    log_error("xacml_request_unmarshal: can't add XACML subject to XACML request at: %d",j);
                    xacml_request_delete(request);
                    xacml_subject_delete(subject);
                    return PEP_IO_ERROR;
                }
            }
        }
        /* resources list */
        else if (strcmp(XACML_HESSIAN_REQUEST_RESOURCES,key) == 0) {
            hessian_object_t * h_resources= hessian_map_getvalue(h_request,i);
            size_t h_resources_l;
            if (hessian_gettype(h_resources) != HESSIAN_LIST) {
                log_error("xacml_request_unmarshal: Hessian map<'%s',value> is not a Hessian list at: %d.",key, i);
                xacml_request_delete(request);
                return PEP_IO_ERROR;
            }
            h_resources_l= hessian_list_length(h_resources);
            for(j= 0; j<h_resources_l; j++) {
                hessian_object_t * h_resource= hessian_list_get(h_resources,j);
                xacml_resource_t * resource= NULL;
                if (xacml_resource_unmarshal(&resource,h_resource) != PEP_IO_OK) {
                    log_error("xacml_request_unmarshal: can't unmarshal XACML resource at: %d.",j);
                    xacml_request_delete(request);
                    return PEP_IO_ERROR;
                }
                if (xacml_request_addresource(request,resource) != PEP_XACML_OK) {
                    log_error("xacml_request_unmarshal: can't add XACML resource to XACML request at: %d",j);
                    xacml_request_delete(request);
                    xacml_resource_delete(resource);
                    return PEP_IO_ERROR;
                }
            }
        }
        /* action (null) */
        else if (strcmp(XACML_HESSIAN_REQUEST_ACTION,key) == 0) {
            hessian_object_t * h_action= hessian_map_getvalue(h_request,i);
            xacml_action_t * action= NULL;
            if (hessian_gettype(h_action) != HESSIAN_NULL) {
                if (xacml_action_unmarshal(&action,h_action) != PEP_IO_OK) {
                    log_error("xacml_request_unmarshal: can't unmarshal XACML action at: %d.",i);
                    xacml_request_delete(request);
                    return PEP_IO_ERROR;
                }
                if (xacml_request_setaction(request,action) != PEP_XACML_OK) {
                    log_error("xacml_request_unmarshal: can't set XACML action to XACML request at: %d.",i);
                    xacml_action_delete(action);
                    xacml_request_delete(request);
                    return PEP_IO_ERROR;
                }
            }
        }
        /* environment (null) */
        else if (strcmp(XACML_HESSIAN_REQUEST_ENVIRONMENT,key) == 0) {
            hessian_object_t * h_environment= hessian_map_getvalue(h_request,i);
            xacml_environment_t * environment= NULL;
            if (hessian_gettype(h_environment) != HESSIAN_NULL) {
                if (xacml_environment_unmarshal(&environment,h_environment) != PEP_IO_OK) {
                    log_error("xacml_request_unmarshal: can't unmarshal XACML environment at: %d.",i);
                    xacml_request_delete(request);
                    return PEP_IO_ERROR;
                }
                if (xacml_request_setenvironment(request,environment) != PEP_XACML_OK) {
                    log_error("xacml_request_unmarshal: can't set XACML environment to XACML request at: %d.",i);
                    xacml_environment_delete(environment);
                    xacml_request_delete(request);
                    return PEP_IO_ERROR;
                }
            }
        }
        /* unkown key ??? */
        else {
            log_warn("xacml_request_unmarshal: unknown Hessian map<key>: %s at: %d.",key,i);
        }
    }

    *req= request;
    return PEP_IO_OK;
}


static int xacml_resource_marshal(const xacml_resource_t * resource, hessian_object_t ** h_res) {
    hessian_object_t * h_resource, * h_attrs, * h_attrs_key;
    const char * content;
    size_t list_l;
    int i;
    if (resource == NULL) {
        log_error("xacml_resource_marshal: NULL resource object.");
        return PEP_IO_ERROR;
    }
    h_resource= hessian_create(HESSIAN_MAP,XACML_HESSIAN_RESOURCE_CLASSNAME);
    if (h_resource == NULL) {
        log_error("xacml_resource_marshal: can't create Hessian map: %s.", XACML_HESSIAN_RESOURCE_CLASSNAME);
        return PEP_IO_ERROR;
    }
    /* optional content */
    content= xacml_resource_getcontent(resource);
    if (content != NULL) {
        hessian_object_t * h_content= hessian_create(HESSIAN_STRING,content);
        hessian_object_t * h_content_key;
        if (h_content == NULL) {
            log_error("xacml_resource_marshal: can't create content Hessian string: %s.", content);
            hessian_delete(h_resource);
            return PEP_IO_ERROR;
        }
        h_content_key= hessian_create(HESSIAN_STRING,XACML_HESSIAN_RESOURCE_CONTENT);
        if (hessian_map_add(h_resource,h_content_key,h_content) != HESSIAN_OK) {
            log_error("xacml_resource_marshal: can't add content Hessian string to resource Hessian map.");
            hessian_delete(h_resource);
            hessian_delete(h_content);
            hessian_delete(h_content_key);
            return PEP_IO_ERROR;
        }
    }
    /* attributes list */
    h_attrs= hessian_create(HESSIAN_LIST);
    if (h_attrs == NULL) {
        log_error("xacml_resource_marshal: can't create attributes Hessian list.");
        hessian_delete(h_resource);
        return PEP_IO_ERROR;
    }
    list_l= xacml_resource_attributes_length(resource);
    for (i= 0; i < list_l; i++) {
        xacml_attribute_t * attr= xacml_resource_getattribute(resource,i);
        hessian_object_t * h_attr= NULL;
        if (xacml_attribute_marshal(attr,&h_attr) != PEP_IO_OK) {
            log_error("xacml_resource_marshal: can't marshal XACML attribute at: %d.", i);
            hessian_delete(h_resource);
            hessian_delete(h_attrs);
            return PEP_IO_ERROR;
        }
        if (hessian_list_add(h_attrs,h_attr) != HESSIAN_OK) {
            log_error("xacml_resource_marshal: can't add Hessian attribute to attributes Hessian list at: %d.", i);
            hessian_delete(h_resource);
            hessian_delete(h_attrs);
            hessian_delete(h_attr);
            return PEP_IO_ERROR;
        }
    }
    h_attrs_key= hessian_create(HESSIAN_STRING,XACML_HESSIAN_RESOURCE_ATTRIBUTES);
    if (hessian_map_add(h_resource,h_attrs_key,h_attrs) != HESSIAN_OK) {
        log_error("xacml_resource_marshal: can't add attributes Hessian list to resource Hessian map.");
        hessian_delete(h_resource);
        hessian_delete(h_attrs);
        hessian_delete(h_attrs_key);
        return PEP_IO_ERROR;
    }
    *h_res= h_resource;
    return PEP_IO_OK;
}

static int xacml_resource_unmarshal(xacml_resource_t ** res, const hessian_object_t * h_resource) {
    const char * map_type;
    xacml_resource_t * resource;
    size_t map_l;
    int i, j;
    if (hessian_gettype(h_resource) != HESSIAN_MAP) {
        log_error("xacml_resource_unmarshal: wrong Hessian type: %d (%s).", hessian_gettype(h_resource), hessian_getclassname(h_resource));
        return PEP_IO_ERROR;
    }
    map_type= hessian_map_gettype(h_resource);
    if (map_type == NULL) {
        log_error("xacml_resource_unmarshal: NULL Hessian map type.");
        return PEP_IO_ERROR;
    }
    if (strcmp(XACML_HESSIAN_RESOURCE_CLASSNAME,map_type) != 0) {
        log_error("xacml_resource_unmarshal: wrong Hessian map type: %s.",map_type);
        return PEP_IO_ERROR;
    }

    resource= xacml_resource_create();
    if (resource == NULL) {
        log_error("xacml_resource_unmarshal: can't create XACML resource.");
        return PEP_IO_ERROR;
    }

    /* parse all map pair<key>s */
    map_l= hessian_map_length(h_resource);
    for(i= 0; i<map_l; i++) {
        hessian_object_t * h_map_key= hessian_map_getkey(h_resource,i);
        const char * key;
        if (hessian_gettype(h_map_key) != HESSIAN_STRING) {
            log_error("xacml_resource_unmarshal: Hessian map<key> is not an Hessian string at: %d.",i);
            xacml_resource_delete(resource);
            return PEP_IO_ERROR;
        }
        key= hessian_string_getstring(h_map_key);
        if (key == NULL) {
            log_error("xacml_resource_unmarshal: Hessian map<key>: NULL string at: %d.",i);
            xacml_resource_delete(resource);
            return PEP_IO_ERROR;
        }
        /* content (can be null) */
        if (strcmp(XACML_HESSIAN_RESOURCE_CONTENT,key) == 0) {
            hessian_object_t * h_string= hessian_map_getvalue(h_resource,i);
            hessian_t h_string_type= hessian_gettype(h_string);
            const char * content;
            if ( h_string_type != HESSIAN_STRING && h_string_type != HESSIAN_NULL) {
                log_error("xacml_resource_unmarshal: Hessian map<'%s',value> is not a Hessian string or null at: %d.",key,i);
                xacml_resource_delete(resource);
                return PEP_IO_ERROR;
            }
            content= NULL;
            if (h_string_type == HESSIAN_STRING) {
                content= hessian_string_getstring(h_string);
            }
            if (xacml_resource_setcontent(resource,content) != PEP_XACML_OK) {
                log_error("xacml_resource_unmarshal: can't set content: %s to XACML resource.",content);
                xacml_resource_delete(resource);
                return PEP_IO_ERROR;
            }
        }
        /* attributes list */
        else if (strcmp(XACML_HESSIAN_RESOURCE_ATTRIBUTES,key) == 0) {
            hessian_object_t * h_attributes= hessian_map_getvalue(h_resource,i);
            size_t h_attributes_l;
            if (hessian_gettype(h_attributes) != HESSIAN_LIST) {
                log_error("xacml_resource_unmarshal: Hessian map<'%s',value> is not a Hessian list at: %d.",key, i);
                xacml_resource_delete(resource);
                return PEP_IO_ERROR;
            }
            h_attributes_l= hessian_list_length(h_attributes);
            for(j= 0; j<h_attributes_l; j++) {
                hessian_object_t * h_attr= hessian_list_get(h_attributes,j);
                xacml_attribute_t * attribute= NULL;
                if (xacml_attribute_unmarshal(&attribute,h_attr) != PEP_IO_OK) {
                    log_error("xacml_resource_unmarshal: can't unmarshal XACML attribute at: %d.",j);
                    xacml_resource_delete(resource);
                    return PEP_IO_ERROR;
                }
                if (xacml_resource_addattribute(resource,attribute) != PEP_XACML_OK) {
                    log_error("xacml_resource_unmarshal: can't add XACML attribute to XACML resource at: %d",j);
                    xacml_resource_delete(resource);
                    xacml_attribute_delete(attribute);
                    return PEP_IO_ERROR;
                }
            }
        }
        else {
            /* unkown key ??? */
            log_warn("xacml_resource_unmarshal: unknown Hessian map<key>: %s at: %d.",key,i);
        }
    }
    *res= resource;
    return PEP_IO_OK;
}


static int xacml_subject_marshal(const xacml_subject_t * subject, hessian_object_t ** h_sub) {
    hessian_object_t * h_subject, * h_attrs, * h_attrs_key;
    const char * category;
    size_t list_l;
    int i;
    if (subject == NULL) {
        log_error("xacml_subject_marshal: NULL subject object.");
        return PEP_IO_ERROR;
    }
    h_subject= hessian_create(HESSIAN_MAP,XACML_HESSIAN_SUBJECT_CLASSNAME);
    if (h_subject == NULL) {
        log_error("xacml_subject_marshal: can't create Hessian map: %s.", XACML_HESSIAN_SUBJECT_CLASSNAME);
        return PEP_IO_ERROR;
    }
    /* category (can be null) */
    category= xacml_subject_getcategory(subject);
    if (category != NULL) {
        hessian_object_t * h_category= hessian_create(HESSIAN_STRING,category);
        hessian_object_t * h_category_key;
        if (h_category == NULL) {
            log_error("xacml_subject_marshal: can't create category Hessian string: %s.", category);
            hessian_delete(h_subject);
            return PEP_IO_ERROR;
        }
        h_category_key= hessian_create(HESSIAN_STRING,XACML_HESSIAN_SUBJECT_CATEGORY);
        if (hessian_map_add(h_subject,h_category_key,h_category) != HESSIAN_OK) {
            log_error("xacml_subject_marshal: can't add category Hessian string to subject Hessian map.");
            hessian_delete(h_subject);
            hessian_delete(h_category_key);
            hessian_delete(h_category);
            return PEP_IO_ERROR;
        }
    }
    /* attributes list */
    h_attrs= hessian_create(HESSIAN_LIST);
    if (h_attrs == NULL) {
        log_error("xacml_subject_marshal: can't create attributes Hessian list.");
        hessian_delete(h_subject);
        return PEP_IO_ERROR;
    }
    list_l= xacml_subject_attributes_length(subject);
    for (i= 0; i < list_l; i++) {
        xacml_attribute_t * attr= xacml_subject_getattribute(subject,i);
        hessian_object_t * h_attr= NULL;
        if (xacml_attribute_marshal(attr,&h_attr) != PEP_IO_OK) {
            log_error("xacml_subject_marshal: can't marshal XACML attribute at: %d.", i);
            hessian_delete(h_subject);
            hessian_delete(h_attrs);
            return PEP_IO_ERROR;
        }
        if (hessian_list_add(h_attrs,h_attr) != HESSIAN_OK) {
            log_error("xacml_subject_marshal: can't add Hessian attribute to attributes Hessian list at: %d.", i);
            hessian_delete(h_subject);
            hessian_delete(h_attrs);
            hessian_delete(h_attr);
            return PEP_IO_ERROR;
        }
    }
    h_attrs_key= hessian_create(HESSIAN_STRING,XACML_HESSIAN_SUBJECT_ATTRIBUTES);
    if (hessian_map_add(h_subject,h_attrs_key,h_attrs) != HESSIAN_OK) {
        log_error("xacml_subject_marshal: can't add attributes Hessian list to subject Hessian map.");
        hessian_delete(h_subject);
        hessian_delete(h_attrs);
        hessian_delete(h_attrs_key);
        return PEP_IO_ERROR;
    }
    *h_sub= h_subject;
    return PEP_IO_OK;
}

static int xacml_subject_unmarshal(xacml_subject_t ** subj, const hessian_object_t * h_subject) {
    const char * map_type;
    xacml_subject_t * subject;
    size_t map_l;
    int i, j;
    if (hessian_gettype(h_subject) != HESSIAN_MAP) {
        log_error("xacml_subject_unmarshal: wrong Hessian type: %d (%s).", hessian_gettype(h_subject), hessian_getclassname(h_subject));
        return PEP_IO_ERROR;
    }
    map_type= hessian_map_gettype(h_subject);
    if (map_type == NULL) {
        log_error("xacml_subject_unmarshal: NULL Hessian map type.");
        return PEP_IO_ERROR;
    }
    if (strcmp(XACML_HESSIAN_SUBJECT_CLASSNAME,map_type) != 0) {
        log_error("xacml_subject_unmarshal: wrong Hessian map type: %s.",map_type);
        return PEP_IO_ERROR;
    }

    subject= xacml_subject_create();
    if (subject == NULL) {
        log_error("xacml_subject_unmarshal: can't create XACML subject.");
        return PEP_IO_ERROR;
    }

    /* parse all map pair<key>s */
    map_l= hessian_map_length(h_subject);
    for(i= 0; i<map_l; i++) {
        hessian_object_t * h_map_key= hessian_map_getkey(h_subject,i);
        const char * key;
        if (hessian_gettype(h_map_key) != HESSIAN_STRING) {
            log_error("xacml_subject_unmarshal: Hessian map<key> is not an Hessian string at: %d.",i);
            xacml_subject_delete(subject);
            return PEP_IO_ERROR;
        }
        key= hessian_string_getstring(h_map_key);
        if (key == NULL) {
            log_error("xacml_subject_unmarshal: Hessian map<key>: NULL string at: %d.",i);
            xacml_subject_delete(subject);
            return PEP_IO_ERROR;
        }
        /* category (can be null) */
        if (strcmp(XACML_HESSIAN_SUBJECT_CATEGORY,key) == 0) {
            hessian_object_t * h_string= hessian_map_getvalue(h_subject,i);
            hessian_t h_string_type= hessian_gettype(h_string);
            const char * category;
            if ( h_string_type != HESSIAN_STRING && h_string_type != HESSIAN_NULL) {
                log_error("xacml_subject_unmarshal: Hessian map<'%s',value> is not a Hessian string or null at: %d.",key,i);
                xacml_subject_delete(subject);
                return PEP_IO_ERROR;
            }
            category= NULL;
            if (h_string_type == HESSIAN_STRING) {
                category= hessian_string_getstring(h_string);
            }
            if (xacml_subject_setcategory(subject,category) != PEP_XACML_OK) {
                log_error("xacml_subject_unmarshal: can't set category: %s to XACML subject.",category);
                xacml_subject_delete(subject);
                return PEP_IO_ERROR;
            }
        }
        /* attributes list */
        else if (strcmp(XACML_HESSIAN_SUBJECT_ATTRIBUTES,key) == 0) {
            hessian_object_t * h_attributes= hessian_map_getvalue(h_subject,i);
            size_t h_attributes_l;
            if (hessian_gettype(h_attributes) != HESSIAN_LIST) {
                log_error("xacml_subject_unmarshal: Hessian map<'%s',value> is not a Hessian list at: %d.",key, i);
                xacml_subject_delete(subject);
                return PEP_IO_ERROR;
            }
            h_attributes_l= hessian_list_length(h_attributes);
            for(j= 0; j<h_attributes_l; j++) {
                hessian_object_t * h_attr= hessian_list_get(h_attributes,j);
                xacml_attribute_t * attribute= NULL;
                if (xacml_attribute_unmarshal(&attribute,h_attr) != PEP_IO_OK) {
                    log_error("xacml_subject_unmarshal: can't unmarshal XACML attribute at: %d.",j);
                    xacml_subject_delete(subject);
                    return PEP_IO_ERROR;
                }
                if (xacml_subject_addattribute(subject,attribute) != PEP_XACML_OK) {
                    log_error("xacml_subject_unmarshal: can't add XACML attribute to XACML subject at: %d",j);
                    xacml_subject_delete(subject);
                    xacml_attribute_delete(attribute);
                    return PEP_IO_ERROR;
                }
            }

        }
        else {
            /* unkown key ??? */
            log_warn("xacml_subject_unmarshal: unknown Hessian map<key>: %s at: %d.",key,i);
        }

    }

    *subj= subject;
    return PEP_IO_OK;
}


/* OK */
pep_error_t xacml_request_marshalling(const xacml_request_t * request, BUFFER * output) {
    hessian_object_t * h_request= NULL;
    if (xacml_request_marshal(request,&h_request) != PEP_IO_OK) {
        log_error("xacml_request_marshalling: can't marshal XACML request into Hessian object.");
        /* pep_errmsg("failed to marshal XACML request into Hessian object"); */
        return PEP_ERR_MARSHALLING_HESSIAN;
    }
    if (hessian_serialize(h_request,output) != HESSIAN_OK) {
        log_error("xacml_request_marshalling: failed to serialize Hessian object.");
        hessian_delete(h_request);
        /* pep_errmsg("failed to serialize Hessian object"); */
        return PEP_ERR_MARSHALLING_IO;
    }
    hessian_delete(h_request);
    return PEP_OK;
}

/* OK */
pep_error_t xacml_response_unmarshalling(xacml_response_t ** response, BUFFER * input) {
    hessian_object_t * h_response= hessian_deserialize(input);
    if (h_response == NULL) {
        log_error("xacml_response_unmarshalling: failed to deserialize Hessian object.");
        /* pep_errmsg("failed to deserialize base64 encoded Hessian object"); */
        return PEP_ERR_UNMARSHALLING_IO;
    }
    if (xacml_response_unmarshal(response, h_response) != PEP_IO_OK) {
        log_error("xacml_response_unmarshalling: can't unmarshal XACML response from Hessian object.");
        hessian_delete(h_response);
        /* pep_errmsg("failed to unmarshal XACML response from Hessian object"); */
        return PEP_ERR_UNMARSHALLING_HESSIAN;
    }
    hessian_delete(h_response);
    return PEP_OK;

}

/* OK */
static int xacml_response_unmarshal(xacml_response_t ** resp, const hessian_object_t * h_response) {
    const char * map_type;
    xacml_response_t * response;
    size_t map_l;
    int i, j;
    if (hessian_gettype(h_response) != HESSIAN_MAP) {
        log_error("xacml_response_unmarshal: wrong Hessian type: %d (%s).", hessian_gettype(h_response), hessian_getclassname(h_response));
        return PEP_IO_ERROR;
    }
    map_type= hessian_map_gettype(h_response);
    if (map_type == NULL) {
        log_error("xacml_response_unmarshal: NULL Hessian map type.");
        return PEP_IO_ERROR;
    }
    if (strcmp(XACML_HESSIAN_RESPONSE_CLASSNAME,map_type) != 0) {
        log_error("xacml_response_unmarshal: wrong Hessian map type: %s.",map_type);
        return PEP_IO_ERROR;
    }

    response= xacml_response_create();
    if (response == NULL) {
        log_error("xacml_response_unmarshal: can't create XACML response.");
        return PEP_IO_ERROR;
    }

    /* parse all map pair<key>s */
    map_l= hessian_map_length(h_response);
    for(i= 0; i<map_l; i++) {
        hessian_object_t * h_map_key= hessian_map_getkey(h_response,i);
        const char * key;
        if (hessian_gettype(h_map_key) != HESSIAN_STRING) {
            log_error("xacml_response_unmarshal: Hessian map<key> is not an Hessian string at: %d.",i);
            xacml_response_delete(response);
            return PEP_IO_ERROR;
        }
        key= hessian_string_getstring(h_map_key);
        if (key == NULL) {
            log_error("xacml_response_unmarshal: Hessian map<key>: NULL string at: %d.",i);
            xacml_response_delete(response);
            return PEP_IO_ERROR;
        }
        /* request (can be null???) */
        if (strcmp(XACML_HESSIAN_RESPONSE_REQUEST,key) == 0) {
            hessian_object_t * h_request= hessian_map_getvalue(h_response,i);
            if (hessian_gettype(h_request) != HESSIAN_NULL) {
                xacml_request_t * request= NULL;
                if (xacml_request_unmarshal(&request,h_request) != PEP_IO_OK) {
                    log_error("xacml_response_unmarshal: can't unmarshal XACML request.");
                    xacml_response_delete(response);
                    return PEP_IO_ERROR;
                }
                if (xacml_response_setrequest(response,request) != PEP_XACML_OK) {
                    log_error("xacml_response_unmarshal: can't set XACML request in XACML response.");
                    xacml_request_delete(request);
                    xacml_response_delete(response);
                    return PEP_IO_ERROR;
                }
            }
            else {
                log_warn("xacml_response_unmarshal: XACML request is NULL.");
            }

        }
        /* results list */
        else if (strcmp(XACML_HESSIAN_RESPONSE_RESULTS,key) == 0) {
            hessian_object_t * h_results= hessian_map_getvalue(h_response,i);
            size_t h_results_l;
            if (hessian_gettype(h_results) != HESSIAN_LIST) {
                log_error("xacml_response_unmarshal: Hessian map<'%s',value> is not a Hessian list at: %d.",key,i);
                xacml_response_delete(response);
                return PEP_IO_ERROR;
            }
            h_results_l= hessian_list_length(h_results);
            for(j= 0; j<h_results_l; j++) {
                hessian_object_t * h_result= hessian_list_get(h_results,j);
                xacml_result_t * result= NULL;
                if (xacml_result_unmarshal(&result,h_result) != PEP_IO_OK) {
                    log_error("xacml_response_unmarshal: can't unmarshal XACML result at: %d.",j);
                    xacml_response_delete(response);
                    return PEP_IO_ERROR;
                }
                if (xacml_response_addresult(response,result) != PEP_XACML_OK) {
                    log_error("xacml_response_unmarshal: can't add XACML result at: %d to XACML response.",j);
                    xacml_result_delete(result);
                    xacml_response_delete(response);
                    return PEP_IO_ERROR;
                }
            }
        }
        else {
            /* unkown key ??? */
            log_warn("xacml_response_unmarshal: unknown Hessian map<key>: %s at: %d.",key,i);
        }
    }
    *resp= response;
    return PEP_IO_OK;
}

/* OK */
static int xacml_result_unmarshal(xacml_result_t ** res, const hessian_object_t * h_result) {
    const char * map_type;
    xacml_result_t * result;
    size_t map_l;
    int i, j;
    if (hessian_gettype(h_result) != HESSIAN_MAP) {
        log_error("xacml_result_unmarshal: wrong Hessian type: %d (%s).", hessian_gettype(h_result), hessian_getclassname(h_result));
        return PEP_IO_ERROR;
    }
    map_type= hessian_map_gettype(h_result);
    if (map_type == NULL) {
        log_error("xacml_result_unmarshal: NULL Hessian map type.");
        return PEP_IO_ERROR;
    }
    if (strcmp(XACML_HESSIAN_RESULT_CLASSNAME,map_type) != 0) {
        log_error("xacml_result_unmarshal: wrong Hessian map type: %s.",map_type);
        return PEP_IO_ERROR;
    }
    result= xacml_result_create();
    if (result == NULL) {
        log_error("xacml_result_unmarshal: can't create XACML result.");
        return PEP_IO_ERROR;
    }

    /* parse all map pair<key>s */
    map_l= hessian_map_length(h_result);
    for(i= 0; i<map_l; i++) {
        hessian_object_t * h_map_key= hessian_map_getkey(h_result,i);
        const char * key;
        if (hessian_gettype(h_map_key) != HESSIAN_STRING) {
            log_error("xacml_result_unmarshal: Hessian map<key> is not an Hessian string at: %d.",i);
            xacml_result_delete(result);
            return PEP_IO_ERROR;
        }
        key= hessian_string_getstring(h_map_key);
        if (key == NULL) {
            log_error("xacml_result_unmarshal: Hessian map<key>: NULL string at: %d.",i);
            xacml_result_delete(result);
            return PEP_IO_ERROR;
        }
        /* decision (enum, mandatory) */
        if (strcmp(XACML_HESSIAN_RESULT_DECISION,key) == 0) {
            hessian_object_t * h_integer= hessian_map_getvalue(h_result,i);
            int32_t decision;
            if (hessian_gettype(h_integer) != HESSIAN_INTEGER) {
                log_error("xacml_result_unmarshal: Hessian map<'%s',value> is not a Hessian integer at: %d.",key,i);
                xacml_result_delete(result);
                return PEP_IO_ERROR;
            }
            decision= hessian_integer_getvalue(h_integer);
            if (xacml_result_setdecision(result,decision) != PEP_XACML_OK) {
                log_error("xacml_result_unmarshal: can't set decision: %d to XACML result.",(int)decision);
                xacml_result_delete(result);
                return PEP_IO_ERROR;
            }
        }
        /* resourceid (optional) */
        else if (strcmp(XACML_HESSIAN_RESULT_RESOURCEID,key) == 0) {
            hessian_object_t * h_string= hessian_map_getvalue(h_result,i);
            hessian_t h_string_type= hessian_gettype(h_string);
            const char * resourceid;
            if ( h_string_type != HESSIAN_STRING && h_string_type != HESSIAN_NULL) {
                log_error("xacml_result_unmarshal: Hessian map<'%s',value> is not a Hessian string or null at: %d.",key,i);
                xacml_result_delete(result);
                return PEP_IO_ERROR;
            }
            resourceid= NULL;
            if (h_string_type == HESSIAN_STRING) {
                resourceid= hessian_string_getstring(h_string);
            }
            if (xacml_result_setresourceid(result, resourceid) != PEP_XACML_OK) {
                log_error("xacml_result_unmarshal: can't set resourceid: %s to XACML result.",resourceid);
                xacml_result_delete(result);
                return PEP_IO_ERROR;
            }
        }
        /* status (null?) */
        else if (strcmp(XACML_HESSIAN_RESULT_STATUS,key) == 0) {
            hessian_object_t * h_status= hessian_map_getvalue(h_result,i);
            if (hessian_gettype(h_status) != HESSIAN_NULL) {
                xacml_status_t * status= NULL;
                if (xacml_status_unmarshal(&status,h_status) != PEP_IO_OK) {
                    log_error("xacml_result_unmarshal: can't unmarshal XACML status.");
                    xacml_result_delete(result);
                    return PEP_IO_ERROR;
                }
                if (xacml_result_setstatus(result,status) != PEP_XACML_OK) {
                    log_error("xacml_result_unmarshal: can't set XACML status to XACML result.");
                    xacml_result_delete(result);
                    xacml_status_delete(status);
                    return PEP_IO_ERROR;
                }
            }
            else {
                log_warn("xacml_result_unmarshal: XACML status is NULL.");
            }
        }
        /* obligations list */
        else if (strcmp(XACML_HESSIAN_RESULT_OBLIGATIONS,key) == 0) {
            hessian_object_t * h_obligations= hessian_map_getvalue(h_result,i);
            size_t h_obligations_l;
            if (hessian_gettype(h_obligations) != HESSIAN_LIST) {
                log_error("xacml_result_unmarshal: Hessian map<'%s',value> is not a Hessian list.",key);
                xacml_result_delete(result);
                return PEP_IO_ERROR;
            }
            h_obligations_l= hessian_list_length(h_obligations);
            for(j= 0; j<h_obligations_l; j++) {
                hessian_object_t * h_obligation= hessian_list_get(h_obligations,j);
                xacml_obligation_t * obligation= NULL;
                if (xacml_obligation_unmarshal(&obligation,h_obligation) != PEP_IO_OK) {
                    log_error("xacml_result_unmarshal: can't unmarshal XACML obligation at: %d.", j);
                    xacml_result_delete(result);
                    return PEP_IO_ERROR;
                }
                if (xacml_result_addobligation(result,obligation) != PEP_XACML_OK) {
                    log_error("xacml_result_unmarshal: can't add XACML obligation at: %d to XACML result.", j);
                    xacml_result_delete(result);
                    xacml_obligation_delete(obligation);
                    return PEP_IO_ERROR;
                }
            }
        }
        else {
            /* unkown key ??? */
            log_warn("xacml_result_unmarshal: unknown map<key>: %s at: %d.",key,i);
        }
    }
    *res= result;
    return PEP_IO_OK;
}

/* OK */
static int xacml_status_unmarshal(xacml_status_t ** st, const hessian_object_t * h_status) {
    const char * map_type;
    xacml_status_t * status;
    size_t map_l;
    int i;
    if (hessian_gettype(h_status) != HESSIAN_MAP) {
        log_error("xacml_status_unmarshal: wrong Hessian type: %d (%s).", hessian_gettype(h_status), hessian_getclassname(h_status));
        return PEP_IO_ERROR;
    }
    map_type= hessian_map_gettype(h_status);
    if (map_type == NULL) {
        log_error("xacml_status_unmarshal: NULL Hessian map type.");
        return PEP_IO_ERROR;
    }
    if (strcmp(XACML_HESSIAN_STATUS_CLASSNAME,map_type) != 0) {
        log_error("xacml_status_unmarshal: wrong Hessian map type: %s.",map_type);
        return PEP_IO_ERROR;
    }
    status= xacml_status_create(NULL);
    if (status == NULL) {
        log_error("xacml_status_unmarshal: can't create XACML status.");
        return PEP_IO_ERROR;
    }
    /* parse all map pair<key>s */
    map_l= hessian_map_length(h_status);
    for(i= 0; i<map_l; i++) {
        hessian_object_t * h_map_key= hessian_map_getkey(h_status,i);
        const char * key;
        if (hessian_gettype(h_map_key) != HESSIAN_STRING) {
            log_error("xacml_status_unmarshal: Hessian map<key> is not an Hessian string at: %d.",i);
            xacml_status_delete(status);
            return PEP_IO_ERROR;
        }
        key= hessian_string_getstring(h_map_key);
        if (key == NULL) {
            log_error("xacml_status_unmarshal: Hessian map<key>: NULL string at: %d.",i);
            xacml_status_delete(status);
            return PEP_IO_ERROR;
        }
        /* message (can be null) */
        if (strcmp(XACML_HESSIAN_STATUS_MESSAGE,key) == 0) {
            hessian_object_t * h_string= hessian_map_getvalue(h_status,i);
            if (hessian_gettype(h_string) != HESSIAN_NULL) {
                const char * message;
                if (hessian_gettype(h_string) != HESSIAN_STRING) {
                    log_error("xacml_status_unmarshal: Hessian map<'%s',value> is not a Hessian string at: %d.",key,i);
                    xacml_status_delete(status);
                    return PEP_IO_ERROR;
                }
                message= hessian_string_getstring(h_string);
                if (xacml_status_setmessage(status,message) != PEP_XACML_OK) {
                    log_error("xacml_status_unmarshal: can't set message: %s to XACML status at: %d",message,i);
                    xacml_status_delete(status);
                    return PEP_IO_ERROR;
                }
            }
        }
        /* subcode (can be null) */
        else if (strcmp(XACML_HESSIAN_STATUS_CODE,key) == 0) {
            hessian_object_t * h_statuscode= hessian_map_getvalue(h_status,i);
            if (hessian_gettype(h_statuscode) != HESSIAN_NULL) {
                xacml_statuscode_t * statuscode= NULL;
                if (xacml_statuscode_unmarshal(&statuscode,h_statuscode) != PEP_IO_OK) {
                    log_error("xacml_status_unmarshal: can't unmarshal XACML statuscode at: %d.", i);
                    xacml_status_delete(status);
                    return PEP_IO_ERROR;
                }
                if (xacml_status_setcode(status,statuscode) != PEP_XACML_OK) {
                    log_error("xacml_status_unmarshal: can't set XACML statuscode to XACML status.");
                    xacml_status_delete(status);
                    xacml_statuscode_delete(statuscode);
                    return PEP_IO_ERROR;
                }
            }
            else {
                log_warn("xacml_status_unmarshal: subcode XACML statuscode is NULL.");
            }
        }
        else {
            log_warn("xacml_status_unmarshal: unknown Hessian map<key>: %s at: %d.",key,i);
        }
    }
    *st= status;
    return PEP_IO_OK;
}

/* OK */
static int xacml_statuscode_unmarshal(xacml_statuscode_t ** stc, const hessian_object_t * h_statuscode) {
    const char * map_type;
    xacml_statuscode_t * statuscode;
    size_t map_l;
    int i;
    if (hessian_gettype(h_statuscode) != HESSIAN_MAP) {
        log_error("xacml_statuscode_unmarshal: wrong Hessian type: %d (%s).", hessian_gettype(h_statuscode), hessian_getclassname(h_statuscode));
        return PEP_IO_ERROR;
    }
    map_type= hessian_map_gettype(h_statuscode);
    if (map_type == NULL) {
        log_error("xacml_statuscode_unmarshal: NULL Hessian map type.");
        return PEP_IO_ERROR;
    }
    if (strcmp(XACML_HESSIAN_STATUSCODE_CLASSNAME,map_type) != 0) {
        log_error("xacml_statuscode_unmarshal: wrong Hessian map type: %s.",map_type);
        return PEP_IO_ERROR;
    }

    statuscode= xacml_statuscode_create(NULL);
    if (statuscode == NULL) {
        log_error("xacml_statuscode_unmarshal: cant't create XACML statuscode.");
        return PEP_IO_ERROR;
    }

    /* parse all map pair<key>s */
    map_l= hessian_map_length(h_statuscode);
    for(i= 0; i<map_l; i++) {
        hessian_object_t * h_map_key= hessian_map_getkey(h_statuscode,i);
        const char * key;
        if (hessian_gettype(h_map_key) != HESSIAN_STRING) {
            log_error("xacml_statuscode_unmarshal: Hessian map<key> is not an Hessian string at: %d.",i);
            xacml_statuscode_delete(statuscode);
            return PEP_IO_ERROR;
        }
        key= hessian_string_getstring(h_map_key);
        if (key == NULL) {
            log_error("xacml_statuscode_unmarshal: Hessian map<key>: NULL string at: %d.",i);
            xacml_statuscode_delete(statuscode);
            return PEP_IO_ERROR;
        }
        /* code (mandatory) */
        if (strcmp(XACML_HESSIAN_STATUSCODE_VALUE,key) == 0) {
            hessian_object_t * h_string= hessian_map_getvalue(h_statuscode,i);
            const char * code;
            if (hessian_gettype(h_string) != HESSIAN_STRING) {
                log_error("xacml_statuscode_unmarshal: Hessian map<'%s',value> is not a Hessian string at: %d.",key,i);
                xacml_statuscode_delete(statuscode);
                return PEP_IO_ERROR;
            }
            code = hessian_string_getstring(h_string);
            if (xacml_statuscode_setvalue(statuscode,code) != PEP_XACML_OK) {
                log_error("xacml_statuscode_unmarshal: can't set value: %s to XACML statuscode at: %d",code,i);
                xacml_statuscode_delete(statuscode);
                return PEP_IO_ERROR;
            }
        }
        /* subcode (can be null) */
        else if (strcmp(XACML_HESSIAN_STATUSCODE_SUBCODE, key) == 0) {
            hessian_object_t * h_subcode= hessian_map_getvalue(h_statuscode,i);
            if (hessian_gettype(h_subcode) != HESSIAN_NULL) {
                xacml_statuscode_t * subcode= NULL;
                if (xacml_statuscode_unmarshal(&subcode,h_subcode) != PEP_IO_OK) {
                    log_error("xacml_statuscode_unmarshal: can't unmarshal subcode XACML statuscode at: %d.",i);
                    xacml_statuscode_delete(statuscode);
                    return PEP_IO_ERROR;
                }
                if (xacml_statuscode_setsubcode(statuscode,subcode) != PEP_XACML_OK) {
                    log_error("xacml_statuscode_unmarshal: can't set subcode XACML statuscode to XACML statuscode at: %d",i);
                    xacml_statuscode_delete(statuscode);
                    xacml_statuscode_delete(subcode);
                    return PEP_IO_ERROR;
                }
            }
        }
        else {
            log_warn("xacml_statuscode_unmarshal: unknown Hessian map<key>: %s at: %d.",key,i);
        }
    }
    *stc= statuscode;
    return PEP_IO_OK;
}

static int xacml_obligation_unmarshal(xacml_obligation_t ** obl, const hessian_object_t * h_obligation) {
    const char * map_type;
    xacml_obligation_t * obligation;
    size_t map_l;
    int i, j;
    if (hessian_gettype(h_obligation) != HESSIAN_MAP) {
        log_error("xacml_obligation_unmarshal: wrong Hessian type: %d (%s).", hessian_gettype(h_obligation), hessian_getclassname(h_obligation));
        return PEP_IO_ERROR;
    }
    map_type= hessian_map_gettype(h_obligation);
    if (map_type == NULL) {
        log_error("xacml_obligation_unmarshal: NULL Hessian map type.");
        return PEP_IO_ERROR;
    }
    if (strcmp(XACML_HESSIAN_OBLIGATION_CLASSNAME,map_type) != 0) {
        log_error("xacml_obligation_unmarshal: wrong Hessian map type: %s.",map_type);
        return PEP_IO_ERROR;
    }
    obligation= xacml_obligation_create(NULL);
    if (obligation == NULL) {
        log_error("xacml_obligation_unmarshal: can't create XACML obligation.");
        return PEP_IO_ERROR;
    }

    /* parse all map pair<key>s */
    map_l= hessian_map_length(h_obligation);
    for(i= 0; i<map_l; i++) {
        hessian_object_t * h_map_key= hessian_map_getkey(h_obligation,i);
        const char * key;
        if (hessian_gettype(h_map_key) != HESSIAN_STRING) {
            log_error("xacml_obligation_unmarshal: Hessian map<key> is not an Hessian string at: %d.",i);
            xacml_obligation_delete(obligation);
            return PEP_IO_ERROR;
        }
        key= hessian_string_getstring(h_map_key);
        if (key == NULL) {
            log_error("xacml_obligation_unmarshal: Hessian map<key>: NULL string at: %d.",i);
            xacml_obligation_delete(obligation);
            return PEP_IO_ERROR;
        }

        /* id (mandatory) */
        if (strcmp(XACML_HESSIAN_OBLIGATION_ID,key) == 0) {
            hessian_object_t * h_string= hessian_map_getvalue(h_obligation,i);
            const char * id;
            if (hessian_gettype(h_string) != HESSIAN_STRING) {
                log_error("xacml_obligation_unmarshal: Hessian map<'%s',value> is not a Hessian string at: %d.",key,i);
                xacml_obligation_delete(obligation);
                return PEP_IO_ERROR;
            }
            id= hessian_string_getstring(h_string);
            if (xacml_obligation_setid(obligation,id) != PEP_XACML_OK) {
                log_error("xacml_obligation_unmarshal: can't set id: %s to XACML obligation at: %d",id,i);
                xacml_obligation_delete(obligation);
                return PEP_IO_ERROR;
            }
        }
        /* fulfillon (enum) */
        else if (strcmp(XACML_HESSIAN_OBLIGATION_FULFILLON,key) == 0) {
            hessian_object_t * h_integer= hessian_map_getvalue(h_obligation,i);
            int32_t fulfillon;
            if (hessian_gettype(h_integer) != HESSIAN_INTEGER) {
                log_error("xacml_obligation_unmarshal: Hessian map<'%s',value> is not a Hessian integer at: %d.",key,i);
                xacml_obligation_delete(obligation);
                return PEP_IO_ERROR;
            }
            fulfillon= hessian_integer_getvalue(h_integer);
            if (xacml_obligation_setfulfillon(obligation,fulfillon) != PEP_XACML_OK) {
                log_error("xacml_obligation_unmarshal: can't set fulfillon: %d to XACML obligation at: %d",(int)fulfillon,i);
                xacml_obligation_delete(obligation);
                return PEP_IO_ERROR;
            }
        }
        /* attribute assignments list */
        else if (strcmp(XACML_HESSIAN_OBLIGATION_ASSIGNMENTS,key) == 0) {
            hessian_object_t * h_assignments= hessian_map_getvalue(h_obligation,i);
            size_t h_assignments_l;
            if (hessian_gettype(h_assignments) != HESSIAN_LIST) {
                log_error("xacml_obligation_unmarshal: Hessian map<'%s',value> is not a Hessian list at: %d.",key, i);
                xacml_obligation_delete(obligation);
                return PEP_IO_ERROR;
            }
            h_assignments_l= hessian_list_length(h_assignments);
            for(j= 0; j<h_assignments_l; j++) {
                hessian_object_t * h_assignment= hessian_list_get(h_assignments,j);
                xacml_attributeassignment_t * attribute= NULL;
                if (xacml_attributeassignment_unmarshal(&attribute,h_assignment) != PEP_IO_OK) {
                    log_error("xacml_obligation_unmarshal: can't unmarshal XACML attribute assignment at: %d.",j);
                    xacml_obligation_delete(obligation);
                    return PEP_IO_ERROR;
                }
                if (xacml_obligation_addattributeassignment(obligation,attribute) != PEP_XACML_OK) {
                    log_error("xacml_obligation_unmarshal: can't add XACML attribute assignment to XACML obligation at: %d",j);
                    xacml_obligation_delete(obligation);
                    xacml_attributeassignment_delete(attribute);
                    return PEP_IO_ERROR;
                }
            }
        }
        else {
            log_warn("xacml_obligation_unmarshal: unknown Hessian map<key>: %s at: %d.",key,i);
        }
    }

    *obl= obligation;
    return PEP_IO_OK;

}

static int xacml_attributeassignment_unmarshal(xacml_attributeassignment_t ** attr, const hessian_object_t * h_attribute) {
    const char * map_type;
    xacml_attributeassignment_t * attribute;
    size_t map_l;
    int i, j;
    if (hessian_gettype(h_attribute) != HESSIAN_MAP) {
        log_error("xacml_attributeassignment_unmarshal: wrong Hessian type: %d (%s).", hessian_gettype(h_attribute), hessian_getclassname(h_attribute));
        return PEP_IO_ERROR;
    }
    map_type= hessian_map_gettype(h_attribute);
    if (map_type == NULL) {
        log_error("xacml_attributeassignment_unmarshal: NULL Hessian map type.");
        return PEP_IO_ERROR;
    }
    if (strcmp(XACML_HESSIAN_ATTRIBUTEASSIGNMENT_CLASSNAME,map_type) != 0) {
        log_error("xacml_attributeassignment_unmarshal: wrong Hessian map type: %s.",map_type);
        return PEP_IO_ERROR;
    }

    attribute= xacml_attributeassignment_create(NULL);
    if (attribute == NULL) {
        log_error("xacml_attributeassignment_unmarshal: can't create XACML attribute assignment.");
        return PEP_IO_ERROR;
    }

    /* parse all map pair<key>s */
    map_l= hessian_map_length(h_attribute);
    for(i= 0; i<map_l; i++) {
        hessian_object_t * h_map_key= hessian_map_getkey(h_attribute,i);
        const char * key;
        if (hessian_gettype(h_map_key) != HESSIAN_STRING) {
            log_error("xacml_attributeassignment_unmarshal: Hessian map<key> is not an Hessian string at: %d.",i);
            xacml_attributeassignment_delete(attribute);
            return PEP_IO_ERROR;
        }
        key= hessian_string_getstring(h_map_key);
        if (key == NULL) {
            log_error("xacml_attributeassignment_unmarshal: Hessian map<key>: NULL string at: %d.",i);
            xacml_attributeassignment_delete(attribute);
            return PEP_IO_ERROR;
        }

        /* id (mandatory) */
        if (strcmp(XACML_HESSIAN_ATTRIBUTEASSIGNMENT_ID,key) == 0) {
            hessian_object_t * h_string= hessian_map_getvalue(h_attribute,i);
            const char * id;
            if (hessian_gettype(h_string) != HESSIAN_STRING) {
                log_error("xacml_attributeassignment_unmarshal: Hessian map<'%s',value> is not a Hessian string at: %d.",key,i);
                xacml_attributeassignment_delete(attribute);
                return PEP_IO_ERROR;
            }
            id= hessian_string_getstring(h_string);
            if (xacml_attributeassignment_setid(attribute,id) != PEP_XACML_OK) {
                log_error("xacml_attributeassignment_unmarshal: can't set id: %s to XACML attribute assignment at: %d",id,i);
                xacml_attributeassignment_delete(attribute);
                return PEP_IO_ERROR;
            }
        }
        /* datatype (optional) */
        else if (strcmp(XACML_HESSIAN_ATTRIBUTEASSIGNMENT_DATATYPE,key) == 0) {
            hessian_object_t * h_string= hessian_map_getvalue(h_attribute,i);
            hessian_t h_string_type= hessian_gettype(h_string);
            const char * datatype= NULL;
            if ( h_string_type != HESSIAN_STRING && h_string_type != HESSIAN_NULL) {
                log_error("xacml_attributeassignment_unmarshal: Hessian map<'%s',value> is not a Hessian string or null at: %d.",key,i);
                xacml_attributeassignment_delete(attribute);
                return PEP_IO_ERROR;
            }
            if (h_string_type == HESSIAN_STRING) {
                datatype= hessian_string_getstring(h_string);
            }
            if (xacml_attributeassignment_setdatatype(attribute,datatype) != PEP_XACML_OK) {
                log_error("xacml_attributeassignment_unmarshal: can't set datatype: %s to XACML attribute assignment at: %d",datatype,i);
                xacml_attributeassignment_delete(attribute);
                return PEP_IO_ERROR;
            }
        }
        /* value (optional) */
        else if (strcmp(XACML_HESSIAN_ATTRIBUTEASSIGNMENT_VALUE,key) == 0) {
            hessian_object_t * h_string= hessian_map_getvalue(h_attribute,i);
            hessian_t h_string_type= hessian_gettype(h_string);
            const char * value;
            if ( h_string_type != HESSIAN_STRING && h_string_type != HESSIAN_NULL) {
                log_error("xacml_attributeassignment_unmarshal: Hessian map<'%s',value> is not a Hessian string or null at: %d.",key,i);
                xacml_attributeassignment_delete(attribute);
                return PEP_IO_ERROR;
            }
            value= NULL;
            if (h_string_type == HESSIAN_STRING) {
                value= hessian_string_getstring(h_string);
            }
            if (xacml_attributeassignment_setvalue(attribute,value) != PEP_XACML_OK) {
                log_error("xacml_attributeassignment_unmarshal: can't set value: %s to XACML attribute assignment at: %d",value,i);
                xacml_attributeassignment_delete(attribute);
                return PEP_IO_ERROR;
            }
        }
        /* multiple values (back compatibility with PEPd <= 1.0) */
        else if (strcmp(XACML_HESSIAN_ATTRIBUTEASSIGNMENT_VALUES,key) == 0) {
            hessian_object_t * h_values= hessian_map_getvalue(h_attribute,i);
            size_t h_values_l;
            if (hessian_gettype(h_values) != HESSIAN_LIST) {
                log_error("xacml_attributeassignment_unmarshal: Hessian map<'%s',...> is not a Hessian list.",key);
                xacml_attributeassignment_delete(attribute);
                return PEP_IO_ERROR;
            }
            log_warn("xacml_attributeassignment_unmarshal: DEPRECATED Hessian map<'%s',...> received at: %d",key,i);
            h_values_l= hessian_list_length(h_values);
            for(j= 0; j<h_values_l; j++) {
                hessian_object_t * h_value= hessian_list_get(h_values,j);
                const char * value;
                if (hessian_gettype(h_value) != HESSIAN_STRING) {
                    log_error("xacml_attributeassignment_unmarshal: Hessian map<'%s',value> is not a Hessian string at: %d.",key,j);
                    xacml_attributeassignment_delete(attribute);
                    return PEP_IO_ERROR;
                }
                value= hessian_string_getstring(h_value);
                if (xacml_attributeassignment_setvalue(attribute,value) != PEP_XACML_OK) {
                    log_error("xacml_attributeassignment_unmarshal: can't set value: %s to XACML attribute assignment at: %d",value,j);
                    xacml_attributeassignment_delete(attribute);
                    return PEP_IO_ERROR;
                }
            }
        }
        else {
            log_warn("xacml_attributeassignment_unmarshal: unknown Hessian map<key>: %s at: %d.",key,i);
        }
    }
    *attr= attribute;
    return PEP_IO_OK;

}
