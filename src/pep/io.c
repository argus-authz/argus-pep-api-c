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
 * $Id: io.c,v 1.1 2008/12/12 11:34:27 vtschopp Exp $
 */

#include <string.h>
#include "pep/io.h"
#include "hessian/hessian.h"

/** functions return codes  */
#define PEP_IO_OK     0
#define PEP_IO_ERROR -1

/**
 * Hessian 1.0 marshalling/unmarshalling prototypes.
 *
 * Returns PEP_IO_OK or PEP_IO_ERROR.
 */
static int pep_attribute_marshal(const pep_attribute_t * attr, hessian_object_t ** h_attribute);
static int pep_attribute_unmarshal(pep_attribute_t ** attr, const hessian_object_t * h_attribute);
static int pep_subject_marshal(const pep_subject_t * subject, hessian_object_t ** h_subject);
static int pep_subject_unmarshal(pep_subject_t ** subject, const hessian_object_t * h_subject); // TODO: impl.
static int pep_resource_marshal(const pep_resource_t * resource, hessian_object_t ** h_resource);
static int pep_resource_unmarshal(pep_resource_t ** resource, const hessian_object_t * h_resource);  // TODO: impl.
static int pep_action_marshal(const pep_action_t * action, hessian_object_t ** h_action);
static int pep_action_unmarshal(pep_action_t ** action, const hessian_object_t * h_action);  // TODO: impl.
static int pep_environment_marshal(const pep_environment_t * env, hessian_object_t ** h_environment);
static int pep_environment_unmarshal(pep_environment_t ** env, const hessian_object_t * h_environment);  // TODO: impl.
static int pep_request_marshal(const pep_request_t * request, hessian_object_t ** h_request);
static int pep_request_unmarshal(pep_request_t ** request, const hessian_object_t * h_request);
static int pep_response_unmarshal(pep_response_t ** response, const hessian_object_t * h_response);
static int pep_result_unmarshal(pep_result_t ** result, const hessian_object_t * h_result);
static int pep_status_unmarshal(pep_status_t ** status, const hessian_object_t * h_status);
static int pep_statuscode_unmarshal(pep_status_code_t ** statuscode, const hessian_object_t * h_statuscode);
static int pep_obligation_unmarshal(pep_obligation_t ** obligation, const hessian_object_t * h_obligation);
static int pep_attributeassignment_unmarshal(pep_attribute_assignment_t ** attr, const hessian_object_t * h_attribute);

/**
 * Returns the Hessian map for this Action or a Hessian null if the Action is null.
 */
static int pep_action_marshal(const pep_action_t * action, hessian_object_t ** h_act) {
	hessian_object_t * h_action= NULL;
	if (action == NULL) {
		h_action= hessian_create(HESSIAN_NULL);
		if (h_action == NULL) {
			fprintf(stderr,"ERROR:pep_action_marshal: NULL action, but can't create hessian null.\n");
			return PEP_IO_ERROR;
		}
		*h_act= h_action;
		return PEP_IO_OK;
	}
	h_action= hessian_create(HESSIAN_MAP,PEP_ACTION_CLASSNAME);
	if (h_action == NULL) {
		fprintf(stderr,"ERROR:pep_action_marshal: can't create hessian map: %s.\n", PEP_ACTION_CLASSNAME);
		return PEP_IO_ERROR;
	}
	// attributes list
	hessian_object_t * h_attrs= hessian_create(HESSIAN_LIST);
	if (h_attrs == NULL) {
		fprintf(stderr,"ERROR:pep_action_marshal: can't create hessian list: %s.\n", PEP_ACTION_ATTRIBUTES);
		hessian_delete(h_action);
		return PEP_IO_ERROR;
	}
	size_t list_l= pep_action_attributes_length(action);
	int i= 0;
	for (i= 0; i < list_l; i++) {
		pep_attribute_t * attr= pep_action_getattribute(action,i);
		hessian_object_t * h_attr= NULL;
		if (pep_attribute_marshal(attr,&h_attr) != PEP_IO_OK) {
			fprintf(stderr,"ERROR:pep_action_marshal: can't marshal attribute at: %d.\n",i);
			hessian_delete(h_action);
			hessian_delete(h_attrs);
			return PEP_IO_ERROR;
		}
		if (hessian_list_add(h_attrs,h_attr) != HESSIAN_OK) {
			fprintf(stderr,"ERROR:pep_action_marshal: can't add hessian attribute to hessian list at: %d.\n",i);
			hessian_delete(h_action);
			hessian_delete(h_attrs);
			hessian_delete(h_attr);
			return PEP_IO_ERROR;
		}
	}
	hessian_object_t * h_attrs_key= hessian_create(HESSIAN_STRING,PEP_ACTION_ATTRIBUTES);
	if (h_attrs_key == NULL) {
		fprintf(stderr,"ERROR:pep_action_marshal: can't create hessian map<key>: %s.\n", PEP_ACTION_ATTRIBUTES);
		hessian_delete(h_action);
		hessian_delete(h_attrs);
		return PEP_IO_ERROR;
	}
	if (hessian_map_add(h_action,h_attrs_key,h_attrs) != HESSIAN_OK) {
		fprintf(stderr,"ERROR:pep_action_marshal: can't add %s hessian list to action hessian map.\n", PEP_ACTION_ATTRIBUTES);
		hessian_delete(h_action);
		hessian_delete(h_attrs);
		hessian_delete(h_attrs_key);
		return PEP_IO_ERROR;
	}
	*h_act= h_action;
	return PEP_IO_OK;
}

/**
 * Returns the Hessian object representing the PEP attribute.
 */
