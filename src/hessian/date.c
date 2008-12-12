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
 * $Id: date.c,v 1.1 2008/12/12 11:33:43 vtschopp Exp $
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hessian/hessian.h"

/**
 * Method prototypes
 */
extern OBJECT_CTOR(hessian_long);
extern OBJECT_SERIALIZE(hessian_long);
extern OBJECT_DESERIALIZE(hessian_long);

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


