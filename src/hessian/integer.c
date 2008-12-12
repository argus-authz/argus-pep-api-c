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
 * $Id: integer.c,v 1.1 2008/12/12 11:33:43 vtschopp Exp $
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hessian/hessian.h"

/**
 * Method prototypes
 */
OBJECT_CTOR(hessian_integer);
OBJECT_SERIALIZE(hessian_integer);
OBJECT_DESERIALIZE(hessian_integer);

/**
 * Initializes and registers the internal Hessian integer class.
 */
static const hessian_class_t _hessian_integer_descr = {
    HESSIAN_INTEGER,
    "hessian.Integer",
    sizeof(hessian_integer_t),
    'I', 0,
    hessian_integer_ctor,
    NULL, // nothing to release
    hessian_integer_serialize,
    hessian_integer_deserialize
};
const void * hessian_integer_class = &_hessian_integer_descr;

/**
 * Hessian integer constructor.
 */
hessian_object_t * hessian_integer_ctor (hessian_object_t * object, va_list * ap) {
    hessian_integer_t * self= object;
    if (self == NULL) {
		fprintf(stderr,"ERROR:hessian_integer_ctor: NULL object pointer.\n");
    	return NULL;
    }
    int32_t value= va_arg( *ap, int32_t);
    self->value= value;
    return self;
}

/**
 * HessianInt serialize method.
 */
int hessian_integer_serialize (const hessian_object_t * object, BUFFER * output) {
    const hessian_integer_t * self= object;
    if (self == NULL) {
		fprintf(stderr,"ERROR:hessian_integer_serialize: NULL object pointer.\n");
    	return HESSIAN_ERROR;
    }
    const hessian_class_t * class= hessian_getclass(object);
    if (class == NULL) {
		fprintf(stderr,"ERROR:hessian_integer_serialize: NULL class descriptor.\n");
    	return HESSIAN_ERROR;
    }
    if (class->type != HESSIAN_INTEGER && class->type != HESSIAN_REF) {
		fprintf(stderr,"ERROR:hessian_integer_serialize: wrong class type: %d.\n", class->type);
    	return HESSIAN_ERROR;
    }
    int32_t value= self->value;
    int32_t b32 = (value >> 24) & 0x000000FF;
    int32_t b24 = (value >> 16) & 0x000000FF;
    int32_t b16 = (value >> 8) & 0x000000FF;
    int32_t b8 = value & 0x000000FF;
    //printf("XXX:integer_serialize: %d\n", value);
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
int hessian_integer_deserialize (hessian_object_t * object, int tag, BUFFER * input) {
    hessian_integer_t * self= object;
    if (self == NULL) {
		fprintf(stderr,"ERROR:hessian_integer_deserialize: NULL object pointer.\n");
    	return HESSIAN_ERROR;
    }
    const hessian_class_t * class= hessian_getclass(object);
    if (class == NULL) {
		fprintf(stderr,"ERROR:hessian_integer_deserialize: NULL class descriptor.\n");
    	return HESSIAN_ERROR;
    }
    if (class->type != HESSIAN_INTEGER && class->type != HESSIAN_REF) {
		fprintf(stderr,"ERROR:hessian_integer_deserialize: wrong class type: %d.\n", class->type);
    	return HESSIAN_ERROR;
    }
    // 'I' or 'R' tag
    if (tag != class->tag) {
		fprintf(stderr,"ERROR:hessian_integer_deserialize: wrong tag: %c (%d).\n",(char)tag,tag);
    	return HESSIAN_ERROR;
    }

    // read int32
    int32_t b32 = buffer_getc(input);
    int32_t b24 = buffer_getc(input);
    int32_t b16 = buffer_getc(input);
    int32_t b8 = buffer_getc(input);

    int32_t value= (b32 << 24) + (b24 << 16) + (b16 << 8) + b8;

    self->value= value;
    return HESSIAN_OK;
}

/**
 * Returns the 32-bit signed integer value. INT32_MIN (-32,768) on error.
 */
int32_t hessian_integer_getvalue(const hessian_object_t * object) {
    const hessian_integer_t * self= object;
    if (self == NULL) {
		fprintf(stderr,"ERROR:hessian_integer_getvalue: NULL object pointer.\n");
    	return INT32_MIN;
    }
    const hessian_class_t * class= hessian_getclass(object);
    if (class == NULL) {
		fprintf(stderr,"ERROR:hessian_integer_getvalue: NULL class descriptor.\n");
    	return INT32_MIN;
    }
    if (class->type != HESSIAN_INTEGER) {
		fprintf(stderr,"ERROR:hessian_integer_getvalue: wrong class type: %d.\n", class->type);
    	return INT32_MIN;
    }
	return self->value;
}


