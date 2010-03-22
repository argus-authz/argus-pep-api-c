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

#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>

#include "util/log.h"
#include "pep/profiles.h"

#ifndef NGROUPS_MAX
#define NGROUPS_MAX 128
#endif

#ifndef GETPW_R_SIZE_MAX
#define GETPW_R_SIZE_MAX 10000
#endif

#ifndef GETGR_R_SIZE_MAX
#define GETGR_R_SIZE_MAX 10000
#endif

/**
 * AuthZ Interop Profile to Grid WN AuthZ Profile PIP adapter
 */
static char AUTHZINTEROP_TO_GRIDWN_ADAPTER_ID[]= "AuthZInterop2GridWNAdapterPIP";

/**
 * Grid WN AuthZ Profile to AuthZ Interop Profile OH adapter
 */
static char GRIDWN_TO_AUTHZINTEROP_ADAPTER_ID[]= "GridWN2AuthZInteropAdapterOH";

/*
 * method prototypes for the static pep_pip_t and pep_obligationhandler_t structs
 */
static int empty_init(void);
static int empty_destroy(void);
static int authzinterop2gridwn_pip_process(xacml_request_t ** request);
static int gridwn2authzinterop_oh_process(xacml_request_t ** request,xacml_response_t ** response);

/*
 * static initialization of the PIP
 */
static const pep_pip_t _authzinterop2gridwn_pip= {
	AUTHZINTEROP_TO_GRIDWN_ADAPTER_ID,
	empty_init,
	authzinterop2gridwn_pip_process,
	empty_destroy
};
/* public variable defined in profiles.h */
const pep_pip_t * authzinterop2gridwn_adapter_pip= &_authzinterop2gridwn_pip;

/*
 * static init of the OH
 */
static const pep_obligationhandler_t _gridwn2authzinterop_oh= {
	GRIDWN_TO_AUTHZINTEROP_ADAPTER_ID,
	empty_init,
	gridwn2authzinterop_oh_process,
	empty_destroy

};
/* public variable defined in profiles.h */
const pep_obligationhandler_t * gridwn2authzinterop_adapter_oh= &_gridwn2authzinterop_oh;

/* Nothing to do */
static int empty_init(void) {
	return 0;
}

/* Nothing to do */
static int empty_destroy(void) {
	return 0;
}

/*
 * Creates a new Subject/Attribute XACML_SUBJECT_KEY_INFO based on the
 * XACML_AUTHZINTEROP_SUBJECT_CERTCHAIN.
 * Creates new Subject/Attributes XACML_GRIDWN_ATTRIBUTE_FQAN and
 * XACML_GRIDWN_ATTRIBUTE_FQAN_PRIMARY based on XACML_AUTHZINTEROP_SUBJECT_VOMS_FQAN and
 * XACML_AUTHZINTEROP_SUBJECT_VOMS_PRIMARY_FQAN.
 * Adds the XACML_GRIDWN_ATTRIBUTE_PROFILE_ID in the Environment if not already present.
 */
