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

#ifndef _HESSIAN_H_
#define _HESSIAN_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdio.h>

#include "types.h"
#include "buffer.h"

/** Hessian return codes */
#define HESSIAN_OK     0
#define HESSIAN_ERROR -1

/**
 * Creates a Hessian object.
 *
 * @param hessian_t type The type of Hessian object to create.
 * @param ... variable arguments depending of the Hessian object type, see below.
 *
 * @return hessian_object_t * pointer to the created Hessian object
 *         or NULL if an error occurs.
 *
 * Hessian object constructor prototypes:
 *
 * hessian_object_t * h_binary= hessian_create(HESSIAN_BINARY, (size_t)size, (const char *)data);
 * hessian_object_t * h_boolean= hessian_create(HESSIAN_BOOLEAN, (int)FALSE);
 * hessian_object_t * h_date= hessian_create(HESSIAN_DATE, (int64_t)epoch);
 * hessian_object_t * h_double= hessian_create(HESSIAN_DOUBLE, (double)value);
 * hessian_object_t * h_integer= hessian_create(HESSIAN_INTEGER, (int32_t)value);
 * hessian_object_t * h_long= hessian_create(HESSIAN_LONG, (int64_t)value);
 * hessian_object_t * h_null= hessian_create(HESSIAN_NULL);
 * hessian_object_t * h_ref= hessian_create(HESSIAN_REF, (int32_t)ref);
 * hessian_object_t * h_remote= hessian_create(HESSIAN_REMOTE, (const char *)type, (const char *)url);
 * hessian_object_t * h_string= hessian_create(HESSIAN_STRING, (const char *)string);
 * hessian_object_t * h_xml= hessian_create(HESSIAN_XML, (const char *)xml);
 *
 * hessian_object_t * h_list= hessian_create(HESSIAN_LIST);
 * hessian_object_t * h_map= hessian_create(HESSIAN_MAP, (const char *)type);
 */
hessian_object_t * hessian_create (hessian_t type, ...);

/**
 * Destroy a Hessian object.
 *
 * @param hessian_object_t * object the pointer to the Hessian object to destroy.
 */
void hessian_delete (hessian_object_t * object);

/**
 * Serializes the Hessian object into the output buffer.
 *
 * @param hessian_object_t * object the pointer to the Hessian object to serialize.
 * @param BUFFER * output pointer to the output buffer.
 *
 * @return HESSIAN_OK or HESSIAN_ERROR if an error occurs
 */
int hessian_serialize (const hessian_object_t * object, BUFFER * output);

/**
 * Deserializes an Hessian object from the input buffer. The first character
 * delimiter is directly read from the buffer.
 *
 * @param BUFFER * input pointer to the input buffer.
 *
 * @return hessian_object_t * pointer to the deserialized Hessian object
 *         or NULL if an error occurs.
 */
hessian_object_t * hessian_deserialize (BUFFER * input);

/**
 * Deserializes an Hessian object from the input buffer, identified with the
 * first tag character delimiter.
 *
 * @param BUFFER * input pointer to the input buffer.
 * @param int tag the first character delimiter.
 *
 * @return hessian_object_t * pointer to the deserialized Hessian object
 *         or NULL if an error occurs.
 */
hessian_object_t * hessian_deserialize_tag (int tag, BUFFER * input);

/**
 * Gets the type hessian_t of an object.
 *
 * @param const hessian_object_t * object pointer to the Hessian object.
 *
 * @return hessian_t the Hessian type or HESSIAN_UNKNOWN on error.
 */
hessian_t hessian_gettype (const hessian_object_t * object);

/**
 * Gets the class name of an object.
 *
 * @param const hessian_object_t * object pointer to the Hessian object.
 *
 * @return const char * the class name string or NULL if an error occurs.
 */
const char * hessian_getclassname (const hessian_object_t * object);