static int pep_attribute_marshal(const pep_attribute_t * attr, hessian_object_t ** h_attr) {
	if (attr == NULL) {
		fprintf(stderr,"ERROR:pep_attribute_marshal: NULL attribute object.\n");
		return PEP_IO_ERROR;
	}
	hessian_object_t * h_attribute= hessian_create(HESSIAN_MAP,PEP_ATTRIBUTE_CLASSNAME);
	if (h_attribute == NULL) {
		fprintf(stderr,"ERROR:pep_attribute_marshal: can't create attribute hessian map: %s\n", PEP_ATTRIBUTE_CLASSNAME);
		return PEP_IO_ERROR;
	}

	// mandatory attribute
	const char * attr_id= pep_attribute_getid(attr);
	hessian_object_t * h_value= hessian_create(HESSIAN_STRING,attr_id);
	if (h_value== NULL) {
		fprintf(stderr,"ERROR:pep_attribute_marshal: can't create hessian string: %s\n", attr_id);
		hessian_delete(h_attribute);
		return PEP_IO_ERROR;
	}
	hessian_object_t * h_key= hessian_create(HESSIAN_STRING,PEP_ATTRIBUTE_ID);
	if (hessian_map_add(h_attribute,h_key,h_value) != HESSIAN_OK) {
		fprintf(stderr,"ERROR:pep_attribute_marshal: can't add pair<'%s','%s'> to hessian map: %s\n", PEP_ATTRIBUTE_ID,attr_id,PEP_ATTRIBUTE_CLASSNAME);
		hessian_delete(h_attribute);
		hessian_delete(h_key);
		hessian_delete(h_value);
		return PEP_IO_ERROR;
	}
	// optional datatype
	const char * attr_dt= pep_attribute_getdatatype(attr);
	if (attr_dt != NULL) {
		h_key= hessian_create(HESSIAN_STRING,PEP_ATTRIBUTE_DATATYPE);
		h_value= hessian_create(HESSIAN_STRING,attr_dt);
		if (hessian_map_add(h_attribute,h_key,h_value) != HESSIAN_OK) {
			fprintf(stderr,"ERROR:pep_attribute_marshal: can't add pair<'%s','%s'> to hessian map: %s\n", PEP_ATTRIBUTE_DATATYPE,attr_dt,PEP_ATTRIBUTE_CLASSNAME);
			hessian_delete(h_attribute);
			hessian_delete(h_key);
			hessian_delete(h_value);
			return PEP_IO_ERROR;
		}
	}
	// optional issuer
	const char * attr_issuer= pep_attribute_getissuer(attr);
	if (attr_issuer != NULL) {
		h_key= hessian_create(HESSIAN_STRING,PEP_ATTRIBUTE_ISSUER);
		h_value= hessian_create(HESSIAN_STRING,attr_issuer);
		if (hessian_map_add(h_attribute,h_key,h_value) != HESSIAN_OK) {
			fprintf(stderr,"ERROR:pep_attribute_marshal: can't add pair<'%s','%s'> to hessian map: %s\n", PEP_ATTRIBUTE_ISSUER,attr_issuer,PEP_ATTRIBUTE_CLASSNAME);
			hessian_delete(h_attribute);
			hessian_delete(h_key);
			hessian_delete(h_value);
			return PEP_IO_ERROR;
		}
	}
	// values list
	hessian_object_t * h_values= hessian_create(HESSIAN_LIST);
	if (h_values == NULL) {
		fprintf(stderr,"ERROR:pep_attribute_marshal: can't create %s hessian list.\n", PEP_ATTRIBUTE_VALUES);
		hessian_delete(h_attribute);
		return PEP_IO_ERROR;
	}
	size_t values_l= pep_attribute_values_length(attr);
	int i= 0;
	for (i= 0; i < values_l; i++) {
		const char * value= pep_attribute_getvalue(attr,i);
		h_value= hessian_create(HESSIAN_STRING,value);
		if (h_value == NULL) {
			fprintf(stderr,"ERROR:pep_attribute_marshal: can't create hessian string: %s at: %d.\n", value, i);
			hessian_delete(h_attribute);
			hessian_delete(h_values);
			return PEP_IO_ERROR;
		}
		if (hessian_list_add(h_values,h_value) != HESSIAN_OK) {
			fprintf(stderr,"ERROR:pep_attribute_marshal: can't add hessian string: %s to hessian list.\n", value);
			hessian_delete(h_attribute);
			hessian_delete(h_values);
			hessian_delete(h_value);
			return PEP_IO_ERROR;
		}
	}
	hessian_object_t * h_values_key= hessian_create(HESSIAN_STRING,PEP_ATTRIBUTE_VALUES);
	if (hessian_map_add(h_attribute,h_values_key,h_values) != HESSIAN_OK) {
		fprintf(stderr,"ERROR:pep_attribute_marshal: can't add attributes hessian list to attribute hessian map.\n");
		hessian_delete(h_attribute);
		hessian_delete(h_values_key);
		hessian_delete(h_values);
		return PEP_IO_ERROR;
	}
	*h_attr= h_attribute;
	return PEP_IO_OK;
}

static int pep_attribute_unmarshal(pep_attribute_t ** attr, const hessian_object_t * h_attribute) {
	if (hessian_gettype(h_attribute) != HESSIAN_MAP) {
		fprintf(stderr,"ERROR:pep_attribute_unmarshal: wrong hessian type: %d (%s).\n", hessian_gettype(h_attribute), hessian_getclassname(h_attribute));
		//pep_attribute_delete(attribute);
		return PEP_IO_ERROR;
	}
	const char * map_type= hessian_map_gettype(h_attribute);
	if (map_type == NULL) {
		fprintf(stderr,"ERROR:pep_attribute_unmarshal: NULL hessian map type.\n");
		//pep_attribute_delete(attribute);
		return PEP_IO_ERROR;
	}
	if (strcmp(PEP_ATTRIBUTE_CLASSNAME,map_type) != 0) {
		fprintf(stderr,"ERROR:pep_attribute_unmarshal: wrong hessian map type: %s.\n",map_type);
		//pep_attribute_delete(attribute);
		return PEP_IO_ERROR;
	}

	pep_attribute_t * attribute= pep_attribute_create(NULL);
	if (attribute == NULL) {
		fprintf(stderr,"ERROR:pep_attribute_unmarshal: can't create PEP attribute.\n");
		//pep_attribute_delete(attribute);
		return PEP_IO_ERROR;
	}

	// parse all map pair<key>s
	size_t map_l= hessian_map_length(h_attribute);
	int i= 0;
	for(i= 0; i<map_l; i++) {
		hessian_object_t * h_map_key= hessian_map_getkey(h_attribute,i);
		if (hessian_gettype(h_map_key) != HESSIAN_STRING) {
			fprintf(stderr,"ERROR:pep_attribute_unmarshal: hessian map<key> is not an hessian string at: %d.\n",i);
			pep_attribute_delete(attribute);
			return PEP_IO_ERROR;
		}
		const char * key= hessian_string_getstring(h_map_key);
		if (key == NULL) {
			fprintf(stderr,"ERROR:pep_attribute_unmarshal: hessian map<key>: NULL string at: %d.\n",i);
			pep_attribute_delete(attribute);
			return PEP_IO_ERROR;
		}

		// id (mandatory)
		if (strcmp(PEP_ATTRIBUTE_ID,key) == 0) {
			hessian_object_t * h_string= hessian_map_getvalue(h_attribute,i);
			if (hessian_gettype(h_string) != HESSIAN_STRING) {
				fprintf(stderr,"ERROR:pep_attribute_unmarshal: hessian map<'%s',value> is not a hessian string at: %d.\n",key,i);
				pep_attribute_delete(attribute);
				return PEP_IO_ERROR;
			}
			const char * id = hessian_string_getstring(h_string);
			if (pep_attribute_setid(attribute,id) != PEP_MODEL_OK) {
				fprintf(stderr,"ERROR:pep_attribute_unmarshal: can't set id: %s to PEP attribute at: %d",id,i);
				pep_attribute_delete(attribute);
				return PEP_IO_ERROR;
			}
		}
		// datatype (optional)
		else if (strcmp(PEP_ATTRIBUTE_DATATYPE,key) == 0) {
			hessian_object_t * h_string= hessian_map_getvalue(h_attribute,i);
			hessian_t h_string_type= hessian_gettype(h_string);
			if ( h_string_type != HESSIAN_STRING && h_string_type != HESSIAN_NULL) {
				fprintf(stderr,"ERROR:pep_attribute_unmarshal: Hessian map<'%s',value> is not a Hessian string or null at: %d.\n",key,i);
				pep_attribute_delete(attribute);
				return PEP_IO_ERROR;
			}
			const char * datatype= NULL;
			if (h_string_type == HESSIAN_STRING)
				datatype= hessian_string_getstring(h_string);
			if (pep_attribute_setdatatype(attribute,datatype) != PEP_MODEL_OK) {
				fprintf(stderr,"ERROR:pep_attribute_unmarshal: can't set datatype: %s to PEP attribute at: %d",datatype,i);
				pep_attribute_delete(attribute);
				return PEP_IO_ERROR;
			}

		}
		// issuer (optional)
		else if (strcmp(PEP_ATTRIBUTE_ISSUER,key) == 0) {
			hessian_object_t * h_string= hessian_map_getvalue(h_attribute,i);
			hessian_t h_string_type= hessian_gettype(h_string);
			if ( h_string_type != HESSIAN_STRING && h_string_type != HESSIAN_NULL) {
				fprintf(stderr,"ERROR:pep_attribute_unmarshal: hessian map<'%s',value> is not a hessian string or null at: %d.\n",key,i);
				pep_attribute_delete(attribute);
				return PEP_IO_ERROR;
			}
			const char * issuer = NULL;
			if (h_string_type == HESSIAN_STRING)
				issuer= hessian_string_getstring(h_string);
			if (pep_attribute_setissuer(attribute,issuer) != PEP_MODEL_OK) {
				fprintf(stderr,"ERROR:pep_attribute_unmarshal: can't set issuer: %s to PEP attribute at: %d",issuer,i);
				pep_attribute_delete(attribute);
				return PEP_IO_ERROR;
			}

		}
		// values list
		else if (strcmp(PEP_ATTRIBUTE_VALUES,key) == 0) {
			hessian_object_t * h_values= hessian_map_getvalue(h_attribute,i);
			if (hessian_gettype(h_values) != HESSIAN_LIST) {
				fprintf(stderr,"ERROR:pep_attribute_unmarshal: hessian map<'%s',value> is not a hessian list.\n",key);
				pep_attribute_delete(attribute);
				return PEP_IO_ERROR;
			}
			size_t h_values_l= hessian_list_length(h_values);
			int j= 0;
			for(j= 0; j<h_values_l; j++) {
				hessian_object_t * h_value= hessian_list_get(h_values,j);
				if (hessian_gettype(h_value) != HESSIAN_STRING) {
					fprintf(stderr,"ERROR:pep_attribute_unmarshal: hessian map<'%s',value> is not a hessian string at: %d.\n",key,i);
					pep_attribute_delete(attribute);
					return PEP_IO_ERROR;
				}
				const char * value = hessian_string_getstring(h_value);
				if (pep_attribute_addvalue(attribute,value) != PEP_MODEL_OK) {
					fprintf(stderr,"ERROR:pep_attribute_unmarshal: can't add value: %s to PEP attribute at: %d",value,j);
					pep_attribute_delete(attribute);
					return PEP_IO_ERROR;
				}
			}

		}
		else {
			fprintf(stderr,"WARNING:pep_attribute_unmarshal: unknown Hessian map<key>: %s at: %d.\n",key,i);
		}
	}
	*attr= attribute;
	return PEP_IO_OK;
}

