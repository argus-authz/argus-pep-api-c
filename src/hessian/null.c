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
 *
 * $Id$
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
static OBJECT_SERIALIZE(hessian_null);
static OBJECT_DESERIALIZE(hessian_null);

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
static int hessian_null_serialize (const hessian_object_t * object, BUFFER * output) {
    const hessian_class_t * class= hessian_getclass(object);
    if (class == NULL) {
		log_error("hessian_null_serialize: NULL class descriptor.");
		return HESSIAN_ERROR;
    }
    if (class->type != HESSIAN_NULL) {
		log_error("hessian_null_serialize: wrong class type: %d.", class->type);
		return HESSIAN_ERROR;
    }
    buffer_putc(class->tag,output);
    return HESSIAN_OK;
}

/**
 * Hessian null deserialize method.
 */
static int hessian_null_deserialize (hessian_object_t * object, int tag, BUFFER * input) {
    const hessian_class_t * class= hessian_getclass(object);
    if (class == NULL) {
		log_error("hessian_null_deserialize: NULL class descriptor.");
		return HESSIAN_ERROR;
    }
    if (tag != class->tag) {
		log_error("hessian_null_deserialize: invalid tag: %c (%d).",(char)tag,tag);
    	return HESSIAN_ERROR;
    }
    return HESSIAN_OK;
}

