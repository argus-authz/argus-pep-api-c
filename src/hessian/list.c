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
 * $Id: list.c,v 1.3 2009/01/29 15:23:14 vtschopp Exp $
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hessian/hessian.h"
#include "util/linkedlist.h"
#include "util/log.h"


/**
 * Method prototypes
 */
static OBJECT_CTOR(hessian_list);
static OBJECT_DTOR(hessian_list);
static OBJECT_SERIALIZE(hessian_list);
static OBJECT_DESERIALIZE(hessian_list);

/**
 * Initializes and registers the Hessian list class.
 */
static const hessian_class_t _hessian_list_descr = {
    HESSIAN_LIST,
    "hessian.List",
    sizeof(hessian_list_t),
    'V', 'z',
    hessian_list_ctor,
    hessian_list_dtor,
    hessian_list_serialize,
    hessian_list_deserialize
};
const void * hessian_list_class = &_hessian_list_descr;


/**
 * Hessian list constructor. Creates an empty Hessian untyped list.
 *
 * hessian_object_t * list= hessian_create(HESSIAN_LIST);
 */
static hessian_object_t * hessian_list_ctor (hessian_object_t * list, va_list * ap) {
    hessian_list_t * self= list;
    if (self == NULL) {
		log_error("hessian_list_ctor: NULL object pointer.");
    	return NULL;
    }
    self->type= NULL;
    self->list= llist_create();
    if (self->list == NULL) {
		log_error("hessian_list_ctor: can't create list.");
    	return NULL;
    }
    return self;
}


/**
 * Hessian list destructor. Recursively delete the contained objects.
 */
static int hessian_list_dtor (hessian_object_t * list) {
    hessian_list_t * self= list;
    if (self == NULL) {
		log_error("hessian_list_dtor: NULL object pointer.");
    	return HESSIAN_ERROR;
    }
	//printf("XXX:list_dtor:%d elts",(int)llist_length(self->list));
    if (self->type != NULL) free(self->type);
    llist_delete_elements(self->list,(delete_element_func)hessian_delete);
    llist_delete(self->list);
    return HESSIAN_OK;
}


/**
 * Adds an hessian_object_t * at the end of the list
 */
int hessian_list_add(hessian_object_t * list, hessian_object_t * object) {
    hessian_list_t * self= list;
    if (self == NULL) {
		log_error("hessian_list_add: NULL object pointer.");
    	return HESSIAN_ERROR;
    }
    const hessian_class_t * class= hessian_getclass(list);
    if (class == NULL) {
		log_error("hessian_list_add: NULL class descriptor.");
    	return HESSIAN_ERROR;
    }
    if (class->type != HESSIAN_LIST) {
		log_error("hessian_list_add: wrong class type: %d.",class->type);
    	return HESSIAN_ERROR;
    }
    if (llist_add(self->list, object) != LLIST_OK) {
		log_error("hessian_list_add: can't add object to list.");
    	return HESSIAN_ERROR;
    }
    return HESSIAN_OK;
}

/**
 * Hessian list serialize method.
 */
static int hessian_list_serialize (const hessian_object_t * list, BUFFER * output) {
    const hessian_list_t * self= list;
    if (self == NULL) {
		log_error("hessian_list_add: NULL object pointer.");
    	return HESSIAN_ERROR;
    }
    const hessian_class_t * class= hessian_getclass(list);
    if (class == NULL) {
		log_error("hessian_list_add: NULL class descriptor.");
    	return HESSIAN_ERROR;
    }
    if (class->type != HESSIAN_LIST) {
		log_error("hessian_list_add: wrong class type: %d.",class->type);
    	return HESSIAN_ERROR;
    }

    buffer_putc(class->tag,output);
    size_t str_l, utf8_l;
    int b32, b24, b16, b8;
    // write type if any
    if (self->type != NULL) {
        //printf("XXX:list_serialize: type: %s",self->type);
        str_l= strlen(self->type);
        utf8_l= utf8_strlen(self->type);
        b16= utf8_l >> 8;
        b8= utf8_l & 0x00FF;
        buffer_putc('t',output);
        buffer_putc(b16,output);
        buffer_putc(b8,output);
        buffer_write(self->type,1,str_l,output);
    }
    // write length if any
    size_t list_l= llist_length(self->list);
    if (list_l > 0) {
        //printf("XXX:list_serialize: length: %ld",list_l);
        int32_t value= (int32_t)list_l;
        b32 = (value >> 24) & 0x000000FF;
        b24 = (value >> 16) & 0x000000FF;
        b16 = (value >> 8) & 0x000000FF;
        b8 = value & 0x000000FF;
        buffer_putc('l',output);
        buffer_putc(b32,output);
        buffer_putc(b24,output);
        buffer_putc(b16,output);
        buffer_putc(b8,output);
    }
    // write all objects
    int i= 0;
    for( i= 0; i < list_l; i++ ) {
    	hessian_object_t * object= llist_get(self->list,i);
    	if (object == NULL) {
    		log_error("hessian_list_add: NULL object pointer at: %d.",i);
        	return HESSIAN_ERROR;
    	}
        if (hessian_serialize(object, output) != HESSIAN_OK) {
    		log_error("hessian_list_add: can't serialize object at: %d.",i);
        	return HESSIAN_ERROR;
        }
    }

    buffer_putc(class->chunk_tag,output);
    return HESSIAN_OK;
}

