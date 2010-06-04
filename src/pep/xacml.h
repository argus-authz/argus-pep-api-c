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

/* * $Id$
 * $Name:  $
 * @author Valery Tschopp <valery.tschopp@switch.ch>
 */
#ifndef _PEP_XACML_H_
#define _PEP_XACML_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include <stddef.h> /* size_t */

/** @defgroup XACML XACML Objects Model
 *
 * The XACML object model (Request, Response, ...) used by the PEP client.
 *
 * TODO: add object schema with the \@image tag
 *
 * @{
 */

/*
 * PEP XACML model functions return codes
 */
#define PEP_XACML_OK     1 /**< PEP XACML model functions return code OK */
/* WARN: PEP_XACML_ERROR should be size_t (unsigned int) compatible! */
#define PEP_XACML_ERROR  0 /**< PEP XACML model functions return code ERROR */


/*
 * XACML Data-types identifiers (XACML 2.0, Appendix B.3)
 */
static const char XACML_DATATYPE_X500NAME[]= "urn:oasis:names:tc:xacml:1.0:data-type:x500Name"; /**<  XACML data-type @c x500Name identifier (XACML 2.0, B.3) */
static const char XACML_DATATYPE_RFC822NAME[]= "urn:oasis:names:tc:xacml:1.0:data-type:rfc822Name"; /**<  XACML data-type @c rfc822Name identifier (XACML 2.0, B.3) */
static const char XACML_DATATYPE_IPADDRESS[]= "urn:oasis:names:tc:xacml:1.0:data-type:ipAddress"; /**<  XACML data-type @c ipAddress identifier (XACML 2.0, B.3) */
static const char XACML_DATATYPE_DNSNAME[]= "urn:oasis:names:tc:xacml:1.0:data-type:dnsName"; /**<  XACML data-type @c dnsName identifier (XACML 2.0, B.3) */
static const char XACML_DATATYPE_STRING[]= "http://www.w3.org/2001/XMLSchema#string"; /**<  XACML data-type @c string identifier (XACML 2.0, B.3) */
static const char XACML_DATATYPE_BOOLEAN[]= "http://www.w3.org/2001/XMLSchema#boolean"; /**<  XACML data-type @c boolean identifier (XACML 2.0, B.3) */
static const char XACML_DATATYPE_INTEGER[]= "http://www.w3.org/2001/XMLSchema#integer"; /**<  XACML data-type @c integer identifier (XACML 2.0, B.3) */
static const char XACML_DATATYPE_DOUBLE[]= "http://www.w3.org/2001/XMLSchema#double"; /**<  XACML data-type @c double identifier (XACML 2.0, B.3) */
static const char XACML_DATATYPE_TIME[]= "http://www.w3.org/2001/XMLSchema#time"; /**<  XACML data-type @c time identifier (XACML 2.0, B.3) */
static const char XACML_DATATYPE_DATE[]= "http://www.w3.org/2001/XMLSchema#date"; /**<  XACML data-type @c date identifier (XACML 2.0, B.3) */
static const char XACML_DATATYPE_DATETIME[]= "http://www.w3.org/2001/XMLSchema#dateTime"; /**<  XACML data-type @c dateTime identifier (XACML 2.0, B.3) */
static const char XACML_DATATYPE_ANYURI[]= "http://www.w3.org/2001/XMLSchema#anyURI"; /**<  XACML data-type @c anyURI identifier (XACML 2.0, B.3) */
static const char XACML_DATATYPE_HEXBINARY[]= "http://www.w3.org/2001/XMLSchema#hexBinary"; /**<  XACML data-type @c hexBinary identifier (XACML 2.0, B.3) */
static const char XACML_DATATYPE_BASE64BINARY[]= "http://www.w3.org/2001/XMLSchema#base64Binary"; /**<  XACML data-type @c base64Binary identifier (XACML 2.0, B.3) */
static const char XACML_DATATYPE_DAY_TIME_DURATION[]= "http://www.w3.org/TR/2002/WD-xquery-operators-20020816#dayTimeDuration"; /**<  XACML data-type @c dayTimeDuration identifier (XACML 2.0, B.3) */
static const char XACML_DATATYPE_YEAR_MONTH_DURATION[]= "http://www.w3.org/TR/2002/WD-xquery-operators-20020816#yearMonthDuration"; /**<  XACML data-type @c yearMonthDuration identifier (XACML 2.0, B.3) */

/**
 * @anchor Attribute
 * PEP XACML Attribute type.
 */
typedef struct xacml_attribute xacml_attribute_t;

/**
 * Creates and initializes a XACML Attribute.
 * @param id the mandatory id attribute
 * @return xacml_attribute_t * pointer to the new Attribute or @a NULL on error.
 */
xacml_attribute_t * xacml_attribute_create(const char * id);

/**
 * Sets the id attribute of the XACML Attribute.
 * @param attr pointer to the XACML Attribute
 * @param id the id attribute
 * @return int {@link #PEP_XACML_OK} or {@link #PEP_XACML_ERROR} on error.
 */
int xacml_attribute_setid(xacml_attribute_t * attr, const char * id);

/**
 * Gets the id attribute of the XACML Attribute.
 * @param attr pointer to the XACML Attribute
 * @return const char * the id attribute or @a NULL
 */
const char * xacml_attribute_getid(const xacml_attribute_t * attr);

/**
 * Sets the datatype attribute of the XACML Attribute.
 * @param attr pointer to the XACML Attribute
 * @param datatype the datatype attribute (can be NULL)
 * @return int {@link #PEP_XACML_OK} or {@link #PEP_XACML_ERROR} on error.
 */
