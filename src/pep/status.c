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
 * $Id: status.c,v 1.1 2008/12/12 11:34:27 vtschopp Exp $
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "util/linkedlist.h"
#include "pep/model.h"

/************************************************************
 * PEP Status functions
 */
struct pep_status {
	char * message;
	pep_status_code_t * code;
};

// message can be null
pep_status_t * pep_status_create(const char * message) {
	pep_status_t * status= calloc(1,sizeof(pep_status_t));
	if (status == NULL) {
		fprintf(stderr,"ERROR:pep_status_create: can't allocate pep_status_t.\n");
		return NULL;
	}
	status->message= NULL;
	if (message != NULL) {
		size_t size= strlen(message);
		status->message= calloc(size + 1,sizeof(char));
		if (status->message == NULL) {
			fprintf(stderr,"ERROR:pep_status_create: can't allocate message (%d bytes).\n",(int)size);
			free(status);
			return NULL;
		}
		strncpy(status->message,message,size);
	}
	status->code= NULL;
	return status;
}

// no NULL message allowed
int pep_status_setmessage(pep_status_t * status, const char * message) {
	if (status == NULL) {
		fprintf(stderr,"ERROR:pep_status_setmessage: NULL status object.\n");
		return PEP_MODEL_ERROR;
	}
	if (message == NULL) {
		fprintf(stderr,"ERROR:pep_status_setmessage: NULL message.\n");
		return PEP_MODEL_ERROR;
	}
	if (status->message != NULL) free(status->message);
	size_t size= strlen(message);
	status->message= calloc(size + 1,sizeof(char));
	if (status->message == NULL) {
		fprintf(stderr,"ERROR:pep_status_setmessage: can't allocate message (%d bytes).\n",(int)size);
		return PEP_MODEL_ERROR;
	}
	strncpy(status->message,message,size);
	return PEP_MODEL_OK;
}


const char * pep_status_getmessage(const pep_status_t * status) {
	if (status == NULL) {
		fprintf(stderr,"ERROR:pep_status_getmessage: NULL status.\n");
		return NULL;
	}
	return status->message;
}

int pep_status_setcode(pep_status_t * status, pep_status_code_t * code) {
	if (status == NULL || code == NULL) {
		fprintf(stderr,"ERROR:pep_status_getcode: NULL status or code.\n");
		return PEP_MODEL_ERROR;
	}
	if (status->code != NULL) {
		pep_status_code_delete(status->code);
	}
	status->code= code;
	return PEP_MODEL_OK;
}

pep_status_code_t * pep_status_getcode(const pep_status_t * status) {
	if (status == NULL) {
		fprintf(stderr,"ERROR:pep_status_getcode: NULL status.\n");
		return NULL;
	}
	return status->code;
}

void pep_status_delete(pep_status_t * status) {
	if (status == NULL) return;
	if (status->message != NULL) free(status->message);
	if (status->code != NULL) {
		pep_status_code_delete(status->code);
	}
	free(status);
	status= NULL;
}

/************************************************************
 * PEP StatusCode functions
 */
struct pep_status_code {
	char * code;
	struct pep_status_code * subcode;
};

// code can be NULL, not recommended
pep_status_code_t * pep_status_code_create(const char * code) {
	pep_status_code_t * status_code= calloc(1,sizeof(pep_status_code_t));
	if (status_code == NULL) {
		fprintf(stderr,"ERROR:pep_status_code_create: can't allocate pep_status_code_t.\n");
		return NULL;
	}
	status_code->code= NULL;
	if (code != NULL) {
		size_t size= strlen(code);
		status_code->code= calloc(size + 1,sizeof(char));
		if (status_code->code == NULL) {
			fprintf(stderr,"ERROR:pep_status_code_create: can't allocate code (%d bytes).\n",(int)size);
			free(status_code);
			return NULL;
		}
		strncpy(status_code->code,code,size);
	}
	status_code->subcode= NULL;
	return status_code;
}

// code NULL not allowed
int pep_status_code_setcode(pep_status_code_t * status_code, const char * code) {
	if (status_code == NULL) {
		fprintf(stderr,"ERROR:pep_status_code_setcode: NULL status_code object.\n");
		return PEP_MODEL_ERROR;
	}
	if (code == NULL) {
		fprintf(stderr,"ERROR:pep_status_code_setcode: NULL code string.\n");
		return PEP_MODEL_ERROR;
	}
	if (status_code->code != NULL) free(status_code->code);
	size_t size= strlen(code);
	status_code->code= calloc(size + 1,sizeof(char));
	if (status_code->code == NULL) {
		fprintf(stderr,"ERROR:pep_status_code_setcode: can't allocate code (%d bytes).\n",(int)size);
		return PEP_MODEL_ERROR;
	}
	strncpy(status_code->code,code,size);
	return PEP_MODEL_OK;
}

const char * pep_status_code_getcode(const pep_status_code_t * status_code) {
	if (status_code == NULL) {
		fprintf(stderr,"ERROR:pep_status_code_getcode: NULL status_code object.\n");
		return NULL;
	}
	return status_code->code;
}

int pep_status_code_setsubcode(pep_status_code_t * status_code, pep_status_code_t * subcode) {
	if (status_code == NULL || subcode == NULL) {
		fprintf(stderr,"ERROR:pep_status_code_setsubcode: NULL status_code or subcode\n");
		return PEP_MODEL_ERROR;
	}
	if (status_code->subcode != NULL) {
		pep_status_code_delete(status_code->subcode);
	}
	status_code->subcode= subcode;
	return PEP_MODEL_OK;
}


pep_status_code_t * pep_status_code_getsubcode(const pep_status_code_t * status_code) {
	if (status_code == NULL) {
		fprintf(stderr,"ERROR:pep_status_code_getcode: NULL status_code.\n");
		return NULL;
	}
	return status_code->subcode;
}

void pep_status_code_delete(pep_status_code_t * status_code) {
	if (status_code == NULL) return;
	if (status_code->code != NULL) free(status_code->code);
	if (status_code->subcode != NULL) {
		pep_status_code_delete(status_code->subcode);
	}
	free(status_code);
	status_code= NULL;
}

