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
 * $Id: ref.c,v 1.1 2008/12/12 11:33:43 vtschopp Exp $
 */
#include "hessian/hessian.h"

/**
 * Uses the integer generic methods.
 */
extern OBJECT_CTOR(hessian_integer);
extern OBJECT_SERIALIZE(hessian_integer);
extern OBJECT_DESERIALIZE(hessian_integer);

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
    NULL, // nothing to release
    hessian_integer_serialize,
    hessian_integer_deserialize
};
const void * hessian_ref_class = &_hessian_ref_descr;

/**
 * Returns the 32-bit signed integer. INT32_MIN (-32,768) on error.
 */
int32_t hessian_ref_getvalue(const hessian_object_t * object) {
    const hessian_ref_t * self= object;
    if (self == NULL) {
		fprintf(stderr,"ERROR:hessian_ref_getvalue: NULL object pointer.\n");
		return INT32_MIN;
	}
    const hessian_class_t * class= hessian_getclass(object);
    if (class == NULL) {
		fprintf(stderr,"ERROR:hessian_ref_getvalue: NULL class descriptor.\n");
		return INT32_MIN;
    }
    if (class->type != HESSIAN_REF) {
		fprintf(stderr,"ERROR:hessian_ref_getvalue: wrong class type: %d.\n", class->type);
		return INT32_MIN;
    }
    return self->value;
}