int xacml_attribute_setdatatype(xacml_attribute_t * attr, const char * datatype);

/**
 * Gets the datatype attribute of the XACML Attribute.
 * @param attr pointer to the XACML Attribute
 * @return const char * the datatype attribute or @a NULL
 */
const char * xacml_attribute_getdatatype(const xacml_attribute_t * attr);

/**
 * Sets the issuer attribute of the XACML Attribute.
 * @param attr pointer to the XACML Attribute
 * @param issuer the issuer attribute
 * @return int {@link #PEP_XACML_OK} or {@link #PEP_XACML_ERROR} on error.
 */
int xacml_attribute_setissuer(xacml_attribute_t * attr, const char * issuer);

/**
 * Gets the issuer attribute of the XACML Attribute.
 * @param attr pointer to the XACML Attribute
 * @return const char * the issuer attribute or @a NULL
 */
const char * xacml_attribute_getissuer(const xacml_attribute_t * attr);

/**
 * Adds a value element to the XACML Attribute.
 * @param attr pointer to the XACML Attribute
 * @param value the value (string) to add
 * @return int {@link #PEP_XACML_OK} or {@link #PEP_XACML_ERROR} on error.
 */
int xacml_attribute_addvalue(xacml_attribute_t * attr, const char *value);

/**
 * Returns the number of AttributeValue in the XACML Attribute.
 * @param attr pointer to the XACML Attribute
 * @return size_t number of AttributeValue [1..n] or @c 0.
 * @see xacml_attribute_getvalue(const xacml_attribute_t * attr,int index) to get an AttributeValue at a particular index.
 */
size_t xacml_attribute_values_length(const xacml_attribute_t * attr);

/**
 * Gets the AttributeValue of the XACML Attribute at index.
 * @param attr pointer to the XACML Attribute
 * @param value_idx index of the AttributeValue to get in range [0..length-1].
 * @return const char * the AttributeValue or @a NULL if index is out of range.
 * @see xacml_attribute_values_length(const xacml_attribute_t * attr) to get the index range.
 */
const char * xacml_attribute_getvalue(const xacml_attribute_t * attr,int value_idx);

/**
 * Deletes the XACML Attribute.
 * @param attr pointer to the XACML Attribute to delete
 * @note The XACML containers (@ref Subject, @ref Resource, @ref Action or @ref Environment) will delete the contained
 * Attributes when deleted.
 */
void xacml_attribute_delete(xacml_attribute_t * attr);

/**
 * Clone the XACML Attribute.
 * @param attr pointer to the XACML Attribute to clone
 * @return xacml_attribute_t * pointer to the new cloned Attribute or @a NULL on error.
 */
xacml_attribute_t * xacml_attribute_clone(const xacml_attribute_t * attr);

/**
 * @anchor Subject
 * PEP XACML Subject type.
 */
typedef struct xacml_subject xacml_subject_t;

/*
 * PEP XACML Subject/Attribute identifiers and Subject/\@SubjectCategory values (XACML 2.0, Appendix B)
 */
static const char XACML_SUBJECT_ID[]= "urn:oasis:names:tc:xacml:1.0:subject:subject-id"; /**<  XACML Subject/Attribute @c subject-id identifier (XACML 2.0, B.4) */
static const char XACML_SUBJECT_ID_QUALIFIER[]= "urn:oasis:names:tc:xacml:1.0:subject:subject-id-qualifier"; /**<  XACML Subject/Attribute @c subject-id-qualifier identifier (XACML 2.0, B.4) */
static const char XACML_SUBJECT_KEY_INFO[]= "urn:oasis:names:tc:xacml:1.0:subject:key-info"; /**<  XACML Subject/Attribute @c key-info identifier (XACML 2.0, B.4) */
static const char XACML_SUBJECT_CATEGORY_ACCESS[]= "urn:oasis:names:tc:xacml:1.0:subject-category:access-subject"; /**<  XACML Subject/\@SubjectCategory attribute @b access-subject value (XACML 2.0, B.2) */
static const char XACML_SUBJECT_CATEGORY_INTERMEDIARY[]= "urn:oasis:names:tc:xacml:1.0:subject-category:intermediary-subject"; /**<  XACML Subject/\@SubjectCategory  attribute @b intermediary-subject value  (XACML 2.0, B.2) */
static const char XACML_SUBJECT_CATEGORY_RECIPIENT[]= "urn:oasis:names:tc:xacml:1.0:subject-category:recipient-subject"; /**<  XACML Subject/\@SubjectCategory  attribute @b recipient-subject value (XACML 2.0, B.2) */
static const char XACML_SUBJECT_CATEGORY_CODEBASE[]= "urn:oasis:names:tc:xacml:1.0:subject-category:codebase"; /**<  XACML Subject/\@SubjectCategory  attribute @b codebase value (XACML 2.0, B.2) */
static const char XACML_SUBJECT_CATEGORY_REQUESTING_MACHINE[]= "urn:oasis:names:tc:xacml:1.0:subject-category:requesting-machine"; /**<  XACML Subject/\@SubjectCategory  attribute @b requesting-machine value (XACML 2.0, B.2) */

/**
 * Creates a XACML Subject.
 * @return xacml_subject_t * pointer to the new XACML Subject or @a NULL on error.
 */
xacml_subject_t * xacml_subject_create(void);