/**
 * Returns the Hessian map for this Environment or a Hessian null if the Environment is null.
 */
static int pep_environment_marshal(const pep_environment_t * env, hessian_object_t ** h_env) {
	if (env == NULL) {
		hessian_object_t * h_null= hessian_create(HESSIAN_NULL);
		if (h_null == NULL) {
			fprintf(stderr,"ERROR:pep_environment_marshal: NULL environment, but can't create hessian null.\n");
			return PEP_IO_ERROR;
		}
		*h_env= h_null;
		return PEP_IO_OK;
	}
	hessian_object_t * h_environment= h_environment= hessian_create(HESSIAN_MAP,PEP_ENVIRONMENT_CLASSNAME);
	if (h_environment == NULL) {
		fprintf(stderr,"ERROR:pep_environment_marshal: can't create hessian map: %s.\n", PEP_ENVIRONMENT_CLASSNAME);
		return PEP_IO_ERROR;
	}
	// attributes list
	hessian_object_t * h_attrs= hessian_create(HESSIAN_LIST);
	if (h_attrs == NULL) {
		fprintf(stderr,"ERROR:pep_environment_marshal: can't create %s hessian list.\n", PEP_ENVIRONMENT_ATTRIBUTES);
		hessian_delete(h_environment);
		return PEP_IO_ERROR;
	}
	size_t list_l= pep_environment_attributes_length(env);
	int i= 0;
	for (i= 0; i < list_l; i++) {
		pep_attribute_t * attr= pep_environment_getattribute(env,i);
		hessian_object_t * h_attr= NULL;
		if (pep_attribute_marshal(attr,&h_attr) != PEP_MODEL_OK) {
			fprintf(stderr,"ERROR:pep_environment_marshal: can't marshall PEP attribute at: %d\n",i);
			hessian_delete(h_environment);
			hessian_delete(h_attrs);
			return PEP_IO_ERROR;
		}
		if (hessian_list_add(h_attrs,h_attr) != HESSIAN_OK) {
			fprintf(stderr,"ERROR:pep_environment_marshal: can't add hessian attribute to hessian list at: %d\n",i);
			hessian_delete(h_environment);
			hessian_delete(h_attrs);
			hessian_delete(h_attr);
			return PEP_IO_ERROR;
		}
	}
	hessian_object_t * h_attrs_key= hessian_create(HESSIAN_STRING,PEP_ENVIRONMENT_ATTRIBUTES);
	if (hessian_map_add(h_environment,h_attrs_key,h_attrs) != HESSIAN_OK) {
		fprintf(stderr,"ERROR:pep_environment_marshal: can't add attributes hessian list to environment hessian map.\n");
		hessian_delete(h_environment);
		hessian_delete(h_attrs);
		hessian_delete(h_attrs_key);
		return PEP_IO_ERROR;
	}
	*h_env= h_environment;
	return PEP_IO_OK;
}

/**
 * Returns PEP_IO_OK or PEP_IO_ERROR
 */
static int pep_request_marshal(const pep_request_t * request, hessian_object_t ** h_req) {
	if (request == NULL) {
		fprintf(stderr,"ERROR:pep_request_marshal: NULL request object.\n");
		return PEP_IO_ERROR;
	}
	// request
	hessian_object_t * h_request= hessian_create(HESSIAN_MAP,PEP_REQUEST_CLASSNAME);
	if (h_request == NULL) {
		fprintf(stderr,"ERROR:pep_request_marshal: can't create request hessian map: %s.\n",PEP_REQUEST_CLASSNAME);
		return PEP_IO_ERROR;
	}
	// subjects list
	hessian_object_t * h_subjects= hessian_create(HESSIAN_LIST);
	if (h_subjects == NULL) {
		fprintf(stderr,"ERROR:pep_request_marshal: can't create subjects hessian list.\n");
		hessian_delete(h_request);
		return PEP_IO_ERROR;
	}
	size_t list_l= pep_request_subjects_length(request);
	int i= 0;
	for (i= 0; i < list_l; i++) {
		pep_subject_t * subject= pep_request_getsubject(request,i);
		hessian_object_t * h_subject= NULL;
		if (pep_subject_marshal(subject,&h_subject) != PEP_IO_OK) {
			fprintf(stderr,"ERROR:pep_request_marshal: failed to marshal PEP subject at: %d.\n",i);
			hessian_delete(h_request);
			hessian_delete(h_subjects);
			return PEP_IO_ERROR;
		}
		if (hessian_list_add(h_subjects,h_subject) != HESSIAN_OK) {
			fprintf(stderr,"ERROR:pep_request_marshal: can't add hessian subject %d in hessian subjects list.\n",i);
			hessian_delete(h_request);
			hessian_delete(h_subjects);
			hessian_delete(h_subject);
			return PEP_IO_ERROR;
		}
	}
	hessian_object_t * h_subjects_key= hessian_create(HESSIAN_STRING,PEP_REQUEST_SUBJECTS);
	if (hessian_map_add(h_request,h_subjects_key,h_subjects) != HESSIAN_OK) {
		fprintf(stderr,"ERROR:pep_request_marshal: can't add hessian subjects list in hessian request map.\n");
		hessian_delete(h_request);
		hessian_delete(h_subjects_key);
		hessian_delete(h_subjects);
		return PEP_IO_ERROR;
	}
	// resources list
	hessian_object_t * h_resources= hessian_create(HESSIAN_LIST);
	if (h_resources == NULL) {
		fprintf(stderr,"ERROR:pep_request_marshal: can't create resources hessian list.\n");
		hessian_delete(h_request);
		return PEP_IO_ERROR;
	}
	list_l= pep_request_resources_length(request);
	for (i= 0; i < list_l; i++) {
		pep_resource_t * resource= pep_request_getresource(request,i);
		hessian_object_t * h_resource= NULL;
		if (pep_resource_marshal(resource,&h_resource) != PEP_IO_OK) {
			fprintf(stderr,"ERROR:pep_request_marshal: failed to marshal PEP resource at: %d.\n",i);
			hessian_delete(h_request);
			hessian_delete(h_resources);
			return PEP_IO_ERROR;
		}
		if (hessian_list_add(h_resources,h_resource) != HESSIAN_OK) {
			fprintf(stderr,"ERROR:pep_request_marshal: can't add hessian resource %d to hessian resources list.\n",i);
			hessian_delete(h_request);
			hessian_delete(h_resources);
			hessian_delete(h_resource);
			return PEP_IO_ERROR;
		}
	}
	hessian_object_t * h_resources_key= hessian_create(HESSIAN_STRING,PEP_REQUEST_RESOURCES);
	if (hessian_map_add(h_request,h_resources_key,h_resources) != HESSIAN_OK) {
		fprintf(stderr,"ERROR:pep_request_marshal: can't add hessian resources list to hessian request map.\n");
		hessian_delete(h_request);
		hessian_delete(h_resources_key);
		hessian_delete(h_resources);
		return PEP_IO_ERROR;
	}
	// action
	pep_action_t * action= pep_request_getaction(request);
	hessian_object_t * h_action= NULL;
	if (pep_action_marshal(action,&h_action) != PEP_IO_OK) {
		fprintf(stderr,"ERROR:pep_request_marshal: failed to marshal PEP action.\n");
		hessian_delete(h_request);
		return PEP_IO_ERROR;
	}
	hessian_object_t * h_action_key= hessian_create(HESSIAN_STRING,PEP_REQUEST_ACTION);
	if (hessian_map_add(h_request,h_action_key,h_action) != HESSIAN_OK) {
		fprintf(stderr,"ERROR:pep_request_marshal: can't add hessian action to hessian request.\n");
		hessian_delete(h_request);
		hessian_delete(h_action_key);
		hessian_delete(h_action);
		return PEP_IO_ERROR;
	}
	// environment
	pep_environment_t * environment= pep_request_getenvironment(request);
	hessian_object_t * h_environment= NULL;
	if (pep_environment_marshal(environment,&h_environment) != PEP_IO_OK) {
		fprintf(stderr,"ERROR:pep_request_marshal: failed to marshal PEP environment.\n");
		hessian_delete(h_request);
		return PEP_IO_ERROR;
	}
	hessian_object_t * h_environment_key= hessian_create(HESSIAN_STRING,PEP_REQUEST_ENVIRONMENT);
	if (hessian_map_add(h_request,h_environment_key,h_environment) != HESSIAN_OK) {
		fprintf(stderr,"ERROR:pep_request_marshal: can't add hessian environment to hessian request.\n");
		hessian_delete(h_request);
		hessian_delete(h_environment_key);
		hessian_delete(h_environment);
		return PEP_IO_ERROR;
	}
	*h_req= h_request;
	return PEP_IO_OK;
}