static int authzinterop2gridwn_pip_process(xacml_request_t ** request) {
	int i, j;
	size_t subjects_l= xacml_request_subjects_length(*request);
	for (i= 0; i<subjects_l; i++) {
		xacml_subject_t * subject= xacml_request_getsubject(*request,i);
		size_t subject_attrs_l= xacml_subject_attributes_length(subject);
		for(j= 0; j<subject_attrs_l; j++) {
			xacml_attribute_t * attr= xacml_subject_getattribute(subject,j);
			const char * attr_id= xacml_attribute_getid(attr);
			if (strncmp(XACML_AUTHZINTEROP_SUBJECT_CERTCHAIN,attr_id,strlen(XACML_AUTHZINTEROP_SUBJECT_CERTCHAIN)) == 0) {
				log_debug("%s: clone subject[%d].attribute[%d].id= %s",AUTHZINTEROP_TO_GRIDWN_ADAPTER_ID, i,j,attr_id);
				xacml_attribute_t * keyinfo= xacml_attribute_clone(attr);
				if (keyinfo!=NULL) {
					log_debug("%s: set cloned attribute id= %s",AUTHZINTEROP_TO_GRIDWN_ADAPTER_ID,XACML_SUBJECT_KEY_INFO);
					xacml_attribute_setid(keyinfo,XACML_SUBJECT_KEY_INFO);
					log_debug("%s: set cloned attribute datatype= %s",AUTHZINTEROP_TO_GRIDWN_ADAPTER_ID,XACML_DATATYPE_STRING);
					xacml_attribute_setdatatype(keyinfo,XACML_DATATYPE_STRING);
					if (xacml_subject_addattribute(subject,keyinfo) != PEP_XACML_OK) {
						log_error("%s: failed to add new attribute{%s} to subject[%d]",AUTHZINTEROP_TO_GRIDWN_ADAPTER_ID,XACML_SUBJECT_KEY_INFO,i);
						xacml_attribute_delete(keyinfo);
					}
				}
				else {
					log_warn("%s: failed to clone subject[%d].attribute[%d]",AUTHZINTEROP_TO_GRIDWN_ADAPTER_ID, i,j);
				}
			}
			else if (strncmp(XACML_AUTHZINTEROP_SUBJECT_VOMS_PRIMARY_FQAN,attr_id,strlen(XACML_AUTHZINTEROP_SUBJECT_VOMS_PRIMARY_FQAN))==0) {
				log_debug("%s: clone subject[%d].attribute[%d].id= %s",AUTHZINTEROP_TO_GRIDWN_ADAPTER_ID, i,j,attr_id);
				xacml_attribute_t * fqan_primary= xacml_attribute_clone(attr);
				if (fqan_primary!=NULL) {
					log_debug("%s: set cloned attribute id= %s",AUTHZINTEROP_TO_GRIDWN_ADAPTER_ID,XACML_GRIDWN_ATTRIBUTE_FQAN_PRIMARY);
					xacml_attribute_setid(fqan_primary,XACML_GRIDWN_ATTRIBUTE_FQAN_PRIMARY);
					log_debug("%s: set cloned attribute datatype= %s",AUTHZINTEROP_TO_GRIDWN_ADAPTER_ID,XACML_GRIDWN_DATATYPE_FQAN);
					xacml_attribute_setdatatype(fqan_primary,XACML_GRIDWN_DATATYPE_FQAN);
					if (xacml_subject_addattribute(subject,fqan_primary) != PEP_XACML_OK) {
						log_error("%s: failed to add new attribute{%s} to subject[%d]",AUTHZINTEROP_TO_GRIDWN_ADAPTER_ID,XACML_GRIDWN_ATTRIBUTE_FQAN_PRIMARY,i);
						xacml_attribute_delete(fqan_primary);
					}
				}
				else {
					log_warn("%s: failed to clone subject[%d].attribute[%d]",AUTHZINTEROP_TO_GRIDWN_ADAPTER_ID, i,j);
				}
			}
			else if (strncmp(XACML_AUTHZINTEROP_SUBJECT_VOMS_FQAN,attr_id,strlen(XACML_AUTHZINTEROP_SUBJECT_VOMS_FQAN))==0) {
				log_debug("%s: clone subject[%d].attribute[%d].id= %s",AUTHZINTEROP_TO_GRIDWN_ADAPTER_ID, i,j,attr_id);
				xacml_attribute_t * fqans= xacml_attribute_clone(attr);
				if (fqans!=NULL) {
					log_debug("%s: set cloned attribute id= %s",AUTHZINTEROP_TO_GRIDWN_ADAPTER_ID,XACML_GRIDWN_ATTRIBUTE_FQAN_PRIMARY);
					xacml_attribute_setid(fqans,XACML_GRIDWN_ATTRIBUTE_FQAN_PRIMARY);
					log_debug("%s: set cloned attribute datatype= %s",AUTHZINTEROP_TO_GRIDWN_ADAPTER_ID,XACML_GRIDWN_DATATYPE_FQAN);
					xacml_attribute_setdatatype(fqans,XACML_GRIDWN_DATATYPE_FQAN);
					if (xacml_subject_addattribute(subject,fqans) != PEP_XACML_OK) {
						log_error("%s: failed to add new attribute{%s} to subject[%d]",AUTHZINTEROP_TO_GRIDWN_ADAPTER_ID,XACML_GRIDWN_ATTRIBUTE_FQAN_PRIMARY,i);
						xacml_attribute_delete(fqans);
					}
				}
				else {
					log_warn("%s: failed to clone subject[%d].attribute[%d]",AUTHZINTEROP_TO_GRIDWN_ADAPTER_ID, i,j);
				}
			}
		}
	}
	// check environment for Grid WN AuthZ Profile ID, if not present add it
	xacml_environment_t * environment= xacml_request_getenvironment(*request);
	// create environment if not already existing
	if (environment==NULL) {
		environment= xacml_environment_create();
		if (environment!=NULL) {
			xacml_request_setenvironment(*request,environment);
		}
		else {
			log_warn("%s: failed to create XACML Environment",AUTHZINTEROP_TO_GRIDWN_ADAPTER_ID);
		}
	}
	size_t environment_attrs_l= xacml_environment_attributes_length(environment);
	int profile_id_present= 0;
	for (i= 0; i<environment_attrs_l; i++) {
		xacml_attribute_t * attr= xacml_environment_getattribute(environment,i);
		const char * attr_id= xacml_attribute_getid(attr);
		if (strncmp(XACML_GRIDWN_ATTRIBUTE_PROFILE_ID,attr_id,strlen(XACML_GRIDWN_ATTRIBUTE_PROFILE_ID))==0) {
			log_debug("%s: found environment.attribute[%d].id= %s",AUTHZINTEROP_TO_GRIDWN_ADAPTER_ID,i,attr_id);
			profile_id_present= 1;
		}
	}
	// profile id is not present, then add it
	if (!profile_id_present && environment) {
		// create env attribute
		xacml_attribute_t * env_attr= xacml_attribute_create(XACML_GRIDWN_ATTRIBUTE_PROFILE_ID);
		if (env_attr!=NULL) {
			xacml_attribute_setdatatype(env_attr,XACML_DATATYPE_ANYURI);
			xacml_attribute_addvalue(env_attr,XACML_GRIDWN_PROFILE_VERSION);
			xacml_environment_addattribute(environment,env_attr);
		}
		else {
			log_warn("%s: failed to create XACML Environment/Attribute{%s}",AUTHZINTEROP_TO_GRIDWN_ADAPTER_ID,XACML_GRIDWN_ATTRIBUTE_PROFILE_ID);
		}
	}
	return 0;
}