/**
 * Sets the XACML Subject/\@SubjectCategory attribute.
 * @param subject pointer to the XACML Subject
 * @param category the SubjectCategory attribute
 * @return int {@link #PEP_XACML_OK} or {@link #PEP_XACML_ERROR} on error.
 * @see #XACML_SUBJECT_CATEGORY_ACCESS constant
 * @see #XACML_SUBJECT_CATEGORY_INTERMEDIARY constant
 * @see #XACML_SUBJECT_CATEGORY_RECIPIENT constant
 * @see #XACML_SUBJECT_CATEGORY_CODEBASE constant
 * @see #XACML_SUBJECT_CATEGORY_REQUESTING_MACHINE constant
 */
int xacml_subject_setcategory(xacml_subject_t * subject, const char * category);

/**
 * Gets the XACML Subject/\@SubjectCategory attribute value.
 * @param subject pointer to the XACML Subject
 * @return const char * the SubjectCategory attribute value or @a NULL
 */
const char * xacml_subject_getcategory(const xacml_subject_t * subject);

/**
 * Adds a XACML Attribute to the Subject.
 * @param subject pointer to the XACML Subject
 * @param attr pointer to the XACML Attribute to add
 * @return int {@link #PEP_XACML_OK} or {@link #PEP_XACML_ERROR} on error.
 * @see @ref Attribute
 */
int xacml_subject_addattribute(xacml_subject_t * subject, xacml_attribute_t * attr);

/**
 * Returns the number of XACML Attribute in the XACML Subject.
 * @param subject pointer to the XACML Subject
 * @return size_t number of XACML Attribute [1..n] contained in the Subject or @c 0.
 * @see xacml_subject_getattribute(const xacml_subject_t * subject, int index) to get a XACML Attribute at a particular index.
 */
size_t xacml_subject_attributes_length(const xacml_subject_t * subject);

/**
 * Gets the XACML Attribute from the XACML Subject at index.
 * @param subject pointer to the XACML Subject
 * @param attr_idx index of the XACML Attribute to get in range [0..length-1].
 * @return xacml_attribute_t * pointer to the XACML Attribute or @a NULL if index is out of range.
 * @see xacml_subject_attributes_length(const xacml_subject_t * subject) to get the index range.
 */
xacml_attribute_t * xacml_subject_getattribute(const xacml_subject_t * subject, int attr_idx);

/**
 * Deletes the XACML Subject.
 * @param subject pointer to the XACML Subject
 * @note The XACML container (@ref Request) will delete the contained XACML
 * Subjects when deleted.
 */
void xacml_subject_delete(xacml_subject_t * subject);


/**
 * PEP XACML Resource type.
 * @anchor Resource
 */
typedef struct xacml_resource xacml_resource_t;

/*
 * XACML Resource/Attribute Identifiers (XACML 2.0, Appendix B)
 */
static const char XACML_RESOURCE_ID[]= "urn:oasis:names:tc:xacml:1.0:resource:resource-id"; /**<  XACML Resource/Attribute @b resource-id identifier (XACML 2.0, B.6) */

/**
 * Creates a XACML Resource.
 * @return xacml_resource_t * pointer to the new XACML Resource or @a NULL on error.
 */
xacml_resource_t * xacml_resource_create(void);

/**
 * Sets the XACML Resource/ResourceContent element as string.
 * @param resource pointer the XACML Resource
 * @param content the ResourceContent as string
 * @return int {@link #PEP_XACML_OK} or {@link #PEP_XACML_ERROR} on error.
 */
int xacml_resource_setcontent(xacml_resource_t * resource, const char * content);

/**
 * Gets the XACML Resource/ResourceContent element as string.
 * @param resource pointer the XACML Resource
 * @return const char * the ResourceContent as string or @a NULL if not set
 */
const char * xacml_resource_getcontent(const xacml_resource_t * resource);

/**
 * Adds a XACML Attribute to the XACML Resource
 * @param resource pointer to the XACML Resource
 * @param attr pointer to the XACML Attribute to add
 * @return int {@link #PEP_XACML_OK} or {@link #PEP_XACML_ERROR} on error.
 */
int xacml_resource_addattribute(xacml_resource_t * resource, xacml_attribute_t * attr);

/**
 * Returns the number of XACML Attribute contained in the XACML Resource.
 * @param resource pointer to the XACML Resource
 * @return size_t number of XACML Attribute [1..n] contained in the Resource or @c 0.
 */
size_t xacml_resource_attributes_length(const xacml_resource_t * resource);

/**
 * Gets the XACML Attribute from the XACML Resource at the given index.
 * @param resource pointer to the XACML Resource
 * @param attr_idx index of the XACML Attribute to get in range [0..length-1].
 * @return xacml_attribute_t * pointer to the XACML Attribute or @a NULL if index is out of range.
 * @see xacml_resource_attributes_length(const xacml_resource_t * resource) to get the index range.
 */
xacml_attribute_t * xacml_resource_getattribute(const xacml_resource_t * resource, int attr_idx);

/**
 * Deletes the XACML Resource. The XACML Attributes contained in the Resource will be deleted.
 * @param resource pointer to the XACML Resource
 */
void xacml_resource_delete(xacml_resource_t * resource);


/**
 * PEP XACML Action type.
 * @anchor Action
 */
typedef struct xacml_action xacml_action_t;

/*
 * XACML Action/Attribute Identifiers (XACML 2.0, Appendix B)
 */
static const char XACML_ACTION_ID[]= "urn:oasis:names:tc:xacml:1.0:action:action-id"; /**<  XACML Action/Attribute @b action-id identifier (XACML 2.0, B.7) */

/**
 * Creates a XACML Action.
 * @return xacml_action_t * pointer to the new XACML Action or @a NULL on error.
 */
xacml_action_t * xacml_action_create(void);

