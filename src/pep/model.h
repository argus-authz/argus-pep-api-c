/*
 * Copyright 2008-2009 Members of the EGEE Collaboration.
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
/*
 * $Id: model.h,v 1.3 2009/02/18 17:02:58 vtschopp Exp $
 * $Name:  $
 * @author Valery Tschopp <valery.tschopp@switch.ch>
 * @version 1.0
 */
#ifndef _PEP_MODEL_H_
#define _PEP_MODEL_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include <stddef.h> // size_t

/** @addtogroup Model
 *
 * The XACML object model (request, response) used by the PEP client.
 *
 * TODO: add object schema with the \@image tag
 *
 * @{
 */

/*
 * PEP model functions return codes
 */
#define PEP_MODEL_OK     0 /**< PEP model functions return code OK */
#define PEP_MODEL_ERROR -1 /**< PEP model functions return code ERROR */

/**
 * @anchor Attribute
 * PEP XACML Attribute type.
 */
typedef struct pep_attribute pep_attribute_t;

/**
 * Creates and initializes a XACML Attribute.
 * @param id the mandatory id attribute
 * @return pep_attribute_t * pointer to the new Attribute or @a NULL on error.
 */
pep_attribute_t * pep_attribute_create(const char * id);

/**
 * Sets the id attribute of the XACML Attribute.
 * @param attr pointer to the XACML Attribute
 * @param id the id attribute
 * @return int {@link #PEP_MODEL_OK} or {@link #PEP_MODEL_ERROR} on error.
 */
int pep_attribute_setid(pep_attribute_t * attr, const char * id);

/**
 * Gets the id attribute of the XACML Attribute.
 * @param attr pointer to the XACML Attribute
 * @return const char * the id attribute or @a NULL
 */
const char * pep_attribute_getid(const pep_attribute_t * attr);

/**
 * Sets the datatype attribute of the XACML Attribute.
 * @param attr pointer to the XACML Attribute
 * @param datatype the datatype attribute
 * @return int {@link #PEP_MODEL_OK} or {@link #PEP_MODEL_ERROR} on error.
 */
int pep_attribute_setdatatype(pep_attribute_t * attr, const char * datatype);

/**
 * Gets the datatype attribute of the XACML Attribute.
 * @param attr pointer to the XACML Attribute
 * @return const char * the datatype attribute or @a NULL
 */
const char * pep_attribute_getdatatype(const pep_attribute_t * attr);

/**
 * Sets the issuer attribute of the XACML Attribute.
 * @param attr pointer to the XACML Attribute
 * @param issuer the issuer attribute
 * @return int {@link #PEP_MODEL_OK} or {@link #PEP_MODEL_ERROR} on error.
 */
int pep_attribute_setissuer(pep_attribute_t * attr, const char * issuer);

/**
 * Gets the issuer attribute of the XACML Attribute.
 * @param attr pointer to the XACML Attribute
 * @return const char * the issuer attribute or @a NULL
 */
const char * pep_attribute_getissuer(const pep_attribute_t * attr);

/**
 * Adds a value element to the XACML Attribute.
 * @param attr pointer to the XACML Attribute
 * @param value the value (string) to add
 * @return int {@link #PEP_MODEL_OK} or {@link #PEP_MODEL_ERROR} on error.
 */
int pep_attribute_addvalue(pep_attribute_t * attr, const char *value);

/**
 * Returns the number of AttributeValue in the XACML Attribute.
 * @param attr pointer to the XACML Attribute
 * @return size_t number of AttributeValue [0..n] or {@link #PEP_MODEL_ERROR} on error.
 * @see pep_attribute_getvalue(const pep_attribute_t * attr,int index) to get an AttributeValue at a particular index.
 */
size_t pep_attribute_values_length(const pep_attribute_t * attr);

/**
 * Gets the AttributeValue of the XACML Attribute at index.
 * @param attr pointer to the XACML Attribute
 * @param index of the AttributeValue to get in range [0..length-1].
 * @return const char * the AttributeValue or @a NULL if index is out of range.
 * @see pep_attribute_values_length(const pep_attribute_t * attr) to get the index range.
 */
const char * pep_attribute_getvalue(const pep_attribute_t * attr,int index);

/**
 * Deletes the XACML Attribute.
 * @param attr pointer to the XACML Attribute to delete
 * @note The XACML containers (@ref Subject, @ref Resource, @ref Action or @ref Environment) will delete the contained
 * Attributes when deleted.
 */
void pep_attribute_delete(pep_attribute_t * attr);


/**
 * @anchor Subject
 * PEP XACML Subject type.
 */