/*
 * prototypes required in the OH
 */
static xacml_obligation_t * create_username_obligation(xacml_fulfillon_t fulfillon, const char * username);
static xacml_obligation_t * create_uidgid_obligation(xacml_fulfillon_t fulfillon, uid_t uid, gid_t gid);
static xacml_obligation_t * create_secondarygids_obligation(xacml_fulfillon_t fulfillon, gid_t gids[], size_t gids_length);
static int resolve_group_gid(const char * groupname, gid_t * gr_gid);
static int resolve_user_uidgid(const char * username, uid_t * pw_uid, gid_t * pw_gid);


/*
 * Converts in the XACML response the local-environment-map/posix obligation to uidgid, secondary-gid and username obligations
 * Resolve uidgid and groups by calling POSIX getpwent and getgrent
 */
static int gridwn2authzinterop_oh_process(xacml_request_t ** request,xacml_response_t ** response) {
	int i, j, k, m;
	size_t results_l= xacml_response_results_length(*response);
	for (i= 0; i<results_l; i++) {
		xacml_result_t * result= xacml_response_getresult(*response,i);
		xacml_decision_t decision= xacml_result_getdecision(result);
		if (decision==XACML_DECISION_PERMIT) {
			size_t obligations_l= xacml_result_obligations_length(result);
			for (j= 0; j<obligations_l; j++) {
				xacml_obligation_t * obligation= xacml_result_getobligation(result,j);
				const char * obligation_id= xacml_obligation_getid(obligation);
				xacml_fulfillon_t obligation_fulfillon= xacml_obligation_getfulfillon(obligation);
				if (strncmp(XACML_GRIDWN_OBLIGATION_LOCAL_ENVIRONMENT_MAP_POSIX,obligation_id,strlen(XACML_GRIDWN_OBLIGATION_LOCAL_ENVIRONMENT_MAP_POSIX))==0) {
					// do local POSIX resolve for uid/gids
					log_debug("%s: resolve local POSIX account mapping",GRIDWN_TO_AUTHZINTEROP_ADAPTER_ID);
					const char * username= NULL;
					const char * groupname= NULL;
					size_t n_groupnames= 0;
					char ** groupnames= calloc(NGROUPS_MAX,sizeof(char *));
					size_t attrs_l= xacml_obligation_attributeassignments_length(obligation);
					for (k= 0; k<attrs_l; k++) {
						xacml_attributeassignment_t * attr= xacml_obligation_getattributeassignment(obligation,k);
						const char * attr_id= xacml_attributeassignment_getid(attr);
						const char * attr_value= xacml_attributeassignment_getvalue(attr);
						if (strcmp(XACML_GRIDWN_ATTRIBUTE_USER_ID,attr_id)==0) {
							username= attr_value;
						}
						else if (strcmp(XACML_GRIDWN_ATTRIBUTE_GROUP_ID_PRIMARY,attr_id)==0) {
							groupname= attr_value;
						}
						else if (strcmp(XACML_GRIDWN_ATTRIBUTE_GROUP_ID,attr_id)==0) {
							groupnames[n_groupnames++]= (char *)attr_value;
						}
					}

					// username obligation
					if (username) {
						xacml_obligation_t * username_obligation= create_username_obligation(obligation_fulfillon,username);
						if (username_obligation) {
							xacml_result_addobligation(result,username_obligation);
						}
					}
					// uidgid obligation
					if (username) {
						// resolve POSIX username and groupname id (uid and gid)
						// if only the username (without groupname), use the user default group
						uid_t user_uid;
						gid_t user_gid, group_gid;
						if (resolve_user_uidgid(username,&user_uid,&user_gid)==0) {
							uid_t obligation_uid= user_uid;
							gid_t obligation_gid= user_gid;
							if (groupname && resolve_group_gid(groupname, &group_gid)==0) {
								obligation_gid= group_gid;
							}
							xacml_obligation_t * uidgid_obligation= create_uidgid_obligation(obligation_fulfillon,obligation_uid,obligation_gid);
							if (uidgid_obligation) {
								xacml_result_addobligation(result,uidgid_obligation);
							}
						}
					}
					// secondary gids obligation
					if (n_groupnames>0) {
						// resolve POSIX secondary groupnames gids
						gid_t * gids= calloc(n_groupnames,sizeof(gid_t));
						int resolve_error= 0;
						for (m= 0; m<n_groupnames; m++) {
							if (resolve_group_gid(groupnames[m],&gids[m])!=0) {
								resolve_error= 1;
								break;
							}
						}
						if (!resolve_error) {
							xacml_obligation_t * secgids_obligation= create_secondarygids_obligation(obligation_fulfillon,gids,n_groupnames);
							if (secgids_obligation) {
								xacml_result_addobligation(result,secgids_obligation);
							}
						}
						free(gids);
					}
					free(groupnames);
				}
			}
		}
	}
	return 0;
}