/**
 * Adds a XACML Attribute to the XACML Action
 * @param action pointer to the XACML Action
 * @param attr pointer to the XACML Attribute to add
 * @return int {@link #PEP_XACML_OK} or {@link #PEP_XACML_ERROR} on error.
 */
int xacml_action_addattribute(xacml_action_t * action, xacml_attribute_t * attr);

/**
 * Returns the number of XACML Attribute contained in the XACML Action.
 * @param action pointer to the XACML Action
 * @return size_t number of XACML Attribute [1..n] contained in the Action or @c 0.
 */
size_t xacml_action_attributes_length(const xacml_action_t * action);

/**
 * Gets the XACML Attribute from the XACML Action at the given index.
 * @param action pointer to the XACML Action
 * @param attr_idx index of the XACML Attribute to get in range [0..length-1].
 * @return xacml_attribute_t * pointer to the XACML Attribute or @a NULL if index is out of range.
 * @see xacml_action_attributes_length(const xacml_action_t * action) to get the index range.
 */
xacml_attribute_t * xacml_action_getattribute(const xacml_action_t * action, int attr_idx);

/**
 * Deletes the XACML Action. The XACML Attributes contained in the Action will be deleted.
 * @param action pointer to the XACML Action to delete
 */
void xacml_action_delete(xacml_action_t * action);


/**
 * PEP XACML Environment type.
 * @anchor Environment
 */
typedef struct xacml_environment xacml_environment_t;

/*
 * PEP XACML Environment/Attribute identifiers (XACML 2.0, Appendix B)
 */
static const char XACML_ENVIRONMENT_CURRENT_TIME[]= "urn:oasis:names:tc:xacml:1.0:environment:current-time"; /**<  XACML Environment/Attribute @c current-time identifier (XACML 2.0, B.8) */
static const char XACML_ENVIRONMENT_CURRENT_DATE[]= "urn:oasis:names:tc:xacml:1.0:environment:current-date"; /**<  XACML Environment/Attribute @c current-date identifier (XACML 2.0, B.8) */
static const char XACML_ENVIRONMENT_CURRENT_DATETIME[]= "urn:oasis:names:tc:xacml:1.0:environment:current-dateTime"; /**<  XACML Environment/Attribute @c current-dateTime identifier (XACML 2.0, B.8) */

/**
 * Creates a XACML Environment.
 * @return xacml_environment_t * pointer to the new XACML Environment or @a NULL on error.
 */
xacml_environment_t * xacml_environment_create(void);

/**
 * Adds a XACML Attribute to the XACML Environment
 * @param env pointer to the XACML Environment
 * @param attr pointer to the XACML Attribute to add
 * @return int {@link #PEP_XACML_OK} or {@link #PEP_XACML_ERROR} on error.
 */
int xacml_environment_addattribute(xacml_environment_t * env, xacml_attribute_t * attr);

/**
 * Returns the number of XACML Attribute contained in the XACML Environment.
 * @param env pointer to the XACML Environment
 * @return size_t number of XACML Attribute [1..n] contained in the Environment or @c 0.
 */
size_t xacml_environment_attributes_length(const xacml_environment_t * env);

/**
 * Gets the XACML Attribute from the XACML Environment at the given index.
 * @param env pointer to the XACML Environment
 * @param attr_idx index of the XACML Attribute to get in range [0..length-1].
 * @return xacml_attribute_t * pointer to the XACML Attribute or @a NULL if index is out of range.
 * @see xacml_environment_attributes_length(const xacml_environment_t * env) to get the index range.
 */
xacml_attribute_t * xacml_environment_getattribute(const xacml_environment_t * env, int attr_idx);

/**
 * Deletes the XACML Environment. The XACML Attributes contained in the Environment will be deleted.
 * @param env pointer to the XACML Environment to delete
 */
void xacml_environment_delete(xacml_environment_t * env);


/**
 * PEP XACML Request type.
 * @anchor Request
 */
typedef struct xacml_request xacml_request_t;

/**
 * Creates a XACML Request.
 * @return xacml_request_t * pointer to the new XACML Request or @a NULL on error.
 */
xacml_request_t * xacml_request_create(void);

/**
 * Adds a XACML Subject to the XACML Request.
 * @param request pointer to the XACML Request
 * @param subject pointer to the XACML Subject to add
 * @return int {@link #PEP_XACML_OK} or {@link #PEP_XACML_ERROR} on error.
 */
int xacml_request_addsubject(xacml_request_t * request, xacml_subject_t * subject);

/**
 * Returns the number of XACML Subject contained in the XACML Request.
 * @param request pointer to the XACML Request
 * @return size_t number of XACML Subject [1..n] contained in the Request or @c 0.
 */
size_t xacml_request_subjects_length(const xacml_request_t * request);

/**
 * Gets the XACML Subject of the XACML Request at the given index.
 * @param request pointer to the XACML Request
 * @param subject_idx index of the XACML Subject to get in range [0..length-1]
 * @return xacml_subject_t * pointer the XACML Subject or @a NULL if index is out of range
 * @see xacml_request_subjects_length(const xacml_request_t * request) to get the index range
 */
xacml_subject_t * xacml_request_getsubject(const xacml_request_t * request, int subject_idx);

/**
 * Adds a XACML Resource to the XACML Request.
 * @param request pointer to the XACML Request
 * @param resource pointer to the XACML Resource to add
 * @return int {@link #PEP_XACML_OK} or {@link #PEP_XACML_ERROR} on error.
 */
int xacml_request_addresource(xacml_request_t * request, xacml_resource_t * resource);

/**
 * Returns the number of XACML Resource contained in the XACML Request.
 * @param request pointer to the XACML Request
 * @return size_t number of XACML Resource [1..n] contained in the Request or @c 0.
 */
