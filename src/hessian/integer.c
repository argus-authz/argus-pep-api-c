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

/***********************************
 * Hessian integer and ref objects *
 ***********************************/

/**
 * Method prototypes
 */
static OBJECT_CTOR(hessian_integer);
static OBJECT_SERIALIZE(hessian_integer);
static OBJECT_DESERIALIZE(hessian_integer);

/**
 * Initializes and registers the internal Hessian integer class.
 */
static const hessian_class_t _hessian_integer_descr = {
    HESSIAN_INTEGER,
    "hessian.Integer",
    sizeof(hessian_integer_t),
    'I', 0,
    hessian_integer_ctor,
    NULL, /* nothing to release */
    hessian_integer_serialize,
    hessian_integer_deserialize
};
const void * hessian_integer_class = &_hessian_integer_descr;

/**
 * Hessian integer constructor.
 */
static hessian_object_t * hessian_integer_ctor (hessian_object_t * object, va_list * ap) {
    hessian_integer_t * self= object;
    int32_t value;
    if (self == NULL) {
		log_error("hessian_integer_ctor: NULL object pointer.");
    	return NULL;
    }
    value= va_arg( *ap, int32_t);
    self->value= value;
    return self;
}

/**
 * HessianInt serialize method.
 */
static int hessian_integer_serialize (const hessian_object_t * object, BUFFER * output) {
    const hessian_integer_t * self= object;
    const hessian_class_t * class;
    int32_t value, b8, b16, b24, b32;
    if (self == NULL) {
		log_error("hessian_integer_serialize: NULL object pointer.");
    	return HESSIAN_ERROR;
    }
    class= hessian_getclass(object);
    if (class == NULL) {
		log_error("hessian_integer_serialize: NULL class descriptor.");
    	return HESSIAN_ERROR;
    }
    if (class->type != HESSIAN_INTEGER && class->type != HESSIAN_REF) {
		log_error("hessian_integer_serialize: wrong class type: %d.", class->type);
    	return HESSIAN_ERROR;
    }
    value= self->value;
    b32 = (value >> 24) & 0x000000FF;
    b24 = (value >> 16) & 0x000000FF;
    b16 = (value >> 8) & 0x000000FF;
    b8 = value & 0x000000FF;
    buffer_putc(class->tag,output);
    buffer_putc(b32,output);
    buffer_putc(b24,output);
    buffer_putc(b16,output);
    buffer_putc(b8,output);
    return HESSIAN_OK;
}

/**
 * HessianInt deserialize method.
 */
static int hessian_integer_deserialize (hessian_object_t * object, int tag, BUFFER * input) {
    hessian_integer_t * self= object;
    const hessian_class_t * class;
    int32_t value, b8, b16, b24, b32;
    if (self == NULL) {
		log_error("hessian_integer_deserialize: NULL object pointer.");
    	return HESSIAN_ERROR;
    }
     class= hessian_getclass(object);
    if (class == NULL) {
		log_error("hessian_integer_deserialize: NULL class descriptor.");
    	return HESSIAN_ERROR;
    }
    if (class->type != HESSIAN_INTEGER && class->type != HESSIAN_REF) {
		log_error("hessian_integer_deserialize: wrong class type: %d.", class->type);
    	return HESSIAN_ERROR;
    }
    /* 'I' or 'R' tag */
    if (tag != class->tag) {
		log_error("hessian_integer_deserialize: wrong tag: %c (%d).",(char)tag,tag);
    	return HESSIAN_ERROR;
    }

    /* read int32 */
    b32 = buffer_getc(input);
    b24 = buffer_getc(input);
    b16 = buffer_getc(input);
    b8 = buffer_getc(input);
    value= (b32 << 24) + (b24 << 16) + (b16 << 8) + b8;

    self->value= value;
    return HESSIAN_OK;
}

/**
 * Returns the 32-bit signed integer value. INT32_MIN (-32,768) on error.
 */
int32_t hessian_integer_getvalue(const hessian_object_t * object) {
    const hessian_integer_t * self= object;
    const hessian_class_t * class;
    if (self == NULL) {
		log_error("hessian_integer_getvalue: NULL object pointer.");
    	return INT32_MIN;
    }
    class= hessian_getclass(object);
    if (class == NULL) {
		log_error("hessian_integer_getvalue: NULL class descriptor.");
    	return INT32_MIN;
    }
    if (class->type != HESSIAN_INTEGER) {
		log_error("hessian_integer_getvalue: wrong class type: %d.", class->type);
    	return INT32_MIN;
    }
	return self->value;
}

/**
 * Initializes and registers the Hessian ref class.
 * Extends the Hessian integer class.
 */
static const hessian_class_t _hessian_ref_descr = {
    HESSIAN_REF,
    "hessian.Ref",
    sizeof(hessian_ref_t),
    'R', 0,
    hessian_integer_ctor,
    NULL, /* nothing to release */
    hessian_integer_serialize,
    hessian_integer_deserialize
};
const void * hessian_ref_class = &_hessian_ref_descr;

/**
 * Returns the 32-bit signed integer. INT32_MIN (-32,768) on error.
 */
int32_t hessian_ref_getvalue(const hessian_object_t * object) {
    const hessian_ref_t * self= object;
    const hessian_class_t * class;
    if (self == NULL) {
		log_error("hessian_ref_getvalue: NULL object pointer.");
		return INT32_MIN;
	}
    class= hessian_getclass(object);
    if (class == NULL) {
		log_error("hessian_ref_getvalue: NULL class descriptor.");
		return INT32_MIN;
    }
    if (class->type != HESSIAN_REF) {
		log_error("hessian_ref_getvalue: wrong class type: %d.", class->type);
		return INT32_MIN;
    }
    return self->value;
}
