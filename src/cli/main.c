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
 * $Id: main.c,v 1.5 2009/03/20 12:46:47 vtschopp Exp $
 */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <stdarg.h>

#ifdef HAVE_CONFIG_H
#include "config.h"  /* PACKAGE_NAME and PACKAGE_VERSION const */
#else
#define PACKAGE_VERSION "1.0.0"
#endif

#include "util/buffer.h"
#include "util/linkedlist.h"
#include "pep/pep.h"

// logging prototypes: log.c
void show_info(const char * format, ...);
void show_error(const char * format, ...);
void show_warn(const char * format, ...);
void show_debug(const char * format, ...);
void log_handler_pep(int level, const char * format, va_list args);


// main return codes
#define E_OK        0 // OK
#define E_MEMORY    1 // memory allocation error
#define E_OPTION    2 // invalid option
#define E_CERTCHAIN 3 // certchain file or content error
#define E_XACMLREQ  4 // XACML request error
#define E_PEPC      5 // PEP client error

// program options
static struct option long_options[] = {
   // verbose
   {"verbose", no_argument, 0, 'v'},
   // PEPd endpoint URL(s)
   {"pepd", required_argument, 0, 'p'},
   // connection timeout (sec)
   {"timeout", required_argument, 0, 't'},
   // ResourceId URN
   {"resourceid", required_argument, 0, 'r'},
   // ActionId URN
   {"actionid", required_argument, 0, 'a'},
   // Proxy X509 certificate filename
   {"certchain", required_argument, 0, 'c'},
   // Display effective Request context
   {"context",  no_argument,  0, 'x'},
   // Display help
   {"help",  no_argument,  0, 'h'},
   {0, 0, 0, 0}
};

// option flags (global, used by log.c)
int verbose= 0;
int debug= 0;
// option intern
static int req_context= 0;
static long timeout= -1;
static char * certchain_filename= NULL;
static char * resourceid= NULL;
static char * actionid= NULL;

// PEM cert delimiters for certchain
static const char CERT_BEGIN[]= "-----BEGIN CERTIFICATE-----";
static const char CERT_END[]= "-----END CERTIFICATE-----";

/**
 * Reads the certificate file and returns the content as a buffer.
 * The returned buffer must be freed by the calling application.
 * Only the CERTIFICATE content will be included.
 *
 * @param filename The proxy or certificate file containing the cert chain
 */
static char * read_certchain(const char * filename) {
	char line[1024];
	FILE * file= fopen(filename,"r");
	if (file==NULL) {
		show_error("failed to open certchain file: %s: %s", filename, strerror(errno));
		return NULL;
	}
	BUFFER * cert_buffer= buffer_create(1024);
	if (cert_buffer==NULL) {
		show_error("can not create buffer");
		return NULL;
	}
	int cert_part= 0;
	while(fgets(line,1024,file) != NULL) {
		if (strncmp(CERT_BEGIN,line,strlen(CERT_BEGIN)) == 0) {
			cert_part= 1; // begin
			show_debug("certificate begin");
		}
		if (cert_part) {
			buffer_write(line,sizeof(char),strlen(line),cert_buffer);
		}
		if (strncmp(CERT_END,line,strlen(CERT_END)) == 0) {
			cert_part= 0; // end
			show_debug("certificate end");
		}
	}
	fclose(file);

	size_t size= buffer_length(cert_buffer);
	show_debug("buffer length: %d",(int)size);
	char * certchain= calloc(size+1,sizeof(char));
	if (certchain==NULL) {
		show_error("can not allocate buffer %d bytes: %s",size,strerror(errno));
		buffer_delete(cert_buffer);
		return NULL;
	}
	// copy
	if (buffer_read(certchain,sizeof(char),size,cert_buffer)==BUFFER_ERROR) {
		show_error("failed to copy certificate content to buffer");
		buffer_delete(cert_buffer);
		free(certchain);
		return NULL;
	}

	buffer_delete(cert_buffer);
	// check empty certchain
	if (strlen(certchain)<=0) {
		show_warn("certchain file: %s does not contain certificate",certchain_filename);
		free(certchain);
		return NULL;
	}
	return certchain;
}

/**
 * Creates a XACML Request with a Subject(cert-chain), Resource(resource-id) and Action(action-id).
 */
