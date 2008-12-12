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
 * $Id: null.c,v 1.1 2008/12/12 11:33:43 vtschopp Exp $
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hessian/hessian.h"

/**
 * Method prototypes
 */
OBJECT_SERIALIZE(hessian_null);
OBJECT_DESERIALIZE(hessian_null);

/**
 * Initializes and registers the Hessian null class.
 */
static const hessian_class_t _hessian_null_descr = {
    HESSIAN_NULL,
    "hessian.Null",
    sizeof(hessian_null_t),
    'N', 0,
    NULL, // nothting to alloc
    NULL, // nothing to release
    hessian_null_serialize,
    hessian_null_deserialize
};
const void * hessian_null_class = &_hessian_null_descr;

/**
 * Hessian null serialize method.
 */
int hessian_null_serialize (const hessian_object_t * object, BUFFER * output) {
    const hessian_class_t * class= hessian_getclass(object);
    if (class == NULL) {
		fprintf(stderr,"ERROR:hessian_null_serialize: NULL class descriptor.\n");
		return HESSIAN_ERROR;
    }
    if (class->type != HESSIAN_NULL) {
		fprintf(stderr,"ERROR:hessian_null_serialize: wrong class type: %d.\n", class->type);
		return HESSIAN_ERROR;
    }
    buffer_putc(class->tag,output);
    return HESSIAN_OK;
}

/**
 * Hessian null deserialize method.
 */
int hessian_null_deserialize (hessian_object_t * object, int tag, BUFFER * input) {
    const hessian_class_t * class= hessian_getclass(object);
    if (class == NULL) {
		fprintf(stderr,"ERROR:hessian_null_deserialize: NULL class descriptor.\n");
		return HESSIAN_ERROR;
    }
    if (tag != class->tag) {
		fprintf(stderr,"ERROR:hessian_null_deserialize: invalid tag: %c (%d).\n",(char)tag,tag);
    	return HESSIAN_ERROR;
    }
    return HESSIAN_OK;
}