typedef struct pep_subject pep_subject_t;

/*
 * PEP Subject/\@SubjectCategory attribute constants (XACML 2.0, B.2)
 */
static const char PEP_SUBJECT_CATEGORY_ACCESS[]= "urn:oasis:names:tc:xacml:1.0:subject-category:access-subject"; /**<  XACML Subject/\@SubjectCategory attribute constants (XACML 2.0, B.2) */
static const char PEP_SUBJECT_CATEGORY_INTERMEDIARY[]= "urn:oasis:names:tc:xacml:1.0:subject-category:intermediary-subject"; /**<  XACML Subject/\@SubjectCategory attribute constants (XACML 2.0, B.2) */
static const char PEP_SUBJECT_CATEGORY_RECIPIENT[]= "urn:oasis:names:tc:xacml:1.0:subject-category:recipient-subject"; /**<  XACML Subject/\@SubjectCategory attribute constants (XACML 2.0, B.2) */
static const char PEP_SUBJECT_CATEGORY_CODEBASE[]= "urn:oasis:names:tc:xacml:1.0:subject-category:codebase"; /**<  XACML Subject/\@SubjectCategory attribute constants (XACML 2.0, B.2) */
static const char PEP_SUBJECT_CATEGORY_REQUESTING_MACHINE[]= "urn:oasis:names:tc:xacml:1.0:subject-category:requesting-machine"; /**<  XACML Subject/\@SubjectCategory attribute constants (XACML 2.0, B.2) */

/**
 * Creates a XACML Subject.
 * @return pep_subject_t * pointer to the new XACML Subject or @a NULL on error.
 */
pep_subject_t * pep_subject_create(void);

/**
 * Sets the XACML Subject/\@SubjectCategory attribute.
 * @param subject pointer to the XACML Subject
 * @param category the SubjectCategory attribute
 * @return int {@link #PEP_MODEL_OK} or {@link #PEP_MODEL_ERROR} on error.
 * @see #PEP_SUBJECT_CATEGORY_ACCESS constant
 * @see #PEP_SUBJECT_CATEGORY_INTERMEDIARY constant
 * @see #PEP_SUBJECT_CATEGORY_RECIPIENT constant
 * @see #PEP_SUBJECT_CATEGORY_CODEBASE constant
 * @see #PEP_SUBJECT_CATEGORY_REQUESTING_MACHINE constant
 */
int pep_subject_setcategory(pep_subject_t * subject, const char * category);

/**
 * Gets the XACML Subject/\@SubjectCategory attribute value.
 * @param subject pointer to the XACML Subject
 * @return const char * the SubjectCategory attribute value or @a NULL
 */
const char * pep_subject_getcategory(const pep_subject_t * subject);

/**
 * Adds a XACML Attribute to the Subject.
 * @param subject pointer to the XACML Subject
 * @param attr pointer to the XACML Attribute to add
 * @return int {@link #PEP_MODEL_OK} or {@link #PEP_MODEL_ERROR} on error.
 * @see @ref Attribute
 */
int pep_subject_addattribute(pep_subject_t * subject, pep_attribute_t * attr);

/**
 * Returns the number of XACML Attribute in the XACML Subject.
 * @param subject pointer to the XACML Subject
 * @return size_t number of XACML Attribute [0..n] contained in the Subject or {@link #PEP_MODEL_ERROR} on error.
 * @see pep_subject_getattribute(const pep_subject_t * subject, int index) to get a XACML Attribute at a particular index.
 */
size_t pep_subject_attributes_length(const pep_subject_t * subject);

/**
 * Gets the XACML Attribute from the XACML Subject at index.
 * @param subject pointer to the XACML Subject
 * @param index of the XACML Attribute to get in range [0..length-1].
 * @return pep_attribute_t * pointer to the XACML Attribute or @a NULL if index is out of range.
 * @see pep_subject_attributes_length(const pep_subject_t * subject) to get the index range.
 */
pep_attribute_t * pep_subject_getattribute(const pep_subject_t * subject, int index);

/**
 * Deletes the XACML Subject.
 * @param subject pointer to the XACML Subject
 * @note The XACML container (@ref Request) will delete the contained XACML
 * Subjects when deleted.
 */
void pep_subject_delete(pep_subject_t * subject);


/**
 * PEP XACML Resource type.
 * @anchor Resource
 */
typedef struct pep_resource pep_resource_t;

/**
 * Creates a XACML Resource.
 * @return pep_resource_t * pointer to the new XACML Resource or @a NULL on error.
 */
pep_resource_t * pep_resource_create(void);

