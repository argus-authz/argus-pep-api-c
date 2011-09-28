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
#include <float.h>

#include "hessian.h"
#include "i_hessian.h"
#include "log.h"

/**
 * Method prototypes
 */
static OBJECT_CTOR(hessian_double);
static OBJECT_SERIALIZE(hessian_double);
static OBJECT_DESERIALIZE(hessian_double);

/**
 * Initializes and registers the internal Hessian double class descriptor.
 */
static const hessian_class_t _hessian_double_descr = {
    HESSIAN_DOUBLE,
    "hessian.Double",
    sizeof(hessian_double_t),
    'D', 0,
    hessian_double_ctor,
    NULL, /* nothing to release */
    hessian_double_serialize,
    hessian_double_deserialize
};
const void * hessian_double_class = &_hessian_double_descr;

/**
 * Hessian double constructor.
 */
static hessian_object_t * hessian_double_ctor (hessian_object_t * object, va_list * ap) {
    hessian_double_t * self= object;
    double value= va_arg( *ap, double);
    if (self == NULL) {
		log_error("hessian_double_ctor: NULL object pointer.");
    	return NULL;
    }
    self->value= value;
    return self;
}

/**
 * HessianDouble serialize method.
 */
static int hessian_double_serialize (const hessian_object_t * object, BUFFER * output) {
    const hessian_double_t * self= object;
    const hessian_class_t * class;
    int64_t *lvalue, value;
    int b8, b16, b24, b32, b40, b48, b56, b64;
    if (self == NULL) {
		log_error("hessian_double_serialize: NULL object pointer.");
    	return HESSIAN_ERROR;
    }
    class= hessian_getclass(object);
    if (class == NULL) {
		log_error("hessian_double_serialize: NULL class descriptor.");
    	return HESSIAN_ERROR;
    }
    if (class->type != HESSIAN_DOUBLE) {
		log_error("hessian_double_serialize: wrong class type: %d.",class->type);
    	return HESSIAN_ERROR;
    }
    /* convert 64-bit double to a 64-bit long */
    lvalue = (int64_t*) &(self->value);
    value= *lvalue;
    b64 = (value >> 56) & 0x000000FF;
    b56 = (value >> 48) & 0x000000FF;
    b48 = (value >> 40) & 0x000000FF;
    b40 = (value >> 32) & 0x000000FF;
    b32 = (value >> 24) & 0x000000FF;
    b24 = (value >> 16) & 0x000000FF;
    b16 = (value >> 8) & 0x000000FF;
    b8 = value & 0x000000FF;
    buffer_putc(class->tag,output);
    buffer_putc(b64,output);
    buffer_putc(b56,output);
    buffer_putc(b48,output);
    buffer_putc(b40,output);
    buffer_putc(b32,output);
    buffer_putc(b24,output);
    buffer_putc(b16,output);
    buffer_putc(b8,output);
    return HESSIAN_OK;
}

/**
 * HessianDouble deserialize method.
 */
static int hessian_double_deserialize (hessian_object_t * object, int tag, BUFFER * input) {
    hessian_double_t * self= object;
    const hessian_class_t * class;
    int64_t b8, b16, b24, b32, b40, b48, b56, b64, lvalue;
    double * value;
    if (self == NULL) {
		log_error("hessian_double_deserialize: NULL object pointer.");
    	return HESSIAN_ERROR;
    }
    class= hessian_getclass(object);
    if (class == NULL) {
		log_error("hessian_double_deserialize: NULL class descriptor.");
    	return HESSIAN_ERROR;
    }
    if (class->type != HESSIAN_DOUBLE) {
		log_error("hessian_double_deserialize: wrong class type: %d.",class->type);
    	return HESSIAN_ERROR;
    }
    if (tag != class->tag) {
		log_error("hessian_double_deserialize: invalid tag: %c (%d).",(char)tag,tag);
    	return HESSIAN_ERROR;
    }
    b64 = buffer_getc(input);
    b56 = buffer_getc(input);
    b48 = buffer_getc(input);
    b40 = buffer_getc(input);
    b32 = buffer_getc(input);
    b24 = buffer_getc(input);
    b16 = buffer_getc(input);
    b8 = buffer_getc(input);
    lvalue= (b64 << 56)
		+ (b56 << 48)
		+ (b48 << 40)
		+ (b40 << 32)
		+ (b32 << 24)
		+ (b24 << 16)
		+ (b16 << 8)
		+ b8;
    /* convert 64bit long to double */
    value= (double *) &lvalue;
    self->value= (*value);
    return HESSIAN_OK;
}

/**
 * Returns the double 64-bit value or DBL_MIN (at least 1E-37) on error.
 */
double hessian_double_getvalue(const hessian_object_t * object) {
    const hessian_double_t * self= object;
    const hessian_class_t * class;
    if (self == NULL) {
		log_error("hessian_double_deserialize: NULL object pointer.");
    	return DBL_MIN;
    }
    class= hessian_getclass(object);
    if (class == NULL) {
		log_error("hessian_double_deserialize: NULL class descriptor.");
    	return DBL_MIN;
    }
    if (class->type != HESSIAN_DOUBLE) {
		log_error("hessian_double_deserialize: wrong class type: %d.",class->type);
    	return DBL_MIN;
    }
	return self->value;
}