size_t xacml_request_resources_length(const xacml_request_t * request);

/**
 * Gets the XACML Resource of the XACML Request at the given index.
 * @param request pointer to the XACML Request
 * @param resource_idx index of the XACML Resource to get in range [0..length-1]
 * @return xacml_resource_t * pointer the XACML Resource or @a NULL if index is out of range
 * @see xacml_request_resources_length(const xacml_request_t * request) to get the index range
 */
xacml_resource_t * xacml_request_getresource(const xacml_request_t * request, int resource_idx);

/**
 * Sets a XACML Action for the XACML Request.
 * @param request pointer to the XACML Request
 * @param action pointer to the XACML Action to set
 * @return int {@link #PEP_XACML_OK} or {@link #PEP_XACML_ERROR} on error.
 */
int xacml_request_setaction(xacml_request_t * request, xacml_action_t * action);

/**
 * Gets a XACML Action of the XACML Request.
 * @param request pointer to the XACML Request
 * @return xacml_action_t * pointer to the XACML Action or @a NULL if not present
 */
xacml_action_t * xacml_request_getaction(const xacml_request_t * request);

/**
 * Sets a XACML Environment for the XACML Request.
 * @param request pointer to the XACML Request
 * @param env pointer to the XACML Environment to set
 * @return int {@link #PEP_XACML_OK} or {@link #PEP_XACML_ERROR} on error.
 */
int xacml_request_setenvironment(xacml_request_t * request, xacml_environment_t * env);

/**
 * Gets a XACML Environment of the XACML Request.
 * @param request pointer to the XACML Request
 * @return xacml_environment_t * pointer to the XACML Environment or @a NULL if not present
 */
xacml_environment_t * xacml_request_getenvironment(const xacml_request_t * request);

/**
 * Deletes the XACML Request. Contained Subjects, Resources, Action and Environment will be recursively deleted.
 * @param request pointer to the XACML Request to delete
 */
void xacml_request_delete(xacml_request_t * request);


/**
 * PEP XACML StatusCode type.
 * @anchor StatusCode
 */
typedef struct xacml_statuscode xacml_statuscode_t;

/*
 * PEP XACML StatusCode/\@Value values (XACML 2.0, B.9)
 */
static const char XACML_STATUSCODE_OK[]= "urn:oasis:names:tc:xacml:1.0:status:ok"; /**< XACML StatusCode/\@Value attribute @c ok value (XACML 2.0, B.9) */
static const char XACML_STATUSCODE_MISSINGATTRIBUTE[]= "urn:oasis:names:tc:xacml:1.0:status:missing-attribute"; /**< XACML StatusCode/\@Value attribute @c missing-attribute value (XACML 2.0, B.9) */
static const char XACML_STATUSCODE_SYNTAXERROR[]= "urn:oasis:names:tc:xacml:1.0:status:syntax-error"; /**< XACML StatusCode/\@Value attribute @c syntax-error value (XACML 2.0, B.9) */
static const char XACML_STATUSCODE_PROCESSINGERROR[]= "urn:oasis:names:tc:xacml:1.0:status:processing-error"; /**< XACML StatusCode/\@Value attribute @c processing-error value (XACML 2.0, B.9) */

/**
 * Creates a XACML StatusCode.
 * @param value the Status/\@Value attribute
 * @return xacml_statuscode_t * pointer to the new XACML StatusCode or @a NULL on error.
 */
xacml_statuscode_t * xacml_statuscode_create(const char * value);

/**
 * Sets the XACML StatusCode/\@Value attribute
 * @param  statuscode pointer the XACML StatusCode
 * @param value the StatusCode/\@Value attribute to set.
 * @return int {@link #PEP_XACML_OK} or {@link #PEP_XACML_ERROR} on error.
 * @see XACML_STATUSCODE_OK value constant
 * @see XACML_STATUSCODE_MISSINGATTRIBUTE value constant
 * @see XACML_STATUSCODE_SYNTAXERROR value constant
 * @see XACML_STATUSCODE_PROCESSINGERROR value constant
 */
int xacml_statuscode_setvalue(xacml_statuscode_t * statuscode, const char * value);

/**
 * Gets the XACML StatusCode/\@Value attribute
 * @param  statuscode pointer the XACML StatusCode
 * @return const char *  the StatusCode/\@Value attribute or @a NULL if not set.
 * @see XACML_STATUSCODE_OK value constant
 * @see XACML_STATUSCODE_MISSINGATTRIBUTE value constant
 * @see XACML_STATUSCODE_SYNTAXERROR value constant
 * @see XACML_STATUSCODE_PROCESSINGERROR value constant
 */
const char * xacml_statuscode_getvalue(const xacml_statuscode_t * statuscode);

/**
 * Gets the minor XACML StatusCode for this XACML StatusCode.
 * @param  statuscode pointer the XACML StatusCode
 * @return xacml_statuscode_t * pointer to the minor child XACML StatusCode or @a NULL if not present.
 */
xacml_statuscode_t * xacml_statuscode_getsubcode(const xacml_statuscode_t * statuscode);

/**
 * Sets the minor XACML StatusCode for this XACML StatusCode.
 * @param  statuscode pointer the XACML StatusCode
 * @param subcode pointer to the minor child XACML StatusCode to set.
 * @return int {@link #PEP_XACML_OK} or {@link #PEP_XACML_ERROR} on error.
 */
int xacml_statuscode_setsubcode(xacml_statuscode_t * statuscode, xacml_statuscode_t * subcode);