/**
 * Sets the XACML Resource/ResourceContent element as string.
 * @param resource pointer the XACML Resource
 * @param content the ResourceContent as string
 * @return int {@link #PEP_MODEL_OK} or {@link #PEP_MODEL_ERROR} on error.
 */
int pep_resource_setcontent(pep_resource_t * resource, const char * content);

/**
 * Gets the XACML Resource/ResourceContent element as string.
 * @param resource pointer the XACML Resource
 * @return const char * the ResourceContent as string or @a NULL if not set
 */
const char * pep_resource_getcontent(const pep_resource_t * resource);

/**
 * Adds a XACML Attribute to the XACML Resource
 * @param resource pointer to the XACML Resource
 * @param attr pointer to the XACML Attribute to add
 * @return int {@link #PEP_MODEL_OK} or {@link #PEP_MODEL_ERROR} on error.
 */
int pep_resource_addattribute(pep_resource_t * resource, pep_attribute_t * attr);

/**
 * Returns the number of XACML Attribute contained in the XACML Resource.
 * @param resource pointer to the XACML Resource
 * @return size_t number of XACML Attribute [0..n] contained in the Resource or {@link #PEP_MODEL_ERROR} on error.
 */
size_t pep_resource_attributes_length(const pep_resource_t * resource);

/**
 * Gets the XACML Attribute from the XACML Resource at the given index.
 * @param resource pointer to the XACML Resource
 * @param index of the XACML Attribute to get in range [0..length-1].
 * @return pep_attribute_t * pointer to the XACML Attribute or @a NULL if index is out of range.
 * @see pep_resource_attributes_length(const pep_resource_t * resource) to get the index range.
 */
pep_attribute_t * pep_resource_getattribute(const pep_resource_t * resource, int index);

/**
 * Deletes the XACML Resource. The XACML Attributes contained in the Resource will be deleted.
 * @param resource pointer to the XACML Resource
 */
void pep_resource_delete(pep_resource_t * resource);


/**
 * PEP XACML Action type.
 * @anchor Action
 */
typedef struct pep_action pep_action_t;

/**
 * Creates a XACML Action.
 * @return pep_action_t * pointer to the new XACML Action or @a NULL on error.
 */
pep_action_t * pep_action_create(void);

/**
 * Adds a XACML Attribute to the XACML Action
 * @param action pointer to the XACML Action
 * @param attr pointer to the XACML Attribute to add
 * @return int {@link #PEP_MODEL_OK} or {@link #PEP_MODEL_ERROR} on error.
 */
int pep_action_addattribute(pep_action_t * action, pep_attribute_t * attr);

/**
 * Returns the number of XACML Attribute contained in the XACML Action.
 * @param action pointer to the XACML Action
 * @return size_t number of XACML Attribute [0..n] contained in the Action or {@link #PEP_MODEL_ERROR} on error.
 */
size_t pep_action_attributes_length(const pep_action_t * action);

/**
 * Gets the XACML Attribute from the XACML Action at the given index.
 * @param action pointer to the XACML Action
 * @param index of the XACML Attribute to get in range [0..length-1].
 * @return pep_attribute_t * pointer to the XACML Attribute or @a NULL if index is out of range.
 * @see pep_action_attributes_length(const pep_action_t * action) to get the index range.
 */
pep_attribute_t * pep_action_getattribute(const pep_action_t * action, int index);

/**
 * Deletes the XACML Action. The XACML Attributes contained in the Action will be deleted.
 * @param action pointer to the XACML Action to delete
 */
void pep_action_delete(pep_action_t * action);


/**
 * PEP XACML Environment type.
 * @anchor Environment
 */
typedef struct pep_environment pep_environment_t;

/**
 * Creates a XACML Environment.
 * @return pep_environment_t * pointer to the new XACML Environment or @a NULL on error.
 */
pep_environment_t * pep_environment_create(void);

/**
 * Adds a XACML Attribute to the XACML Environment
 * @param env pointer to the XACML Environment
 * @param attr pointer to the XACML Attribute to add
 * @return int {@link #PEP_MODEL_OK} or {@link #PEP_MODEL_ERROR} on error.
 */
int pep_environment_addattribute(pep_environment_t * env, pep_attribute_t * attr);

/**
 * Returns the number of XACML Attribute contained in the XACML Environment.
 * @param env pointer to the XACML Environment
 * @return size_t number of XACML Attribute [0..n] contained in the Environment or {@link #PEP_MODEL_ERROR} on error.
 */
size_t pep_environment_attributes_length(const pep_environment_t * env);