/*
 * return NULL on error
 */
static xacml_obligation_t * create_username_obligation(xacml_fulfillon_t fulfillon, const char * username) {
	xacml_obligation_t * username_obligation= xacml_obligation_create(XACML_AUTHZINTEROP_OBLIGATION_USERNAME);
	if (username_obligation==NULL) {
		log_error("failed to create Obligation{%s}",XACML_AUTHZINTEROP_OBLIGATION_USERNAME);
		return NULL;
	}
	xacml_obligation_setfulfillon(username_obligation,fulfillon);

	xacml_attributeassignment_t * username_attr= xacml_attributeassignment_create(XACML_AUTHZINTEROP_OBLIGATION_ATTR_USERNAME);
	if (username_attr==NULL) {
		log_error("failed to create Obligation/AttributeAssignment{%s}",XACML_AUTHZINTEROP_OBLIGATION_ATTR_USERNAME);
		xacml_obligation_delete(username_obligation);
		return NULL;
	}
	xacml_attributeassignment_setvalue(username_attr,username);
	xacml_attributeassignment_setdatatype(username_attr,XACML_DATATYPE_STRING);
	xacml_obligation_addattributeassignment(username_obligation,username_attr);
	return username_obligation;
}

/*
 * return NULL on error
 */
static xacml_obligation_t * create_uidgid_obligation(xacml_fulfillon_t fulfillon, uid_t uid, gid_t gid) {
	xacml_obligation_t * uidgid_obligation= xacml_obligation_create(XACML_AUTHZINTEROP_OBLIGATION_UIDGID);
	if (uidgid_obligation==NULL) {
		log_error("failed to create Obligation{%s}",XACML_AUTHZINTEROP_OBLIGATION_UIDGID);
		return NULL;
	}
	xacml_obligation_setfulfillon(uidgid_obligation,fulfillon);

	char value[1024];
	xacml_attributeassignment_t * uid_attr= xacml_attributeassignment_create(XACML_AUTHZINTEROP_OBLIGATION_ATTR_POSIX_UID);
	if (uid_attr==NULL) {
		log_error("failed to create Obligation/AttributeAssignment{%s}",XACML_AUTHZINTEROP_OBLIGATION_ATTR_POSIX_UID);
		xacml_obligation_delete(uidgid_obligation);
		return NULL;
	}
	snprintf(value,1024,"%u",uid);
	xacml_attributeassignment_setvalue(uid_attr,value);
	xacml_attributeassignment_setdatatype(uid_attr,XACML_DATATYPE_INTEGER);
	xacml_obligation_addattributeassignment(uidgid_obligation,uid_attr);

	xacml_attributeassignment_t * gid_attr= xacml_attributeassignment_create(XACML_AUTHZINTEROP_OBLIGATION_ATTR_POSIX_GID);
	if (gid_attr==NULL) {
		log_error("failed to create Obligation/AttributeAssignment{%s}",XACML_AUTHZINTEROP_OBLIGATION_ATTR_POSIX_GID);
		xacml_obligation_delete(uidgid_obligation);
		return NULL;
	}
	snprintf(value,1024,"%u",gid);
	xacml_attributeassignment_setvalue(gid_attr,value);
	xacml_attributeassignment_setdatatype(gid_attr,XACML_DATATYPE_INTEGER);
	xacml_obligation_addattributeassignment(uidgid_obligation,gid_attr);

	return uidgid_obligation;
}

