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
#include <stdio.h>
#include <string.h>

#include "hessian/hessian.h"
#include "hessian/i_hessian.h"
#include "util/log.h"

/**
 * Method prototypes
 */
static OBJECT_CTOR(hessian_boolean);
static OBJECT_SERIALIZE(hessian_boolean);
static OBJECT_DESERIALIZE(hessian_boolean);


/**
 * Initializes and registers the internal Hessian boolean class descriptor.
 */
static const hessian_class_t _hessian_boolean_descr = {
    HESSIAN_BOOLEAN,
    "hessian.Boolean",
    sizeof(hessian_boolean_t),
    'T', 'F',
    hessian_boolean_ctor,
    NULL, // nothing to release
    hessian_boolean_serialize,
    hessian_boolean_deserialize
};
const void * hessian_boolean_class = &_hessian_boolean_descr;

/**
 * Hessian boolean constructor.
 *
 * hessian_object_t * boolean= hessian_create(HESSIAN_BOOLEAN, FALSE);
 */
static hessian_object_t * hessian_boolean_ctor (hessian_object_t * _self, va_list * ap) {
    hessian_boolean_t * self= _self;
    if (self == NULL) {
		log_error("hessian_boolean_ctor: NULL pointer.");
    	return NULL;
    }
    int value= va_arg( *ap, int);
    if (value == TRUE) self->value= TRUE;
    else self->value= FALSE;
    return self;
}


/**
 * Hessian boolean serialize method.
 */
static int hessian_boolean_serialize (const hessian_object_t * object, BUFFER * output) {
    const hessian_boolean_t * self= object;
    if (self == NULL) {
    	log_error("hessian_boolean_serialize: NULL object pointer.");
    	return HESSIAN_ERROR;
    }
    const hessian_class_t * class= hessian_getclass(object);
    if (class == NULL) {
    	log_error("hessian_boolean_serialize: NULL class descriptor.");
    	return HESSIAN_ERROR;
    }
    if (class->type != HESSIAN_BOOLEAN) {
    	log_error("hessian_boolean_serialize: wrong class type: %d.", class->type);
    	return HESSIAN_ERROR;
    }
    //printf("XXX:boolean_serialize:'%s'",self->value ? "T" : "F");
    int b= self->value == TRUE ? class->tag : class->chunk_tag;
    buffer_putc(b,output);
    return HESSIAN_OK;
}

/**
 * Hessian boolean deserialize method.
 */
static int hessian_boolean_deserialize (hessian_object_t * object, int tag, BUFFER * input) {
    hessian_boolean_t * self= object;
    if (self == NULL) {
    	log_error("hessian_boolean_deserialize: NULL object pointer.");
    	return HESSIAN_ERROR;
    }
    const hessian_class_t * class= hessian_getclass(object);
    if (class == NULL) {
    	log_error("hessian_boolean_deserialize: NULL class descriptor.");
    	return HESSIAN_ERROR;
    }
    if (class->type != HESSIAN_BOOLEAN) {
    	log_error("hessian_boolean_deserialize: wrong class type: %d.", class->type);
    	return HESSIAN_ERROR;
    }
    if (tag != class->tag && tag != class->chunk_tag) {
    	log_error("hessian_boolean_deserialize: invalid tag: %c (%d).", (char)tag, tag);
    	return HESSIAN_ERROR;
    }
    self->value= tag == class->tag ? TRUE : FALSE;
    return HESSIAN_OK;
}

/**
 * Returns the Hessian boolean value (TRUE or FALSE) or HESSIAN_ERROR (-1) on error.
 */
int hessian_boolean_getvalue(const hessian_object_t * object) {
	const hessian_boolean_t * self= object;
    if (self == NULL) {
    	log_error("hessian_boolean_getvalue: NULL object pointer.");
    	return HESSIAN_ERROR;
    }
    const hessian_class_t * class= hessian_getclass(object);
    if (class == NULL) {
    	log_error("hessian_boolean_getvalue: NULL class descriptor.");
    	return HESSIAN_ERROR;
    }
    if (class->type != HESSIAN_BOOLEAN) {
    	log_error("hessian_boolean_getvalue: wrong class type: %d.", class->type);
    	return HESSIAN_ERROR;
    }
    return self->value;
}
