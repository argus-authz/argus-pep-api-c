/*
 * Copyright 2008 Members of the EGEE Collaboration.
 * See http://www.eu-egee.org/partners for details on the copyright holders.
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

//
// Simple PEP-C client example
//
// gcc -I/opt/glite/include -L/opt/glite/lib -lpep-c pep_client_example.c -o pep_client_example
//
// Author: Valery Tschopp <valery.tschopp@switch.ch>
// $Id$
#include <stdio.h>
#include <stdlib.h>

#include <pep/pep.h>

// prototypes
static int create_xacml_request(xacml_request_t ** request,const char * subjectid, const char * resourceid, const char * actionid);
static int process_xacml_response(const xacml_response_t * response);
static const char * decision_str(int decision);

int main(void) {
    // functions return code
    pep_error_t pep_rc; // PEP-C function
    int rc;             // others
    
    // XACML request and response
    xacml_request_t * request;
    xacml_response_t * response;

    // dump library version
    fprintf(stdout,"using libpep-c v.%s\n",pep_version());

    
    // initialize the PEP client library
    pep_rc= pep_initialize();
    if (pep_rc != PEP_OK) {
       fprintf(stderr,"failed to initialize PEP client: %s\n", pep_strerror(pep_rc));
       exit(1);
    }

    // debugging
    /*
    pep_setoption(PEP_OPTION_LOG_STDERR,stderr);
    pep_setoption(PEP_OPTION_LOG_LEVEL,PEP_LOGLEVEL_DEBUG);
    */

    // configure PEP client: PEPd url
    char * pep_url= "http://pepd.example.org:8154/authz";
    pep_rc= pep_setoption(PEP_OPTION_ENDPOINT_URL,pep_url);
    if (pep_rc != PEP_OK) {
       fprintf(stderr,"failed to set PEPd url: %s: %s\n", pep_url, pep_strerror(pep_rc));
       exit(1);
    }   

    // create the XACML request
    char * subjectid= "CN=John Doe,O=Example Org,C=US";
    char * resourceid= "x-urn:example.org:resource:resourceid:condor:57657";
    char * actionid= "x-urn:example.org:action:actionid:submit";
    rc= create_xacml_request(&request,subjectid,resourceid,actionid);
    if (rc != 0) {
       fprintf(stderr,"failed to create XACML request\n");
       exit(1);
    }

    // submit the XACML request
    pep_rc= pep_authorize(&request, &response);
    if (pep_rc != PEP_OK) {
       fprintf(stderr,"failed to authorize XACML request: %s\n", pep_strerror(pep_rc));
       exit(1);
    }   
    
    // parse and process XACML response
    rc= process_xacml_response(response);
    
    // delete resquest and response objs
    xacml_request_delete(request);
    xacml_response_delete(response);
        
    // release the PEP client 
    pep_rc= pep_destroy();
    if (pep_rc != PEP_OK) {
        fprintf(stderr,"failed to release PEP client: %s\n", pep_strerror(pep_rc));
        exit(1);
    }

    return 0;
}

/**
 * Creates a XACML Request containing a XACML Subject with the given subjectid, a XACML Resource
 * with the given resourceid and a XACML Action with the given actionid.
 * 
 * @param [in/out] request address of the pointer to the XACML request object
 * @param [in] subjectid attribute value of the XACML Request/Subject element 
 * @param [in] resourceid  attribute value of the XACML Request/Resource element
 * @param [in] actionid  attribute value of the XACML Request/Action element
 * @return 0 on success or error code on failure.
 */
