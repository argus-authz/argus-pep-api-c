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
 * $Id: model.h,v 1.2 2009/01/29 17:16:36 vtschopp Exp $
 */
#ifndef _PEP_MODEL_H_
#define _PEP_MODEL_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include <stddef.h> // size_t

#include "pep/error.h"

/**
 * PEP model functions return codes
 */
#define PEP_MODEL_OK     0
#define PEP_MODEL_ERROR -1

/*****************************************************************
 * PEP Attribute type
 */
typedef struct pep_attribute pep_attribute_t;

pep_attribute_t * pep_attribute_create(const char * id);
int pep_attribute_setid(pep_attribute_t * attr, const char * id);
const char * pep_attribute_getid(const pep_attribute_t * attr);
int pep_attribute_setdatatype(pep_attribute_t * attr, const char * datatype);
const char * pep_attribute_getdatatype(const pep_attribute_t * attr);
int pep_attribute_setissuer(pep_attribute_t * attr, const char * issuer);
const char * pep_attribute_getissuer(const pep_attribute_t * attr);
int pep_attribute_addvalue(pep_attribute_t * attr, const char *value);
size_t pep_attribute_values_length(const pep_attribute_t * attr);
const char * pep_attribute_getvalue(const pep_attribute_t * attr,int index);
void pep_attribute_delete(pep_attribute_t * attr);


/*****************************************************************
 * PEP Subject type
 */
typedef struct pep_subject pep_subject_t;

/**
 * PEP Subject category constants (XACML 2.0, B.2)
 */
static const char PEP_SUBJECT_CATEGORY_ACCESS[]= "urn:oasis:names:tc:xacml:1.0:subject-category:access-subject";
static const char PEP_SUBJECT_CATEGORY_RECIPIENT[]= "urn:oasis:names:tc:xacml:1.0:subject-category:recipient-subject";
static const char PEP_SUBJECT_CATEGORY_INTERMEDIARY[]= "urn:oasis:names:tc:xacml:1.0:subject-category:intermediary-subject";
static const char PEP_SUBJECT_CATEGORY_CODEBASE[]= "urn:oasis:names:tc:xacml:1.0:subject-category:codebase";
static const char PEP_SUBJECT_CATEGORY_REQUESTING_MACHINE[]= "urn:oasis:names:tc:xacml:1.0:subject-category:requesting-machine";

pep_subject_t * pep_subject_create();
int pep_subject_setcategory(pep_subject_t * subject, const char * category);
const char * pep_subject_getcategory(const pep_subject_t * subject);
int pep_subject_addattribute(pep_subject_t * subject, pep_attribute_t * attr);
size_t pep_subject_attributes_length(const pep_subject_t * subject);
pep_attribute_t * pep_subject_getattribute(const pep_subject_t * subject, int index);
void pep_subject_delete(pep_subject_t * subject);


/*****************************************************************
 * PEP Resource type
 */
typedef struct pep_resource pep_resource_t;

pep_resource_t * pep_resource_create();
int pep_resource_setcontent(pep_resource_t * resource, const char * content);
const char * pep_resource_getcontent(const pep_resource_t * resource);
int pep_resource_addattribute(pep_resource_t * resource, pep_attribute_t * attr);
size_t pep_resource_attributes_length(const pep_resource_t * resource);
pep_attribute_t * pep_resource_getattribute(const pep_resource_t * resource, int index);
void pep_resource_delete(pep_resource_t * resource);


/*****************************************************************
 * PEP Action type
 */
typedef struct pep_action pep_action_t;

pep_action_t * pep_action_create();
int pep_action_addattribute(pep_action_t * action, pep_attribute_t * attr);
size_t pep_action_attributes_length(const pep_action_t * action);
pep_attribute_t * pep_action_getattribute(const pep_action_t * action, int index);
void pep_action_delete(pep_action_t * action);


/*****************************************************************
 * PEP Environment type
 */
typedef struct pep_environment pep_environment_t;

pep_environment_t * pep_environment_create();
int pep_environment_addattribute(pep_environment_t * env, pep_attribute_t * attr);
size_t pep_environment_attributes_length(const pep_environment_t * env);
pep_attribute_t * pep_environment_getattribute(const pep_environment_t * env, int index);
void pep_environment_delete(pep_environment_t * env);


/*****************************************************************
 * PEP Request type
 */
typedef struct pep_request pep_request_t;

pep_request_t * pep_request_create();
int pep_request_addsubject(pep_request_t * request, pep_subject_t * subject);
size_t pep_request_subjects_length(const pep_request_t * request);
pep_subject_t * pep_request_getsubject(const pep_request_t * request, int index);
int pep_request_addresource(pep_request_t * request, pep_resource_t * resource);
size_t pep_request_resources_length(const pep_request_t * request);
pep_resource_t * pep_request_getresource(const pep_request_t * request, int index);
int pep_request_setaction(pep_request_t * request, pep_action_t * action);
pep_action_t * pep_request_getaction(const pep_request_t * request);
int pep_request_setenvironment(pep_request_t * request, pep_environment_t * env);
pep_environment_t * pep_request_getenvironment(const pep_request_t * request);
void pep_request_delete(pep_request_t * request);


/*****************************************************************
 * PEP StatusCode type
 */