static xacml_request_t * create_xacml_request(const char * certchain, const char * resourceid, const char * actionid) {
	// Subject cert-chain
	xacml_subject_t * subject= xacml_subject_create();
	if (subject==NULL) {
		show_error("can not allocate XACML Subject");
		return NULL;
	}
	xacml_attribute_t * subject_attr_id= xacml_attribute_create(XACML_AUTHZINTEROP_SUBJECT_CERTCHAIN);
	if (subject_attr_id==NULL) {
		show_error("can not allocate XACML Subject/Attribute: %s",XACML_AUTHZINTEROP_SUBJECT_CERTCHAIN);
		xacml_subject_delete(subject);
		return NULL;
	}
	xacml_attribute_setdatatype(subject_attr_id,XACML_DATATYPE_BASE64BINARY);
	xacml_attribute_addvalue(subject_attr_id,certchain);
	xacml_subject_addattribute(subject,subject_attr_id);
	// Resource resource-id
	xacml_resource_t * resource= xacml_resource_create();
	if (resource==NULL) {
		show_error("can not allocate XACML Resource");
		xacml_subject_delete(subject);
		return NULL;
	}
	xacml_attribute_t * resource_attr_id= xacml_attribute_create(XACML_RESOURCE_ID);
	if (resource_attr_id==NULL) {
		show_error("can not allocate XAMCL Resource/Attribute: %s",XACML_RESOURCE_ID);
		xacml_subject_delete(subject);
		xacml_resource_delete(resource);
		return NULL;
	}
	xacml_attribute_addvalue(resource_attr_id,resourceid);
	xacml_resource_addattribute(resource,resource_attr_id);
	// Action action-id
	xacml_action_t * action= xacml_action_create();
	if (action==NULL) {
		show_error("can not allocate XACML Action");
		xacml_subject_delete(subject);
		xacml_resource_delete(resource);
		return NULL;
	}
	xacml_attribute_t * action_attr_id= xacml_attribute_create(XACML_ACTION_ID);
	if (action_attr_id==NULL) {
		show_error("can not allocate XACML Action/Attribute: %s",XACML_ACTION_ID);
		xacml_subject_delete(subject);
		xacml_resource_delete(resource);
		xacml_action_delete(action);
		return NULL;
	}
	xacml_attribute_addvalue(action_attr_id,actionid);
	xacml_action_addattribute(action,action_attr_id);
	// Request (Subject, Resource, Action)
	xacml_request_t * request= xacml_request_create();
	if (request==NULL) {
		show_error("can not allocate XACML Request");
		xacml_subject_delete(subject);
		xacml_resource_delete(resource);
		xacml_action_delete(action);
		return NULL;
	}
	xacml_request_addsubject(request,subject);
	xacml_request_addresource(request,resource);
	xacml_request_setaction(request,action);

	return request;
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
static int show_request(xacml_request_t * request) {
	if (request == NULL) {
		show_error("dump_request: request is NULL");
		return 1;
	}
	size_t subjects_l= xacml_request_subjects_length(request);
	show_info("request: %d subjects", (int)subjects_l);
	int i= 0;
	for (i= 0; i<subjects_l; i++) {
		xacml_subject_t * subject= xacml_request_getsubject(request,i);
		show_info("request.subject[%d].category= %s", i, xacml_subject_getcategory(subject));
		size_t attrs_l= xacml_subject_attributes_length(subject);
		show_info("request.subject[%d]: %d attributes", i, (int)attrs_l);
		int j= 0;
		for(j= 0; j<attrs_l; j++) {
			xacml_attribute_t * attr= xacml_subject_getattribute(subject,j);
			show_info("request.subject[%d].attribute[%d].id= %s", i,j,xacml_attribute_getid(attr));
			show_info("request.subject[%d].attribute[%d].datatype= %s", i,j,xacml_attribute_getdatatype(attr));
			show_info("request.subject[%d].attribute[%d].issuer= %s", i,j,xacml_attribute_getissuer(attr));
			size_t values_l= xacml_attribute_values_length(attr);
			show_info("request.subject[%d].attribute[%d]: %d values", i,j,(int)values_l);
			int k= 0;
			for (k= 0; k<values_l; k++) {
				show_info("request.subject[%d].attribute[%d].value[%d]= %s", i,j,k,xacml_attribute_getvalue(attr,k));
			}
		}
	}
	size_t resources_l= xacml_request_resources_length(request);
	show_info("request: %d resources", (int)resources_l);
	for (i= 0; i<resources_l; i++) {
		xacml_resource_t * resource= xacml_request_getresource(request,i);
		show_info("request.resource[%d].content= %s", i, xacml_resource_getcontent(resource));
		size_t attrs_l= xacml_resource_attributes_length(resource);
		show_info("request.resource[%d]: %d attributes", i, (int)attrs_l);
		int j= 0;
		for(j= 0; j<attrs_l; j++) {
			xacml_attribute_t * attr= xacml_resource_getattribute(resource,j);
			show_info("request.resource[%d].attribute[%d].id= %s", i,j,xacml_attribute_getid(attr));
			show_info("request.resource[%d].attribute[%d].datatype= %s", i,j,xacml_attribute_getdatatype(attr));
			show_info("request.resource[%d].attribute[%d].issuer= %s", i,j,xacml_attribute_getissuer(attr));
			size_t values_l= xacml_attribute_values_length(attr);
			show_info("request.resource[%d].attribute[%d]: %d values", i,j,(int)values_l);
			int k= 0;
			for (k= 0; k<values_l; k++) {
				show_info("request.resource[%d].attribute[%d].value[%d]= %s", i,j,k,xacml_attribute_getvalue(attr,k));
			}
		}
	}
	xacml_action_t * action= xacml_request_getaction(request);
	size_t act_attrs_l= xacml_action_attributes_length(action);
	show_info("request.action: %d attributes",(int)act_attrs_l);
	int j= 0;
	for (j= 0; j<act_attrs_l; j++) {
		xacml_attribute_t * attr= xacml_action_getattribute(action,j);
		show_info("request.action.attribute[%d].id= %s", j,xacml_attribute_getid(attr));
		show_info("request.action.attribute[%d].datatype= %s", j,xacml_attribute_getdatatype(attr));
		show_info("request.action.attribute[%d].issuer= %s", j,xacml_attribute_getissuer(attr));
		size_t values_l= xacml_attribute_values_length(attr);
		show_info("request.action.attribute[%d]: %d values", j,(int)values_l);
		int k= 0;
		for (k= 0; k<values_l; k++) {
			show_info("request.action.attribute[%d].value[%d]= %s",j,k,xacml_attribute_getvalue(attr,k));
		}
	}
	xacml_environment_t * env= xacml_request_getenvironment(request);
	size_t env_attrs_l= xacml_environment_attributes_length(env);
	show_info("request.environment: %d attributes",(int)env_attrs_l);
	for (j= 0; j<env_attrs_l; j++) {
		xacml_attribute_t * attr= xacml_environment_getattribute(env,j);
		show_info("request.environment.attribute[%d].id= %s", j,xacml_attribute_getid(attr));
		show_info("request.environment.attribute[%d].datatype= %s", j,xacml_attribute_getdatatype(attr));
		show_info("request.environment.attribute[%d].issuer= %s", j,xacml_attribute_getissuer(attr));
		size_t values_l= xacml_attribute_values_length(attr);
		show_info("request.environment.attribute[%d]: %d values", j,(int)values_l);
		int k= 0;
		for (k= 0; k<values_l; k++) {
			show_info("request.environment.attribute[%d].value[%d]= %s",j,k,xacml_attribute_getvalue(attr,k));
		}
	}
	return 0;
}

/*
 * Dumps a XACML response.
 */
static int show_response(xacml_response_t * response) {
	if (response == NULL) {
		show_error("dump_response: response is NULL");
		return 1;
	}
	size_t results_l= xacml_response_results_length(response);
	show_info("response: %d results", (int)results_l);
	int i= 0;
	for(i= 0; i<results_l; i++) {
		xacml_result_t * result= xacml_response_getresult(response,i);
		show_info("response.result[%d].decision= %s", i, decision_str(xacml_result_getdecision(result)));
		show_info("response.result[%d].resourceid= %s", i, xacml_result_getresourceid(result));
		xacml_status_t * status= xacml_result_getstatus(result);
		show_info("response.result[%d].status.message= %s", i, xacml_status_getmessage(status));
		xacml_statuscode_t * statuscode= xacml_status_getcode(status);
		show_info("response.result[%d].status.code.value= %s", i, xacml_statuscode_getvalue(statuscode));
		xacml_statuscode_t * subcode= xacml_statuscode_getsubcode(statuscode);
		if (subcode != NULL) {
			show_info("response.result[%d].status.code.subcode.value= %s", i, xacml_statuscode_getvalue(subcode));
		}
		size_t obligations_l= xacml_result_obligations_length(result);
		show_info("response.result[%d]: %d obligations", i, (int)obligations_l);
		int j=0;
		for(j= 0; j<obligations_l; j++) {
			xacml_obligation_t * obligation= xacml_result_getobligation(result,j);
			show_info("response.result[%d].obligation[%d].id= %s",i,j, xacml_obligation_getid(obligation));
			show_info("response.result[%d].obligation[%d].fulfillOn= %s",i,j, decision_str(xacml_obligation_getfulfillon(obligation)));
			size_t attrs_l= xacml_obligation_attributeassignments_length(obligation);
			show_info("response.result[%d].obligation[%d]: %d attribute assignments",i,j,(int)attrs_l);
			int k= 0;
			for (k= 0; k<attrs_l; k++) {
				xacml_attributeassignment_t * attr= xacml_obligation_getattributeassignment(obligation,k);
				show_info("response.result[%d].obligation[%d].attributeassignment[%d].id= %s",i,j,k,xacml_attributeassignment_getid(attr));
				size_t values_l= xacml_attributeassignment_values_length(attr);
				int l= 0;
				for (l= 0; l<values_l; l++) {
					show_info("response.result[%d].obligation[%d].attributeassignment[%d].value[%d]= %s",i,j,k,l,xacml_attributeassignment_getvalue(attr,l));
				}
			}
		}
	}
	return 0;
}

/**
 * Shows help
 */
static void show_help() {
	fprintf(stdout,"PEP-C client CLI v." PACKAGE_VERSION "\n");
	fprintf(stdout,"Usage: pepcli --pepd <URL> --certchain <FILE> --resourceid <URI> --actionid <URI> [options...]\n");
	fprintf(stdout,"\n");
	fprintf(stdout,"Submit a XACML Request to the PEPd and show the XACML Response.\n");
	fprintf(stdout,"\n");
	fprintf(stdout,"Options:\n");
	fprintf(stdout," -p|--pepd <URL>         PEPd endpoint URL. Add multiple --pepd options for failover\n");
	fprintf(stdout," -c|--certchain <FILE>   XACML Subject cert-chain: proxy or X509 file\n");
	fprintf(stdout," -r|--resourceid <URI>   XACML Resource identifier\n");
	fprintf(stdout," -a|--actionid <URI>     XACML Action identifier\n");
	fprintf(stdout," -t|--timeout <SEC>      Connection timeout in second\n");
	fprintf(stdout," -x|--requestcontext     Show effective XACML Request context\n");
	fprintf(stdout," -v|--verbose            Verbose\n");
	fprintf(stdout," -d|--debug              Show debug information\n");
	fprintf(stdout," -h|--help               This help\n");
}

/**
 * Main
 */
int main(int argc, char **argv) {
	linkedlist_t * pepds= llist_create();
	if (pepds==NULL) {
		show_error("Can not allocate PEPd url list.");
		exit(E_MEMORY);
	}
	// parse arguments
	int c;
	while ((c= getopt_long(argc, argv, "dvp:t:r:a:c:xh", long_options, NULL)) != -1) {
		switch(c) {
		case 'd':
			debug= 1;
			show_debug("debug set.");
			break;
		case 'v':
			show_debug("verbose set.");
			verbose= 1;
			break;
		case 'x':
			show_debug("effective Request context.");
			req_context= 1;
			break;
		case 'p':
			show_debug("pepd: %s",optarg);
			// add url to list
			if (strlen(optarg) > 0) {
				llist_add(pepds,optarg);
			}
			break;
		case 'c':
			show_debug("certchain: %s",optarg);
			if (strlen(optarg) > 0) {
				certchain_filename= optarg;
			}
			break;
		case 'r':
			show_debug("resourceid: %s",optarg);
			if (strlen(optarg) > 0) {
				resourceid= optarg;
			}
			break;
		case 'a':
			show_debug("actionid: %s",optarg);
			if (strlen(optarg) > 0) {
				actionid= optarg;
			}
			break;
		case 't':
			show_debug("timeout: %s",optarg);
			timeout= (int)strtol(optarg,NULL,10);
			if (errno == EINVAL || errno == ERANGE || timeout <= 0) {
				timeout= -1;
				show_warn("timeout %s can not be converted. Using default.",optarg);
			}
			break;
		case 'h':
			show_help();
			exit(E_OK);
			break;
		case '?':
            // getopt_long already printed an error message.
			//show_help();
			exit(E_OPTION);
			break;
		default:
			break;
		}
	}

	// check mandatory options
	int pepds_l= llist_length(pepds);
	if (pepds_l<1) {
		show_error("mandatory option --pepd <URL> is missing");
		show_help();
		exit(E_OPTION);
	}
	int i= 0;
	for(i= 0; i<pepds_l; i++) {
		show_info("PEPd: %s",(char *)llist_get(pepds,i));
	}

	if (certchain_filename==NULL) {
		show_error("mandatory option --certchain <FILE> is missing");
		show_help();
		exit(E_OPTION);
	}
	show_info("certchain: %s", certchain_filename);

	if (resourceid==NULL) {
		show_error("mandatory option --resourceid <URI> is missing");
		show_help();
		exit(E_OPTION);
	}
	show_info("resourceid: %s", resourceid);

	if (actionid==NULL) {
		show_error("mandatory option --actionid <URI> is missing");
		show_help();
		exit(E_OPTION);
	}
	show_info("actionid: %s",actionid);

	// read certchain file
	show_info("read certchain from: %s",certchain_filename);
	char * certchain= read_certchain(certchain_filename);
	if (certchain==NULL) {
		show_error("certchain is empty");
		exit(E_CERTCHAIN);
	}
	show_debug("certchain:[\n%s]", certchain);

	// PEP client
	show_debug("create PEP client...");
	pep_error_t pep_rc= pep_initialize();
	if (pep_rc!=PEP_OK) {
		show_error("failed to init PEP client: %s", pep_strerror(pep_rc));
		exit(E_PEPC);
	}
	//
	// PEP client options
	//
	// verbose and debug
	pep_setoption(PEP_OPTION_LOG_HANDLER,log_handler_pep);
	if (debug) {
		pep_setoption(PEP_OPTION_LOG_LEVEL,PEP_LOGLEVEL_DEBUG);
	}
	if (verbose) {
		pep_setoption(PEP_OPTION_LOG_LEVEL,PEP_LOGLEVEL_INFO);
	}
	// endpoint urls
	for(i=0; i<pepds_l; i++) {
		char * url= llist_get(pepds,i);
		show_debug("set PEPd url: %s",url);
		pep_rc= pep_setoption(PEP_OPTION_ENDPOINT_URL,url);
		if (pep_rc!=PEP_OK) {
			show_error("failed to set PEPd url: %s: %s",url,pep_strerror(pep_rc));
			exit(E_PEPC);
		}
	}
	if (timeout>0) {
		show_debug("set PEP-C client timeout: %d",timeout);
		pep_rc= pep_setoption(PEP_OPTION_ENDPOINT_TIMEOUT, timeout);
		if (pep_rc!=PEP_OK) {
			show_warn("failed to set PEP client timeout: %d: %s",timeout,pep_strerror(pep_rc));
		}
	}
	// no SSL validation
	show_debug("disable PEPd SSL validation");
	pep_rc= pep_setoption(PEP_OPTION_ENDPOINT_SSL_VALIDATION, 0);
	if (pep_rc!=PEP_OK) {
		show_warn("failed to disable PEPd SSL validation: %s",pep_strerror(pep_rc));
	}

	show_debug("create XACML request");
	xacml_request_t * request= create_xacml_request(certchain,resourceid,actionid);
	if (request==NULL) {
		show_error("failed to create XACML request");
		exit(E_XACMLREQ);
	}

	// submit request
	show_info("authorize XACML request");
	xacml_response_t * response= NULL;
	pep_rc= pep_authorize(&request,&response);
	if (pep_rc!=PEP_OK) {
		show_error("failed to authorize XACML request: %s",pep_strerror(pep_rc));
		exit(E_PEPC);
	}
	int old_verbose= verbose;
	verbose= 1;
	if (req_context) {
		show_request(request);
	}
	show_response(response);
	verbose= old_verbose;

	// clean up
	show_info("done.");
	pep_destroy();
	llist_delete(pepds);
	free(certchain);
	exit(E_OK);
}
