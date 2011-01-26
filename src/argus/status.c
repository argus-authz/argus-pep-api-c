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

#include <stdlib.h>
#include <string.h>

#include "util/linkedlist.h"
#include "util/log.h"
#include "argus/xacml.h"

/************************************************************
 * PEP Status functions
 */
struct xacml_status {
    char * message;
    xacml_statuscode_t * code;
};

/* message can be null */
xacml_status_t * xacml_status_create(const char * message) {
    xacml_status_t * status= calloc(1,sizeof(xacml_status_t));
    if (status == NULL) {
        log_error("xacml_status_create: can't allocate xacml_status_t.");
        return NULL;
    }
    status->message= NULL;
    if (message != NULL) {
        size_t size= strlen(message);
        status->message= calloc(size + 1,sizeof(char));
        if (status->message == NULL) {
            log_error("xacml_status_create: can't allocate message (%d bytes).",(int)size);
            free(status);
            return NULL;
        }
        strncpy(status->message,message,size);
    }
    status->code= NULL;
    return status;
}

/* no NULL message allowed */
int xacml_status_setmessage(xacml_status_t * status, const char * message) {
    size_t size;
    if (status == NULL) {
        log_error("xacml_status_setmessage: NULL status object.");
        return PEP_XACML_ERROR;
    }
    if (message == NULL) {
        log_error("xacml_status_setmessage: NULL message.");
        return PEP_XACML_ERROR;
    }
    if (status->message != NULL) free(status->message);
    size= strlen(message);
    status->message= calloc(size + 1,sizeof(char));
    if (status->message == NULL) {
        log_error("xacml_status_setmessage: can't allocate message (%d bytes).",(int)size);
        return PEP_XACML_ERROR;
    }
    strncpy(status->message,message,size);
    return PEP_XACML_OK;
}


const char * xacml_status_getmessage(const xacml_status_t * status) {
    if (status == NULL) {
        log_error("xacml_status_getmessage: NULL status.");
        return NULL;
    }
    return status->message;
}

int xacml_status_setcode(xacml_status_t * status, xacml_statuscode_t * code) {
    if (status == NULL || code == NULL) {
        log_error("xacml_status_getcode: NULL status or code.");
        return PEP_XACML_ERROR;
    }
    if (status->code != NULL) {
        xacml_statuscode_delete(status->code);
    }
    status->code= code;
    return PEP_XACML_OK;
}

xacml_statuscode_t * xacml_status_getcode(const xacml_status_t * status) {
    if (status == NULL) {
        log_error("xacml_status_getcode: NULL status.");
        return NULL;
    }
    return status->code;
}

void xacml_status_delete(xacml_status_t * status) {
    if (status == NULL) return;
    if (status->message != NULL) free(status->message);
    if (status->code != NULL) {
        xacml_statuscode_delete(status->code);
    }
    free(status);
    status= NULL;
}

/************************************************************
 * PEP StatusCode functions
 */
struct xacml_statuscode {
    char * value;
    struct xacml_statuscode * subcode;
};

/* value can be NULL, not recommended */
xacml_statuscode_t * xacml_statuscode_create(const char * value) {
    xacml_statuscode_t * status_code= calloc(1,sizeof(xacml_statuscode_t));
    if (status_code == NULL) {
        log_error("xacml_statuscode_create: can't allocate xacml_statuscode_t.");
        return NULL;
    }
    status_code->value= NULL;
    if (value != NULL) {
        size_t size= strlen(value);
        status_code->value= calloc(size + 1,sizeof(char));
        if (status_code->value == NULL) {
            log_error("xacml_statuscode_create: can't allocate value (%d bytes).",(int)size);
            free(status_code);
            return NULL;
        }
        strncpy(status_code->value,value,size);
    }
    status_code->subcode= NULL;
    return status_code;
}

/* value NULL not allowed */
int xacml_statuscode_setvalue(xacml_statuscode_t * status_code, const char * value) {
    size_t size;
    if (status_code == NULL) {
        log_error("xacml_statuscode_setcode: NULL status_code object.");
        return PEP_XACML_ERROR;
    }
    if (value == NULL) {
        log_error("xacml_statuscode_setcode: NULL value string.");
        return PEP_XACML_ERROR;
    }
    if (status_code->value != NULL) free(status_code->value);
    size= strlen(value);
    status_code->value= calloc(size + 1,sizeof(char));
    if (status_code->value == NULL) {
        log_error("xacml_statuscode_setcode: can't allocate value (%d bytes).",(int)size);
        return PEP_XACML_ERROR;
    }
    strncpy(status_code->value,value,size);
    return PEP_XACML_OK;
}

const char * xacml_statuscode_getvalue(const xacml_statuscode_t * status_code) {
    if (status_code == NULL) {
        log_error("xacml_statuscode_getcode: NULL status_code object.");
        return NULL;
    }
    return status_code->value;
}

int xacml_statuscode_setsubcode(xacml_statuscode_t * status_code, xacml_statuscode_t * subcode) {
    if (status_code == NULL || subcode == NULL) {
        log_error("xacml_statuscode_setsubcode: NULL status_code or subcode");
        return PEP_XACML_ERROR;
    }
    if (status_code->subcode != NULL) {
        xacml_statuscode_delete(status_code->subcode);
    }
    status_code->subcode= subcode;
    return PEP_XACML_OK;
}


xacml_statuscode_t * xacml_statuscode_getsubcode(const xacml_statuscode_t * status_code) {
    if (status_code == NULL) {
        log_error("xacml_statuscode_getcode: NULL status_code.");
        return NULL;
    }
    return status_code->subcode;
}

void xacml_statuscode_delete(xacml_statuscode_t * status_code) {
    if (status_code == NULL) return;
    if (status_code->value != NULL) free(status_code->value);
    if (status_code->subcode != NULL) {
        xacml_statuscode_delete(status_code->subcode);
    }
    free(status_code);
    status_code= NULL;
}