/**
 * Gets the XACML Attribute from the XACML Environment at the given index.
 * @param env pointer to the XACML Environment
 * @param index of the XACML Attribute to get in range [0..length-1].
 * @return pep_attribute_t * pointer to the XACML Attribute or @a NULL if index is out of range.
 * @see pep_environment_attributes_length(const pep_environment_t * env) to get the index range.
 */
pep_attribute_t * pep_environment_getattribute(const pep_environment_t * env, int index);

/**
 * Deletes the XACML Environment. The XACML Attributes contained in the Environment will be deleted.
 * @param env pointer to the XACML Environment to delete
 */
void pep_environment_delete(pep_environment_t * env);


/**
 * PEP XACML Request type.
 * @anchor Request
 */
typedef struct pep_request pep_request_t;

/**
 * Creates a XACML Request.
 * @return pep_request_t * pointer to the new XACML Request or @a NULL on error.
 */
pep_request_t * pep_request_create(void);

/**
 * Adds a XACML Subject to the XACML Request.
 * @param request pointer to the XACML Request
 * @param subject pointer to the XACML Subject to add
 * @return int {@link #PEP_MODEL_OK} or {@link #PEP_MODEL_ERROR} on error.
 */
int pep_request_addsubject(pep_request_t * request, pep_subject_t * subject);

/**
 * Returns the number of XACML Subject contained in the XACML Request.
 * @param request pointer to the XACML Request
 * @return size_t number of XACML Subject [0..n] contained in the Request or {@link #PEP_MODEL_ERROR} on error.
 */
size_t pep_request_subjects_length(const pep_request_t * request);

/**
 * Gets the XACML Subject of the XACML Request at the given index.
 * @param request pointer to the XACML Request
 * @param index of the XACML Subject to get in range [0..length-1]
 * @return pep_subject_t * pointer the XACML Subject or @a NULL if index is out of range
 * @see pep_request_subjects_length(const pep_request_t * request) to get the index range
 */
pep_subject_t * pep_request_getsubject(const pep_request_t * request, int index);

/**
 * Adds a XACML Resource to the XACML Request.
 * @param request pointer to the XACML Request
 * @param resource pointer to the XACML Resource to add
 * @return int {@link #PEP_MODEL_OK} or {@link #PEP_MODEL_ERROR} on error.
 */
int pep_request_addresource(pep_request_t * request, pep_resource_t * resource);

/**
 * Returns the number of XACML Resource contained in the XACML Request.
 * @param request pointer to the XACML Request
 * @return size_t number of XACML Resource [0..n] contained in the Request or {@link #PEP_MODEL_ERROR} on error.
 */
size_t pep_request_resources_length(const pep_request_t * request);

/**
 * Gets the XACML Resource of the XACML Request at the given index.
 * @param request pointer to the XACML Request
 * @param index of the XACML Resource to get in range [0..length-1]
 * @return pep_resource_t * pointer the XACML Resource or @a NULL if index is out of range
 * @see pep_request_resources_length(const pep_request_t * request) to get the index range
 */
pep_resource_t * pep_request_getresource(const pep_request_t * request, int index);

/**
 * Sets a XACML Action for the XACML Request.
 * @param request pointer to the XACML Request
 * @param action pointer to the XACML Action to set
 * @return int {@link #PEP_MODEL_OK} or {@link #PEP_MODEL_ERROR} on error.
 */
int pep_request_setaction(pep_request_t * request, pep_action_t * action);

/**
 * Gets a XACML Action of the XACML Request.
 * @param request pointer to the XACML Request
 * @return pep_action_t * pointer to the XACML Action or @a NULL if not present
 */
pep_action_t * pep_request_getaction(const pep_request_t * request);

/**
 * Sets a XACML Environment for the XACML Request.
 * @param request pointer to the XACML Request
 * @param env pointer to the XACML Environment to set
 * @return int {@link #PEP_MODEL_OK} or {@link #PEP_MODEL_ERROR} on error.
 */
int pep_request_setenvironment(pep_request_t * request, pep_environment_t * env);

/**
 * Gets a XACML Environment of the XACML Request.
 * @param request pointer to the XACML Request
 * @return pep_environment_t * pointer to the XACML Environment or @a NULL if not present
 */
pep_environment_t * pep_request_getenvironment(const pep_request_t * request);

/**
 * Deletes the XACML Request. Contained Subjects, Resources, Action and Environment will be recursively deleted.
 * @param request pointer to the XACML Request to delete
 */
void pep_request_delete(pep_request_t * request);