static int pep_request_unmarshal(pep_request_t ** req, const hessian_object_t * h_request) {
	// FIXME: implement
	fprintf(stderr,"TODO:pep_request_unmarshal not yet implemented\n");
	pep_request_t * request= pep_request_create();
	*req= request;
	return PEP_IO_OK;
}


static int pep_resource_marshal(const pep_resource_t * resource, hessian_object_t ** h_res) {
	if (resource == NULL) {
		fprintf(stderr,"ERROR:pep_resource_marshal: NULL resource object.\n");
		return PEP_IO_ERROR;
	}
	hessian_object_t * h_resource= hessian_create(HESSIAN_MAP,PEP_RESOURCE_CLASSNAME);
	if (h_resource == NULL) {
		fprintf(stderr,"ERROR:pep_resource_marshal: can't create hessian map: %s.\n", PEP_RESOURCE_CLASSNAME);
		return PEP_IO_ERROR;
	}
	// optional content
	const char * content= pep_resource_getcontent(resource);
	if (content != NULL) {
		hessian_object_t * h_content= hessian_create(HESSIAN_STRING,content);
		if (h_content == NULL) {
			fprintf(stderr,"ERROR:pep_resource_marshal: can't create content hessian string: %s.\n", content);
			hessian_delete(h_resource);
			return PEP_IO_ERROR;
		}
		hessian_object_t * h_content_key= hessian_create(HESSIAN_STRING,PEP_RESOURCE_CONTENT);
		if (hessian_map_add(h_resource,h_content_key,h_content) != HESSIAN_OK) {
			fprintf(stderr,"ERROR:pep_resource_marshal: can't add content hessian string to resource hessian map.\n");
			hessian_delete(h_resource);
			hessian_delete(h_content);
			hessian_delete(h_content_key);
			return PEP_IO_ERROR;
		}
	}
	// attributes list
	hessian_object_t * h_attrs= hessian_create(HESSIAN_LIST);
	if (h_attrs == NULL) {
		fprintf(stderr,"ERROR:pep_resource_marshal: can't create attributes hessian list.\n");
		hessian_delete(h_resource);
		return PEP_IO_ERROR;
	}
	size_t list_l= pep_resource_attributes_length(resource);
	int i= 0;
	for (i= 0; i < list_l; i++) {
		pep_attribute_t * attr= pep_resource_getattribute(resource,i);
		hessian_object_t * h_attr= NULL;
		if (pep_attribute_marshal(attr,&h_attr) != PEP_MODEL_OK) {
			fprintf(stderr,"ERROR:pep_resource_marshal: can't marshal PEP attribute at: %d.\n", i);
			hessian_delete(h_resource);
			hessian_delete(h_attrs);
			return PEP_IO_ERROR;
		}
		if (hessian_list_add(h_attrs,h_attr) != HESSIAN_OK) {
			fprintf(stderr,"ERROR:pep_resource_marshal: can't add hessian attribute to attributes hessian list at: %d.\n", i);
			hessian_delete(h_resource);
			hessian_delete(h_attrs);
			hessian_delete(h_attr);
			return PEP_IO_ERROR;
		}
	}
	hessian_object_t * h_attrs_key= hessian_create(HESSIAN_STRING,PEP_RESOURCE_ATTRIBUTES);
	if (hessian_map_add(h_resource,h_attrs_key,h_attrs) != HESSIAN_OK) {
		fprintf(stderr,"ERROR:pep_resource_marshal: can't add attributes hessian list to resource hessian map.\n");
		hessian_delete(h_resource);
		hessian_delete(h_attrs);
		hessian_delete(h_attrs_key);
		return PEP_IO_ERROR;
	}
	*h_res= h_resource;
	return PEP_IO_OK;
}

static int pep_subject_marshal(const pep_subject_t * subject, hessian_object_t ** h_sub) {
	// TODO: error handling
	if (subject == NULL) {
		fprintf(stderr,"ERROR:pep_subject_marshal: NULL subject object.\n");
		return PEP_IO_ERROR;
	}
	hessian_object_t * h_subject= hessian_create(HESSIAN_MAP,PEP_SUBJECT_CLASSNAME);
	if (h_subject == NULL) {
		fprintf(stderr,"ERROR:pep_subject_marshal: can't create hessian map: %s.\n", PEP_SUBJECT_CLASSNAME);
		return PEP_IO_ERROR;
	}
	const char * category= pep_subject_getcategory(subject);
	if (category != NULL) {
		hessian_object_t * h_category= hessian_create(HESSIAN_STRING,category);
		if (h_category == NULL) {
			fprintf(stderr,"ERROR:pep_subject_marshal: can't create category hessian string: %s.\n", category);
			hessian_delete(h_subject);
			return PEP_IO_ERROR;
		}
		hessian_object_t * h_category_key= hessian_create(HESSIAN_STRING,PEP_SUBJECT_CATEGORY);
		if (hessian_map_add(h_subject,h_category_key,h_category) != HESSIAN_OK) {
			fprintf(stderr,"ERROR:pep_subject_marshal: can't add category hessian string to subject hessian map.\n");
			hessian_delete(h_subject);
			hessian_delete(h_category_key);
			hessian_delete(h_category);
			return PEP_IO_ERROR;
		}
	}
	// attributes list
	hessian_object_t * h_attrs= hessian_create(HESSIAN_LIST);
	if (h_attrs == NULL) {
		fprintf(stderr,"ERROR:pep_subject_marshal: can't create attributes hessian list.\n");
		hessian_delete(h_subject);
		return PEP_IO_ERROR;
	}
	size_t list_l= pep_subject_attributes_length(subject);
	int i= 0;
	for (i= 0; i < list_l; i++) {
		pep_attribute_t * attr= pep_subject_getattribute(subject,i);
		hessian_object_t * h_attr= NULL;
		if (pep_attribute_marshal(attr,&h_attr) != PEP_IO_OK) {
			fprintf(stderr,"ERROR:pep_subject_marshal: can't marshal PEP attribute at: %d.\n", i);
			hessian_delete(h_subject);
			hessian_delete(h_attrs);
			return PEP_IO_ERROR;
		}
		if (hessian_list_add(h_attrs,h_attr) != HESSIAN_OK) {
			fprintf(stderr,"ERROR:pep_subject_marshal: can't add hessian attribute to attributes hessian list at: %d.\n", i);
			hessian_delete(h_subject);
			hessian_delete(h_attrs);
			hessian_delete(h_attr);
			return PEP_IO_ERROR;
		}
	}
	hessian_object_t * h_attrs_key= hessian_create(HESSIAN_STRING,PEP_SUBJECT_ATTRIBUTES);
	if (hessian_map_add(h_subject,h_attrs_key,h_attrs) != HESSIAN_OK) {
		fprintf(stderr,"ERROR:pep_subject_marshal: can't add attributes hessian list to subject hessian map.\n");
		hessian_delete(h_subject);
		hessian_delete(h_attrs);
		hessian_delete(h_attrs_key);
		return PEP_IO_ERROR;
	}
	*h_sub= h_subject;
	return PEP_IO_OK;
}

