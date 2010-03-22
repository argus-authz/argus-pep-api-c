/*
 * Copyright (c) Members of the EGEE Collaboration. 2008.
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
 *
 * $Id$
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>

/*
 * compiler flags: CFLAGS=-I/opt/glite/include
 * linker flags: LDFLAGS=-L/opt/glite/lib
 */
#include "pep/pep.h"

/*
 * local logging functions
 */
static void _vfprintf(FILE * fd, const char * level, const char * format, va_list args) {
	int BUFFER_SIZE= 1024;
	char BUFFER[BUFFER_SIZE];
	memset(BUFFER,0,BUFFER_SIZE);
	size_t size= BUFFER_SIZE;
	strncat(BUFFER,level,size);
	size= size - strlen(BUFFER);
	strncat(BUFFER,": ",size);
	size= size - strlen(BUFFER);
	strncat(BUFFER,format,size);
	size= size - strlen(BUFFER);
	strncat(BUFFER,"\n",size);
	vfprintf(fd,BUFFER,args);
}

/*
 * Logs an INFO message on stdout
 */
static void info(const char * format, ...) {
	va_list args;
	va_start(args,format);
	_vfprintf(stdout,"INFO",format,args);
	va_end(args);
}

/*
 * Logs an ERROR message on stderr
 */
static void error(const char * format, ...) {
	va_list args;
	va_start(args,format);
	_vfprintf(stderr,"ERROR",format,args);
	va_end(args);
}

/*
 * Logs an DEBUG message on stdout
 */
static void debug(const char * format, ...) {
	va_list args;
	va_start(args,format);
	_vfprintf(stdout,"DEBUG",format,args);
	va_end(args);
}

/*
 * PEP-C logging callback function
 */
static void log_handler_pep(int level, const char * format, va_list args) {
	fprintf(stdout,"XXX:PEP-C[%d]: ",level);
	vfprintf(stdout,format,args);
	fprintf(stdout,"\n");
}

/*
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

/*
 * Dumps a XACML request.
 */