/**
 * PEP XACML StatusCode type.
 * @anchor StatusCode
 */
typedef struct pep_status_code pep_status_code_t;

/*
 * PEP StatusCode/\@Value constants (XACML 2.0, B.9)
 */
static const char PEP_STATUSCODE_OK[]= "urn:oasis:names:tc:xacml:1.0:status:ok"; /**< XACML StatusCode/\@Value constants (XACML 2.0, B.9) */
static const char PEP_STATUSCODE_MISSING_ATTRIBUTE[]= "urn:oasis:names:tc:xacml:1.0:status:missing-attribute"; /**< XACML StatusCode/\@Value constants (XACML 2.0, B.9) */
static const char PEP_STATUSCODE_SYNTAX_ERROR[]= "urn:oasis:names:tc:xacml:1.0:status:syntax-error"; /**< XACML StatusCode/\@Value constants (XACML 2.0, B.9) */
static const char PEP_STATUSCODE_PROCESSING_ERROR[]= "urn:oasis:names:tc:xacml:1.0:status:processing-error"; /**< XACML StatusCode/\@Value constants (XACML 2.0, B.9) */

/**
 * Creates a XACML StatusCode.
 * @param value the Status/\@Value attribute
 * @return pep_status_code_t * pointer to the new XACML StatusCode or @a NULL on error.
 */
pep_status_code_t * pep_status_code_create(const char * value);

/**
 * Sets the XACML StatusCode/\@Value attribute
 * @param  statuscode pointer the XACML StatusCode
 * @param value the StatusCode/\@Value attribute to set.
 * @return int {@link #PEP_MODEL_OK} or {@link #PEP_MODEL_ERROR} on error.
 * @see PEP_STATUSCODE_OK value constant
 * @see PEP_STATUSCODE_MISSING_ATTRIBUTE value constant
 * @see PEP_STATUSCODE_SYNTAX_ERROR value constant
 * @see PEP_STATUSCODE_PROCESSING_ERROR value constant
 */
int pep_status_code_setvalue(pep_status_code_t * statuscode, const char * value);

/**
 * Gets the XACML StatusCode/\@Value attribute
 * @param  statuscode pointer the XACML StatusCode
 * @return const char *  the StatusCode/\@Value attribute or @a NULL if not set.
 * @see PEP_STATUSCODE_OK value constant
 * @see PEP_STATUSCODE_MISSING_ATTRIBUTE value constant
 * @see PEP_STATUSCODE_SYNTAX_ERROR value constant
 * @see PEP_STATUSCODE_PROCESSING_ERROR value constant
 */
const char * pep_status_code_getvalue(const pep_status_code_t * statuscode);

/**
 * Gets the minor XACML StatusCode for this XACML StatusCode.
 * @param  statuscode pointer the XACML StatusCode
 * @return pep_status_code_t * pointer to the minor child XACML StatusCode or @a NULL if not present.
 */
pep_status_code_t * pep_status_code_getsubcode(const pep_status_code_t * statuscode);

/**
 * Sets the minor XACML StatusCode for this XACML StatusCode.
 * @param  statuscode pointer the XACML StatusCode
 * @param subcode pointer to the minor child XACML StatusCode to set.
 * @return int {@link #PEP_MODEL_OK} or {@link #PEP_MODEL_ERROR} on error.
 */
int pep_status_code_setsubcode(pep_status_code_t * statuscode, pep_status_code_t * subcode);

/**
 * Deletes the XACML StatusCode. Optional minor child StatusCode (subcode) is recursively deleted.
 * @param  statuscode pointer the XACML StatusCode
 */
void pep_status_code_delete(pep_status_code_t * statuscode);

/**
 * PEP XACML Status type.
 * @anchor Status
 */
typedef struct pep_status pep_status_t;

/**
 * Creates a XACML Status.
 * @param message the Status/StatusMessage element (string)
 * @return pep_status_t * pointer to the new XACML Status or @a NULL on error.
 */
pep_status_t * pep_status_create(const char * message);

/**
 * Sets the XACML Status/StatusMessage element (string)
 * @param status pointer to the XACML Status
 * @param message the StatusMessage to set.
 * @return int {@link #PEP_MODEL_OK} or {@link #PEP_MODEL_ERROR} on error.
 */
int pep_status_setmessage(pep_status_t * status, const char * message);

/**
 * Gets the XACML Status/StatusMessage element (string)
 * @param status pointer to the XACML Status
 * @return const char * the StatusMessage or @a NULL if not set.
 */
const char * pep_status_getmessage(const pep_status_t * status);