// OK
pep_error_t pep_request_marshalling(const pep_request_t * request, BUFFER * output) {
	hessian_object_t * h_request= NULL;
	if (pep_request_marshal(request,&h_request) != PEP_IO_OK) {
		fprintf(stderr,"ERROR:pep_request_marshalling: can't marshal PEP request into Hessian object.\n");
		return PEP_ERR_MARSHALLING_HESSIAN;
	}
	if (hessian_serialize(h_request,output) != HESSIAN_OK) {
		fprintf(stderr,"ERROR:pep_request_marshalling: failed to serialize Hessian object.\n");
		hessian_delete(h_request);
		return PEP_ERR_MARSHALLING_IO;
	}
	hessian_delete(h_request);
	return PEP_OK;
}

// OK
pep_error_t pep_response_unmarshalling(pep_response_t ** response, BUFFER * input) {
	hessian_object_t * h_response= hessian_deserialize(input);
	if (h_response == NULL) {
		fprintf(stderr,"ERROR:pep_response_unmarshalling: failed to deserialize Hessian object.\n");
		return PEP_ERR_UNMARSHALLING_IO;
	}
	if (pep_response_unmarshal(response, h_response) != PEP_IO_OK) {
		fprintf(stderr,"ERROR:pep_response_unmarshalling: can't unmarshal PEP response from Hessian object.\n");
		hessian_delete(h_response);
		return PEP_ERR_UNMARSHALLING_HESSIAN;
	}
	hessian_delete(h_response);
	return PEP_OK;

}

// OK
static int pep_response_unmarshal(pep_response_t ** resp, const hessian_object_t * h_response) {
	if (hessian_gettype(h_response) != HESSIAN_MAP) {
		fprintf(stderr,"ERROR:pep_response_unmarshal: wrong hessian type: %d (%s).\n", hessian_gettype(h_response), hessian_getclassname(h_response));
		return PEP_IO_ERROR;
	}
	const char * map_type= hessian_map_gettype(h_response);
	if (map_type == NULL) {
		fprintf(stderr,"ERROR:pep_response_unmarshal: NULL hessian map type.\n");
		return PEP_IO_ERROR;
	}
	if (strcmp(PEP_RESPONSE_CLASSNAME,map_type) != 0) {
		fprintf(stderr,"ERROR:pep_response_unmarshal: wrong hessian map type: %s.\n",map_type);
		return PEP_IO_ERROR;
	}

	pep_response_t * response= pep_response_create();
	if (response == NULL) {
		fprintf(stderr,"ERROR:pep_response_unmarshal: can't create PEP response.\n");
		return PEP_IO_ERROR;
	}

	// parse all map pair<key>s
	size_t map_l= hessian_map_length(h_response);
	int i= 0;
	for(i= 0; i<map_l; i++) {
		hessian_object_t * h_map_key= hessian_map_getkey(h_response,i);
		if (hessian_gettype(h_map_key) != HESSIAN_STRING) {
			fprintf(stderr,"ERROR:pep_response_unmarshal: hessian map<key> is not an hessian string at: %d.\n",i);
			pep_response_delete(response);
			return PEP_IO_ERROR;
		}
		const char * key= hessian_string_getstring(h_map_key);
		if (key == NULL) {
			fprintf(stderr,"ERROR:pep_response_unmarshal: hessian map<key>: NULL string at: %d.\n",i);
			pep_response_delete(response);
			return PEP_IO_ERROR;
		}
		// request (can be null???)
		if (strcmp(PEP_RESPONSE_REQUEST,key) == 0) {
			hessian_object_t * h_request= hessian_map_getvalue(h_response,i);
			if (hessian_gettype(h_request) != HESSIAN_NULL) {
				pep_request_t * request= NULL;
				if (pep_request_unmarshal(&request,h_request) != PEP_IO_OK) {
					fprintf(stderr,"ERROR:pep_response_unmarshal: can't unmarshal PEP request.\n");
					pep_response_delete(response);
					return PEP_IO_ERROR;
				}
				if (pep_response_setrequest(response,request) != PEP_MODEL_OK) {
					fprintf(stderr,"ERROR:pep_response_unmarshal: can't set PEP request in PEP response.\n");
					pep_request_delete(request);
					pep_response_delete(response);
					return PEP_IO_ERROR;
				}
			}
			else {
				fprintf(stderr,"WARN:pep_response_unmarshal: PEP request is NULL.\n");
			}

		}
		// results list
		else if (strcmp(PEP_RESPONSE_RESULTS,key) == 0) {
			hessian_object_t * h_results= hessian_map_getvalue(h_response,i);
			if (hessian_gettype(h_results) != HESSIAN_LIST) {
				fprintf(stderr,"ERROR:pep_response_unmarshal: hessian map<'%s',value> is not a hessian list at: %d.\n",key,i);
				pep_response_delete(response);
				return PEP_IO_ERROR;
			}
			size_t h_results_l= hessian_list_length(h_results);
			int j= 0;
			for(j= 0; j<h_results_l; j++) {
				hessian_object_t * h_result= hessian_list_get(h_results,j);
				pep_result_t * result= NULL;
				if (pep_result_unmarshal(&result,h_result) != PEP_IO_OK) {
					fprintf(stderr,"ERROR:pep_response_unmarshal: can't unmarshal PEP result at: %d.\n",j);
					pep_response_delete(response);
					return PEP_IO_ERROR;
				}
				if (pep_response_addresult(response,result) != PEP_MODEL_OK) {
					fprintf(stderr,"ERROR:pep_response_unmarshal: can't add PEP result at: %d to PEP response.\n",j);
					pep_result_delete(result);
					pep_response_delete(response);
					return PEP_IO_ERROR;
				}
			}
		}
		else {
			// unkown key ???
			fprintf(stderr,"WARN:pep_response_unmarshal: unknown Hessian map<key>: %s at: %d.\n",key,i);
		}
	}
	*resp= response;
	return PEP_IO_OK;
}

