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

#include "hessian.h"
#include "i_hessian.h"
#include "log.h"

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
    NULL, /* nothing to release */
    hessian_long_serialize,
    hessian_long_deserialize
};
const void * hessian_long_class = &_hessian_long_descr;

/**
 * Hessian long constructor.
 */
static hessian_object_t * hessian_long_ctor (hessian_object_t * object, va_list * ap) {
    hessian_long_t * self= object;
    int64_t value;
    if (self == NULL) {
        pep_log_error("hessian_long_ctor: NULL object pointer.");
        return NULL;
    }
    value= va_arg( *ap, int64_t);
    self->value= value;
    return self;
}

/**
 * hessian_long serialize method.
 */
static int hessian_long_serialize (const hessian_object_t * object, pep_buffer_t * output) {
    const hessian_long_t * self= object;
    const hessian_class_t * class;
    int64_t value;
    int b8, b16, b24, b32, b40, b48, b56, b64;
    if (self == NULL) {
        pep_log_error("hessian_long_serialize: NULL object pointer.");
        return HESSIAN_ERROR;
    }
    class= hessian_getclass(object);
    if (class == NULL) {
        pep_log_error("hessian_long_serialize: NULL class descriptor.");
        return HESSIAN_ERROR;
    }
    if (class->type != HESSIAN_LONG && class->type != HESSIAN_DATE) {
        pep_log_error("hessian_long_serialize: wrong class type: %d.",class->type);
        return HESSIAN_ERROR;
    }
    value= self->value;
    b64 = (value >> 56) & 0x000000FF;
    b56 = (value >> 48) & 0x000000FF;
    b48 = (value >> 40) & 0x000000FF;
    b40 = (value >> 32) & 0x000000FF;
    b32 = (value >> 24) & 0x000000FF;
    b24 = (value >> 16) & 0x000000FF;
    b16 = (value >> 8) & 0x000000FF;
    b8 = value & 0x000000FF;
    pep_buffer_putc(class->tag,output);
    pep_buffer_putc(b64,output);
    pep_buffer_putc(b56,output);
    pep_buffer_putc(b48,output);
    pep_buffer_putc(b40,output);
    pep_buffer_putc(b32,output);
    pep_buffer_putc(b24,output);
    pep_buffer_putc(b16,output);
    pep_buffer_putc(b8,output);
    return HESSIAN_OK;
}

/**
 * hessian_long deserialize method.
 */
static int hessian_long_deserialize (hessian_object_t * object, int tag, pep_buffer_t * input) {
    hessian_long_t * self= object;
    const hessian_class_t * class;
    int64_t b8, b16, b24, b32, b40, b48, b56, b64, value;
    if (self == NULL) {
        pep_log_error("hessian_long_deserialize: NULL object pointer.");
        return HESSIAN_ERROR;
    }
    class= hessian_getclass(object);
    if (class == NULL) {
        pep_log_error("hessian_long_deserialize: NULL class descriptor.");
        return HESSIAN_ERROR;
    }
    if (class->type != HESSIAN_LONG && class->type != HESSIAN_DATE) {
        pep_log_error("hessian_long_deserialize: wrong class type: %d.",class->type);
        return HESSIAN_ERROR;
    }
    if (tag != class->tag) {
        pep_log_error("hessian_long_deserialize: invalid tag: %c (%d).",(char)tag,tag);
        return HESSIAN_ERROR;
    }
    b64 = pep_buffer_getc(input);
    b56 = pep_buffer_getc(input);
    b48 = pep_buffer_getc(input);
    b40 = pep_buffer_getc(input);
    b32 = pep_buffer_getc(input);
    b24 = pep_buffer_getc(input);
    b16 = pep_buffer_getc(input);
    b8 = pep_buffer_getc(input);
    value= (b64 << 56)
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
 * Returns the 64-bit Hessian long. 0 on error.
 */
int64_t hessian_long_getvalue(const hessian_object_t * object) {
    const hessian_long_t * self= object;
    const hessian_class_t * class;
    if (self == NULL) {
        pep_log_error("hessian_long_getvalue: NULL object pointer.");
        return 0;
    }
    class= hessian_getclass(object);
    if (class == NULL) {
        pep_log_error("hessian_long_getvalue: NULL class descriptor.");
        return 0;
    }
    if (class->type != HESSIAN_LONG && class->type != HESSIAN_DATE) {
        pep_log_error("hessian_long_getvalue: wrong class type: %d.",class->type);
        return 0;
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
    NULL, /* nothing to release */
    hessian_long_serialize,
    hessian_long_deserialize
};
const void * hessian_date_class = &_hessian_date_descr;

/**
 * Returns the 64-bit long Hessian date (epoch). 0 on error.
 */
int64_t hessian_date_getvalue(const hessian_object_t * self) {
    return hessian_long_getvalue(self);
}

