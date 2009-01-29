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
 * $Id: long.c,v 1.3 2009/01/29 16:19:58 vtschopp Exp $
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hessian/hessian.h"
#include "util/log.h"

/*********************************
 * Hessian long and date objects *
 *********************************/

/**
 * Method prototypes
 */
static OBJECT_CTOR(hessian_long);
static OBJECT_SERIALIZE(hessian_long);
static OBJECT_DESERIALIZE(hessian_long);


/**
 * Initializes and registers the internal Hessian long class descriptor.
 */
static const hessian_class_t _hessian_long_descr = {
    HESSIAN_LONG,
    "hessian.Long",
    sizeof(hessian_long_t),
    'L', 0,
    hessian_long_ctor,
    NULL, // nothing to release
    hessian_long_serialize,
    hessian_long_deserialize
};
const void * hessian_long_class = &_hessian_long_descr;

/**
 * Hessian long constructor.
 */
static hessian_object_t * hessian_long_ctor (hessian_object_t * object, va_list * ap) {
    hessian_long_t * self= object;
    if (self == NULL) {
		log_error("hessian_long_ctor: NULL object pointer.");
    	return NULL;
    }
    int64_t value= va_arg( *ap, int64_t);
    self->value= value;
    return self;
}

/**
 * hessian_long serialize method.
 */
static int hessian_long_serialize (const hessian_object_t * object, BUFFER * output) {
    const hessian_long_t * self= object;
    if (self == NULL) {
		log_error("hessian_long_serialize: NULL object pointer.");
    	return HESSIAN_ERROR;
    }
    const hessian_class_t * class= hessian_getclass(object);
    if (class == NULL) {
		log_error("hessian_long_serialize: NULL class descriptor.");
    	return HESSIAN_ERROR;
    }
    if (class->type != HESSIAN_LONG && class->type != HESSIAN_DATE) {
		log_error("hessian_long_serialize: wrong class type: %d.",class->type);
    	return HESSIAN_ERROR;
    }
    int64_t value= self->value;
    int b64 = (value >> 56) & 0x000000FF;
    int b56 = (value >> 48) & 0x000000FF;
    int b48 = (value >> 40) & 0x000000FF;
    int b40 = (value >> 32) & 0x000000FF;
    int b32 = (value >> 24) & 0x000000FF;
    int b24 = (value >> 16) & 0x000000FF;
    int b16 = (value >> 8) & 0x000000FF;
    int b8 = value & 0x000000FF;
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
 * hessian_long deserialize method.
 */
static int hessian_long_deserialize (hessian_object_t * object, int tag, BUFFER * input) {
    hessian_long_t * self= object;
    if (self == NULL) {
		log_error("hessian_long_serialize: NULL object pointer.");
    	return HESSIAN_ERROR;
    }
    const hessian_class_t * class= hessian_getclass(object);
    if (class == NULL) {
		log_error("hessian_long_serialize: NULL class descriptor.");
    	return HESSIAN_ERROR;
    }
    if (class->type != HESSIAN_LONG && class->type != HESSIAN_DATE) {
		log_error("hessian_long_serialize: wrong class type: %d.",class->type);
    	return HESSIAN_ERROR;
    }
    if (tag != class->tag) {
		log_error("hessian_long_serialize: invalid tag: %c (%d).",(char)tag,tag);
    	return HESSIAN_ERROR;
    }
    int64_t b64 = buffer_getc(input);
    int64_t b56 = buffer_getc(input);
    int64_t b48 = buffer_getc(input);
    int64_t b40 = buffer_getc(input);
    int64_t b32 = buffer_getc(input);
    int64_t b24 = buffer_getc(input);
    int64_t b16 = buffer_getc(input);
    int64_t b8 = buffer_getc(input);

    int64_t value= (b64 << 56)
		+ (b56 << 48)
		+ (b48 << 40)
		+ (b40 << 32)
		+ (b32 << 24)
		+ (b24 << 16)
		+ (b16 << 8)
		+ b8;

    self->value= value;
    return HESSIAN_OK;
}

/**
 * Returns the 64-bit Hessian long. INT64_MIN (-9,223,372,036,854,775,808) on error.
 */
int64_t hessian_long_getvalue(const hessian_object_t * object) {
    const hessian_long_t * self= object;
    if (self == NULL) {
		log_error("hessian_long_getvalue: NULL object pointer.");
    	return INT64_MIN;
    }
    const hessian_class_t * class= hessian_getclass(object);
    if (class == NULL) {
		log_error("hessian_long_getvalue: NULL class descriptor.");
    	return INT64_MIN;
    }
    if (class->type != HESSIAN_LONG && class->type != HESSIAN_DATE) {
		log_error("hessian_long_getvalue: wrong class type: %d.",class->type);
    	return INT64_MIN;
    }
	return self->value;
}

/**
 * Initializes and registers the internal Hessian date class descriptor.
 */
static const hessian_class_t _hessian_date_descr = {
    HESSIAN_DATE,
    "hessian.Date",
    sizeof(hessian_date_t),
    'd', 0,
    hessian_long_ctor,
    NULL, // nothing to release
    hessian_long_serialize,
    hessian_long_deserialize
};
const void * hessian_date_class = &_hessian_date_descr;

/**
 * Returns the 64-bit long Hessian date (epoch). INT64_MIN (-9,223,372,036,854,775,808) on error.
 */
int64_t hessian_date_getvalue(const hessian_object_t * self) {
	return hessian_long_getvalue(self);
}