// OK
static int pep_result_unmarshal(pep_result_t ** res, const hessian_object_t * h_result) {
	if (hessian_gettype(h_result) != HESSIAN_MAP) {
		fprintf(stderr,"ERROR:pep_result_unmarshal: wrong Hessian type: %d (%s).\n", hessian_gettype(h_result), hessian_getclassname(h_result));
		return PEP_IO_ERROR;
	}
	const char * map_type= hessian_map_gettype(h_result);
	if (map_type == NULL) {
		fprintf(stderr,"ERROR:pep_result_unmarshal: NULL Hessian map type.\n");
		return PEP_IO_ERROR;
	}
	if (strcmp(PEP_RESULT_CLASSNAME,map_type) != 0) {
		fprintf(stderr,"ERROR:pep_result_unmarshal: wrong Hessian map type: %s.\n",map_type);
		return PEP_IO_ERROR;
	}
	pep_result_t * result= pep_result_create();
	if (result == NULL) {
		fprintf(stderr,"ERROR:pep_result_unmarshal: can't create PEP result.\n");
		return PEP_IO_ERROR;
	}

	// parse all map pair<key>s
	size_t map_l= hessian_map_length(h_result);
	int i= 0;
	for(i= 0; i<map_l; i++) {
		hessian_object_t * h_map_key= hessian_map_getkey(h_result,i);
		if (hessian_gettype(h_map_key) != HESSIAN_STRING) {
			fprintf(stderr,"ERROR:pep_result_unmarshal: Hessian map<key> is not an Hessian string at: %d.\n",i);
			pep_result_delete(result);
			return PEP_IO_ERROR;
		}
		const char * key= hessian_string_getstring(h_map_key);
		if (key == NULL) {
			fprintf(stderr,"ERROR:pep_result_unmarshal: Hessian map<key>: NULL string at: %d.\n",i);
			pep_result_delete(result);
			return PEP_IO_ERROR;
		}
		// decision (enum, mandatory)
		if (strcmp(PEP_RESULT_DECISION,key) == 0) {
			hessian_object_t * h_integer= hessian_map_getvalue(h_result,i);
			if (hessian_gettype(h_integer) != HESSIAN_INTEGER) {
				fprintf(stderr,"ERROR:pep_result_unmarshal: Hessian map<'%s',value> is not a Hessian integer at: %d.\n",key,i);
				pep_result_delete(result);
				return PEP_IO_ERROR;
			}
			int32_t decision= hessian_integer_getvalue(h_integer);
			if (pep_result_setdecision(result,decision) != PEP_MODEL_OK) {
				fprintf(stderr,"ERROR:pep_result_unmarshal: can't set decision: %d to PEP result.\n",(int)decision);
				pep_result_delete(result);
				return PEP_IO_ERROR;
			}
		}
		// resourceid (optional)
		else if (strcmp(PEP_RESULT_RESOURCEID,key) == 0) {
			hessian_object_t * h_string= hessian_map_getvalue(h_result,i);
			hessian_t h_string_type= hessian_gettype(h_string);
			if ( h_string_type != HESSIAN_STRING && h_string_type != HESSIAN_NULL) {
				fprintf(stderr,"ERROR:pep_result_unmarshal: Hessian map<'%s',value> is not a Hessian string or null at: %d.\n",key,i);
				pep_result_delete(result);
				return PEP_IO_ERROR;
			}
			const char * resourceid= NULL;
			if (h_string_type == HESSIAN_STRING) {
				resourceid= hessian_string_getstring(h_string);
			}
			if (pep_result_setresourceid(result, resourceid) != PEP_MODEL_OK) {
				fprintf(stderr,"ERROR:pep_result_unmarshal: can't set resourceid: %s to PEP result.\n",resourceid);
				pep_result_delete(result);
				return PEP_IO_ERROR;
			}
		}
		// status (null?)
		else if (strcmp(PEP_RESULT_STATUS,key) == 0) {
			hessian_object_t * h_status= hessian_map_getvalue(h_result,i);
			if (hessian_gettype(h_status) != HESSIAN_NULL) {
				pep_status_t * status= NULL;
				if (pep_status_unmarshal(&status,h_status) != PEP_IO_OK) {
					fprintf(stderr,"ERROR:pep_result_unmarshal: can't unmarshal PEP status.\n");
					pep_result_delete(result);
					return PEP_IO_ERROR;
				}
				if (pep_result_setstatus(result,status) != PEP_MODEL_OK) {
					fprintf(stderr,"ERROR:pep_result_unmarshal: can't set PEP status to PEP result.\n");
					pep_result_delete(result);
					pep_status_delete(status);
					return PEP_IO_ERROR;
				}
			}
			else {
				fprintf(stderr,"WARN:pep_result_unmarshal: PEP status is NULL.\n");
			}

		}
		// obligations list
		else if (strcmp(PEP_RESULT_OBLIGATIONS,key) == 0) {
			hessian_object_t * h_obligations= hessian_map_getvalue(h_result,i);
			if (hessian_gettype(h_obligations) != HESSIAN_LIST) {
				fprintf(stderr,"ERROR:pep_result_unmarshal: Hessian map<'%s',value> is not a Hessian list.\n",key);
				pep_result_delete(result);
				return PEP_IO_ERROR;
			}
			size_t h_obligations_l= hessian_list_length(h_obligations);
			int j= 0;
			for(j= 0; j<h_obligations_l; j++) {
				hessian_object_t * h_obligation= hessian_list_get(h_obligations,j);
				pep_obligation_t * obligation= NULL;
				if (pep_obligation_unmarshal(&obligation,h_obligation) != PEP_IO_OK) {
					fprintf(stderr,"ERROR:pep_result_unmarshal: can't unmarshal PEP obligation at: %d.\n", j);
					pep_result_delete(result);
					return PEP_IO_ERROR;
				}
				if (pep_result_addobligation(result,obligation) != PEP_MODEL_OK) {
					fprintf(stderr,"ERROR:pep_result_unmarshal: can't add PEP obligation at: %d to PEP result.\n", j);
					pep_result_delete(result);
					pep_obligation_delete(obligation);
					return PEP_IO_ERROR;
				}
			}
		}
		else {
			// unkown key ???
			fprintf(stderr,"WARN:pep_result_unmarshal: unknown map<key>: %s at: %d.\n",key,i);
		}
	}
	*res= result;
	return PEP_IO_OK;
}

// OK
static int pep_status_unmarshal(pep_status_t ** st, const hessian_object_t * h_status) {
	if (hessian_gettype(h_status) != HESSIAN_MAP) {
		fprintf(stderr,"ERROR:pep_status_unmarshal: wrong Hessian type: %d (%s).\n", hessian_gettype(h_status), hessian_getclassname(h_status));
		return PEP_IO_ERROR;
	}
	const char * map_type= hessian_map_gettype(h_status);
	if (map_type == NULL) {
		fprintf(stderr,"ERROR:pep_status_unmarshal: NULL Hessian map type.\n");
		return PEP_IO_ERROR;
	}
	if (strcmp(PEP_STATUS_CLASSNAME,map_type) != 0) {
		fprintf(stderr,"ERROR:pep_status_unmarshal: wrong Hessian map type: %s.\n",map_type);
		return PEP_IO_ERROR;
	}
	pep_status_t * status= pep_status_create(NULL);
	if (status == NULL) {
		fprintf(stderr,"ERROR:pep_status_unmarshal: can't create PEP status.\n");
		return PEP_IO_ERROR;
	}
	// parse all map pair<key>s
	size_t map_l= hessian_map_length(h_status);
	int i= 0;
	for(i= 0; i<map_l; i++) {
		hessian_object_t * h_map_key= hessian_map_getkey(h_status,i);
		if (hessian_gettype(h_map_key) != HESSIAN_STRING) {
			fprintf(stderr,"ERROR:pep_status_unmarshal: Hessian map<key> is not an Hessian string at: %d.\n",i);
			pep_status_delete(status);
			return PEP_IO_ERROR;
		}
		const char * key= hessian_string_getstring(h_map_key);
		if (key == NULL) {
			fprintf(stderr,"ERROR:pep_status_unmarshal: Hessian map<key>: NULL string at: %d.\n",i);
			pep_status_delete(status);
			return PEP_IO_ERROR;
		}
		// message (mandatory)
		if (strcmp(PEP_STATUS_MESSAGE,key) == 0) {
			hessian_object_t * h_string= hessian_map_getvalue(h_status,i);
			if (hessian_gettype(h_string) != HESSIAN_STRING) {
				fprintf(stderr,"ERROR:pep_status_unmarshal: Hessian map<'%s',value> is not a Hessian string at: %d.\n",key,i);
				pep_status_delete(status);
				return PEP_IO_ERROR;
			}
			const char * message = hessian_string_getstring(h_string);
			if (pep_status_setmessage(status,message) != PEP_MODEL_OK) {
				fprintf(stderr,"ERROR:pep_status_unmarshal: can't set message: %s to PEP status at: %d",message,i);
				pep_status_delete(status);
				return PEP_IO_ERROR;
			}
		}
		// subcode (can be null)
		else if (strcmp(PEP_STATUS_CODE,key) == 0) {
			hessian_object_t * h_statuscode= hessian_map_getvalue(h_status,i);
			if (hessian_gettype(h_status) != HESSIAN_NULL) {
				pep_status_code_t * statuscode= NULL;
				if (pep_statuscode_unmarshal(&statuscode,h_statuscode) != PEP_IO_OK) {
					fprintf(stderr,"ERROR:pep_status_unmarshal: can't unmarshal PEP statuscode at: %d.\n", i);
					pep_status_delete(status);
					return PEP_IO_ERROR;
				}
				if (pep_status_setcode(status,statuscode) != PEP_MODEL_OK) {
					fprintf(stderr,"ERROR:pep_status_unmarshal: can't set PEP statuscode to PEP status.\n");
					pep_status_delete(status);
					pep_status_code_delete(statuscode);
					return PEP_IO_ERROR;
				}
			}
			else {
				fprintf(stderr,"WARN:pep_status_unmarshal: subcode PEP statuscode is NULL.\n");
			}
		}
		else {
			fprintf(stderr,"WARN:pep_status_unmarshal: unknown Hessian map<key>: %s at: %d.\n",key,i);
		}
	}
	*st= status;
	return PEP_IO_OK;
}