/**
 * Hessian list deserialize method.
 */
static int hessian_list_deserialize (hessian_object_t * list, int tag, BUFFER * input) {
    hessian_list_t * self= list;
    if (self == NULL) {
		log_error("hessian_list_deserialize: NULL object pointer.");
    	return HESSIAN_ERROR;
    }
    const hessian_class_t * class= hessian_getclass(list);
    if (class == NULL) {
		log_error("hessian_list_deserialize: NULL class descriptor.");
    	return HESSIAN_ERROR;
    }
    if (class->type != HESSIAN_LIST) {
		log_error("hessian_list_deserialize: wrong class type: %d.",class->type);
    	return HESSIAN_ERROR;
    }
    // tag is 'V'
    if (tag != class->tag) {
		log_error("hessian_list_deserialize: invalid tag: %c (%d).",(char)tag,tag);
    	return HESSIAN_ERROR;
    }
    int32_t length= -1;
    // alloc the refs list for Hessian ref handling;
    linkedlist_t * refs= llist_create();
    if (refs == NULL) {
		log_error("hessian_list_deserialize: can't create temp references list.");
    	return HESSIAN_ERROR;
    }
    // begin parsing
    int next_tag= buffer_getc(input);
	//printf("XXX:hessian_list_deserialize: next tag: %c", next_tag);
    // optional type
    if (next_tag == 't') {
        // read the utf8 type length
   		int b16= buffer_getc(input);
   		int b8= buffer_getc(input);
		size_t utf8_l= (b16 << 8) + b8;
		char * type= utf8_bgets(utf8_l,input);
		if (type == NULL) {
			log_error("hessian_list_deserialize: can't read list type: %d chars.", (int)utf8_l);
			llist_delete(refs);
			return HESSIAN_ERROR;
		}
		//printf("XXX:hessian_list_deserialize: type: %s", type);
		self->type= type;
		next_tag= buffer_getc(input);
	}
	// optional length, unused.
	if (next_tag == 'l') {
		// read int32, don't do anything with it...
		int32_t b32 = buffer_getc(input);
		int32_t b24 = buffer_getc(input);
		int32_t b16 = buffer_getc(input);
		int32_t b8 = buffer_getc(input);
		length= (b32 << 24) + (b24 << 16) + (b16 << 8) + b8;
		//printf("XXX:hessian_list_deserialize: length: %d", length);
		next_tag= buffer_getc(input);
	}
    // do until tag != 'z'
    while( next_tag != class->chunk_tag && next_tag != BUFFER_EOF) {
		// standard Hessian object, add to the refs list.
		hessian_object_t * o= hessian_deserialize_tag(next_tag,input);
		if (o == NULL) {
			log_error("hessian_list_deserialize: can't deserialize object with tag: %c.", next_tag);
			llist_delete_elements(refs,(delete_element_func)hessian_delete);
			llist_delete(refs);
			return HESSIAN_ERROR;
		}
		//printf("XXX:hessian_list_deserialize: object: %s", hessian_getclassname(o));
		if (llist_add(refs,o) != LLIST_OK) {
			log_error("hessian_list_deserialize: can't add object to temp references list.");
			hessian_delete(o);
			llist_delete_elements(refs,(delete_element_func)hessian_delete);
			llist_delete(refs);
			return HESSIAN_ERROR;
		}
    	next_tag= buffer_getc(input);
    	//printf("XXX:hessian_list_deserialize: next tag: %c", next_tag);
    }

    // alloc the objects list and fill with element from the refs lists.
    self->list= llist_create();
    if (self->list == NULL) {
		log_error("hessian_list_deserialize: can't create list.");
		llist_delete_elements(refs,(delete_element_func)hessian_delete);
		llist_delete(refs);
    	return HESSIAN_ERROR;
    }
    size_t refs_l= llist_length(refs);
    int i= 0;
    for (i= 0; i < refs_l; i++) {
    	hessian_object_t * o= (hessian_object_t *)llist_get(refs,i);
		if (o == NULL) {
			log_error("hessian_list_deserialize: NULL object in refs at: %d.",i);
			llist_delete_elements(refs,(delete_element_func)hessian_delete);
			llist_delete(refs);
	    	return HESSIAN_ERROR;
		}
    	// handle the ref object
    	if (hessian_gettype(o) == HESSIAN_REF) {
    		// get the ref index
    		int ref_index= hessian_ref_getvalue(o);
    		hessian_delete(o); // not needed anymore
    		// get the real object
    		o= (hessian_object_t *)llist_get(refs,ref_index);
    		if (o == NULL) {
    			log_error("hessian_list_deserialize: NULL referenced object in refs at: %d.",ref_index);
    			llist_delete_elements(refs,(delete_element_func)hessian_delete);
    			llist_delete(refs);
    	    	return HESSIAN_ERROR;
    		}
        	//printf("XXX:hessian_list_deserialize: ref: %d object: %s", ref, hessian_getclassname(o));
    	}
    	if (llist_add(self->list,o) != LLIST_OK) {
			log_error("hessian_list_deserialize: can't add object to list.");
			hessian_delete(o);
			llist_delete_elements(refs,(delete_element_func)hessian_delete);
			llist_delete(refs);
	    	return HESSIAN_ERROR;
    	}
    }
    llist_delete(refs);
    return HESSIAN_OK;
}

