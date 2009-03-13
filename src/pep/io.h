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
 * $Id: io.h,v 1.3 2009/03/13 12:02:17 vtschopp Exp $
 */
#ifndef _PEP_IO_H_
#define _PEP_IO_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include "pep/error.h"
#include "pep/xacml.h"
#include "util/buffer.h"

/**
 * Marshalls the PEP XACML request object and writes the serialized Hessian bytes
 * into the output buffer.
 *
 * @param const xacml_request_t * request the PEP XACML request to marshal.
 * @param BUFFER * output buffer.
 *
 * @return pep_error_t PEP_OK or an error code.
 */
pep_error_t xacml_request_marshalling(const xacml_request_t * request, BUFFER * output);

/**
 * Reads the serialized Hessian bytes from the input buffer and unmarshalls the PEP
 * XACML response object.
 *
 * On error, return code != PEP_OK, the PEP response object state is indeterminate.
 * (should be NULL)
 *
 * @param xacml_response_t ** response the unmarshalled PEP XACML response (output).
 * @param BUFFER * input the buffer to read from.
 *
 * @return pep_error_t PEP_OK or an error code.
 *
 * Example:
 *   xacml_response_t * response= NULL;
 *   int rc= pep_response_unmarshalling(&response, input);
 *   if (rc == PEP_OK) {
 *      process response...
 *   }
 *   else {
 *      error handling...
 *   }
 */
pep_error_t xacml_response_unmarshalling(xacml_response_t ** response, BUFFER * input);

/**
 * The Java class namespaces and variable name constants for the PEP model
 * Hessian serialization and deserialization mapping.
 */
static const char PEP_ATTRIBUTE_CLASSNAME[]= "org.glite.authz.pep.model.Attribute";
static const char PEP_ATTRIBUTE_ID[]= "id";
static const char PEP_ATTRIBUTE_DATATYPE[]= "dataType";
static const char PEP_ATTRIBUTE_ISSUER[]= "issuer";
static const char PEP_ATTRIBUTE_VALUES[]= "values";

static const char PEP_SUBJECT_CLASSNAME[]= "org.glite.authz.pep.model.Subject";
static const char PEP_SUBJECT_CATEGORY[]= "category";
static const char PEP_SUBJECT_ATTRIBUTES[]= "attributes";

static const char PEP_RESOURCE_CLASSNAME[]= "org.glite.authz.pep.model.Resource";
static const char PEP_RESOURCE_CONTENT[]= "resourceContent";
static const char PEP_RESOURCE_ATTRIBUTES[]= "attributes";

static const char PEP_ACTION_CLASSNAME[]= "org.glite.authz.pep.model.Action";
static const char PEP_ACTION_ATTRIBUTES[]= "attributes";

static const char PEP_ENVIRONMENT_CLASSNAME[]= "org.glite.authz.pep.model.Environment";
static const char PEP_ENVIRONMENT_ATTRIBUTES[]= "attributes";

static const char PEP_REQUEST_CLASSNAME[]= "org.glite.authz.pep.model.Request";
static const char PEP_REQUEST_SUBJECTS[]= "subjects";
static const char PEP_REQUEST_RESOURCES[]= "resources";
static const char PEP_REQUEST_ACTION[]= "action";
static const char PEP_REQUEST_ENVIRONMENT[]= "environment";

static const char PEP_STATUSCODE_CLASSNAME[]= "org.glite.authz.pep.model.StatusCode";
static const char PEP_STATUSCODE_VALUE[]= "code";
static const char PEP_STATUSCODE_SUBCODE[]= "subCode";

static const char PEP_STATUS_CLASSNAME[]= "org.glite.authz.pep.model.Status";
static const char PEP_STATUS_MESSAGE[]= "message";
static const char PEP_STATUS_CODE[]= "statusCode";

static const char PEP_ATTRIBUTEASSIGNMENT_CLASSNAME[]= "org.glite.authz.pep.model.AttributeAssignment";
static const char PEP_ATTRIBUTEASSIGNMENT_ID[]= "attributeId";
static const char PEP_ATTRIBUTEASSIGNMENT_VALUES[]= "values";

static const char PEP_OBLIGATION_CLASSNAME[]= "org.glite.authz.pep.model.Obligation";
static const char PEP_OBLIGATION_ID[]= "id";
static const char PEP_OBLIGATION_FULFILLON[]= "fulfillOn";
static const char PEP_OBLIGATION_ASSIGNMENTS[]= "attributeAssignments";

static const char PEP_RESULT_CLASSNAME[]=  "org.glite.authz.pep.model.Result";
static const char PEP_RESULT_DECISION[]=  "decision";
static const char PEP_RESULT_RESOURCEID[]=  "resourceId";
static const char PEP_RESULT_STATUS[]=  "status";
static const char PEP_RESULT_OBLIGATIONS[]=  "obligations";

static const char PEP_RESPONSE_CLASSNAME[]= "org.glite.authz.pep.model.Response";
static const char PEP_RESPONSE_REQUEST[]= "request";
static const char PEP_RESPONSE_RESULTS[]= "results";


#ifdef  __cplusplus
}
#endif

#endif // _PEP_IO_H_