// OK
static int pep_statuscode_unmarshal(pep_status_code_t ** stc, const hessian_object_t * h_statuscode) {
	if (hessian_gettype(h_statuscode) != HESSIAN_MAP) {
		fprintf(stderr,"ERROR:pep_statuscode_unmarshal: wrong Hessian type: %d (%s).\n", hessian_gettype(h_statuscode), hessian_getclassname(h_statuscode));
		return PEP_IO_ERROR;
	}
	const char * map_type= hessian_map_gettype(h_statuscode);
	if (map_type == NULL) {
		fprintf(stderr,"ERROR:pep_statuscode_unmarshal: NULL Hessian map type.\n");
		return PEP_IO_ERROR;
	}
	if (strcmp(PEP_STATUS_CODE_CLASSNAME,map_type) != 0) {
		fprintf(stderr,"ERROR:pep_statuscode_unmarshal: wrong Hessian map type: %s.\n",map_type);
		return PEP_IO_ERROR;
	}

	pep_status_code_t * statuscode= pep_status_code_create(NULL);
	if (statuscode == NULL) {
		fprintf(stderr,"ERROR:pep_statuscode_unmarshal: cant't create PEP statuscode.\n");
		return PEP_IO_ERROR;
	}

	// parse all map pair<key>s
	size_t map_l= hessian_map_length(h_statuscode);
	int i= 0;
	for(i= 0; i<map_l; i++) {
		hessian_object_t * h_map_key= hessian_map_getkey(h_statuscode,i);
		if (hessian_gettype(h_map_key) != HESSIAN_STRING) {
			fprintf(stderr,"ERROR:pep_statuscode_unmarshal: Hessian map<key> is not an Hessian string at: %d.\n",i);
			pep_status_code_delete(statuscode);
			return PEP_IO_ERROR;
		}
		const char * key= hessian_string_getstring(h_map_key);
		if (key == NULL) {
			fprintf(stderr,"ERROR:pep_statuscode_unmarshal: Hessian map<key>: NULL string at: %d.\n",i);
			pep_status_code_delete(statuscode);
			return PEP_IO_ERROR;
		}
		// code (mandatory)
		if (strcmp(PEP_STATUS_CODE_CODE,key) == 0) {
			hessian_object_t * h_string= hessian_map_getvalue(h_statuscode,i);
			if (hessian_gettype(h_string) != HESSIAN_STRING) {
				fprintf(stderr,"ERROR:pep_statuscode_unmarshal: Hessian map<'%s',value> is not a Hessian string at: %d.\n",key,i);
				pep_status_code_delete(statuscode);
				return PEP_IO_ERROR;
			}
			const char * code = hessian_string_getstring(h_string);
			if (pep_status_code_setcode(statuscode,code) != PEP_MODEL_OK) {
				fprintf(stderr,"ERROR:pep_statuscode_unmarshal: can't set code: %s to PEP statuscode at: %d",code,i);
				pep_status_code_delete(statuscode);
				return PEP_IO_ERROR;
			}

		}
		// subcode (can be null)
		else if (strcmp(PEP_STATUS_CODE_SUBCODE, key) == 0) {
			hessian_object_t * h_subcode= hessian_map_getvalue(h_statuscode,i);
			if (hessian_gettype(h_subcode) != HESSIAN_NULL) {
				pep_status_code_t * subcode= NULL;
				if (pep_statuscode_unmarshal(&subcode,h_subcode) != PEP_IO_OK) {
					fprintf(stderr,"ERROR:pep_statuscode_unmarshal: can't unmarshal subcode PEP statuscode at: %d.\n",i);
					pep_status_code_delete(statuscode);
					return PEP_IO_ERROR;
				}
				if (pep_status_code_setsubcode(statuscode,subcode) != PEP_MODEL_OK) {
					fprintf(stderr,"ERROR:pep_statuscode_unmarshal: can't set subcode PEP statuscode to PEP statuscode at: %d",i);
					pep_status_code_delete(statuscode);
					pep_status_code_delete(subcode);
					return PEP_IO_ERROR;
				}
			}
			//else {
			//	fprintf(stderr,"WARN:pep_statuscode_unmarshal: subcode PEP statuscode is NULL.\n");
			//}
		}
		else {
			fprintf(stderr,"WARN:pep_statuscode_unmarshal: unknown Hessian map<key>: %s at: %d.\n",key,i);
		}
	}
	*stc= statuscode;
	return PEP_IO_OK;
}

