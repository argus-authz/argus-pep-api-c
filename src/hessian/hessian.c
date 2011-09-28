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
#include <string.h>
#include <stdio.h>

#include "hessian.h"
#include "log.h"

/**
 * Hessian class descriptors
 */
extern const void * hessian_boolean_class;
extern const void * hessian_integer_class;
extern const void * hessian_long_class;
extern const void * hessian_double_class;
extern const void * hessian_string_class;
extern const void * hessian_xml_class;
extern const void * hessian_binary_class;
extern const void * hessian_remote_class;
extern const void * hessian_date_class;
extern const void * hessian_list_class;
extern const void * hessian_map_class;
extern const void * hessian_ref_class;
extern const void * hessian_null_class;


/**
 * Returns the class descriptor for the given type or NULL.
 */
static const hessian_class_t * _getclass(hessian_t type) {
	const hessian_class_t * class = NULL;
	switch (type) {
	case HESSIAN_BOOLEAN:
		class = hessian_boolean_class;
		break;
	case HESSIAN_INTEGER:
		class = hessian_integer_class;
		break;
	case HESSIAN_LONG:
		class = hessian_long_class;
		break;
	case HESSIAN_DOUBLE:
		class = hessian_double_class;
		break;
	case HESSIAN_STRING:
		class = hessian_string_class;
		break;
	case HESSIAN_XML:
		class = hessian_xml_class;
		break;
	case HESSIAN_BINARY:
		class = hessian_binary_class;
		break;
	case HESSIAN_REMOTE:
		class = hessian_remote_class;
		break;
	case HESSIAN_DATE:
		class = hessian_date_class;
		break;
	case HESSIAN_LIST:
		class = hessian_list_class;
		break;
	case HESSIAN_MAP:
		class = hessian_map_class;
		break;
	case HESSIAN_REF:
		class = hessian_ref_class;
		break;
	case HESSIAN_NULL:
		class = hessian_null_class;
		break;
	default:
		log_error("_getclass: unknown hessian_t type: %d", type);
		class = NULL;
		break;

	}
	return class;
}

/**
 * Returns the hessian_t for the given serialization tag or HESSIAN_UNKNOWN.
 */
static hessian_t _gettype(int tag) {
	hessian_t type= HESSIAN_UNKNOWN;
	switch (tag) {
	case 'N':
		type= HESSIAN_NULL;
		break;
	case 'T':
	case 'F':
		type= HESSIAN_BOOLEAN;
		break;
	case 'I':
		type= HESSIAN_INTEGER;
		break;
	case 'L':
		type= HESSIAN_LONG;
		break;
	case 'D':
		type= HESSIAN_DOUBLE;
		break;
	case 'd':
		type= HESSIAN_DATE;
		break;
	case 's':
	case 'S':
		type= HESSIAN_STRING;
		break;
	case 'x':
	case 'X':
		type= HESSIAN_XML;
		break;
	case 'b':
	case 'B':
		type= HESSIAN_BINARY;
		break;
	case 'V':
		type= HESSIAN_LIST;
		break;
	case 'M':
		type= HESSIAN_MAP;
		break;
	case 'R':
		type= HESSIAN_REF;
		break;
	case 'r':
		type= HESSIAN_REMOTE;
		break;
	default:
		log_error("_gettype: unknown tag: %c (0x%0X)", tag, tag);
		type= HESSIAN_UNKNOWN;
		break;
	}
	return type;
}

/**
 * Creates an Hessian object.
 *
 * Returns the Hessian object or NULL
 */
hessian_object_t * hessian_create(hessian_t type, ...) {
	const hessian_class_t * class = _getclass(type);
    void * object;
	if (class == NULL) {
		log_error("hessian_create: no class descriptor for type: %d", (int)type);
		return NULL;
	}
	object = calloc(1, class->size);
	if (object == NULL) {
		log_error("hessian_create: can't allocate object descriptor (%d bytes).", (int)class->size);
		return NULL;
	}
	/* first memory element of object is the class descriptor pointer */
	*(const hessian_class_t **) object = class;
	/* call constructor if any */
	if (class->ctor) {
		va_list ap;
		va_start(ap, type);
		if ( class->ctor(object, &ap) == NULL ) {
			log_error("hessian_create: object constructor failed.");
			free(object);
			object= NULL;
		}
		va_end(ap);
	}
	return object;
}

/**
 * Delete an Hessian object.
 */
void hessian_delete(hessian_object_t * object) {
	const hessian_class_t * class;
	if (object == NULL) return;
	class = hessian_getclass(object);
	if (class == NULL) {
		log_error("hessian_delete: no class descriptor.");
		return;
	}
	if (class->dtor) {
		if ( class->dtor(object) == HESSIAN_ERROR ) {
			log_error("hessian_delete: object destructor failed.");
		}
	}
	if (object != NULL) free(object);
	object= NULL;
}

int hessian_serialize(const hessian_object_t * object, BUFFER * output) {
	const hessian_class_t * class = hessian_getclass(object);
	if (class == NULL) {
		log_error("hessian_serialize: NULL class descriptor.");
		return HESSIAN_ERROR;
	}
	if (class->serialize) {
		return class->serialize(object, output);
	}
	else {
		log_error("hessian_serialize: No serializer defined for class %s",
				class->name);
		return HESSIAN_ERROR;
	}
    return HESSIAN_OK;
}

hessian_object_t * hessian_deserialize(BUFFER * input) {
	int tag= buffer_getc(input);
	return hessian_deserialize_tag(tag,input);
}

hessian_object_t * hessian_deserialize_tag(int tag, BUFFER * input) {
	hessian_t type= _gettype(tag);
	const hessian_class_t * class;
    void * object;
	if (type == HESSIAN_UNKNOWN) {
		log_error("hessian_deserialize: unknown serialization tag: %c", tag );
		return NULL;
	}
	class = _getclass(type);
	if (class == NULL) {
		log_error("hessian_deserialize: NULL class for tag: %c", tag );
		return NULL;
	}
	/* allocate the object class descriptor */
	object = calloc(1, class->size);
	if (object == NULL) {
		log_error("hessian_deserialize: can't allocate object (%d bytes)", (int)class->size );
		return NULL;
	}
	/* first memory element of object is the class pointer */
	*(const hessian_class_t **) object = class;
	/* deserialize the object */
	if (class->deserialize) {
		if (class->deserialize(object, tag, input) == HESSIAN_OK) return object;
		else {
			log_error("hessian_deserialize: failed to deserialize object: %s tag: %c", class->name, tag);
			return NULL;
		}
	}
	else {
		log_error("hessian_deserialize: No deserializer defined for class %s",
				class->name);
		return NULL;
	}
}

/*******************************************************/

const hessian_class_t * hessian_getclass(const hessian_object_t * object) {
	/* get the class pointer, first memory pointer of the struct */
	const hessian_class_t * const *cp = object;
	if (object == NULL) {
		log_error("hessian_getclass: NULL pointer object.");
		return NULL;
	}
	return (*cp);
}

const char * hessian_getclassname(const hessian_object_t * object) {
	const hessian_class_t * class= hessian_getclass(object);
	if (class == NULL) {
		log_error("hessian_getclassname: class descriptor is NULL.");
		return NULL;
	}
	return class->name;
}

hessian_t hessian_gettype(const hessian_object_t * object) {
	const hessian_class_t * class= hessian_getclass(object);
	if (class == NULL) {
		log_error("hessian_gettype: class descriptor is NULL.");
		return HESSIAN_UNKNOWN;
	}
	return class->type;
}

/*******************************************************/