/**
 * Deletes the XACML StatusCode. Optional minor child StatusCode (subcode) is recursively deleted.
 * @param  statuscode pointer the XACML StatusCode
 */
void xacml_statuscode_delete(xacml_statuscode_t * statuscode);

/**
 * PEP XACML Status type.
 * @anchor Status
 */
typedef struct xacml_status xacml_status_t;

/**
 * Creates a XACML Status.
 * @param message the Status/StatusMessage element (string)
 * @return xacml_status_t * pointer to the new XACML Status or @a NULL on error.
 */
xacml_status_t * xacml_status_create(const char * message);

/**
 * Sets the XACML Status/StatusMessage element (string)
 * @param status pointer to the XACML Status
 * @param message the StatusMessage to set.
 * @return int {@link #PEP_XACML_OK} or {@link #PEP_XACML_ERROR} on error.
 */
int xacml_status_setmessage(xacml_status_t * status, const char * message);

/**
 * Gets the XACML Status/StatusMessage element (string)
 * @param status pointer to the XACML Status
 * @return const char * the StatusMessage or @a NULL if not set.
 */
const char * xacml_status_getmessage(const xacml_status_t * status);

/**
 * Gets the XACML StatusCode for this XACML Status
 * @param status pointer to the XACML Status
 * @return xacml_statuscode_t * pointer to the XACML StatusCode or @a NULL if not set
 */
xacml_statuscode_t * xacml_status_getcode(const xacml_status_t * status);

/**
 * Sets the XACML StatusCode for this XACML Status
 * @param status pointer to the XACML Status
 * @param statuscode pointer to the XACML StatusCode to set
 * @return int {@link #PEP_XACML_OK} or {@link #PEP_XACML_ERROR} on error.
 */
int xacml_status_setcode(xacml_status_t * status, xacml_statuscode_t * statuscode);

/**
 * Deletes the XACML Status. The StatusCode contained in the Status is recursively deleted.
 * @param status pointer to the XACML Status
 */
void xacml_status_delete(xacml_status_t * status);

/**
 * PEP  XACML AttributeAssignment type.
 * @anchor AttributeAssignment
 */
typedef struct xacml_attributeassignment xacml_attributeassignment_t;

/**
 * Creates a XACML AttributeAssignment.
 * @param id the mandatory AttributeAssignment/\@AttributeId attribute
 * @return xacml_attributeassignment_t * pointer to the new XACML AttributeAssignment or @a NULL on error.
 */
xacml_attributeassignment_t * xacml_attributeassignment_create(const char * id);

/**
 * Sets the XACML AttributeAssignment/\@AttributeId attribute.
 * @param attr pointer to the XACML AttributeAssignment
 * @param id the AttributeAssignment/\@AttributeId attribute to set.
 * @return int {@link #PEP_XACML_OK} or {@link #PEP_XACML_ERROR} on error.
 */
int xacml_attributeassignment_setid(xacml_attributeassignment_t * attr, const char * id);

/**
 * Gets the XACML AttributeAssignment/\@AttributeId attribute.
 * @param attr pointer to the XACML AttributeAssignment
 * @return const char * the AttributeAssignment/\@AttributeId attribute or @a NULL if not set
 */
const char * xacml_attributeassignment_getid(const xacml_attributeassignment_t * attr);

/**
 * Sets the XACML AttributeAssignment/\@DataType attribute.
 * @param attr pointer to the XACML AttributeAssignment
 * @param datatype the AttributeAssignment/\@DataType attribute to set (can be NULL).
 * @return int {@link #PEP_XACML_OK} or {@link #PEP_XACML_ERROR} on error.
 */
int xacml_attributeassignment_setdatatype(xacml_attributeassignment_t * attr, const char * datatype);

/**
 * Gets the XACML AttributeAssignment/\@DataType attribute.
 * @param attr pointer to the XACML AttributeAssignment
 * @return const char * the AttributeAssignment/\@DataType attribute or @a NULL if not set
 */
const char * xacml_attributeassignment_getdatatype(const xacml_attributeassignment_t * attr);

/**
 * @deprecated AttributeAssignment can only have one value, use xacml_attributeassignment_getvalue(const xacml_attributeassignment_t * attr) instead.
 *
 * Returns the number of values (string) for the XACML AttributeAssignment.
 * @param attr pointer to the XACML AttributeAssignment
 * @return size_t number of values (always @c 1) for the AttributeAssignment or @c 0 on error.
 */
size_t xacml_attributeassignment_values_length(const xacml_attributeassignment_t * attr);

/**
 * Gets the value from the XACML AttributeAssignment.
 * @param attr pointer to the XACML AttributeAssignment
 * @param ... optional index of the value (ignored, back compatibility)
 * @return const char * the value or @a NULL if no value is set or if an error occurs.
 */
const char * xacml_attributeassignment_getvalue(const xacml_attributeassignment_t * attr, ...);


/**
 * @deprecated AttributeAssignment can only have one value, use xacml_attributeassignment_setvalue(xacml_attributeassignment_t * attr, const char *value) instead.
 *
 * Adds a value to the XACML AttributeAssignment.
 * @param attr pointer to the XACML AttributeAssignment
 * @param value the AttributeValue to add.
 * @return int {@link #PEP_XACML_OK} or {@link #PEP_XACML_ERROR} on error.
 */
int xacml_attributeassignment_addvalue(xacml_attributeassignment_t * attr, const char *value);

/**
 * Sets the value to the XACML AttributeAssignment.
 * @param attr pointer to the XACML AttributeAssignment
 * @param value the value to set.
 * @return int {@link #PEP_XACML_OK} or {@link #PEP_XACML_ERROR} on error.
 */