/**
 * Gets the XACML StatusCode for this XACML Status
 * @param status pointer to the XACML Status
 * @return pep_status_code_t * pointer to the XACML StatusCode or @a NULL if not set
 */
pep_status_code_t * pep_status_getcode(const pep_status_t * status);

/**
 * Sets the XACML StatusCode for this XACML Status
 * @param status pointer to the XACML Status
 * @param statuscode pointer to the XACML StatusCode to set
 * @return int {@link #PEP_MODEL_OK} or {@link #PEP_MODEL_ERROR} on error.
 */
int pep_status_setcode(pep_status_t * status, pep_status_code_t * statuscode);

/**
 * Deletes the XACML Status. The StatusCode contained in the Status is recursively deleted.
 * @param status pointer to the XACML Status
 */
void pep_status_delete(pep_status_t * status);

/**
 * PEP  XACML AttributeAssignment type.
 * @anchor AttributeAssignment
 */
typedef struct pep_attribute_assignment pep_attribute_assignment_t;

/**
 * Creates a XACML AttributeAssignment.
 * @param id the mandatory AttributeAssignment/\@AttributeId attribute
 * @return pep_attribute_assignment_t * pointer to the new XACML AttributeAssignment or @a NULL on error.
 */
pep_attribute_assignment_t * pep_attribute_assignment_create(const char * id);

/**
 * Sets the XACML AttributeAssignment/\@AttributeId attribute.
 * @param attr pointer to the XACML AttributeAssignment
 * @param id the AttributeAssignment/\@AttributeId attribute to set.
 * @return int {@link #PEP_MODEL_OK} or {@link #PEP_MODEL_ERROR} on error.
 */
int pep_attribute_assignment_setid(pep_attribute_assignment_t * attr, const char * id);

/**
 * Gets the XACML AttributeAssignment/\@AttributeId attribute.
 * @param attr pointer to the XACML AttributeAssignment
 * @return const char * the AttributeAssignment/\@AttributeId attribute or @a NULL if not set
 */
const char * pep_attribute_assignment_getid(const pep_attribute_assignment_t * attr);

/**
 * Returns the number of AttributeValue (string) for the XACML AttributeAssignment.
 * @param attr pointer to the XACML AttributeAssignment
 * @return size_t number of AttributeValue [0..n] for the AttributeAssignment or {@link #PEP_MODEL_ERROR} on error.
 */
size_t pep_attribute_assignment_values_length(const pep_attribute_assignment_t * attr);

/**
 * Gets the AttributeValue from the XACML AttributeAssignment at the given index.
 * @param attr pointer to the XACML AttributeAssignment
 * @param index of the AttributeValue to get in range [0..length-1]
 * @return const char * the AttributeValue or @a NULL if out of range.
 */
const char * pep_attribute_assignment_getvalue(const pep_attribute_assignment_t * attr,int index);

/**
 * Adds an AttributeValue to the XACML AttributeAssignment.
 * @param attr pointer to the XACML AttributeAssignment
 * @param value the AttributeValue to add.
 * @return int {@link #PEP_MODEL_OK} or {@link #PEP_MODEL_ERROR} on error.
 */
int pep_attribute_assignment_addvalue(pep_attribute_assignment_t * attr, const char *value);

/**
 * Deletes the XACML AttributeAssignment. The AttributeValues contained in the AttributeAssignment are also deleted.
 * @param attr pointer to the XACML AttributeAssignment
 */
void pep_attribute_assignment_delete(pep_attribute_assignment_t * attr);

/**
 * PEP XACML Obligation/\@FulfillOn attribute constants.
 */
typedef enum pep_fulfillon {
	PEP_FULFILLON_DENY = 0, /**< Fulfill the Obligation on Deny decision */
	PEP_FULFILLON_PERMIT /**< Fulfill the Obligation on Permit decision */
} pep_fulfillon_t;

/**
 * PEP XACML Obligation type.
 * @anchor Obligation
 */
typedef struct pep_obligation pep_obligation_t;

/**
 * Creates a XACML Obligation.
 * @param id the mandatory Obligation/\@ObligationId attribute
 * @return pep_obligation_t * pointer to the new XACML Obligation or @a NULL on error.
 */
pep_obligation_t * pep_obligation_create(const char * id);

/**
 * Sets the XACML Obligation/\@ObligationId attribute.
 * @param obligation pointer to the XACML Obligation
 * @param id the Obligation/\@ObligationId attribute
 * @return int {@link #PEP_MODEL_OK} or {@link #PEP_MODEL_ERROR} on error.
 */
int pep_obligation_setid(pep_obligation_t * obligation, const char * id);

