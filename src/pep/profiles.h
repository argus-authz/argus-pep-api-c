/*
 * Copyright 2008 Members of the EGEE Collaboration.
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
 */

/* * $Id: xacml.h 1299 2009-10-15 15:29:34Z vtschopp $
 * $Name:  $
 * @author Valery Tschopp <valery.tschopp@switch.ch>
 * @version 1.1
 */
#ifndef _PEP_PROFILES_H_
#define _PEP_PROFILES_H_

#ifdef  __cplusplus
extern "C" {
#endif

/** @defgroup AuthzInterop Authorization Interoperability Profile
 *  @ingroup Profiles
 *
 * The XACML Attribute and Obligation Profile for Authorization Interoperability in Grids (version 1.1)
 *
 * @{
 */

/*
 * XACML Authz Interop Subject/Attribute identifiers (XACML Authz Interop Profile 1.0)
 */
static const char XACML_AUTHZINTEROP_SUBJECT_X509_ID[]= "http://authz-interop.org/xacml/subject/subject-x509-id"; /**<  XACML AuthZ Interop Subject/Attribute @b subject-x509-id identifier (Datatype: string, OpenSSL format) */
static const char XACML_AUTHZINTEROP_SUBJECT_X509_ISSUER[]= "http://authz-interop.org/xacml/subject/subject-x509-issuer"; /**<  XACML AuthZ Interop Subject/Attribute @b subject-x509-issuer identifier (Datatype: string, OpenSSL format) */
static const char XACML_AUTHZINTEROP_SUBJECT_VO[]= "http://authz-interop.org/xacml/subject/vo"; /**<  XACML AuthZ Interop Subject/Attribute @b vo identifier (Datatype: string) */
static const char XACML_AUTHZINTEROP_SUBJECT_CERTCHAIN[]= "http://authz-interop.org/xacml/subject/cert-chain"; /**<  XACML AuthZ Interop Subject/Attribute @b cert-chain identifier (Datatype: base64Binary) */
static const char XACML_AUTHZINTEROP_SUBJECT_VOMS_FQAN[]= "http://authz-interop.org/xacml/subject/voms-fqan"; /**<  XACML AuthZ Interop Subject/Attribute @b voms-fqan identifier (Datatype: string) */
static const char XACML_AUTHZINTEROP_SUBJECT_VOMS_PRIMARY_FQAN[]= "http://authz-interop.org/xacml/subject/voms-primary-fqan"; /**<  XACML AuthZ Interop Subject/Attribute @b voms-primary-fqan identifier (Datatype: string) */

/*
 * XACML Authz Interop Obligation and Obligation/AttributeAssignment identifiers (XACML Authz Interop Profile 1.0)
 */
static const char XACML_AUTHZINTEROP_OBLIGATION_UIDGID[]= "http://authz-interop.org/xacml/obligation/uidgid"; /**<  XACML AuthZ Interop Obligation @b uidgid identifier (XACML Authz Interop: UID GID) */
static const char XACML_AUTHZINTEROP_OBLIGATION_SECONDARY_GIDS[]= "http://authz-interop.org/xacml/obligation/secondary-gids"; /**<  XACML AuthZ Interop Obligation @b secondary-gids identifier (XACML Authz Interop: Multiple Secondary GIDs) */
static const char XACML_AUTHZINTEROP_OBLIGATION_USERNAME[]= "http://authz-interop.org/xacml/obligation/username"; /**<  XACML AuthZ Interop Obligation @b username identifier (XACML Authz Interop: Username) */
static const char XACML_AUTHZINTEROP_OBLIGATION_AFS_TOKEN[]= "http://authz-interop.org/xacml/obligation/afs-token"; /**<  XACML AuthZ Interop Obligation @b afs-token identifier (XACML Authz Interop: AFS Token) */
static const char XACML_AUTHZINTEROP_OBLIGATION_ATTR_POSIX_UID[]= "http://authz-interop.org/xacml/attribute/posix-uid"; /**<  XACML AuthZ Interop Obligation/AttributeAssignment @b posix-uid identifier (C Datatype: string, must be converted to integer) */
static const char XACML_AUTHZINTEROP_OBLIGATION_ATTR_POSIX_GID[]= "http://authz-interop.org/xacml/attribute/posix-gid"; /**<  XACML AuthZ Interop Obligation/AttributeAssignment @b posix-gid identifier (C Datatype: string, must be converted to integer) */
static const char XACML_AUTHZINTEROP_OBLIGATION_ATTR_USERNAME[]= "http://authz-interop.org/xacml/attribute/username"; /**<  XACML AuthZ Interop Obligation/AttributeAssignment @b username identifier (Datatype: string) */
static const char XACML_AUTHZINTEROP_OBLIGATION_ATTR_AFS_TOKEN[]= "http://authz-interop.org/xacml/attribute/afs-token"; /**<  XACML AuthZ Interop Obligation/AttributeAssignment @b afs-token identifier (Datatype: base64Binary) */

/** @} */

#ifdef  __cplusplus
}
#endif

#endif // _PEP_PROFILES_H_
