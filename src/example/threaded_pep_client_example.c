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
 * Multi-threaded Argus PEP client example
 *
 * gcc -I/usr/include -L/usr/lib64 -pthread -largus-pep threaded_pep_client_example.c -o threaded_pep_client_example
 *
 * Author: Valery Tschopp <valery.tschopp@switch.ch>
 * $Id$
 ************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <curl/curl.h>

/* include Argus PEP client API header */
#include <argus/pep.h>

/* prototypes */
static int create_xacml_request(xacml_request_t ** request,const char * subjectid, const char * resourceid, const char * actionid);
static int process_xacml_response(PEP * pep,const xacml_response_t * response);
static const char * decision_tostring(xacml_decision_t decision);
static const char * fulfillon_tostring(xacml_fulfillon_t fulfillon);

/* thread proto */
void *thread_authorize(void * thread_endpoint);

/*
 * main
 */
#define N_THREADS 20
 
int main( void ) {
    pthread_t threads[N_THREADS];
    int i, j;
    char * endpoint= "https://chaos.switch.ch:8154/authz";

    fprintf(stdout,"curl_global_init(CURL_GLOBAL_ALL)\n");
    curl_global_init(CURL_GLOBAL_ALL);
    
    fprintf(stdout,"creates %d threads...\n", N_THREADS);
    for (i= 0; i < N_THREADS; i++) {
        pthread_create( &threads[i], NULL, &thread_authorize, (void*)endpoint );
    }

    fprintf(stdout,"joins %d threads...\n", N_THREADS);
    for (j= 0; j < N_THREADS; j++) {
        pthread_join( threads[j], NULL );
    }

    fprintf(stdout,"done.\n");
    
    fprintf(stdout,"curl_global_cleanup()\n");
    curl_global_cleanup();
}

 
void *thread_authorize(void * thread_endpoint) {

    /* Argus PEP client handle */
    PEP * pep;

    /* functions return code */
    pep_error_t pep_rc; /* PEP function error */
    int rc;             /* others functions */
    
    /* XACML request and response */
    xacml_request_t * request;
    xacml_response_t * response;

    char * pep_url, * subjectid, * resourceid, * actionid;

    /* create the PEP client handle */
    pep= pep_initialize();
    if (pep == NULL) {
       fprintf(stderr,"failed to create PEP client\n");
       return;
    }

    fprintf(stdout,"TH[%ld]: PEP[%d] endpoint %s\n",pthread_self(), pep_getid(pep),(char*)thread_endpoint);

    /* debugging options */
    pep_setoption(pep,PEP_OPTION_LOG_STDERR,stderr);
    pep_setoption(pep,PEP_OPTION_LOG_LEVEL,PEP_LOGLEVEL_ERROR);

    /* configure PEP client: PEP Server endpoint url */
    pep_url= (char *)thread_endpoint;
    pep_rc= pep_setoption(pep,PEP_OPTION_ENDPOINT_URL,pep_url);
    if (pep_rc != PEP_OK) {
       fprintf(stderr,"PEP[%d]: failed to set PEP endpoint: %s: %s\n", pep_getid(pep), pep_url, pep_strerror(pep_rc));
       return;
    }   
    /* configure PEP client: private key and certificate required to access the PEP Server */
    /* endpoint (HTTPS with client authentication) */
    pep_rc= pep_setoption(pep,PEP_OPTION_ENDPOINT_CLIENT_KEY,"/etc/grid-security/hostkey.pem");
    if (pep_rc != PEP_OK) {
       fprintf(stderr,"PEP[%d]: failed to set client key: %s: %s\n", "/etc/grid-security/hostkey.pem", pep_getid(pep), pep_strerror(pep_rc));
       return;
    }   
    pep_rc= pep_setoption(pep,PEP_OPTION_ENDPOINT_CLIENT_CERT,"/etc/grid-security/hostcert.pem");
    if (pep_rc != PEP_OK) {
       fprintf(stderr,"PEP[%d]: failed to set client cert: %s: %s\n", pep_getid(pep), "/etc/grid-security/hostcert.pem", pep_strerror(pep_rc));
       return;
    }   
    /* server certificate CA path for validation */
    pep_rc= pep_setoption(pep,PEP_OPTION_ENDPOINT_SERVER_CAPATH,"/etc/grid-security/certificates");
    if (pep_rc != PEP_OK) {
       fprintf(stderr,"PEP[%d]: failed to set server CA path: %s: %s\n", pep_getid(pep), "/etc/grid-security/certificates", pep_strerror(pep_rc));
       return;
    }   

    /* create the XACML request */
    subjectid= "CN=Valery Tschopp,O=SWITCH,C=ch";
    resourceid= "test-emi";
    actionid= "test";
    rc= create_xacml_request(&request,subjectid,resourceid,actionid);
    if (rc != 0) {
       fprintf(stderr,"failed to create XACML request\n");
       return;
    }

    /* submit the XACML request */
    fprintf(stdout,"TH[%ld]: PEP[%d]: authorizing XACML request...\n", pthread_self(), pep_getid(pep));
    pep_rc= pep_authorize(pep,&request, &response);
    if (pep_rc != PEP_OK) {
       fprintf(stderr,"PEP[%d]: failed to authorize XACML request: %s\n", pep_getid(pep), pep_strerror(pep_rc));
       return;
    }   
    
    /* parse and process XACML response */
    size_t results_l;
    int i, j, k;
    if (response == NULL) {
        fprintf(stderr,"response is NULL\n");
        return;
    }
    results_l= xacml_response_results_length(response);
    for(i= 0; i<results_l; i++) {
        xacml_result_t * result;
        result= xacml_response_getresult(response,i);
        fprintf(stdout,"TH[%ld]: PEP[%d] response Decision= %s\n", pthread_self(), pep_getid(pep), decision_tostring(xacml_result_getdecision(result)));
    }
    
    /* delete resquest and response objs */
    xacml_request_delete(request);
    xacml_response_delete(response);
        
    /* release the PEP client handle */
    pep_destroy(pep);

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