/**
 * Gets the Hessian class descriptor of an object.
 *
 * @param const hessian_object_t * object pointer to the Hessian object.
 *
 * @return const hessian_class_t * the pointer to the class descriptor
 *         or NULL if an error occurs.
 */
const hessian_class_t * hessian_getclass (const hessian_object_t * object);

/**
 * Hessian boolean getter.
 *
 * @param const hessian_object_t * object pointer to the Hessian boolean object.
 *
 * @return int TRUE or FALSE, or HESSIAN_ERROR if an error occurs.
 */
int hessian_boolean_getvalue(const hessian_object_t * boolean);

/**
 * Hessian 32-bit int getter
 *
 * @param const hessian_object_t * object pointer to the Hessian integer object.
 *
 * @return int32_t the 4 bytes integer value, 0 on error.
 */
int32_t hessian_integer_getvalue(const hessian_object_t * integer);

/**
 * Hessian 64-bit long getter
 *
 * @param const hessian_object_t * object pointer to the Hessian long object.
 *
 * @return int64_t the 8 bytes long value, 0 on error.
 */
int64_t hessian_long_getvalue(const hessian_object_t * h_long);

/**
 * Hessian 64-bit double getter
 *
 * @param const hessian_object_t * object pointer to the Hessian double object.
 *
 * @return double the 8 bytes double value, 0 on error.
 */
double hessian_double_getvalue(const hessian_object_t * h_double);

/**
 *  Hessian UTF-8 string getters
 */
size_t hessian_string_utf8_length(const hessian_object_t *string);
size_t hessian_string_length(const hessian_object_t *string);
const char * hessian_string_getstring(const hessian_object_t *string);

/**
 *  UTF-8 string utilities
 */
size_t utf8_strlen(const char *array);
char * utf8_bgets(size_t utf8_l, BUFFER * input);

/**
 *  Hessian XML getters
 */
size_t hessian_xml_utf8_length(const hessian_object_t * xml);
size_t hessian_xml_length(const hessian_object_t * xml);
const char * hessian_xml_getxml(const hessian_object_t * xml);

/**
 * Hessian binary getters
 */
size_t hessian_binary_length(const hessian_object_t * binary);
const char * hessian_binary_getdata(const hessian_object_t * binary);

/**
 * Hessian remote getters
 */
const char * hessian_remote_gettype(const hessian_object_t * remote);
const char * hessian_remote_geturl(const hessian_object_t * remote);

/**
 * Hessian date getter
 */
int64_t hessian_date_getvalue(const hessian_object_t * date);

/**
 * Sets the optional Hessian list type.
 */
int hessian_list_settype(hessian_object_t * list, const char * type);

/**
 * Returns the optional Hessian list type or NULL;
 */
const char * hessian_list_gettype(const hessian_object_t * list);

/**
 * Adds a Hessian object to the Hessian list.
 */
int hessian_list_add(hessian_object_t * list, hessian_object_t * object);

/**
 * Returns the length of the Hessian list.
 */
size_t hessian_list_length(const hessian_object_t * list);

/**
 * Returns the Hessian object at position i from the Hessian list. NULL if index > length.
 */
hessian_object_t * hessian_list_get(const hessian_object_t * list, int index);

/**
 * Hessian 32-bit ref getter
 */
int32_t hessian_ref_getvalue(const hessian_object_t * ref);

/**
 * Adds a Hessian <key,value> objects pair to the Hessian map.
 */
int hessian_map_add(hessian_object_t * map, hessian_object_t * key, hessian_object_t * value);
const char * hessian_map_gettype(const hessian_object_t * map);
size_t hessian_map_length(const hessian_object_t * map);
hessian_object_t * hessian_map_getkey(const hessian_object_t * map, int index);
hessian_object_t * hessian_map_getvalue(const hessian_object_t * map, int index);

/**
 * Stupid boolean constants
 */
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE  1
#endif

#ifdef  __cplusplus
}
#endif


#endif