/**
 * Sets the optional Hessian list type.
 */
int hessian_list_settype(hessian_object_t * list, const char * type) {
    hessian_list_t * self= list;
    if (self == NULL) {
		log_error("hessian_list_settype: NULL object pointer.");
    	return HESSIAN_ERROR;
    }
    const hessian_class_t * class= hessian_getclass(list);
    if (class == NULL) {
		log_error("hessian_list_settype: NULL class descriptor.");
    	return HESSIAN_ERROR;
    }
    if (class->type != HESSIAN_LIST) {
		log_error("hessian_list_settype: wrong class type: %d.",class->type);
    	return HESSIAN_ERROR;
    }
    // free if already set
    if (self->type != NULL) {
    	free(self->type);
    	self->type= NULL;
    }
    if (type != NULL) {
        size_t type_l= strlen(type);
        self->type= calloc(type_l + 1, sizeof(char));
        if (self->type == NULL) {
    		log_error("hessian_list_settype: can't allocate type (%d chars).",(int)type_l);
        	return HESSIAN_ERROR;
        }
        strncpy(self->type,type,type_l);
    }
    else {
    	self->type= NULL;
    }
    return HESSIAN_OK;
}

/**
 * Returns the optional Hessian list type or NULL;
 */
const char * hessian_list_gettype(const hessian_object_t * list) {
    const hessian_list_t * self= list;
    if (self == NULL) {
		log_error("hessian_list_gettype: NULL object pointer.");
    	return NULL;
    }
    const hessian_class_t * class= hessian_getclass(list);
    if (class == NULL) {
		log_error("hessian_list_gettype: NULL class descriptor.");
    	return NULL;
    }
    if (class->type != HESSIAN_LIST) {
		log_error("hessian_list_gettype: wrong class type: %d.",class->type);
    	return NULL;
    }
    return self->type;
}

/**
 * Returns the length of the Hessian list. returns HESSIAN_ERROR (-1) on error.
 */
size_t hessian_list_length(const hessian_object_t * list) {
    const hessian_list_t * self= list;
    if (self == NULL) {
		log_error("hessian_list_settype: NULL object pointer.");
    	return HESSIAN_ERROR;
    }
    const hessian_class_t * class= hessian_getclass(list);
    if (class == NULL) {
		log_error("hessian_list_settype: NULL class descriptor.");
    	return HESSIAN_ERROR;
    }
    if (class->type != HESSIAN_LIST) {
		log_error("hessian_list_settype: wrong class type: %d.",class->type);
    	return HESSIAN_ERROR;
    }
    return llist_length(self->list);
}

/**
 * Returns the Hessian object at index from the Hessian list. NULL if index > length.
 */
hessian_object_t * hessian_list_get(const hessian_object_t * list, int index) {
    const hessian_list_t * self= list;
    if (self == NULL) {
		log_error("hessian_list_gettype: NULL object pointer.");
    	return NULL;
    }
    const hessian_class_t * class= hessian_getclass(list);
    if (class == NULL) {
		log_error("hessian_list_gettype: NULL class descriptor.");
    	return NULL;
    }
    if (class->type != HESSIAN_LIST) {
		log_error("hessian_list_gettype: wrong class type: %d.",class->type);
    	return NULL;
    }
    return (hessian_object_t *) llist_get(self->list,index);
}

