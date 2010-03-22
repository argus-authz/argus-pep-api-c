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

#ifndef _INTERNAL_HESSIAN_H_
#define _INTERNAL_HESSIAN_H_

#ifdef  __cplusplus
extern "C" {
#endif

/*
 * INTERNAL HESSIAN macros and constants
 */

/**
 * Object methods prototype macro
 */
#define OBJECT_CTOR(objname) \
    hessian_object_t * objname ## _ctor (hessian_object_t * self, va_list * app)
#define OBJECT_DTOR(objname) \
    int objname ## _dtor (hessian_object_t * self)
#define OBJECT_SERIALIZE(objname) \
    int objname ## _serialize (const hessian_object_t * self, BUFFER * output)
#define OBJECT_DESERIALIZE(objname) \
    int objname ## _deserialize (hessian_object_t * self, int tag, BUFFER * input)

/*
 * Hessian serialization chunk size
 */
#ifndef HESSIAN_CHUNK_SIZE
#include <stdint.h>
#define HESSIAN_CHUNK_SIZE INT16_MAX
#endif

#ifdef  __cplusplus
}
#endif


#endif // _INTERNAL_HESSIAN_H_
