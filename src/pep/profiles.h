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
 */
#ifndef _PEP_PROFILES_H_
#define _PEP_PROFILES_H_

#ifdef  __cplusplus
extern "C" {
#endif

/** @defgroup Profiles XACML Profiles
 *
 * XACML constants, PIPs and Obligation Handlers for the implemented XACML Profiles.
 */

#include "pep/xacml.h"
#include "pep/pip.h"
#include "pep/oh.h"

/** @defgroup AuthzInterop Authorization Interoperability Profile
 *  @ingroup Profiles
 *
 * XACML Attribute and Obligation Profile for Authorization Interoperability in Grids (Version 1.1)
 *
 * Document: https://edms.cern.ch/document/929867/2
 *
 * XACML Subject's Attribute identifiers, XACML Obligation and Obligation's AttributeAssignment
 * identifiers for the AuthZ Interop Profile
 * @{
 */

/*
 * XACML Authz Interop Subject/Attribute identifiers (XACML Authz Interop Profile 1.1)
 */
static const char XACML_AUTHZINTEROP_SUBJECT_X509_ID[]= "http://authz-interop.org/xacml/subject/subject-x509-id"; /**<  XACML AuthZ Interop Subject/Attribute @b subject-x509-id identifier (Datatype: string, OpenSSL format) */
static const char XACML_AUTHZINTEROP_SUBJECT_X509_ISSUER[]= "http://authz-interop.org/xacml/subject/subject-x509-issuer"; /**<  XACML AuthZ Interop Subject/Attribute @b subject-x509-issuer identifier (Datatype: string, OpenSSL format) */
static const char XACML_AUTHZINTEROP_SUBJECT_VO[]= "http://authz-interop.org/xacml/subject/vo"; /**<  XACML AuthZ Interop Subject/Attribute @b vo identifier (Datatype: string) */
static const char XACML_AUTHZINTEROP_SUBJECT_CERTCHAIN[]= "http://authz-interop.org/xacml/subject/cert-chain"; /**<  XACML AuthZ Interop Subject/Attribute @b cert-chain identifier (Datatype: base64Binary) */
static const char XACML_AUTHZINTEROP_SUBJECT_VOMS_FQAN[]= "http://authz-interop.org/xacml/subject/voms-fqan"; /**<  XACML AuthZ Interop Subject/Attribute @b voms-fqan identifier (Datatype: string) */
static const char XACML_AUTHZINTEROP_SUBJECT_VOMS_PRIMARY_FQAN[]= "http://authz-interop.org/xacml/subject/voms-primary-fqan"; /**<  XACML AuthZ Interop Subject/Attribute @b voms-primary-fqan identifier (Datatype: string) */

/*
 * XACML Authz Interop Obligation and Obligation/AttributeAssignment identifiers (XACML Authz Interop Profile 1.1)
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

/** @defgroup GridWNAuthZ Grid Worker Node Authorization Profile
 *  @ingroup Profiles
 *
 * XACML Grid Worker Node Authorization Profile (Version 1.0)
 *
 * TODO: link to profile document http://edms.cern.ch/file/NUMBER/FILENAME.pdf
 *
 * Profile version, XACML Attribute identifiers, XACML Obligation identifiers, and datatypes for the Grid WN AuthZ Profile.
 * @{
 */
/*
 * XACML Grid WN AuthZ Profile version
 */
static const char XACML_GRIDWN_PROFILE_VERSION[]= "http://glite.org/xacml/profile/grid-wn/1.0"; /**< XACML Grid WN AuthZ Profile version value [XACML Grid WN AuthZ 1.0, 3.1.1] */

/*
 * XACML Grid WN AuthZ Attribute identifiers
 */
static const char XACML_GRIDWN_ATTRIBUTE_PROFILE_ID[]= "http://glite.org/xacml/attribute/profile-id"; /**< XACML Grid WN AuthZ Environment/Attribute @b profile-id identifier. Datatype: string, see #XACML_DATATYPE_ANYURI [XACML Grid WN AuthZ 1.0, 3.1.1] */
static const char XACML_GRIDWN_ATTRIBUTE_SUBJECT_ISSUER[]= "http://glite.org/xacml/attribute/subject-issuer"; /**< XACML Grid WN AuthZ Subject/Attribute @b subject-issuer identifier. Datatype: string, see #XACML_DATATYPE_X500NAME [XACML Grid WN AuthZ 1.0, 3.2.2 and 4.2] */
static const char XACML_GRIDWN_ATTRIBUTE_VIRTUAL_ORGANIZATION[]= "http://glite.org/xacml/attribute/virtual-organization"; /**< XACML Grid WN AuthZ Subject/Attribute @b virutal-organization identifier. Datatype: string, see #XACML_DATATYPE_STRING [XACML Grid WN AuthZ 1.0, 3.2.3 and 4.3] */
static const char XACML_GRIDWN_ATTRIBUTE_FQAN[]= "http://glite.org/xacml/attribute/fqan"; /**< XACML Grid WN AuthZ Subject/Attribute @b fqan identifier. Datatype: string, see #XACML_GRIDWN_DATATYPE_FQAN [XACML Grid WN AuthZ 1.0, 3.2.4 and 4.4] */
static const char XACML_GRIDWN_ATTRIBUTE_FQAN_PRIMARY[]= "http://glite.org/xacml/attribute/fqan/primary"; /**< XACML Grid WN AuthZ Subject/Attribute @b fqan/primary identifier. Datatype: string, see #XACML_GRIDWN_DATATYPE_FQAN [XACML Grid WN AuthZ 1.0, 3.2.5] */
static const char XACML_GRIDWN_ATTRIBUTE_PILOT_JOB_CLASSIFIER[]= "http://glite.org/xacml/attribute/pilot-job-classifer"; /**< XACML Grid WN AuthZ Action/Attribute @b pilot-job-classifer identifier. Datatype: string, see #XACML_DATATYPE_STRING [XACML Grid WN AuthZ 1.0, 3.4.2] */
static const char XACML_GRIDWN_ATTRIBUTE_VOMS_ISSUER[]= "http://glite.org/xacml/attribute/voms-issuer"; /**< XACML Grid WN AuthZ Subject/Attribute @b voms-issuer identifier [XACML Grid WN AuthZ 1.0, 4.6.2] */
static const char XACML_GRIDWN_ATTRIBUTE_USER_ID[]= "http://glite.org/xacml/attribute/user-id"; /**< XACML Grid WN AuthZ Obligation/AttributeAssignment @b user-id identifier [XACML Grid WN AuthZ 1.0, 3.6.1] */
static const char XACML_GRIDWN_ATTRIBUTE_GROUP_ID[]= "http://glite.org/xacml/attribute/group-id"; /**< XACML Grid WN AuthZ Obligation/AttributeAssignment @b group-id identifier [XACML Grid WN AuthZ 1.0, 3.6.2] */
static const char XACML_GRIDWN_ATTRIBUTE_GROUP_ID_PRIMARY[]= "http://glite.org/xacml/attribute/group-id/primary"; /**< XACML Grid WN AuthZ Obligation/AttributeAssignment @b group-id/primary identifier [XACML Grid WN AuthZ 1.0, 3.6.3] */

/*
 * XACML Grid WN AuthZ Obligation identifiers
 */
static const char XACML_GRIDWN_OBLIGATION_LOCAL_ENVIRONMENT_MAP[]= "http://glite.org/xacml/obligation/local-environment-map"; /**< XACML Grid WN AuthZ Obligation @b local-environment-map identifier [XACML Grid WN AuthZ 1.0, 3.5.1] */
static const char XACML_GRIDWN_OBLIGATION_LOCAL_ENVIRONMENT_MAP_POSIX[]= "http://glite.org/xacml/obligation/local-environment-map/posix"; /**< XACML Grid WN AuthZ Obligation @b local-environment-map/posix identifier [XACML Grid WN AuthZ 1.0, 3.5.2] */

/*
 * XACML Grid WN AuthZ datatypes
 */
static const char XACML_GRIDWN_DATATYPE_FQAN[]= "http://glite.org/xacml/datatype/fqan"; /**< XACML Grid WN AuthZ @b fqan datatype [XACML Grid WN AuthZ 1.0, 3.7.1] */

/** @} */

/** @defgroup ProfilesAdapters PIP and Obligation Handler Profile Adapters
 *  @ingroup Profiles
 *
 *  PIPs and Obligation Handlers to adapt a XACML profile to another XACML profile.
 *
 *  See @ref PIP, @ref ObligationHandler and @ref Profiles
 *
 * @{
 */

/** AuthZ Interop Profile to Grid WN AuthZ Profile PIP adapter
 *
 * This PIP transforms the outgoing XACML request as follow:
 * -# The AuthZ Interop XACML Attribute @b "http://authz-interop.org/xacml/subject/cert-chain"
 *    is copied into a Grid WN AuthZ XACML Subject/Attribute @b "urn:oasis:names:tc:xacml:1.0:subject:key-info".
 * -# The AuthZ Interop XACML Attributes @b "http://authz-interop.org/xacml/subject/voms-fqan"
 *    and @b "http://authz-interop.org/xacml/subject/voms-primary-fqan" are copied into the Grid WN AuthZ XACML
 *    Subject/Attributes @b fqan/primary and @b fqan (see @ref XACML_GRIDWN_ATTRIBUTE_FQAN_PRIMARY and
 *    @ref XACML_GRIDWN_ATTRIBUTE_FQAN_PRIMARY).
 * -# The Grid WN AuthZ XACML Attribute @b profile-id is add to the XACML Environment
 *    (see @ref XACML_GRIDWN_ATTRIBUTE_PROFILE_ID and @ref XACML_GRIDWN_PROFILE_VERSION).
 *
 * The @c authzinterop2gridwn_adapter_pip->process function never failed and always return @c 0.
 *
 * You must register this PIP as the @b last PIP for the PEP-C client.
 * Example:
 * @code
 * // add your own PIPs to the PEP-C client
 * pep_addpip(your_pip_1);
 * ...
 * pep_addpip(your_pip_n);
 * // then add the AuthZ Interop Profile to Grid WN AuthZ Profile PIP adapter as last PIP
 * pep_addpip(authzinterop2gridwn_adapter_pip);
 * @endcode
 *
 * See @ref PIP and @ref Profiles for more information
 */
extern const pep_pip_t * authzinterop2gridwn_adapter_pip;

/** Grid WN AuthZ Profile to AuthZ Interop Profile ObligationHandler adapter
 *
 * This OH transforms the incoming XACML response as follow:
 * -# The AttributeAssignments from the Grid WN AuthZ XACML Obligation @b local-environment-map/posix will be
 *    resolved (see @ref XACML_GRIDWN_OBLIGATION_LOCAL_ENVIRONMENT_MAP_POSIX).
 *    The user POSIX uid, gid and secondary gids are locally resolved, based on the Grid WN AuthZ
 *    AttributeAssignments @b user-id (@ref XACML_GRIDWN_ATTRIBUTE_USER_ID) and @b group-id/primary
 *    (@ref XACML_GRIDWN_ATTRIBUTE_GROUP_ID_PRIMARY) and @b group-id (@ref XACML_GRIDWN_ATTRIBUTE_GROUP_ID_PRIMARY) of
 *    the Obligation.
 *    -# Creates the AuthZ Interop XACML Obligation @b "http://authz-interop.org/xacml/obligation/username"
 *       with the AttributeAssignment @b "http://authz-interop.org/xacml/attribute/username" (Datatype: string).
 *    -# Creates the AuthZ Interop XACML Obligation @b "http://authz-interop.org/xacml/obligation/uidgid"
 *       with the AttributeAssignments @b "http://authz-interop.org/xacml/attribute/posix-uid" (datatype: integer)
 *       and @b "http://authz-interop.org/xacml/attribute/posix-gid" (Dataype: integer).
 *    -# Creates the AuthZ Interop XACML Obligation @b "http://authz-interop.org/xacml/obligation/secondary-gids"
 *       with the AttributeAssignments @b "http://authz-interop.org/xacml/attribute/posix-gid" (datatype: integer)
 *
 * The @c gridwn2authzinterop_adapter_oh->process function never failed and always return @c 0.
 *
 * You must register this OH as the @b first OH for the PEP-C client.
 * Example:
 * @code
 * // add the Grid WN AuthZ Profile to AuthZ Interop Profile OH adapter as first OH
 * pep_addobligationhandler(gridwn2authzinterop_adapter_oh);
 * // then add your own OHs after
 * pep_addobligationhandler(your_oh_1);
 * ...
 * pep_addobligationhandler(your_oh_n);
 * @endcode
 *
 * See @ref ObligationHandler and @ref Profiles for more information
 */
extern const pep_obligationhandler_t * gridwn2authzinterop_adapter_oh;

/** @} */

#ifdef  __cplusplus
}
#endif

#endif // _PEP_PROFILES_H_
