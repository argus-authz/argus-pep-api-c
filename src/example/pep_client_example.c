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

/*************
 * Simple Argus PEP client example
 *
 * gcc -I/usr/include -L/usr/lib64 -largus_pep pep_client_example.c -o pep_client_example
 *
 * Author: Valery Tschopp <valery.tschopp@switch.ch>
 * $Id$
 ************/

#include <stdio.h>
#include <stdlib.h>

/* include Argus PEP API header */
#include "argus/pep.h"

/* prototypes */
static int create_xacml_request(xacml_request_t ** request,const char * subjectid, const char * resourceid, const char * actionid);
static int process_xacml_response(const xacml_response_t * response);
static const char * decision_tostring(xacml_decision_t decision);
static const char * fulfillon_tostring(xacml_fulfillon_t fulfillon);

/*
 * main
 */
int main(void) {

    /* Argus PEP client handle */
    PEP * pep;

    /* functions return code */
    pep_error_t pep_rc; /* PEP function error */
    int rc;             /* others functions */
    
    /* XACML request and response */
    xacml_request_t * request;
    xacml_response_t * response;

    char * pep_url, * subjectid, * resourceid, * actionid;

    /* dump library version */
    fprintf(stdout,"using %s\n",pep_version());

    
    /* create the PEP client handle */
    pep= pep_initialize();
    if (pep == NULL) {
       fprintf(stderr,"failed to create PEP client\n");
       exit(1);
    }

    /* debugging options */
    pep_setoption(pep,PEP_OPTION_LOG_STDERR,stderr);
    pep_setoption(pep,PEP_OPTION_LOG_LEVEL,PEP_LOGLEVEL_DEBUG);

    /* configure PEP client: PEPd url */
    pep_url= "http://demeter.switch.ch:8154/authz";
    pep_rc= pep_setoption(pep,PEP_OPTION_ENDPOINT_URL,pep_url);
    if (pep_rc != PEP_OK) {
       fprintf(stderr,"failed to set PEPd url: %s: %s\n", pep_url, pep_strerror(pep_rc));
       exit(1);
    }   

    /* create the XACML request */
    subjectid= "CN=Valery Tschopp 9FEE5EE3,O=Switch - Teleinformatikdienste fuer Lehre und Forschung,DC=slcs,DC=switch,DC=ch";
    resourceid= "switch";
    actionid= "switch";
    rc= create_xacml_request(&request,subjectid,resourceid,actionid);
    if (rc != 0) {
       fprintf(stderr,"failed to create XACML request\n");
       exit(1);
    }

    /* submit the XACML request */
    pep_rc= pep_authorize(pep,&request, &response);
    if (pep_rc != PEP_OK) {
       fprintf(stderr,"failed to authorize XACML request: %s\n", pep_strerror(pep_rc));
       exit(1);
    }   
    
    /* parse and process XACML response */
    rc= process_xacml_response(response);
    
    /* delete resquest and response objs */
    xacml_request_delete(request);
    xacml_response_delete(response);
        
    /* release the PEP client handle */
    pep_destroy(pep);

    return 0;
}

/*
 * Creates a XACML Request containing a XACML Subject with the given subjectid, a XACML Resource
 * with the given resourceid and a XACML Action with the given actionid.
 * 
 * @param [in/out] request address of the pointer to the XACML request object
 * @param [in] subjectid, a X.509 DN, attribute value of the XACML Request/Subject element 
 * @param [in] resourceid  attribute value of the XACML Request/Resource element
 * @param [in] actionid  attribute value of the XACML Request/Action element
 * @return 0 on success or error code on failure.
 */
static int create_xacml_request(xacml_request_t ** request,const char * subjectid, const char * resourceid, const char * actionid)
{
    xacml_subject_t * subject;
    xacml_attribute_t * subject_attr_id;
    xacml_resource_t * resource;
    xacml_attribute_t * resource_attr_id;
    xacml_action_t * action;
    xacml_attribute_t * action_attr_id;
    
    /* XACML Subject with subjectid Attribute value */
    subject= xacml_subject_create();
    if (subject == NULL) {
        fprintf(stderr,"can not create XACML Subject\n");
        return 1;
    }
    subject_attr_id= xacml_attribute_create(XACML_SUBJECT_ID);
    if (subject_attr_id == NULL) {
        fprintf(stderr,"can not create XACML Subject/Attribute:%s\n",XACML_SUBJECT_ID);
        xacml_subject_delete(subject);
        return 1;
    }
    // set X.509 DN value
    xacml_attribute_addvalue(subject_attr_id,subjectid);
    // set attribute datatype for X.509 DN
    xacml_attribute_setdatatype(subject_attr_id,XACML_DATATYPE_X500NAME); 
    xacml_subject_addattribute(subject,subject_attr_id);

    /* XACML Resource with resourceid Attribute value */
    resource= xacml_resource_create();
    if (resource == NULL) {
        fprintf(stderr,"can not create XACML Resource\n");
        xacml_subject_delete(subject);
        return 2;
    }
    resource_attr_id= xacml_attribute_create(XACML_RESOURCE_ID);
    if (resource_attr_id == NULL) {
        fprintf(stderr,"can not create XACML Resource/Attribute:%s\n",XACML_RESOURCE_ID);
        xacml_subject_delete(subject);
        xacml_resource_delete(resource);
        return 2;
    }
    xacml_attribute_addvalue(resource_attr_id,resourceid);
    xacml_resource_addattribute(resource,resource_attr_id);

    /* XACML Action with actionid Attribute value */
    action= xacml_action_create();
    if (action == NULL) {
        fprintf(stderr,"can not create XACML Action\n");
        xacml_subject_delete(subject);
        xacml_resource_delete(resource);
        return 3;
    }
    action_attr_id= xacml_attribute_create(XACML_ACTION_ID);
    if (action_attr_id == NULL) {
        fprintf(stderr,"can not create XACML Action/Attribute:%s\n",XACML_ACTION_ID);
        xacml_subject_delete(subject);
        xacml_resource_delete(resource);
        xacml_action_delete(action);
        return 3;
    }
    xacml_attribute_addvalue(action_attr_id,actionid);
    xacml_action_addattribute(action,action_attr_id);

    /* XACML Request with all elements */
    *request= xacml_request_create();
    if (*request == NULL) {
        fprintf(stderr,"can not create XACML Request\n");
        xacml_subject_delete(subject);
        xacml_resource_delete(resource);
        xacml_action_delete(action);
        return 4;
    }
    xacml_request_addsubject(*request,subject);
    xacml_request_addresource(*request,resource);
    xacml_request_setaction(*request,action);
    
    return 0;
}

