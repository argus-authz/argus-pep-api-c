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
 * $Id: main.c,v 1.2 2009/03/13 16:22:27 vtschopp Exp $
 */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>

#include "pep/pep.h"
#include "util/buffer.h"
#include "util/linkedlist.h"

#ifdef HAVE_CONFIG_H
#include "config.h"  /* PACKAGE_NAME and PACKAGE_VERSION const */
#else
#define PACKAGE_VERSION "1.0.0"
#endif

// logging prototypes: log.c
void info(const char * format, ...);
void error(const char * format, ...);
void debug(const char * format, ...);

// main return codes
#define E_OK     0  // OK
#define E_MEMORY 1  // memory allocation error
#define E_OPTION 2  // invalid option

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
   {"certificate", required_argument, 0, 'c'},
   // Display effective Request context
   {"context",  no_argument,  0, 'x'},
   // Display help
   {"help",  no_argument,  0, 'h'},
   {0, 0, 0, 0}
};

// option flags
static int verbose= 0;
static int req_context= 0;
static linkedlist_t * pepd_urls= NULL;
static long timeout= -1;
static char * cert_filename= NULL;
static char * resourceid= NULL;
static char * actionid= NULL;

// buffers
static char * cert_content= NULL;

static int read_cert_content(const char * filename) {
	FILE * file= fopen(filename,"r");
	if (file==NULL) {
		error("failed to open certificate file: %s: ", filename, strerror(errno));
		return 1;
	}
	BUFFER * certb= buffer_create(1024);
	if (certb==NULL) {
		error("can not create buffer");
		return 2;
	}
	if (buffer_fread(certb,file) != BUFFER_OK) {
		error("failed to read certificate file: %s", filename);
		buffer_delete(certb);
		return 3;
	}

	size_t size= buffer_length(certb);
	if (cert_content!=NULL) {
		free(cert_content);
	}
	cert_content= calloc(size+1,sizeof(char));
	if (cert_content==NULL) {
		error("can not allocate buffer %d bytes: %s",size,strerror(errno));
		buffer_delete(certb);
		return 4;
	}
	// copy
	if (buffer_read(cert_content,size,sizeof(char),certb)==BUFFER_ERROR) {
		error("failed to copy certificate content to buffer");
		buffer_delete(certb);
		free(cert_content);
		cert_content= NULL;
		return 4;
	}

	buffer_delete(certb);
	return 0;
}

static xacml_request_t * create_xacml_request(const char * certificate, const char * resourceid, const char * actionid) {
	xacml_subject_t * subject= xacml_subject_create();
	if (subject==NULL) {
		error("can not allocate subject");
		return NULL;
	}
	xacml_attribute_t * subject_attr_id= xacml_attribute_create(XACML_AUTHZINTEROP_SUBJECT_CERTCHAIN);
	xacml_attribute_setdatatype(subject_attr_id,XACML_DATATYPE_BASE64BINARY);
	xacml_attribute_addvalue(subject_attr_id,certificate);
	xacml_subject_addattribute(subject,subject_attr_id);

	xacml_resource_t * resource= xacml_resource_create();
	if (resource==NULL) {
		error("can not allocate resource");
		xacml_subject_delete(subject);
		return NULL;
	}
	xacml_attribute_t * resource_attr_id= xacml_attribute_create(XACML_RESOURCE_ID);
	if (resource_attr_id==NULL) {
		error("can not allocate resource.attribute");
		xacml_subject_delete(subject);
		xacml_resource_delete(resource);
		return NULL;
	}
	xacml_attribute_addvalue(resource_attr_id,resourceid);
	xacml_resource_addattribute(resource,resource_attr_id);

	xacml_action_t * action= xacml_action_create();
	if (action==NULL) {
		error("can not allocate action");
		xacml_subject_delete(subject);
		xacml_resource_delete(resource);
		return NULL;
	}
	xacml_attribute_t * action_attr_id= xacml_attribute_create(XACML_ACTION_ID);
	if (action_attr_id==NULL) {
		error("can not allocate action.attribute");
		xacml_subject_delete(subject);
		xacml_resource_delete(resource);
		xacml_action_delete(action);
		return NULL;
	}
	xacml_attribute_addvalue(action_attr_id,actionid);
	xacml_action_addattribute(action,action_attr_id);

	xacml_request_t * request= xacml_request_create();
	if (request==NULL) {
		error("can not allocate request");
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

void show_help() {
	fprintf(stdout,"PEP client CLI v." PACKAGE_VERSION "\n");
	fprintf(stdout,"Usage: pepcli --pepd <URL> [options...]\n");
	fprintf(stdout,"\nSubmit a XACML Request to the PEPd and show the XACML Response.\n");
	fprintf(stdout,"\nOptions:\n");
	fprintf(stdout," -p|--pepd <URL>         PEPd endpoint URL. Add multiple --pepd options for failover\n");
	fprintf(stdout," -c|--certificate <FILE> XACML Subject proxy or X509 content\n");
	fprintf(stdout," -r|--resourceid <URI>   XACML Resource identifier\n");
	fprintf(stdout," -a|--actionid <URI>     XACML Action identifier\n");
	fprintf(stdout," -t|--timeout <SEC>      Connection timeout in second\n");
	fprintf(stdout," -c|--context            Show effective XACML Request context\n");
	fprintf(stdout," -v|--verbose            Verbose\n");
	fprintf(stdout," -h|--help               This help\n");
}

int main(int argc, char **argv) {
	pepd_urls= llist_create();
	if (pepd_urls==NULL) {
		error("Can not allocate list.");
		exit(E_MEMORY);
	}
	// parse arguments
	int c;
	while ((c= getopt_long(argc, argv, "vp:t:r:a:c:xh", long_options, NULL)) != -1) {
		switch(c) {
		case 'v':
			debug("verbose set.");
			verbose= 1;
			break;
		case 'x':
			debug("show effective Request context.");
			req_context= 1;
			break;
		case 'p':
			debug("PEPd url: %s",optarg);
			// add url to list
			llist_add(pepd_urls,optarg);
			break;
		case 'c':
			debug("certificate: %s",optarg);
			cert_filename= optarg;
			break;
		case 'r':
			debug("Resource id: %s",optarg);
			resourceid= optarg;
			break;
		case 'a':
			debug("Action id: %s",optarg);
			actionid= optarg;
			break;
		case 't':
			debug("Connection timeout(sec): %s",optarg);
			timeout= (int)strtol(optarg,NULL,10);
			if (errno == EINVAL || errno == ERANGE) {
				timeout= -1;
				error("Timeout %s can not be converted. Using default.",optarg);
			}
			break;
		case 'h':
			show_help();
			exit(E_OK);
			break;
		case '?':
            // getopt_long already printed an error message.
			show_help();
			exit(E_OPTION);
			break;
		default:
			debug("Not implemented option: %c arg: %s",(char)c, optarg);
			break;
		}
	}

	int l= llist_length(pepd_urls);
	if (l<1) {
		error("mandatory option --pepd is missing");
		show_help();
		exit(E_OPTION);
	}
	int i= 0;
	for(i= 0; i<l; i++) {
		char * url= (char *)llist_get(pepd_urls,i);
		debug("PEPd URL: %s",url);
	}
	debug("verbose: %s", verbose ? "true" : "false");
	debug("context: %s", req_context ? "true" : "false");
	llist_delete(pepd_urls);
	return 0;
}