typedef struct pep_status_code pep_status_code_t;

/**
 * PEP StatusCode code constants (XACML 2.0, B.9)
 */
static const char PEP_STATUSCODE_OK[]= "urn:oasis:names:tc:xacml:1.0:status:ok";
static const char PEP_STATUSCODE_MISSING_ATTRIBUTE[]= "urn:oasis:names:tc:xacml:1.0:status:missing-attribute";
static const char PEP_STATUSCODE_SYNTAX_ERROR[]= "urn:oasis:names:tc:xacml:1.0:status:syntax-error";
static const char PEP_STATUSCODE_PROCESSING_ERROR[]= "urn:oasis:names:tc:xacml:1.0:status:processing-error";

pep_status_code_t * pep_status_code_create(const char * code);
int pep_status_code_setcode(pep_status_code_t * status_code, const char * code);
const char * pep_status_code_getcode(const pep_status_code_t * status_code);
pep_status_code_t * pep_status_code_getsubcode(const pep_status_code_t * status_code);
int pep_status_code_setsubcode(pep_status_code_t * status_code, pep_status_code_t * subcode);
void pep_status_code_delete(pep_status_code_t * status_code);

/*****************************************************************
 * PEP Status type
 */
typedef struct pep_status pep_status_t;

pep_status_t * pep_status_create(const char * message);
int pep_status_setmessage(pep_status_t * status, const char * message);
const char * pep_status_getmessage(const pep_status_t * status);
pep_status_code_t * pep_status_getcode(const pep_status_t * status);
int pep_status_setcode(pep_status_t * status, pep_status_code_t * code);
void pep_status_delete(pep_status_t * status);

/*****************************************************************
 * PEP AttributeAssignment type
 */
typedef struct pep_attribute_assignment pep_attribute_assignment_t;

pep_attribute_assignment_t * pep_attribute_assignment_create(const char * id);
int pep_attribute_assignment_setid(pep_attribute_assignment_t * attr, const char * id);
const char * pep_attribute_assignment_getid(const pep_attribute_assignment_t * attr);
size_t pep_attribute_assignment_values_length(const pep_attribute_assignment_t * attr);
const char * pep_attribute_assignment_getvalue(const pep_attribute_assignment_t * attr,int i);
int pep_attribute_assignment_addvalue(pep_attribute_assignment_t * attr, const char *value);
void pep_attribute_assignment_delete(pep_attribute_assignment_t * attr);

/*****************************************************************
 * PEP Obligation type
 */
typedef enum pep_fulfillon {
	PEP_FULFILLON_DENY = 0,
	PEP_FULFILLON_PERMIT
} pep_fulfillon_t;

typedef struct pep_obligation pep_obligation_t;

pep_obligation_t * pep_obligation_create(const char * id);
int pep_obligation_setid(pep_obligation_t * obligation, const char * id);
const char * pep_obligation_getid(const pep_obligation_t * obligation);
pep_fulfillon_t pep_obligation_getfulfillon(const pep_obligation_t * obligation);
int pep_obligation_setfulfillon(pep_obligation_t * obl, pep_fulfillon_t fulfillon);
int pep_obligation_addattributeassignment(pep_obligation_t * obligation, pep_attribute_assignment_t * attr);
size_t pep_obligation_attributeassignments_length(const pep_obligation_t * obligation);
pep_attribute_assignment_t * pep_obligation_getattributeassignment(const pep_obligation_t * obligation,int i);
void pep_obligation_delete(pep_obligation_t * obligation);

/*****************************************************************
 * PEP Result type
 */
typedef enum pep_decision {
	PEP_DECISION_DENY = 0,
	PEP_DECISION_PERMIT,
	PEP_DECISION_INDETERMINATE,
	PEP_DECISION_NOT_APPLICABLE
} pep_decision_t;

typedef struct pep_result pep_result_t;

pep_result_t * pep_result_create();
pep_decision_t pep_result_getdecision(const pep_result_t * result);
int pep_result_setdecision(pep_result_t * result, pep_decision_t decision);
const char * pep_result_getresourceid(const pep_result_t * result);
int pep_result_setresourceid(pep_result_t * result, const char * resourceid);
pep_status_t * pep_result_getstatus(const pep_result_t * result);
int pep_result_setstatus(pep_result_t * result, pep_status_t * status);
int pep_result_addobligation(pep_result_t * result, pep_obligation_t * obligation);
size_t pep_result_obligations_length(const pep_result_t * result);
pep_obligation_t * pep_result_getobligation(const pep_result_t * result, int index);
void pep_result_delete(pep_result_t * result);

/*****************************************************************
 * PEP Response type
 */
typedef struct pep_response pep_response_t;

pep_response_t * pep_response_create();
int pep_response_setrequest(pep_response_t * response, pep_request_t * request);
pep_request_t * pep_response_getrequest(const pep_response_t * response);
int pep_response_addresult(pep_response_t * response, pep_result_t * result);
size_t pep_response_results_length(const pep_response_t * response);
pep_result_t * pep_response_getresult(const pep_response_t * response, int index);
void pep_response_delete(pep_response_t * response);


#ifdef  __cplusplus
}
#endif

#endif // _PEP_MODEL_H_
