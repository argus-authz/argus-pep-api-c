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
 * $Id: main.c,v 1.1 2009/03/13 12:36:42 vtschopp Exp $
 */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <getopt.h>

#include "pep/pep.h"
#include "util/buffer.h"
#include "util/linkedlist.h"

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
   {0, 0, 0, 0}
};

// option flags
static int verbose= 0;
static int req_context= 0;
static linkedlist_t * pepd_urls= NULL;
static long timeout= -1;
static char * proxy= NULL;
static char * resourceid= NULL;
static char * actionid= NULL;


static BUFFER * read_px509(const char * filename) {
	return NULL;
}

static xacml_request_t * create_xacml_request(const char * px509, const char * resourceid, const char * actionid) {
	xacml_subject_t * subject= xacml_subject_create();
	if (subject==NULL) {
		error("can not allocate subject");
		return NULL;
	}
	xacml_attribute_t * subject_attr_id= xacml_attribute_create(XACML_AUTHZINTEROP_SUBJECT_CERTCHAIN);
	xacml_attribute_setdatatype(subject_attr_id,XACML_DATATYPE_BASE64BINARY);
	xacml_attribute_addvalue(subject_attr_id,px509);
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

int main(int argc, char **argv) {
	pepd_urls= llist_create();
	if (pepd_urls==NULL) {
		error("Can not allocate list.");
		exit(E_MEMORY);
	}
	int c;
	while ((c= getopt_long(argc, argv, "vp:t:r:a:c:x", long_options, NULL)) != -1) {
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
			proxy= optarg;
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
		case '?':
            // getopt_long already printed an error message.
			error("invalid option");
			exit(E_OPTION);
			break;
		default:
			debug("Not implemented option: %c arg: %s",(char)c, optarg);
			break;
		}
	}

	printf("all options parsed:\n");
	printf("verbose: %s\n", verbose ? "true" : "false");
	printf("context: %s\n", req_context ? "true" : "false");
	int l= llist_length(pepd_urls);
	int i= 0;
	for(i= 0; i<l; i++) {
		char * url= (char *)llist_get(pepd_urls,i);
		printf("PEPd URL: %s\n",url);
	}
	llist_delete(pepd_urls);
	return 0;
}