static int create_xacml_request(xacml_request_t ** request,const char * subjectid, const char * resourceid, const char * actionid) {

    // XACML Subject with subjectid Attribute value
    xacml_subject_t * subject= xacml_subject_create();
    if (subject == NULL) {
        fprintf(stderr,"can not create XACML Subject\n");
        return 1;
    }
    xacml_attribute_t * subject_attr_id= xacml_attribute_create(XACML_SUBJECT_ID);
    if (subject_attr_id == NULL) {
        fprintf(stderr,"can not create XACML Subject/Attribute:%s\n",XACML_SUBJECT_ID);
        xacml_subject_delete(subject);
        return 1;
    }
    xacml_attribute_addvalue(subject_attr_id,subjectid);
    xacml_subject_addattribute(subject,subject_attr_id);

    // XACML Resource with resourceid Attribute value
    xacml_resource_t * resource= xacml_resource_create();
    if (resource == NULL) {
        fprintf(stderr,"can not create XACML Resource\n");
        xacml_subject_delete(subject);
        return 2;
    }
    xacml_attribute_t * resource_attr_id= xacml_attribute_create(XACML_RESOURCE_ID);
    if (resource_attr_id == NULL) {
        fprintf(stderr,"can not create XACML Resource/Attribute:%s\n",XACML_RESOURCE_ID);
        xacml_subject_delete(subject);
        xacml_resource_delete(resource);
        return 2;
    }
    xacml_attribute_addvalue(resource_attr_id,resourceid);
    xacml_resource_addattribute(resource,resource_attr_id);

    // XACML Action with actionid Attribute value
    xacml_action_t * action= xacml_action_create();
    if (action == NULL) {
        fprintf(stderr,"can not create XACML Action\n");
        xacml_subject_delete(subject);
        xacml_resource_delete(resource);
        return 3;
    }
    xacml_attribute_t * action_attr_id= xacml_attribute_create(XACML_ACTION_ID);
    if (action_attr_id == NULL) {
        fprintf(stderr,"can not create XACML Action/Attribute:%s\n",XACML_ACTION_ID);
        xacml_subject_delete(subject);
        xacml_resource_delete(resource);
        xacml_action_delete(action);
        return 3;
    }
    xacml_attribute_addvalue(action_attr_id,actionid);
    xacml_action_addattribute(action,action_attr_id);

    // XACML Request with all elements
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

/**
 * Simply dump the XACML response.
 *
 * @param [in] response the XAXML response
 * @return 0 on success or error code on failure. 
 */
static int process_xacml_response(const xacml_response_t * response) {
    if (response == NULL) {
        fprintf(stderr,"response is NULL\n");
        return 1;
    }
    size_t results_l= xacml_response_results_length(response);
    fprintf(stdout,"response: %d results\n", (int)results_l);
    int i= 0;
    for(i= 0; i<results_l; i++) {
        xacml_result_t * result= xacml_response_getresult(response,i);
        fprintf(stdout,"response.result[%d].decision= %s\n", i, decision_str(xacml_result_getdecision(result)));

        fprintf(stdout,"response.result[%d].resourceid= %s\n", i, xacml_result_getresourceid(result));
        xacml_status_t * status= xacml_result_getstatus(result);
        fprintf(stdout,"response.result[%d].status.message= %s\n", i, xacml_status_getmessage(status));
        xacml_statuscode_t * statuscode= xacml_status_getcode(status);
        fprintf(stdout,"response.result[%d].status.code.value= %s\n", i, xacml_statuscode_getvalue(statuscode));
        xacml_statuscode_t * subcode= xacml_statuscode_getsubcode(statuscode);
        if (subcode != NULL) {
            fprintf(stdout,"response.result[%d].status.code.subcode.value= %s\n", i, xacml_statuscode_getvalue(subcode));
        }
        size_t obligations_l= xacml_result_obligations_length(result);
        fprintf(stdout,"response.result[%d]: %d obligations\n", i, (int)obligations_l);
        int j=0;
        for(j= 0; j<obligations_l; j++) {
            xacml_obligation_t * obligation= xacml_result_getobligation(result,j);
            fprintf(stdout,"response.result[%d].obligation[%d].id= %s\n",i,j, xacml_obligation_getid(obligation));
            fprintf(stdout,"response.result[%d].obligation[%d].fulfillOn= %s\n",i,j, decision_str(xacml_obligation_getfulfillon(obligation)));
            size_t attrs_l= xacml_obligation_attributeassignments_length(obligation);
            fprintf(stdout,"response.result[%d].obligation[%d]: %d attribute assignments\n",i,j,(int)attrs_l);
            int k= 0;
            for (k= 0; k<attrs_l; k++) {
                xacml_attributeassignment_t * attr= xacml_obligation_getattributeassignment(obligation,k);
                fprintf(stdout,"response.result[%d].obligation[%d].attributeassignment[%d].id= %s\n",i,j,k,xacml_attributeassignment_getid(attr));
                size_t values_l= xacml_attributeassignment_values_length(attr);
                int l= 0;
                for (l= 0; l<values_l; l++) {
                    fprintf(stdout,"response.result[%d].obligation[%d].attributeassignment[%d].value[%d]= %s\n",i,j,k,l,xacml_attributeassignment_getvalue(attr,l));
                }
            }
        }
    }
    return 0;
}

/**
 * Returns the string representation of the decision.
 */
static const char * decision_str(int decision) {
    switch(decision) {
    case 0:
        return "Deny";
        break;
    case 1:
        return "Permit";
        break;
    case 2:
        return "Indeterminate";
        break;
    case 3:
        return "Not Applicable";
        break;
    default:
        return "Deny (Unknown!?!)";
        break;
    }
}