static int pep_obligation_unmarshal(pep_obligation_t ** obl, const hessian_object_t * h_obligation) {
	if (hessian_gettype(h_obligation) != HESSIAN_MAP) {
		fprintf(stderr,"ERROR:pep_obligation_unmarshal: wrong Hessian type: %d (%s).\n", hessian_gettype(h_obligation), hessian_getclassname(h_obligation));
		return PEP_IO_ERROR;
	}
	const char * map_type= hessian_map_gettype(h_obligation);
	if (map_type == NULL) {
		fprintf(stderr,"ERROR:pep_obligation_unmarshal: NULL Hessian map type.\n");
		return PEP_IO_ERROR;
	}
	if (strcmp(PEP_OBLIGATION_CLASSNAME,map_type) != 0) {
		fprintf(stderr,"ERROR:pep_obligation_unmarshal: wrong Hessian map type: %s.\n",map_type);
		return PEP_IO_ERROR;
	}
	pep_obligation_t * obligation= pep_obligation_create(NULL);
	if (obligation == NULL) {
		fprintf(stderr,"ERROR:pep_obligation_unmarshal: can't create PEP obligation.\n");
		return PEP_IO_ERROR;
	}

	// parse all map pair<key>s
	size_t map_l= hessian_map_length(h_obligation);
	int i= 0;
	for(i= 0; i<map_l; i++) {
		hessian_object_t * h_map_key= hessian_map_getkey(h_obligation,i);
		if (hessian_gettype(h_map_key) != HESSIAN_STRING) {
			fprintf(stderr,"ERROR:pep_obligation_unmarshal: Hessian map<key> is not an Hessian string at: %d.\n",i);
			pep_obligation_delete(obligation);
			return PEP_IO_ERROR;
		}
		const char * key= hessian_string_getstring(h_map_key);
		if (key == NULL) {
			fprintf(stderr,"ERROR:pep_obligation_unmarshal: Hessian map<key>: NULL string at: %d.\n",i);
			pep_obligation_delete(obligation);
			return PEP_IO_ERROR;
		}

		// id (mandatory)
		if (strcmp(PEP_OBLIGATION_ID,key) == 0) {
			hessian_object_t * h_string= hessian_map_getvalue(h_obligation,i);
			if (hessian_gettype(h_string) != HESSIAN_STRING) {
				fprintf(stderr,"ERROR:pep_obligation_unmarshal: Hessian map<'%s',value> is not a Hessian string at: %d.\n",key,i);
				pep_obligation_delete(obligation);
				return PEP_IO_ERROR;
			}
			const char * id = hessian_string_getstring(h_string);
			if (pep_obligation_setid(obligation,id) != PEP_MODEL_OK) {
				fprintf(stderr,"ERROR:pep_obligation_unmarshal: can't set id: %s to PEP obligation at: %d",id,i);
				pep_obligation_delete(obligation);
				return PEP_IO_ERROR;
			}
		}
		// fulfillon (enum)
		else if (strcmp(PEP_OBLIGATION_FULFILLON,key) == 0) {
			hessian_object_t * h_integer= hessian_map_getvalue(h_obligation,i);
			if (hessian_gettype(h_integer) != HESSIAN_INTEGER) {
				fprintf(stderr,"ERROR:pep_obligation_unmarshal: Hessian map<'%s',value> is not a Hessian integer at: %d.\n",key,i);
				pep_obligation_delete(obligation);
				return PEP_IO_ERROR;
			}
			int32_t fulfillon= hessian_integer_getvalue(h_integer);
			if (pep_obligation_setfulfillon(obligation,fulfillon) != PEP_MODEL_OK) {
				fprintf(stderr,"ERROR:pep_obligation_unmarshal: can't set fulfillon: %d to PEP obligation at: %d",(int)fulfillon,i);
				pep_obligation_delete(obligation);
				return PEP_IO_ERROR;
			}
		}
		// attribute assignments list
		else if (strcmp(PEP_OBLIGATION_ASSIGNMENTS,key) == 0) {
			hessian_object_t * h_assignments= hessian_map_getvalue(h_obligation,i);
			if (hessian_gettype(h_assignments) != HESSIAN_LIST) {
				fprintf(stderr,"ERROR:pep_obligation_unmarshal: Hessian map<'%s',value> is not a Hessian list at: %d.\n",key, i);
				pep_obligation_delete(obligation);
				return PEP_IO_ERROR;
			}
			size_t h_assignments_l= hessian_list_length(h_assignments);
			int j= 0;
			for(j= 0; j<h_assignments_l; j++) {
				hessian_object_t * h_assignment= hessian_list_get(h_assignments,j);
				pep_attribute_assignment_t * attribute= NULL;
				if (pep_attributeassignment_unmarshal(&attribute,h_assignment)) {
					fprintf(stderr,"ERROR:pep_obligation_unmarshal: can't unmarshal PEP attribute assignment at: %d.\n",j);
					pep_obligation_delete(obligation);
					return PEP_IO_ERROR;
				}
				if (pep_obligation_addattributeassignment(obligation,attribute) != PEP_MODEL_OK) {
					fprintf(stderr,"ERROR:pep_obligation_unmarshal: can't add PEP attribute assignment to PEP obligation at: %d",j);
					pep_obligation_delete(obligation);
					pep_attribute_assignment_delete(attribute);
					return PEP_IO_ERROR;
				}
			}
		}
		else {
			fprintf(stderr,"WARNING:pep_obligation_unmarshal: unknown Hessian map<key>: %s at: %d.\n",key,i);
		}
	}

	*obl= obligation;
	return PEP_IO_OK;

}

static int pep_attributeassignment_unmarshal(pep_attribute_assignment_t ** attr, const hessian_object_t * h_attribute) {
	if (hessian_gettype(h_attribute) != HESSIAN_MAP) {
		fprintf(stderr,"ERROR:pep_attributeassignment_unmarshal: wrong Hessian type: %d (%s).\n", hessian_gettype(h_attribute), hessian_getclassname(h_attribute));
		return PEP_IO_ERROR;
	}
	const char * map_type= hessian_map_gettype(h_attribute);
	if (map_type == NULL) {
		fprintf(stderr,"ERROR:pep_attributeassignment_unmarshal: NULL Hessian map type.\n");
		return PEP_IO_ERROR;
	}
	if (strcmp(PEP_ATTRIBUTEASSIGNMENT_CLASSNAME,map_type) != 0) {
		fprintf(stderr,"ERROR:pep_attributeassignment_unmarshal: wrong Hessian map type: %s.\n",map_type);
		return PEP_IO_ERROR;
	}

	pep_attribute_assignment_t * attribute= pep_attribute_assignment_create(NULL);
	if (attribute == NULL) {
		fprintf(stderr,"ERROR:pep_attributeassignment_unmarshal: can't create PEP attribute assignment.\n");
		return PEP_IO_ERROR;
	}

	// parse all map pair<key>s
	size_t map_l= hessian_map_length(h_attribute);
	int i= 0;
	for(i= 0; i<map_l; i++) {
		hessian_object_t * h_map_key= hessian_map_getkey(h_attribute,i);
		if (hessian_gettype(h_map_key) != HESSIAN_STRING) {
			fprintf(stderr,"ERROR:pep_attributeassignment_unmarshal: Hessian map<key> is not an Hessian string at: %d.\n",i);
			pep_attribute_assignment_delete(attribute);
			return PEP_IO_ERROR;
		}
		const char * key= hessian_string_getstring(h_map_key);
		if (key == NULL) {
			fprintf(stderr,"ERROR:pep_attributeassignment_unmarshal: Hessian map<key>: NULL string at: %d.\n",i);
			pep_attribute_assignment_delete(attribute);
			return PEP_IO_ERROR;
		}

		// id (mandatory)
		if (strcmp(PEP_ATTRIBUTEASSIGNMENT_ID,key) == 0) {
			hessian_object_t * h_string= hessian_map_getvalue(h_attribute,i);
			if (hessian_gettype(h_string) != HESSIAN_STRING) {
				fprintf(stderr,"ERROR:pep_attributeassignment_unmarshal: Hessian map<'%s',value> is not a Hessian string at: %d.\n",key,i);
				pep_attribute_assignment_delete(attribute);
				return PEP_IO_ERROR;
			}
			const char * id = hessian_string_getstring(h_string);
			if (pep_attribute_assignment_setid(attribute,id) != PEP_MODEL_OK) {
				fprintf(stderr,"ERROR:pep_attributeassignment_unmarshal: can't set id: %s to PEP attribute assignment at: %d",id,i);
				pep_attribute_assignment_delete(attribute);
				return PEP_IO_ERROR;
			}
		}
		// values list
		else if (strcmp(PEP_ATTRIBUTEASSIGNMENT_VALUES,key) == 0) {
			hessian_object_t * h_values= hessian_map_getvalue(h_attribute,i);
			if (hessian_gettype(h_values) != HESSIAN_LIST) {
				fprintf(stderr,"ERROR:pep_attributeassignment_unmarshal: Hessian map<'%s',value> is not a Hessian list.\n",key);
				pep_attribute_assignment_delete(attribute);
				return PEP_IO_ERROR;
			}
			size_t h_values_l= hessian_list_length(h_values);
			int j= 0;
			for(j= 0; j<h_values_l; j++) {
				hessian_object_t * h_value= hessian_list_get(h_values,j);
				if (hessian_gettype(h_value) != HESSIAN_STRING) {
					fprintf(stderr,"ERROR:pep_attributeassignment_unmarshal: Hessian map<'%s',value> is not a Hessian string at: %d.\n",key,i);
					pep_attribute_assignment_delete(attribute);
					return PEP_IO_ERROR;
				}
				const char * value = hessian_string_getstring(h_value);
				if (pep_attribute_assignment_addvalue(attribute,value) != PEP_MODEL_OK) {
					fprintf(stderr,"ERROR:pep_attributeassignment_unmarshal: can't add value: %s to PEP attribute at: %d",value,j);
					pep_attribute_assignment_delete(attribute);
					return PEP_IO_ERROR;
				}
			}

		}
		else {
			fprintf(stderr,"WARNING:pep_attributeassignment_unmarshal: unknown Hessian map<key>: %s at: %d.\n",key,i);
		}
	}
	*attr= attribute;
	return PEP_IO_OK;

}
