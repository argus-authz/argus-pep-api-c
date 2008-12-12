#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "pep/pep.h"

//int run_test_pep(int argc, char **argv) {
int main(int argc, char **argv) {

	char * url= "http://localhost:8080/PEPd/authz";
	//char * url= "http://localhost:8081/PEPd/authz"; // TCPMon
	//char * url= "http://130.59.4.166:8080/authz"; // Chad Jetty
	//char * url= "http://localhost:8081/authz"; // TCPMon
	if (argc == 2) {
		url= argv[1];
		printf("%s: using endpoint URL: %s\n",argv[0], url);
	}
	printf("initialize PEP...\n");
	int pep_rc= pep_initialize();
	if (pep_rc != PEP_OK) {
		printf("ERROR:run_test_pep: pep_initialize() failed: %d: %s\n",pep_rc,pep_strerror(pep_rc));
		pep_rc= pep_destroy();
		if (pep_rc != PEP_OK) {
			printf("ERROR:run_test_pep: pep_destroy() failed: %d: %s\n",pep_rc,pep_strerror(pep_rc));
			return pep_rc;
		}
		return -1;
	}

	printf("create PEP request...\n");
	pep_request_t * request= pep_request_create();
	assert(request);
	printf("add PEP subject(proxy)...\n");
	pep_subject_t * subject= pep_subject_create();
	assert(subject);
	pep_attribute_t * proxy= pep_attribute_create("x-urn:authz:subject:proxy");
	assert(proxy);
	pep_attribute_addvalue(proxy,"PEM_ENCODE_PROXY_CONTENT");
	pep_subject_addattribute(subject,proxy);
	pep_request_addsubject(request,subject);
	printf("add PEP resource(jdl,name)...\n");
	pep_resource_t * resource= pep_resource_create();
	assert(resource);
	pep_attribute_t * jdl= pep_attribute_create("x-urn:authz:resource:jdl");
	assert(jdl);
	pep_attribute_addvalue(jdl,"JDL_FILE_CONTENT");
	pep_resource_addattribute(resource,jdl);
	pep_attribute_t * name= pep_attribute_create("x-urn:authz:resource:name");
	assert(name);
	pep_attribute_addvalue(name,"Val\xC3\xA9ry");
	pep_resource_addattribute(resource,name);
	pep_request_addresource(request,resource);
	printf("set PEP action(command)...\n");
	pep_action_t * action= pep_action_create();
	assert(action);
	pep_attribute_t * command= pep_attribute_create("x-urn:authz:action:command");
	assert(command);
	pep_attribute_addvalue(command,"/usr/bin/echo");
	pep_action_addattribute(action,command);
	pep_request_setaction(request,action);
	printf("set PEP environment(path)...\n");
	pep_environment_t * environment= pep_environment_create();
	assert(environment);
	pep_attribute_t * path= pep_attribute_create("x-urn:authz:env:path");
	assert(path);
	pep_attribute_addvalue(path,"/usr/bin");
	pep_attribute_addvalue(path,"/opt/glite/bin");
	pep_attribute_addvalue(path,"/home/johndoe/bin");
	pep_environment_addattribute(environment,path);
	pep_request_setenvironment(request,environment);

	// send authz request and process
	printf("send PEP request to: %s\n", url);
	pep_rc= pep_setoption(PEP_OPTION_ENDPOINT_URL, url);
	if (pep_rc != PEP_OK) {
		printf("ERROR:run_test_pep: pep_setoption(EP_OPTION_ENDPOINT_URL,%s) failed: %d: %s\n",url,pep_rc,pep_strerror(pep_rc));
		pep_rc= pep_destroy();
		if (pep_rc != PEP_OK) {
			printf("ERROR:run_test_pep: pep_destroy() failed: %d: %s\n",pep_rc,pep_strerror(pep_rc));
			return pep_rc;
		}
		return -1;
	}

	pep_response_t * response= NULL;
	pep_rc= pep_authorize(&request,&response);
	if (pep_rc != PEP_OK) {
		printf("ERROR:run_test_pep: pep_authorize(request,response) failed: %d: %s\n",pep_rc,pep_strerror(pep_rc));
		pep_rc= pep_destroy();
		if (pep_rc != PEP_OK) {
			printf("ERROR:run_test_pep: pep_destroy() failed: %d: %s\n",pep_rc,pep_strerror(pep_rc));
			return pep_rc;
		}
		return -1;
	}

	printf("process PEP response...\n");
	pep_request_t * request2= pep_response_getrequest(response);
	assert(request2);
	printf("PEP response.request: %s\n", (request2 == NULL) ? "NULL" : "PRESENT");
	size_t results_l= pep_response_results_length(response);
	printf("PEP response: %d results\n", (int)results_l);
	int i= 0;
	for(i= 0; i<results_l; i++) {
		pep_result_t * result= pep_response_getresult(response,i);
		assert(result);
		printf("PEP response.result[%d].decision= %d\n", i, pep_result_getdecision(result));
		printf("PEP response.result[%d].resourceid= %s\n", i, pep_result_getresourceid(result));
		pep_status_t * status= pep_result_getstatus(result);
		printf("PEP response.result[%d].status: %s\n", i, (status == NULL) ? "NULL" : "PRESENT");
		assert(status);
		printf("PEP response.result[%d].status.message= %s\n", i, pep_status_getmessage(status));
		pep_status_code_t * statuscode= pep_status_getcode(status);
		printf("PEP response.result[%d].status.code: %s\n", i, (statuscode == NULL) ? "NULL" : "PRESENT");
		assert(statuscode);
		printf("PEP response.result[%d].status.code.code= %s\n", i, pep_status_code_getcode(statuscode));
		pep_status_code_t * subcode= pep_status_code_getsubcode(statuscode);
		printf("PEP response.result[%d].status.code.subcode: %s\n", i, (subcode == NULL) ? "NULL" : "PRESENT");
		size_t obligations_l= pep_result_obligations_length(result);
		printf("PEP response.result[%d]: %d obligations\n", i, (int)obligations_l);
		int j=0;
		for(j= 0; j<obligations_l; j++) {
			pep_obligation_t * obligation= pep_result_getobligation(result,j);
			assert(obligation);
			printf("PEP response.result[%d].obligation[%d].id= %s\n",i,j, pep_obligation_getid(obligation));
			printf("PEP response.result[%d].obligation[%d].fulfillOn= %d\n",i,j, pep_obligation_getfulfillon(obligation));
			size_t attrs_l= pep_obligation_attributeassignments_length(obligation);
			printf("PEP response.result[%d].obligation[%d]: %d attribute assignments\n",i,j,(int)attrs_l);
			int k= 0;
			for (k= 0; k<attrs_l; k++) {
				pep_attribute_assignment_t * attr= pep_obligation_getattributeassignment(obligation,k);
				assert(attr);
				printf("PEP response.result[%d].obligation[%d].attributeassignment[%d].id= %s\n",i,j,k,pep_attribute_assignment_getid(attr));
				size_t values_l= pep_attribute_assignment_values_length(attr);
				int l= 0;
				for (l= 0; l<values_l; l++) {
					printf("PEP response.result[%d].obligation[%d].attributeassignment[%d].value[%d]= %s\n",i,j,k,l,pep_attribute_assignment_getvalue(attr,l));
				}
			}
		}
	}

	printf("delete PEP request and response...\n");
	pep_request_delete(request);
	pep_response_delete(response);

	printf("destroy PEP...\n");
	pep_rc= pep_destroy();
	if (pep_rc != PEP_OK) {
		printf("ERROR:run_test_pep: pep_destroy() failed: %d: %s\n",pep_rc,pep_strerror(pep_rc));
		return pep_rc;
	}

	printf("done.\n");
	return 0;
}