static int dump_request(xacml_request_t ** request) {
	xacml_request_t * request2= *request;
	if (request == NULL) {
		error("dump_request: request is NULL");
		return 1;
	}
	size_t subjects_l= xacml_request_subjects_length(request2);
	info("request: %d subjects", (int)subjects_l);
	int i= 0;
	for (i= 0; i<subjects_l; i++) {
		xacml_subject_t * subject= xacml_request_getsubject(request2,i);
		info("request.subject[%d].category= %s", i, xacml_subject_getcategory(subject));
		size_t attrs_l= xacml_subject_attributes_length(subject);
		info("request.subject[%d]: %d attributes", i, (int)attrs_l);
		int j= 0;
		for(j= 0; j<attrs_l; j++) {
			xacml_attribute_t * attr= xacml_subject_getattribute(subject,j);
			info("request.subject[%d].attribute[%d].id= %s", i,j,xacml_attribute_getid(attr));
			info("request.subject[%d].attribute[%d].datatype= %s", i,j,xacml_attribute_getdatatype(attr));
			info("request.subject[%d].attribute[%d].issuer= %s", i,j,xacml_attribute_getissuer(attr));
			size_t values_l= xacml_attribute_values_length(attr);
			info("request.subject[%d].attribute[%d]: %d values", i,j,(int)values_l);
			int k= 0;
			for (k= 0; k<values_l; k++) {
				info("request.subject[%d].attribute[%d].value[%d]= %s", i,j,k,xacml_attribute_getvalue(attr,k));
			}
		}
	}
	size_t resources_l= xacml_request_resources_length(request2);
	info("request: %d resources", (int)resources_l);
	for (i= 0; i<resources_l; i++) {
		xacml_resource_t * resource= xacml_request_getresource(request2,i);
		info("request.resource[%d].content= %s", i, xacml_resource_getcontent(resource));
		size_t attrs_l= xacml_resource_attributes_length(resource);
		info("request.resource[%d]: %d attributes", i, (int)attrs_l);
		int j= 0;
		for(j= 0; j<attrs_l; j++) {
			xacml_attribute_t * attr= xacml_resource_getattribute(resource,j);
			info("request.resource[%d].attribute[%d].id= %s", i,j,xacml_attribute_getid(attr));
			info("request.resource[%d].attribute[%d].datatype= %s", i,j,xacml_attribute_getdatatype(attr));
			info("request.resource[%d].attribute[%d].issuer= %s", i,j,xacml_attribute_getissuer(attr));
			size_t values_l= xacml_attribute_values_length(attr);
			info("request.resource[%d].attribute[%d]: %d values", i,j,(int)values_l);
			int k= 0;
			for (k= 0; k<values_l; k++) {
				info("request.resource[%d].attribute[%d].value[%d]= %s", i,j,k,xacml_attribute_getvalue(attr,k));
			}
		}
	}
	xacml_action_t * action2= xacml_request_getaction(request2);
	size_t attrs_l= xacml_action_attributes_length(action2);
	info("request.action: %d attributes",(int)attrs_l);
	int j= 0;
	for (j= 0; j<attrs_l; j++) {
		xacml_attribute_t * attr= xacml_action_getattribute(action2,j);
		info("request.action.attribute[%d].id= %s", j,xacml_attribute_getid(attr));
		info("request.action.attribute[%d].datatype= %s", j,xacml_attribute_getdatatype(attr));
		info("request.action.attribute[%d].issuer= %s", j,xacml_attribute_getissuer(attr));
		size_t values_l= xacml_attribute_values_length(attr);
		info("request.action.attribute[%d]: %d values", j,(int)values_l);
		int k= 0;
		for (k= 0; k<values_l; k++) {
			info("request.action.attribute[%d].value[%d]= %s",j,k,xacml_attribute_getvalue(attr,k));
		}
	}
	xacml_environment_t * env2= xacml_request_getenvironment(request2);
	attrs_l= xacml_environment_attributes_length(env2);
	info("request.environment: %d attributes",(int)attrs_l);
	for (j= 0; j<attrs_l; j++) {
		xacml_attribute_t * attr= xacml_environment_getattribute(env2,j);
		info("request.environment.attribute[%d].id= %s", j,xacml_attribute_getid(attr));
		info("request.environment.attribute[%d].datatype= %s", j,xacml_attribute_getdatatype(attr));
		info("request.environment.attribute[%d].issuer= %s", j,xacml_attribute_getissuer(attr));
		size_t values_l= xacml_attribute_values_length(attr);
		info("request.environment.attribute[%d]: %d values", j,(int)values_l);
		int k= 0;
		for (k= 0; k<values_l; k++) {
			info("request.environment.attribute[%d].value[%d]= %s",j,k,xacml_attribute_getvalue(attr,k));
		}
	}
	return 0;
}

/*
 * Dumps a XACML response.
 */