int xacml_attributeassignment_setvalue(xacml_attributeassignment_t * attr, const char *value);

/**
 * Deletes the XACML AttributeAssignment. The AttributeValues contained in the AttributeAssignment are also deleted.
 * @param attr pointer to the XACML AttributeAssignment
 */
void xacml_attributeassignment_delete(xacml_attributeassignment_t * attr);

/**
 * PEP XACML Obligation/\@FulfillOn attribute constants.
 */
typedef enum xacml_fulfillon {
	XACML_FULFILLON_DENY = 0, /**< Fulfill the Obligation on @b Deny decision */
	XACML_FULFILLON_PERMIT /**< Fulfill the Obligation on @b Permit decision */
} xacml_fulfillon_t;

/**
 * PEP XACML Obligation type.
 * @anchor Obligation
 */
typedef struct xacml_obligation xacml_obligation_t;


/**
 * Creates a XACML Obligation.
 * @param id the mandatory Obligation/\@ObligationId attribute
 * @return xacml_obligation_t * pointer to the new XACML Obligation or @a NULL on error.
 */
xacml_obligation_t * xacml_obligation_create(const char * id);

/**
 * Sets the XACML Obligation/\@ObligationId attribute.
 * @param obligation pointer to the XACML Obligation
 * @param id the Obligation/\@ObligationId attribute
 * @return int {@link #PEP_XACML_OK} or {@link #PEP_XACML_ERROR} on error.
 */
int xacml_obligation_setid(xacml_obligation_t * obligation, const char * id);

/**
 * Gets the XACML Obligation/\@ObligationId attribute.
 * @param obligation pointer to the XACML Obligation
 * @return const char * the ObligationId attribute or @a NULL if not set.
 */
const char * xacml_obligation_getid(const xacml_obligation_t * obligation);

/**
 * Gets the XACML Obligation/\@FulfillOn attribute.
 * @param obligation pointer to the XACML Obligation
 * @return xacml_fulfillon_t the FulfillOn attribute value.
 * @see xacml_fulfillon for attribute values.
 */
xacml_fulfillon_t xacml_obligation_getfulfillon(const xacml_obligation_t * obligation);

/**
 * Sets the XACML Obligation/\@FulfillOn attribute.
 * @param obligation pointer to the XACML Obligation
 * @param fulfillon the FulfillOn attribute to set.
 * @return int {@link #PEP_XACML_OK} or {@link #PEP_XACML_ERROR} on error.
 * @see xacml_fulfillon for FulfillOn values.
 */
int xacml_obligation_setfulfillon(xacml_obligation_t * obligation, xacml_fulfillon_t fulfillon);

/**
 * Adds a XACML AttributeAssignment to the XACML Obligation.
 * @param obligation pointer to the XACML Obligation
 * @param attr pointer to the XACML AttributeAssignment to add.
 * @return int {@link #PEP_XACML_OK} or {@link #PEP_XACML_ERROR} on error.
 */
int xacml_obligation_addattributeassignment(xacml_obligation_t * obligation, xacml_attributeassignment_t * attr);

/**
 * Returns the number of XACML AttributeAssignment in the XACML Obligation.
 * @param obligation pointer to the XACML Obligation
 * @return size_t number of XACML AttributeAssignment [1..n] contained in the Obligation or @c 0.
 * @see xacml_obligation_getattributeassignment(const xacml_obligation_t * obligation,int index) to get a XACML AttributeAssignment at a particular index.
 */
size_t xacml_obligation_attributeassignments_length(const xacml_obligation_t * obligation);

/**
 * Gets the XACML AttributeAssignment from the Obligation at the given index.
 * @param obligation pointer to the XACML Obligation
 * @param attr_idx index of the XACML AttributeAssignment to get in range [0..length-1].
 * @return xacml_attributeassignment_t * pointer to the XACML AtttibuteAssignment or @a NULL if out of range.
 * @see xacml_obligation_attributeassignments_length(const xacml_obligation_t * obligation) to get the index range.
 */
xacml_attributeassignment_t * xacml_obligation_getattributeassignment(const xacml_obligation_t * obligation,int attr_idx);

/**
 * Deletes the XACML Obligation. The contained AttributeAssignments will be recusively deleted.
 * @param obligation pointer to the XACML Obligation
 */
void xacml_obligation_delete(xacml_obligation_t * obligation);

/**
 * PEP XACML Result/Decision element constants.
 */
typedef enum xacml_decision {
	XACML_DECISION_DENY = 0, /**< Decision is @b Deny */
	XACML_DECISION_PERMIT, /**< Decision is @b Permit */
	XACML_DECISION_INDETERMINATE, /**< Decision is @b Indeterminate, the PEP was unable to evaluate the request */
	XACML_DECISION_NOT_APPLICABLE /**< Decision is @b NotApplicable, the PEP does not have any policy that applies to the request */
} xacml_decision_t;

/**
 * PEP XACML Result type.
 * @anchor Result
 */
typedef struct xacml_result xacml_result_t;

/**
 * Creates a XACML Result.
 * @return xacml_result_t * pointer to the new XACML Result or @a NULL on error.
 */
xacml_result_t * xacml_result_create(void);

/**
 * Gets the XACML Result/Decision value.
 * @param result pointer to the XACML Result
 * @return xacml_decision_t the XACML Result/Decision.
 * @see xacml_decision for valid Decision constants.
 */
xacml_decision_t xacml_result_getdecision(const xacml_result_t * result);