/**
 * Gets the XACML Obligation/\@ObligationId attribute.
 * @param obligation pointer to the XACML Obligation
 * @return const char * the ObligationId attribute or @a NULL if not set.
 */
const char * pep_obligation_getid(const pep_obligation_t * obligation);

/**
 * Gets the XACML Obligation/\@FulfillOn attribute.
 * @param obligation pointer to the XACML Obligation
 * @return pep_fulfillon_t the FulfillOn attribute value.
 * @see pep_fulfillon for attribute values.
 */
pep_fulfillon_t pep_obligation_getfulfillon(const pep_obligation_t * obligation);

/**
 * Sets the XACML Obligation/\@FulfillOn attribute.
 * @param obligation pointer to the XACML Obligation
 * @param fulfillon the FulfillOn attribute to set.
 * @return int {@link #PEP_MODEL_OK} or {@link #PEP_MODEL_ERROR} on error.
 * @see pep_fulfillon for FulfillOn values.
 */
int pep_obligation_setfulfillon(pep_obligation_t * obligation, pep_fulfillon_t fulfillon);

/**
 * Adds a XACML AttributeAssignment to the XACML Obligation.
 * @param obligation pointer to the XACML Obligation
 * @param attr pointer to the XACML AttributeAssignment to add.
 * @return int {@link #PEP_MODEL_OK} or {@link #PEP_MODEL_ERROR} on error.
 */
int pep_obligation_addattributeassignment(pep_obligation_t * obligation, pep_attribute_assignment_t * attr);

/**
 * Returns the number of XACML AttributeAssignment in the XACML Obligation.
 * @param obligation pointer to the XACML Obligation
 * @return size_t number of XACML AttributeAssignment [0..n] contained in the Obligation or {@link #PEP_MODEL_ERROR} on error.
 * @see pep_obligation_getattributeassignment(const pep_obligation_t * obligation,int index) to get a XACML AttributeAssignment at a particular index.
 */
size_t pep_obligation_attributeassignments_length(const pep_obligation_t * obligation);

/**
 * Gets the XACML AttributeAssignment from the Obligation at the given index.
 * @param obligation pointer to the XACML Obligation
 * @param index of the XACML AttributeAssignment to get in range [0..length-1].
 * @return pep_attribute_assignment_t * pointer to the XACML AtttibuteAssignment or @a NULL if out of range.
 * @see pep_obligation_attributeassignments_length(const pep_obligation_t * obligation) to get the index range.
 */
pep_attribute_assignment_t * pep_obligation_getattributeassignment(const pep_obligation_t * obligation,int index);

/**
 * Deletes the XACML Obligation. The contained AttributeAssignments will be recusively deleted.
 * @param obligation pointer to the XACML Obligation
 */
void pep_obligation_delete(pep_obligation_t * obligation);

/**
 * PEP XACML Result/Decision element constants.
 */
typedef enum pep_decision {
	PEP_DECISION_DENY = 0, /**< Decision is Deny */
	PEP_DECISION_PERMIT, /**< Decision is Permit */
	PEP_DECISION_INDETERMINATE, /**< Decision is Indeterminate, the PEP was unable to evaluate the request */
	PEP_DECISION_NOT_APPLICABLE /**< Decision is NotApplicable, the PEP does not have any policy that applies to the request */
} pep_decision_t;

/**
 * PEP XACML Result type.
 * @anchor Result
 */
typedef struct pep_result pep_result_t;

/**
 * Creates a XACML Result.
 * @return pep_result_t * pointer to the new XACML Result or @a NULL on error.
 */
pep_result_t * pep_result_create(void);

/**
 * Gets the XACML Result/Decision value.
 * @param result pointer to the XACML Result
 * @return pep_decision_t the XACML Result/Decision.
 * @see pep_decision for valid Decision constants.
 */
pep_decision_t pep_result_getdecision(const pep_result_t * result);

/**
 * Sets the XACML Result/Decision value.
 * @param result pointer to the XACML Result
 * @param decision the XACML Result/Decision to set.
 * @return int {@link #PEP_MODEL_OK} or {@link #PEP_MODEL_ERROR} on error.
 * @see pep_decision for valid Decision constants.
 */
int pep_result_setdecision(pep_result_t * result, pep_decision_t decision);

/**
 * Gets the XACML Result/\@ResourceId attribute.
 * @param result pointer to the XACML Result
 * @return const char * the ResourceId attribute or @a NULL if not set.
 */
const char * pep_result_getresourceid(const pep_result_t * result);