static int dump_response(xacml_response_t ** response_ptr) {
	xacml_response_t * response= *response_ptr;
	if (response == NULL) {
		error("dump_response: response is NULL");
		return 1;
	}
	size_t results_l= xacml_response_results_length(response);
	info("response: %d results", (int)results_l);
	int i= 0;
	for(i= 0; i<results_l; i++) {
		xacml_result_t * result= xacml_response_getresult(response,i);
		info("response.result[%d].decision= %s", i, decision_str(xacml_result_getdecision(result)));
		info("response.result[%d].resourceid= %s", i, xacml_result_getresourceid(result));
		xacml_status_t * status= xacml_result_getstatus(result);
		info("response.result[%d].status.message= %s", i, xacml_status_getmessage(status));
		xacml_statuscode_t * statuscode= xacml_status_getcode(status);
		info("response.result[%d].status.code.value= %s", i, xacml_statuscode_getvalue(statuscode));
		xacml_statuscode_t * subcode= xacml_statuscode_getsubcode(statuscode);
		if (subcode != NULL) {
			info("response.result[%d].status.code.subcode.value= %s", i, xacml_statuscode_getvalue(subcode));
		}
		size_t obligations_l= xacml_result_obligations_length(result);
		info("response.result[%d]: %d obligations", i, (int)obligations_l);
		int j=0;
		for(j= 0; j<obligations_l; j++) {
			xacml_obligation_t * obligation= xacml_result_getobligation(result,j);
			info("response.result[%d].obligation[%d].id= %s",i,j, xacml_obligation_getid(obligation));
			info("response.result[%d].obligation[%d].fulfillOn= %s",i,j, decision_str(xacml_obligation_getfulfillon(obligation)));
			size_t attrs_l= xacml_obligation_attributeassignments_length(obligation);
			info("response.result[%d].obligation[%d]: %d attribute assignments",i,j,(int)attrs_l);
			int k= 0;
			for (k= 0; k<attrs_l; k++) {
				xacml_attributeassignment_t * attr= xacml_obligation_getattributeassignment(obligation,k);
				info("response.result[%d].obligation[%d].attributeassignment[%d].id= %s",i,j,k,xacml_attributeassignment_getid(attr));
				info("response.result[%d].obligation[%d].attributeassignment[%d].datatype= %s",i,j,k,xacml_attributeassignment_getdatatype(attr));
			    info("response.result[%d].obligation[%d].attributeassignment[%d].value= %s",i,j,k,xacml_attributeassignment_getvalue(attr));
			}
		}
	}
	return 0;
}

/*
 * PIP init function called by the PEP client in pep_addpip(...);
 */
static int pip_init(void) {
	debug("pip_init() called...");
	return 0;
}

/*
 * PIP process function called by the PEP client before submitting to PEPd in pep_authorize(...)
 */
static int pip_process(xacml_request_t ** request) {
	debug("pip_process(request) called...");
	info("PIP dump request:");
	return dump_request(request);
}


/*
 * PIP destroy function called by the PEP client in pep_destroy();
 */
static int pip_destroy(void) {
	debug("pip_destroy() called...");
	return 0;
}

/*
 * Allocates and initializes a PIP struct.
 */
static pep_pip_t * pip_create(const char * id, pip_init_func * init, pip_process_func * process, pip_destroy_func * destroy) {
	pep_pip_t * pip= calloc(1,sizeof(pep_pip_t));
	if (pip == NULL) {
		error("can't allocate pep_pip_t");
		return NULL;
	}
	size_t size= strlen(id);
	pip->id= calloc(size + 1, sizeof(char));
	if (pip->id == NULL) {
		error("can't allocate pip->id (%d bytes)",size);
		free(pip);
		return NULL;
	}
	strncpy(pip->id,id,size);
	pip->init= init;
	pip->process= process;
	pip->destroy= destroy;
	return pip;
}

/*
 * Free an allocated PIP struct.
 */
static int pip_delete(pep_pip_t * pip) {
	if (pip != NULL) {
		if (pip->id != NULL) {
			free(pip->id);
		}
		free(pip);
		return 0;
	}
	return -1;
}


/*
 * OH init function called by the PEP client in pep_addobligationhandler(...)
 */
static int oh_init(void) {
	debug("oh_init() called...");
	return 0;
}

/*
 * OH process(request,response) called by the PEP client after receiving the PEPd reponse in pep_authorize(...)
 */
static int oh_process(xacml_request_t ** request, xacml_response_t ** response) {
	debug("oh_process(request,response) called...");
	info("OH dump response:");
	return dump_response(response);
}

/*
 * OH destroy called by the PEP client in pep_destroy()
 */
static int oh_destroy(void) {
	debug("oh_destroy() called...");
	return 0;
}

/*
 * Allocates and initialize an OH struct.
 */
static pep_obligationhandler_t * oh_create(const char * id, oh_init_func * init, oh_process_func * process, oh_destroy_func * destroy) {
	pep_obligationhandler_t * oh= calloc(1,sizeof(pep_obligationhandler_t));
	if (oh == NULL) {
		error("can't allocate pep_obligationhandler_t");
		return NULL;
	}
	size_t size= strlen(id);
	oh->id= calloc(size + 1, sizeof(char));
	if (oh->id == NULL) {
		error("can't allocate oh->id (%d bytes)",size);
		free(oh);
		return NULL;
	}
	strncpy(oh->id,id,size);
	oh->init= init;
	oh->process= process;
	oh->destroy= destroy;
	return oh;
}