/*
 * return NULL on error
 */
static xacml_obligation_t * create_secondarygids_obligation(xacml_fulfillon_t fulfillon, gid_t gids[], size_t gids_length) {
	int i;
	xacml_obligation_t * gids_obligation= xacml_obligation_create(XACML_AUTHZINTEROP_OBLIGATION_SECONDARY_GIDS);
	if (gids_obligation==NULL) {
		log_error("failed to create Obligation{%s}",XACML_AUTHZINTEROP_OBLIGATION_SECONDARY_GIDS);
		return NULL;
	}
	xacml_obligation_setfulfillon(gids_obligation,fulfillon);

	char value[1024];
	for (i= 0; i<gids_length; i++) {
		xacml_attributeassignment_t * gid_attr= xacml_attributeassignment_create(XACML_AUTHZINTEROP_OBLIGATION_ATTR_POSIX_GID);
		if (gid_attr==NULL) {
			log_error("failed to create Obligation/AttributeAssignment{%s}",XACML_AUTHZINTEROP_OBLIGATION_ATTR_POSIX_GID);
			xacml_obligation_delete(gids_obligation);
			return NULL;
		}
		snprintf(value,1024,"%u",gids[i]);
		xacml_attributeassignment_setvalue(gid_attr,value);
		xacml_attributeassignment_setdatatype(gid_attr,XACML_DATATYPE_INTEGER);
		xacml_obligation_addattributeassignment(gids_obligation,gid_attr);
	}

	return gids_obligation;
}

/*
 * resolve the POSIX gid for the groupname
 * return 0 on success
 */
static int resolve_group_gid(const char * groupname, gid_t * gid) {
	if (groupname==NULL) {
		log_warn("resolve_group_gid: groupname is NULL");
		return -1;
	}
	struct group gr;
	struct group *result;
	char buf[GETGR_R_SIZE_MAX];
	size_t bufsize= GETGR_R_SIZE_MAX;
	log_debug("resolve_group_gid for %s",groupname);
	int rc= getgrnam_r(groupname,&gr,buf,bufsize,&result);
	if (rc==0 && result!=NULL) {
		*gid= gr.gr_gid;
		log_debug("resolve_group_gid: gid=%d",gr.gr_gid);
		return 0;
	}
	else {
		log_error("resolve_group_gid: failed to resolve POSIX gid for %s: %s",groupname,strerror(errno));
		return -2;
	}
}

/*
 * resolve the POSIX uid and gid for the username
 * return 0 on success
 */
static int resolve_user_uidgid(const char * username, uid_t * uid, gid_t * gid) {
	if (username==NULL) {
		log_warn("resolve_user_uidgid: username is NULL");
		return -1;
	}
	struct passwd pw;
	struct passwd *result;
	char buf[GETPW_R_SIZE_MAX];
	size_t bufsize= GETPW_R_SIZE_MAX;
	log_debug("resolve_user_uidgid for %s",username);
	int rc= getpwnam_r(username,&pw,buf,bufsize,&result);
	if (rc==0 && result!=NULL) {
		*uid= pw.pw_uid;
		*gid= pw.pw_gid;
		log_debug("resolve_user_uidgid: uid=%d, gid=%d",pw.pw_uid,pw.pw_gid);
		return 0;
	}
	else {
		log_error("failed to resolve POSIX uid/gid for %s: %s", username, strerror(errno));
		return -2;
	}
}