/**
 * Sets the XACML Result/\@ResourceId attribute.
 * @param result pointer to the XACML Result
 * @param resourceid the ResourceId attribute to set
 * @return int {@link #PEP_MODEL_OK} or {@link #PEP_MODEL_ERROR} on error.
 */
int pep_result_setresourceid(pep_result_t * result, const char * resourceid);

/**
 * Gets the XACML Status from Result.
 * @param result pointer to the XACML Result
 * @return pep_status_t * pointer to the XACML Status or @a NULL if not set.
 * @see pep_status_t XACML Status
 */
pep_status_t * pep_result_getstatus(const pep_result_t * result);

/**
 * Sets the XACML Status in the XACML Result.
 * @param result pointer to the XACML Result
 * @param status pointer to the XACML Status
 * @return int {@link #PEP_MODEL_OK} or {@link #PEP_MODEL_ERROR} on error.
 * @see pep_status_t XACML Status
 */
int pep_result_setstatus(pep_result_t * result, pep_status_t * status);

/**
 * Adds a XACML Obligation to the XACML Result.
 * @param result pointer to the XACML Result
 * @param obligation pointer to the XACML Obligation to add
 * @return int {@link #PEP_MODEL_OK} or {@link #PEP_MODEL_ERROR} on error.
 */
int pep_result_addobligation(pep_result_t * result, pep_obligation_t * obligation);

/**
 * Returns the number of XACML Obligation in the XACML Result.
 * @param result pointer to the XACML Result
 * @return size_t number of XACML Obligation [0..n] contained in the Result or {@link #PEP_MODEL_ERROR} on error.
 * @see pep_result_getobligation(const pep_result_t * result, int index) to get a XACML Obligation at a particular index.
 */
size_t pep_result_obligations_length(const pep_result_t * result);

/**
 * Gets the XACML Obligation from the XACML Result at the given index.
 * @param result pointer to the XACML Result
 * @param index of the XACML Obligation to get in range [0..length-1]
 * @see pep_result_obligations_length(const pep_result_t * result) to get the index range.
 */
pep_obligation_t * pep_result_getobligation(const pep_result_t * result, int index);

/**
 * Deletes the XACML Result. The contained Obligations will be recursively deleted.
 * @param result pointer to the XACML Result
 */
void pep_result_delete(pep_result_t * result);

/**
 * PEP XACML Response type.
 * @anchor Response
 */
typedef struct pep_response pep_response_t;

/**
 * Creates a XACML Response.
 * @return  pep_response_t * pointer to the new XACML Response or @a NULL on error.
 */
pep_response_t * pep_response_create(void);

/** @internal
 * Sets the XACML Request associated to the XACML Response.
 * @param response pointer to the XACML Response
 * @param request pointer to the XACML Request to associate.
 * @return int {@link #PEP_MODEL_OK} or {@link #PEP_MODEL_ERROR} on error.
 */
int pep_response_setrequest(pep_response_t * response, pep_request_t * request);

/** @internal
 * Gets the XACML Request associated to the XACML Response.
 * @param response pointer to the XACML Response
 * @return pep_request_t * pointer to the associated XACML Request or @a NULL if no Request is associated with the Response.
 */
pep_request_t * pep_response_getrequest(const pep_response_t * response);

/**
 * Adds a XACML Result associated to the XACML Response.
 * @param response pointer to the XACML Response
 * @param result pointer to the XACML Result to add.
 * @return int {@link #PEP_MODEL_OK} or {@link #PEP_MODEL_ERROR} on error.
 */
int pep_response_addresult(pep_response_t * response, pep_result_t * result);

/**
 * Returns the number of XACML Result in the XACML Response.
 * @param response pointer to the XACML Response
 * @return size_t number of XACML Result [0..n] contained in the Response or {@link #PEP_MODEL_ERROR} on error.
 * @see pep_response_getresult(const pep_response_t * response, int index) to get a XACML Result at a particular index.
 */
size_t pep_response_results_length(const pep_response_t * response);

/**
 * Gets the XACML Result from the XACML Response at the given index.
 * @param response pointer to the XACML Response
 * @param index of the XACML Result to get in range [0..length-1]
 * @see pep_response_results_length(const pep_response_t * response) to get the index range.
 */
pep_result_t * pep_response_getresult(const pep_response_t * response, int index);

/**
 * Deletes the XACML Response. The elements contained in the Response will be recursively deleted.
 * @param response pointer to the XACML Response
 */
void pep_response_delete(pep_response_t * response);

/** @} */

#ifdef  __cplusplus
}
#endif

#endif // _PEP_MODEL_H_