/*
 * Free an allocated OH struct.
 */
static int oh_delete(pep_obligationhandler_t * oh) {
	if (oh != NULL) {
		if (oh->id != NULL) {
			free(oh->id);
		}
		free(oh);
		return 0;
	}
	return -1;
}

/*
 * MAIN
 *
 * usage: ./test-pep <URL>
 */
int main(int argc, char **argv) {


	char * url= "http://localhost:8080/PEPd/authz?random";
	if (argc == 2) {
		url= argv[1];
		info("%s: using endpoint URL: %s",argv[0], url);
	}
	info("initialize PEP...");
	pep_error_t pep_rc= pep_initialize();
	if (pep_rc != PEP_OK) {
		error("test_pep: pep_initialize() failed: %s",pep_strerror(pep_rc));
		pep_destroy();
		return -1;
	}

	info("set LOG options...");
	pep_setoption(PEP_OPTION_LOG_STDERR,stderr);
	pep_setoption(PEP_OPTION_LOG_LEVEL,PEP_LOGLEVEL_DEBUG); // DEBUG, INFO, WARN and ERROR
	pep_setoption(PEP_OPTION_LOG_HANDLER,log_handler_pep); // will override stderr log handler

	info("create PIP");
	pep_pip_t * pip= pip_create("PIPRequestDumper",pip_init,pip_process,pip_destroy);
	if (pip == NULL) {
		error("test_pep: pip_create(...) failed");
		pep_destroy();
		return -1;
	}

	info("install PIP: %s",pip->id);
	pep_rc= pep_addpip(pip);
	if (pep_rc != PEP_OK) {
		error("test_pep: pep_addpip() failed: %s",pep_strerror(pep_rc));
		pep_destroy();
		return -1;
	}

	info("install PIP: %s",authzinterop2gridwn_adapter_pip->id);
	pep_rc= pep_addpip(authzinterop2gridwn_adapter_pip);
	if (pep_rc != PEP_OK) {
		error("test_pep: pep_addpip() failed: %s",pep_strerror(pep_rc));
		pep_destroy();
		return -1;
	}

	info("install PIP: %s",pip->id);
	pep_rc= pep_addpip(pip);
	if (pep_rc != PEP_OK) {
		error("test_pep: pep_addpip() failed: %s",pep_strerror(pep_rc));
		pep_destroy();
		return -1;
	}

	info("create OH and add to PEP...");
	pep_obligationhandler_t * oh= oh_create("OHResponseDumper",oh_init,oh_process,oh_destroy);
	if (oh == NULL) {
		error("test_pep: oh_create(...) failed");
		pep_destroy();
		return -1;
	}
	pep_rc= pep_addobligationhandler(oh);
	if (pep_rc != PEP_OK) {
		error("test_pep: pep_addobligationhandler() failed: %s",pep_strerror(pep_rc));
		pep_destroy();
		return -1;
	}

	// create a XACML request
	info("create XACML request...");
	xacml_request_t * request= xacml_request_create();
	assert(request);
	info("add XACML subject(cert-chain)...");
	xacml_subject_t * subject= xacml_subject_create();
	assert(subject);
	xacml_attribute_t * certchain= xacml_attribute_create(XACML_AUTHZINTEROP_SUBJECT_CERTCHAIN);
	assert(certchain);
	xacml_attribute_addvalue(certchain,"PEM_ENCODE_PROXY_CERTCHAIN...");
	xacml_attribute_setdatatype(certchain,XACML_DATATYPE_BASE64BINARY);
	xacml_subject_addattribute(subject,certchain);

	xacml_request_addsubject(request,subject);
	info("add XACML resource(resource-id)...");
	xacml_resource_t * resource= xacml_resource_create();
	assert(resource);
	xacml_attribute_t * resource_id= xacml_attribute_create(XACML_RESOURCE_ID);
	assert(resource_id);
	xacml_attribute_addvalue(resource_id,"http://authz-interop.org/xacml/resource/resource-type/wn");
	xacml_resource_addattribute(resource,resource_id);
	xacml_request_addresource(request,resource);
	info("set XACML action(action-id)...");
	xacml_action_t * action= xacml_action_create();
	assert(action);
	xacml_attribute_t * action_id= xacml_attribute_create(XACML_ACTION_ID);
	assert(action_id);
	xacml_attribute_addvalue(action_id,"http://authz-interop.org/xacml/action/action-type/execute-now");
	xacml_action_addattribute(action,action_id);
	xacml_request_setaction(request,action);
	info("set XACML environment(path)...");
	xacml_environment_t * environment= xacml_environment_create();
	assert(environment);
	xacml_attribute_t * path= xacml_attribute_create("x-urn:authz:env:path");
	assert(path);
	xacml_attribute_addvalue(path,"/usr/bin");
	xacml_attribute_addvalue(path,"/opt/glite/bin");
	xacml_attribute_addvalue(path,"/home/johndoe/bin");
	xacml_environment_addattribute(environment,path);
	xacml_request_setenvironment(request,environment);

	// add many PEPd endpoints for failover testing
	info("set PEPd endpoint: %s", url);
	pep_rc= pep_setoption(PEP_OPTION_ENDPOINT_URL, url);
	info("set PEPd endpoint: %s", "http://www.google.com");
	pep_rc= pep_setoption(PEP_OPTION_ENDPOINT_URL, "http://www.google.com");
	info("set PEPd endpoint: %s", "http://localhost:8080/PEPd/authz?s7");
	pep_rc= pep_setoption(PEP_OPTION_ENDPOINT_URL, "http://localhost:8080/PEPd/authz?s7");
	info("set PEPd endpoint: %s", "http://nasjflkasdjflj.com");
	pep_rc= pep_setoption(PEP_OPTION_ENDPOINT_URL, "http://nasjflkasdjflj.com");
	info("set PEPd endpoint: %s", "http://localhost:8080/PEPd/authz?s3");
	pep_rc= pep_setoption(PEP_OPTION_ENDPOINT_URL, "http://localhost:8080/PEPd/authz?s3");
	info("set PEPd endpoint: %s", "http://hestia.switch.ch/PEPd/authz?s8");
	pep_rc= pep_setoption(PEP_OPTION_ENDPOINT_URL, "http://hestia.switch.ch/PEPd/authz?s8");
	info("set PEPd endpoint: %s", "http://localhost:8080"); // respond OK 200
	pep_rc= pep_setoption(PEP_OPTION_ENDPOINT_URL, "http://localhost:8080");
	// send authz request and process
	if (pep_rc != PEP_OK) {
		error("test_pep: pep_setoption(PEP_OPTION_ENDPOINT_URL,%s) failed: %s",url,pep_strerror(pep_rc));
		pep_destroy();
		return -1;
	}

	info("send XACML request to PEPd");
	xacml_response_t * response= NULL;
	pep_rc= pep_authorize(&request,&response);
	if (pep_rc != PEP_OK) {
		error("test_pep: pep_authorize(request,response) failed: %s",pep_strerror(pep_rc));
		xacml_request_delete(request);
		xacml_response_delete(response);
		pep_destroy();
		pip_delete(pip);
		oh_delete(oh);
		return -1;
	}

	info("delete XACML request and response...");
	xacml_request_delete(request);
	xacml_response_delete(response);

	info("destroy PEP...");
	pep_rc= pep_destroy();
	if (pep_rc != PEP_OK) {
		error("test_pep: pep_destroy() failed: %s",pep_strerror(pep_rc));
		pip_delete(pip);
		oh_delete(oh);
		return pep_rc;
	}

	// WARNING: call these only AFTER pep_destroy()...
	info("delete PIP and OH structs...");
	pip_delete(pip);
	oh_delete(oh);

	info("done.");
	return 0;
}