/*
 * Simply dump the XACML response.
 *
 * @param [in] response the XAXML response
 * @return 0 on success or error code on failure. 
 */
static int process_xacml_response(const xacml_response_t * response) {
    size_t results_l;
    int i, j, k;
    if (response == NULL) {
        fprintf(stderr,"response is NULL\n");
        return 1;
    }
    results_l= xacml_response_results_length(response);
    fprintf(stdout,"response: %d results\n", (int)results_l);
    for(i= 0; i<results_l; i++) {
        xacml_result_t * result;
        xacml_status_t * status;
        xacml_statuscode_t * statuscode, * subcode;
        size_t obligations_l;
        
        result= xacml_response_getresult(response,i);
        fprintf(stdout,"response.result[%d].decision= %s\n", i, decision_tostring(xacml_result_getdecision(result)));
        fprintf(stdout,"response.result[%d].resourceid= %s\n", i, xacml_result_getresourceid(result));
        
        status= xacml_result_getstatus(result);
        fprintf(stdout,"response.result[%d].status.message= %s\n", i, xacml_status_getmessage(status));
        statuscode= xacml_status_getcode(status);
        fprintf(stdout,"response.result[%d].status.code.value= %s\n", i, xacml_statuscode_getvalue(statuscode));
        subcode= xacml_statuscode_getsubcode(statuscode);
        if (subcode != NULL) {
            fprintf(stdout,"response.result[%d].status.code.subcode.value= %s\n", i, xacml_statuscode_getvalue(subcode));
        }
        obligations_l= xacml_result_obligations_length(result);
        fprintf(stdout,"response.result[%d]: %d obligations\n", i, (int)obligations_l);
        for(j= 0; j<obligations_l; j++) {
            size_t attrs_l;
            xacml_obligation_t * obligation= xacml_result_getobligation(result,j);
            fprintf(stdout,"response.result[%d].obligation[%d].id= %s\n",i,j, xacml_obligation_getid(obligation));
            fprintf(stdout,"response.result[%d].obligation[%d].fulfillOn= %s\n",i,j, fulfillon_tostring(xacml_obligation_getfulfillon(obligation)));
            attrs_l= xacml_obligation_attributeassignments_length(obligation);
            fprintf(stdout,"response.result[%d].obligation[%d]: %d attribute assignments\n",i,j,(int)attrs_l);
            for (k= 0; k<attrs_l; k++) {
                xacml_attributeassignment_t * attr= xacml_obligation_getattributeassignment(obligation,k);
                fprintf(stdout,"response.result[%d].obligation[%d].attributeassignment[%d].id= %s\n",i,j,k,xacml_attributeassignment_getid(attr));
                fprintf(stdout,"response.result[%d].obligation[%d].attributeassignment[%d].datatype= %s\n",i,j,k,xacml_attributeassignment_getdatatype(attr));
                fprintf(stdout,"response.result[%d].obligation[%d].attributeassignment[%d].value= %s\n",i,j,k,xacml_attributeassignment_getvalue(attr));
            }
        }
    }
    return 0;
}

/*
 * Returns the string representation of the decision.
 */
static const char * decision_tostring(xacml_decision_t decision) {
    switch(decision) {
    case XACML_DECISION_DENY:
        return "Deny";
        break;
    case XACML_DECISION_PERMIT:
        return "Permit";
        break;
    case XACML_DECISION_INDETERMINATE:
        return "Indeterminate";
        break;
    case XACML_DECISION_NOT_APPLICABLE:
        return "Not Applicable";
        break;
    default:
        return "ERROR (Unknown Decision)";
        break;
    }
}

/*
 * Returns the string representation of the fulfillOn.
 */
static const char * fulfillon_tostring(xacml_fulfillon_t fulfillon) {
    switch(fulfillon) {
    case XACML_FULFILLON_DENY:
        return "Deny";
        break;
    case XACML_FULFILLON_PERMIT:
        return "Permit";
        break;
    default:
        return "ERROR (Unknown FulfillOn)";
        break;
    }
}
