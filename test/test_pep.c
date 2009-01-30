#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>

#include "pep/pep.h"

/**
 * simple logging functions
 */
static void _vfprintf(FILE * fd, const char * level, const char * format, va_list args) {
	int BUFFER_SIZE= 1024;
	char BUFFER[BUFFER_SIZE];
	memset(BUFFER,0,BUFFER_SIZE);
//	time_t epoch;
//	time(&epoch);
//	struct tm * time= localtime(&epoch);
//	strftime(BUFFER,BUFFER_SIZE,"%F %T ",time);
//	size_t size= BUFFER_SIZE - strlen(BUFFER);
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

static void info(const char * format, ...) {
	va_list args;
	va_start(args,format);
	_vfprintf(stdout,"INFO",format,args);
	va_end(args);
}

static void error(const char * format, ...) {
	va_list args;
	va_start(args,format);
	_vfprintf(stderr,"ERROR",format,args);
	va_end(args);
}

static void debug(const char * format, ...) {
	va_list args;
	va_start(args,format);
	_vfprintf(stdout,"DEBUG",format,args);
	va_end(args);
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

/**
 * Dumps the request.
 */
static int dump_request(pep_request_t ** request) {
	pep_request_t * request2= *request;
	if (request == NULL) {
		error("dump_request: request is NULL");
		return 1;
	}
	size_t subjects_l= pep_request_subjects_length(request2);
	info("request: %d subjects", (int)subjects_l);
	int i= 0;
	for (i= 0; i<subjects_l; i++) {
		pep_subject_t * subject= pep_request_getsubject(request2,i);
		info("request.subject[%d].category= %s", i, pep_subject_getcategory(subject));
		size_t attrs_l= pep_subject_attributes_length(subject);
		info("request.subject[%d]: %d attributes", i, (int)attrs_l);
		int j= 0;
		for(j= 0; j<attrs_l; j++) {
			pep_attribute_t * attr= pep_subject_getattribute(subject,j);
			info("request.subject[%d].attribute[%d].id= %s", i,j,pep_attribute_getid(attr));
			info("request.subject[%d].attribute[%d].datatype= %s", i,j,pep_attribute_getdatatype(attr));
			info("request.subject[%d].attribute[%d].issuer= %s", i,j,pep_attribute_getissuer(attr));
			size_t values_l= pep_attribute_values_length(attr);
			info("request.subject[%d].attribute[%d]: %d values", i,j,(int)values_l);
			int k= 0;
			for (k= 0; k<values_l; k++) {
				info("request.subject[%d].attribute[%d].value[%d]= %s", i,j,k,pep_attribute_getvalue(attr,k));
			}
		}
	}
	size_t resources_l= pep_request_resources_length(request2);
	info("request: %d resources", (int)resources_l);
	for (i= 0; i<resources_l; i++) {
		pep_resource_t * resource= pep_request_getresource(request2,i);
		info("request.resource[%d].content= %s", i, pep_resource_getcontent(resource));
		size_t attrs_l= pep_resource_attributes_length(resource);
		info("request.resource[%d]: %d attributes", i, (int)attrs_l);
		int j= 0;
		for(j= 0; j<attrs_l; j++) {
			pep_attribute_t * attr= pep_resource_getattribute(resource,j);
			info("request.resource[%d].attribute[%d].id= %s", i,j,pep_attribute_getid(attr));
			info("request.resource[%d].attribute[%d].datatype= %s", i,j,pep_attribute_getdatatype(attr));
			info("request.resource[%d].attribute[%d].issuer= %s", i,j,pep_attribute_getissuer(attr));
			size_t values_l= pep_attribute_values_length(attr);
			info("request.resource[%d].attribute[%d]: %d values", i,j,(int)values_l);
			int k= 0;
			for (k= 0; k<values_l; k++) {
				info("request.resource[%d].attribute[%d].value[%d]= %s", i,j,k,pep_attribute_getvalue(attr,k));
			}
		}
	}
	pep_action_t * action2= pep_request_getaction(request2);
	size_t attrs_l= pep_action_attributes_length(action2);
	info("request.action: %d attributes",(int)attrs_l);
	int j= 0;
	for (j= 0; j<attrs_l; j++) {
		pep_attribute_t * attr= pep_action_getattribute(action2,j);
		info("request.action.attribute[%d].id= %s", j,pep_attribute_getid(attr));
		info("request.action.attribute[%d].datatype= %s", j,pep_attribute_getdatatype(attr));
		info("request.action.attribute[%d].issuer= %s", j,pep_attribute_getissuer(attr));
		size_t values_l= pep_attribute_values_length(attr);
		info("request.action.attribute[%d]: %d values", j,(int)values_l);
		int k= 0;
		for (k= 0; k<values_l; k++) {
			info("request.action.attribute[%d].value[%d]= %s",j,k,pep_attribute_getvalue(attr,k));
		}
	}
	pep_environment_t * env2= pep_request_getenvironment(request2);
	attrs_l= pep_environment_attributes_length(env2);
	info("request.environment: %d attributes",(int)attrs_l);
	for (j= 0; j<attrs_l; j++) {
		pep_attribute_t * attr= pep_environment_getattribute(env2,j);
		info("request.environment.attribute[%d].id= %s", j,pep_attribute_getid(attr));
		info("request.environment.attribute[%d].datatype= %s", j,pep_attribute_getdatatype(attr));
		info("request.environment.attribute[%d].issuer= %s", j,pep_attribute_getissuer(attr));
		size_t values_l= pep_attribute_values_length(attr);
		info("request.environment.attribute[%d]: %d values", j,(int)values_l);
		int k= 0;
		for (k= 0; k<values_l; k++) {
			info("request.environment.attribute[%d].value[%d]= %s",j,k,pep_attribute_getvalue(attr,k));
		}
	}
	return 0;
}

static int dump_response(pep_response_t ** response_ptr) {
	pep_response_t * response= *response_ptr;
	if (response == NULL) {
		error("dump_response: response is NULL");
		return 1;
	}
	size_t results_l= pep_response_results_length(response);
	info("response: %d results", (int)results_l);
	int i= 0;
	for(i= 0; i<results_l; i++) {
		pep_result_t * result= pep_response_getresult(response,i);
		info("response.result[%d].decision= %s", i, decision_str(pep_result_getdecision(result)));
		info("response.result[%d].resourceid= %s", i, pep_result_getresourceid(result));
		pep_status_t * status= pep_result_getstatus(result);
		info("response.result[%d].status.message= %s", i, pep_status_getmessage(status));
		pep_status_code_t * statuscode= pep_status_getcode(status);
		info("response.result[%d].status.code.code= %s", i, pep_status_code_getcode(statuscode));
		pep_status_code_t * subcode= pep_status_code_getsubcode(statuscode);
		if (subcode != NULL) {
			info("response.result[%d].status.code.subcode.code= %s", i, pep_status_code_getcode(subcode));
		}
		size_t obligations_l= pep_result_obligations_length(result);
		info("response.result[%d]: %d obligations", i, (int)obligations_l);
		int j=0;
		for(j= 0; j<obligations_l; j++) {
			pep_obligation_t * obligation= pep_result_getobligation(result,j);
			info("response.result[%d].obligation[%d].id= %s",i,j, pep_obligation_getid(obligation));
			info("response.result[%d].obligation[%d].fulfillOn= %s",i,j, decision_str(pep_obligation_getfulfillon(obligation)));
			size_t attrs_l= pep_obligation_attributeassignments_length(obligation);
			info("response.result[%d].obligation[%d]: %d attribute assignments",i,j,(int)attrs_l);
			int k= 0;
			for (k= 0; k<attrs_l; k++) {
				pep_attribute_assignment_t * attr= pep_obligation_getattributeassignment(obligation,k);
				info("response.result[%d].obligation[%d].attributeassignment[%d].id= %s",i,j,k,pep_attribute_assignment_getid(attr));
				size_t values_l= pep_attribute_assignment_values_length(attr);
				int l= 0;
				for (l= 0; l<values_l; l++) {
					info("response.result[%d].obligation[%d].attributeassignment[%d].value[%d]= %s",i,j,k,l,pep_attribute_assignment_getvalue(attr,l));
				}
			}
		}
	}
	return 0;
}

/**
 * PIP init
 */
static int pip_init(void) {
	debug("pip_init() called...");
	return 0;
}

/**
 * PIP processes the PEP request before submitting to PEPd
 */
static int pip_process(pep_request_t ** request) {
	debug("pip_process(request) called...");
	info("PIP dump request:");
	return dump_request(request);
}


/**
 * PIP destroy
 */
static int pip_destroy(void) {
	debug("pip_destroy() called...");
	return 0;
}

/**
 * Allocate and initialize the PIP struct.
 */
static pep_pip_t * pip_create(const char * id, pip_init_func init, pip_process_func process, pip_destroy_func destroy) {
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

/**
 * Free the allocated PIP struct.
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


/**
 * OH init
 */
static int oh_init(void) {
	debug("oh_init() called...");
	return 0;
}

/**
 * OH process(request,response)
 */
static int oh_process(pep_request_t ** request, pep_response_t ** response) {
	debug("oh_process(request,response) called...");
	info("OH dump response:");
	return dump_response(response);
}

/**
 * OH destroy
 */
static int oh_destroy(void) {
	debug("oh_destroy() called...");
	return 0;
}

/**
 * Allocates and initialize an OH struct.
 */
static pep_obligationhandler_t * oh_create(const char * id, oh_init_func init, oh_process_func process, oh_destroy_func destroy) {
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

/**
 * Free the allocated OH struct.
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

/**
 * MAIN
 *
 * usage: ./test-pep <URL>
 */
int main(int argc, char **argv) {

	info("set LOG options...");
	pep_setoption(PEP_OPTION_LOG_STDERR, stderr);
	pep_setoption(PEP_OPTION_LOG_LEVEL, PEP_LOGLEVEL_DEBUG);

	char * url= "http://localhost:8080/PEPd/authz?random";
	//char * url= "http://localhost:8081/PEPd/authz"; // TCPMon
	//char * url= "http://130.59.4.166:8080/authz"; // Chad Jetty
	//char * url= "http://localhost:8081/authz"; // TCPMon
	if (argc == 2) {
		url= argv[1];
		info("%s: using endpoint URL: %s",argv[0], url);
	}
	info("initialize PEP...");
	int pep_rc= pep_initialize();
	if (pep_rc != PEP_OK) {
		error("test_pep: pep_initialize() failed: %s",pep_strerror(pep_rc));
		pep_destroy();
		return -1;
	}

	info("create PIP and add to PEP...");
	pep_pip_t * pip= pip_create("PIPRequestDumper",pip_init,pip_process,pip_destroy);
	if (pip == NULL) {
		error("test_pep: pip_create(...) failed");
		pep_destroy();
		return -1;
	}
	pep_rc= pep_addpip(pip);
	if (pep_rc != PEP_OK) {
		error("test_pep: pep_addpip() failed: %s",pep_strerror(pep_rc));
		pep_destroy();
		return -1;
	}
	pep_rc= pep_setoption(PEP_OPTION_ENABLE_PIPS,1);
	if (pep_rc != PEP_OK) {
		error("test_pep: pep_setoption(PEP_OPTION_ENABLE_PIPS,1) failed: %s",pep_strerror(pep_rc));
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
	pep_rc= pep_setoption(PEP_OPTION_ENABLE_OBLIGATIONHANDLERS,1);
	if (pep_rc != PEP_OK) {
		error("test_pep: pep_setoption(PEP_OPTION_ENABLE_OBLIGATIONHANDLERS,1) failed: %s",pep_strerror(pep_rc));
		pep_destroy();
		return -1;
	}


	info("create PEP request...");
	pep_request_t * request= pep_request_create();
	assert(request);
	info("add PEP subject(proxy)...");
	pep_subject_t * subject= pep_subject_create();
	assert(subject);
	pep_attribute_t * proxy= pep_attribute_create("x-urn:authz:subject:proxy");
	assert(proxy);
	pep_attribute_addvalue(proxy,"PEM_ENCODE_PROXY_CONTENT");
	pep_subject_addattribute(subject,proxy);
	pep_request_addsubject(request,subject);
	info("add PEP resource(jdl,name)...");
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
	info("set PEP action(command)...");
	pep_action_t * action= pep_action_create();
	assert(action);
	pep_attribute_t * command= pep_attribute_create("x-urn:authz:action:command");
	assert(command);
	pep_attribute_addvalue(command,"/usr/bin/echo");
	pep_action_addattribute(action,command);
	pep_request_setaction(request,action);
	info("set PEP environment(path)...");
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
	info("set PEP endpoint: %s", url);
	pep_rc= pep_setoption(PEP_OPTION_ENDPOINT_URL, url);
	if (pep_rc != PEP_OK) {
		error("test_pep: pep_setoption(PEP_OPTION_ENDPOINT_URL,%s) failed: %s",url,pep_strerror(pep_rc));
		pep_destroy();
		return -1;
	}

	info("send PEP request to: %s", url);
	pep_response_t * response= NULL;
	pep_rc= pep_authorize(&request,&response);
	if (pep_rc != PEP_OK) {
		error("test_pep: pep_authorize(request,response) failed: %s",pep_strerror(pep_rc));
		pep_destroy();
		return -1;
	}

	info("delete PEP request and response...");
	pep_request_delete(request);
	pep_response_delete(response);

	info("destroy PEP...");
	pep_rc= pep_destroy();
	if (pep_rc != PEP_OK) {
		error("test_pep: pep_destroy() failed: %s",pep_strerror(pep_rc));
		return pep_rc;
	}

	// WARNING: call this only AFTER pep_destroy()...
	info("delete PIP and OH structs...");
	pip_delete(pip);
	oh_delete(oh);

	info("done.");
	return 0;
}
