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

/******************************************/
/* ONLY FOR HESSIAN INTERNAL CLASS TYPES. */
/*         DO NOT INCLUDE.                */
/******************************************/

#ifndef _HESSIAN_TYPES_H_
#define _HESSIAN_TYPES_H_

#include <stdint.h>
#include "util/buffer.h"
#include "util/linkedlist.h"

/**
 * Hessian object types
 */
typedef enum {
    HESSIAN_UNKNOWN,
    /* Hessian object */
    HESSIAN_BOOLEAN,
    HESSIAN_INTEGER,
    HESSIAN_LONG,
    HESSIAN_DOUBLE,
    HESSIAN_DATE,
    HESSIAN_STRING,
    HESSIAN_XML,
    HESSIAN_BINARY,
    HESSIAN_REMOTE,
    HESSIAN_LIST,
    HESSIAN_MAP,
    HESSIAN_NULL,
    HESSIAN_REF
} hessian_t;

/**
 * Generic Hessian object type
 */
typedef void hessian_object_t;

/**
 * Hessian internal class descriptor type.
 *
 * TODO: add equality function: 'int (* equals) (const hessian_object_t * self, const hessian_object_t * other)'
 */
typedef struct hessian_class {
    hessian_t type;
    const char * name;
    size_t size;
    const char tag;
    const char chunk_tag;
    hessian_object_t * (* ctor) (hessian_object_t * self, va_list * app);
    int (* dtor) (hessian_object_t * self);
    int (* serialize) (const hessian_object_t * self, BUFFER * output);
    int (* deserialize) (hessian_object_t * self, int tag, BUFFER * input);
} hessian_class_t;

/**
 * Hessian internal binary class type
 */
typedef struct hessian_binary {
    const void * class;
    char * data;
    size_t length;
} hessian_binary_t;

/**
 * Hessian internal boolean class type
 */
typedef struct hessian_boolean {
    const void * class;
    int value;
} hessian_boolean_t;

/**
 * Hessian internal date and 64-bits long integer class types.
 */
typedef struct hessian_long {
    const void * class;
    int64_t value;
} hessian_long_t, hessian_date_t;


/**
 * Hessian internal 32-bits integer and ref class types.
 */
typedef struct hessian_integer {
    const void * class;
    int32_t value;
} hessian_integer_t, hessian_ref_t;

/**
 * Hessian internal list class type.
 */
typedef struct hessian_list {
    const void * class;
    char * type;
    linkedlist_t * list;
} hessian_list_t;

/**
 * Hessian map class type
 */
typedef struct hessian_map {
    const void * class;
    char * type;
    linkedlist_t * map; /* <object,object> pairs (key,value) */
} hessian_map_t;

/**
 * Hessian internal double class type.
 */
typedef struct hessian_double {
    const void * class;
    double value;
} hessian_double_t;

/**
 * Hessian UTF-8 string and XML class types.
 */
typedef struct hessian_string {
    const void * class;
    char * string;
} hessian_string_t, hessian_xml_t;

/**
 * Hessian null class type
 */
typedef struct hessian_null {
    const void * class;
} hessian_null_t;

/**
 * Hessian remote class type.
 */
typedef struct hessian_remote {
    const void * class;
    char * type;
    char * url;
} hessian_remote_t;

#endif