/**
 * Sets the XACML Result/Decision value.
 * @param result pointer to the XACML Result
 * @param decision the XACML Result/Decision to set.
 * @return int {@link #PEP_XACML_OK} or {@link #PEP_XACML_ERROR} on error.
 * @see xacml_decision for valid Decision constants.
 */
int xacml_result_setdecision(xacml_result_t * result, xacml_decision_t decision);

/**
 * Gets the XACML Result/\@ResourceId attribute.
 * @param result pointer to the XACML Result
 * @return const char * the ResourceId attribute or @a NULL if not set.
 */
const char * xacml_result_getresourceid(const xacml_result_t * result);

/**
 * Sets the XACML Result/\@ResourceId attribute.
 * @param result pointer to the XACML Result
 * @param resourceid the ResourceId attribute to set
 * @return int {@link #PEP_XACML_OK} or {@link #PEP_XACML_ERROR} on error.
 */
int xacml_result_setresourceid(xacml_result_t * result, const char * resourceid);

/**
 * Gets the XACML Status from Result.
 * @param result pointer to the XACML Result
 * @return xacml_status_t * pointer to the XACML Status or @a NULL if not set.
 * @see xacml_status_t XACML Status
 */
xacml_status_t * xacml_result_getstatus(const xacml_result_t * result);

/**
 * Sets the XACML Status in the XACML Result.
 * @param result pointer to the XACML Result
 * @param status pointer to the XACML Status
 * @return int {@link #PEP_XACML_OK} or {@link #PEP_XACML_ERROR} on error.
 * @see xacml_status_t XACML Status
 */
int xacml_result_setstatus(xacml_result_t * result, xacml_status_t * status);

/**
 * Adds a XACML Obligation to the XACML Result.
 * @param result pointer to the XACML Result
 * @param obligation pointer to the XACML Obligation to add
 * @return int {@link #PEP_XACML_OK} or {@link #PEP_XACML_ERROR} on error.
 */
int xacml_result_addobligation(xacml_result_t * result, xacml_obligation_t * obligation);

/**
 * Returns the number of XACML Obligation in the XACML Result.
 * @param result pointer to the XACML Result
 * @return size_t number of XACML Obligation [1..n] contained in the Result or @c 0.
 * @see xacml_result_getobligation(const xacml_result_t * result, int index) to get a XACML Obligation at a particular index.
 */
size_t xacml_result_obligations_length(const xacml_result_t * result);

/**
 * Gets the XACML Obligation from the XACML Result at the given index.
 * @param result pointer to the XACML Result
 * @param obligation_idx index of the XACML Obligation to get in range [0..length-1]
 * @see xacml_result_obligations_length(const xacml_result_t * result) to get the index range.
 */
xacml_obligation_t * xacml_result_getobligation(const xacml_result_t * result, int obligation_idx);

/**
 * Deletes the XACML Result. The contained Obligations will be recursively deleted.
 * @param result pointer to the XACML Result
 */
void xacml_result_delete(xacml_result_t * result);

/**
 * PEP XACML Response type.
 * @anchor Response
 */
typedef struct xacml_response xacml_response_t;

/**
 * Creates a XACML Response.
 * @return  xacml_response_t * pointer to the new XACML Response or @a NULL on error.
 */
xacml_response_t * xacml_response_create(void);

/** @internal
 * Sets the effective XACML Request associated to the XACML Response. The effective
 * XACML Request is normally send back from the PEPd.
 * @param response pointer to the XACML Response
 * @param request pointer to the XACML Request to associate.
 * @return int {@link #PEP_XACML_OK} or {@link #PEP_XACML_ERROR} on error.
 */
int xacml_response_setrequest(xacml_response_t * response, xacml_request_t * request);

/** @internal
 * Gets the effective XACML Request associated to the XACML Response.
 * @param response pointer to the XACML Response
 * @return xacml_request_t * pointer to the associated XACML Request or @a NULL if no Request is associated with the Response.
 */
xacml_request_t * xacml_response_getrequest(const xacml_response_t * response);

/** @internal
 * Relinquish (surrender, waive) the effective XACML Request associated to the XACML Response.
 * The caller is then responsible to delete the request returned by the function.
 * @param response pointer to the XACML Response
 * @return xacml_request_t * pointer to the effective XACML Request or @a NULL if no Request is associated with the Response.
 */
xacml_request_t * xacml_response_relinquishrequest(xacml_response_t * response);

/**
 * Adds a XACML Result associated to the XACML Response.
 * @param response pointer to the XACML Response
 * @param result pointer to the XACML Result to add.
 * @return int {@link #PEP_XACML_OK} or {@link #PEP_XACML_ERROR} on error.
 */
int xacml_response_addresult(xacml_response_t * response, xacml_result_t * result);

/**
 * Returns the number of XACML Result in the XACML Response.
 * @param response pointer to the XACML Response
 * @return size_t number of XACML Result [1..n] contained in the Response or @c 0.
 * @see xacml_response_getresult(const xacml_response_t * response, int index) to get a XACML Result at a particular index.
 */
size_t xacml_response_results_length(const xacml_response_t * response);

/**
 * Gets the XACML Result from the XACML Response at the given index.
 * @param response pointer to the XACML Response
 * @param result_idx index of the XACML Result to get in range [0..length-1]
 * @see xacml_response_results_length(const xacml_response_t * response) to get the index range.
 */
xacml_result_t * xacml_response_getresult(const xacml_response_t * response, int result_idx);

/**
 * Deletes the XACML Response. The elements contained in the Response will be recursively deleted.
 * @param response pointer to the XACML Response
 */
void xacml_response_delete(xacml_response_t * response);

/** @} */

#ifdef  __cplusplus
}
#endif

#endif
