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
 * $Id: map.c,v 1.2 2009/01/29 15:27:04 vtschopp Exp $
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hessian/hessian.h"
#include "util/log.h"

/**
 * Method prototypes
 */
OBJECT_CTOR(hessian_map);
OBJECT_DTOR(hessian_map);
OBJECT_SERIALIZE(hessian_map);
OBJECT_DESERIALIZE(hessian_map);

/**
 * Initializes and registers the Hessian map class.
 */
static const hessian_class_t _hessian_map_descr = {
    HESSIAN_MAP,
    "hessian.Map",
    sizeof(hessian_map_t),
    'M', 'z',
    hessian_map_ctor,
    hessian_map_dtor,
    hessian_map_serialize,
    hessian_map_deserialize
};
const void * hessian_map_class = &_hessian_map_descr;

/**
 * Internal map <key,value> pair types
 */
typedef struct map_pair {
	hessian_object_t * key;
	hessian_object_t * value;
} map_pair_t;

map_pair_t * map_pair_create(hessian_object_t * key, hessian_object_t * value);
void map_pair_delete(map_pair_t * pair);


/**
 * Hessian map constructor. Creates an empty typed map.
 *
 * hessian_object_t * map= hessian_create(HESSIAN_MAP, "type");
 */
hessian_object_t * hessian_map_ctor (hessian_object_t * object, va_list * ap) {
    hessian_map_t * self= object;
    if (self == NULL) {
		log_error("hessian_map_ctor: NULL object pointer.");
    	return NULL;
    }
    const char * type= va_arg(*ap,const char *);
    if (type == NULL) {
		log_error("hessian_map_ctor: NULL type parameter 2.");
    	return NULL;
    }
    size_t type_l= strlen(type);
    self->type= calloc(type_l + 1, sizeof(char));
    if (self->type == NULL) {
		log_error("hessian_map_ctor: can't allocate type (%d chars).", (int)type_l);
    	return NULL;
    }
    strncpy(self->type,type,type_l);
    self->map= llist_create();
    if (self->map == NULL) {
		log_error("hessian_map_ctor: can't create map.");
		free(self->type);
    	return NULL;
    }
	return self;
}

/**
 * Hessian map destructor. Recursively delete the contained objects.
 */
int hessian_map_dtor (hessian_object_t * object) {
    hessian_map_t * self= object;
    if (self == NULL) {
		log_error("hessian_map_dtor: NULL object pointer.");
    	return HESSIAN_ERROR;
    }
    // free map pairs in one single list (references handling)
	linkedlist_t * keys_values= llist_create();
    if (keys_values == NULL) {
		log_error("hessian_map_dtor: can't create temp keys_values list.");
    	return HESSIAN_ERROR;
    }
    while( llist_length(self->map) > 0 ) {
    	map_pair_t * kv= (map_pair_t *)llist_remove(self->map,0);
    	if (kv != NULL) {
    		llist_add(keys_values,kv->key);
    		llist_add(keys_values,kv->value);
    		free(kv);
    	}
    }
    llist_delete_elements(keys_values,(delete_element_func)hessian_delete);
    llist_delete(keys_values);
    // should be empty
    llist_delete(self->map);
    if (self->type != NULL) free(self->type);
    return HESSIAN_OK;
}

/**
 * Hessian map serialize method.
 */
int hessian_map_serialize (const hessian_object_t * object, BUFFER * output) {
    const hessian_map_t * self= object;
    if (self == NULL) {
		log_error("hessian_map_serialize: NULL object pointer.");
    	return HESSIAN_ERROR;
    }
    const hessian_class_t * class= hessian_getclass(object);
    if (class == NULL) {
		log_error("hessian_map_serialize: NULL class descriptor.");
    	return HESSIAN_ERROR;
    }
    if (class->type != HESSIAN_MAP) {
		log_error("hessian_map_serialize: wrong class type: %d.",class->type);
    	return HESSIAN_ERROR;
    }
    buffer_putc(class->tag,output);
    size_t str_l, utf8_l;
    int b16, b8;
    // write type if any
    if (self->type != NULL) {
        //printf("XXX:map_serialize: type: %s",self->type);
        str_l= strlen(self->type);
        utf8_l= utf8_strlen(self->type);
        b16= utf8_l >> 8;
        b8= utf8_l & 0x00FF;
        buffer_putc('t',output);
        buffer_putc(b16,output);
        buffer_putc(b8,output);
        buffer_write(self->type,1,str_l,output);
    }

    // write all <key,value> pair
    size_t map_l= llist_length(self->map);
    int i= 0;
    for( i= 0; i < map_l; i++ ) {
    	map_pair_t * kv= (map_pair_t *)llist_get(self->map,i);
    	if (kv==NULL) {
    		log_error("hessian_map_serialize: NULL map pair<key,value> at %d.",i);
    		return HESSIAN_ERROR;
    	}
        hessian_object_t * key= kv->key;
        if (hessian_serialize(key, output) != HESSIAN_OK) {
    		log_error("hessian_map_serialize: failed to serialize pair<key> at %d.",i);
    		return HESSIAN_ERROR;
        }
        hessian_object_t * value= kv->value;
        if (hessian_serialize(value, output) != HESSIAN_OK) {
    		log_error("hessian_map_serialize: failed to serialize pair<value> at %d.",i);
    		return HESSIAN_ERROR;
        }
    }

    // end of map
    buffer_putc(class->chunk_tag,output);
    return HESSIAN_OK;
}

/**
 * Hessian map deserialize method.
 */
int hessian_map_deserialize (hessian_object_t * object, int tag, BUFFER * input) {
    hessian_map_t * self= object;
    if (self == NULL) {
		log_error("hessian_map_deserialize: NULL object pointer.");
    	return HESSIAN_ERROR;
    }
    const hessian_class_t * class= hessian_getclass(object);
    if (class == NULL) {
		log_error("hessian_map_deserialize: NULL class descriptor.");
    	return HESSIAN_ERROR;
    }
    if (class->type != HESSIAN_MAP) {
		log_error("hessian_map_deserialize: wrong class type: %d.",class->type);
    	return HESSIAN_ERROR;
    }
    // tag is 'M'
    if (tag != class->tag) {
		log_error("hessian_map_deserialize: invalid tag: %c (%d).",(char)tag,tag);
    	return HESSIAN_ERROR;
    }
    // alloc the refs list for Hessian ref handling
    linkedlist_t * refs= llist_create();
    if (refs == NULL) {
		log_error("hessian_map_deserialize: can't create temp references list.");
    	return HESSIAN_ERROR;
    }
    // begin parsing
    int next_tag= buffer_getc(input);
	//printf("XXX:hessian_map_deserialize: next tag: %c", next_tag);
	// map type is optional or not?
	self->type= NULL;
	if (next_tag == 't') {
	    // read the utf8 type length
		int b16= buffer_getc(input);
		int b8= buffer_getc(input);
		size_t utf8_l= (b16 << 8) + b8;
		// TODO: handle empty type (0 length)
		char * type= utf8_bgets(utf8_l,input);
		if (type == NULL) {
			log_error("hessian_map_deserialize: can't read map type: %d chars.", (int)utf8_l);
			llist_delete(refs);
			return HESSIAN_ERROR;
		}
		//printf("XXX:hessian_map_deserialize: type: %s", type);
		self->type= type;
		next_tag= buffer_getc(input);
		//printf("XXX:hessian_map_deserialize: next tag: %c", next_tag);
	}
    // do until tag != 'z'
    while( next_tag != class->chunk_tag && next_tag != BUFFER_EOF) {
    	// standard Hessian object, add to the refs list.
    	hessian_object_t * key= hessian_deserialize_tag(next_tag,input);
		if (key == NULL) {
			log_error("hessian_map_deserialize: can't deserialize map pair<key> with tag: %c.", next_tag);
			llist_delete_elements(refs,(delete_element_func)map_pair_delete);
			llist_delete(refs);
			return HESSIAN_ERROR;
		}
		next_tag= buffer_getc(input);
    	//printf("XXX:hessian_map_deserialize: key: %s", hessian_getclassname(key));
    	hessian_object_t * value= hessian_deserialize_tag(next_tag,input);
		if (value == NULL) {
			log_error("hessian_map_deserialize: can't deserialize map pair<value> with tag: %c.", next_tag);
			hessian_delete(key);
			llist_delete_elements(refs,(delete_element_func)map_pair_delete);
			llist_delete(refs);
			return HESSIAN_ERROR;
		}
    	//printf("XXX:hessian_map_deserialize: value: %s", hessian_getclassname(value));

        map_pair_t * pair= map_pair_create(key,value);
        if (pair == NULL) {
			log_error("hessian_map_deserialize: can't create map pair<key,value>.");
			hessian_delete(key);
			hessian_delete(value);
			llist_delete_elements(refs,(delete_element_func)map_pair_delete);
			llist_delete(refs);
			return HESSIAN_ERROR;
        }
    	if (llist_add(refs,pair) != LLIST_OK) {
			log_error("hessian_map_deserialize: can't add map pair<key,value> to temp references list.");
			hessian_delete(key);
			hessian_delete(value);
			free(pair);
			llist_delete_elements(refs,(delete_element_func)map_pair_delete);
			llist_delete(refs);
			return HESSIAN_ERROR;
    	}

    	next_tag= buffer_getc(input);
    	//printf("XXX:hessian_map_deserialize: next tag: %c", next_tag);
    }
    // alloc the objects list and fill with element from the refs lists.
    self->map= llist_create();
    if(self->map == NULL) {
		log_error("hessian_map_deserialize: can't create map pairs list.");
		llist_delete_elements(refs,(delete_element_func)map_pair_delete);
		llist_delete(refs);
		return HESSIAN_ERROR;
    }
    // references handling, replace ref object by real referenced object.
    size_t refs_l= llist_length(refs);
    int i= 0;
    for (i= 0; i < refs_l; i++) {
    	map_pair_t * pair= llist_get(refs,i);
        if(pair == NULL) {
    		log_error("hessian_map_deserialize: NULL map pair in temp references list at: %d.",i);
    		llist_delete_elements(refs,(delete_element_func)map_pair_delete);
    		llist_delete(refs);
    		return HESSIAN_ERROR;
        }
    	// handle the ref value
    	if (hessian_gettype(pair->value) == HESSIAN_REF) {
    		hessian_object_t * ref= pair->value;
    		// get the ref index
    		int ref_index= hessian_ref_getvalue(ref);
    		// get the real value
    		map_pair_t * real_pair= llist_get(refs,ref_index);
    		if (real_pair != NULL) {
				hessian_delete(ref); // ref object not needed anymore
    			pair->value= real_pair->value;
    		}
    		else {
    			log_warn("hessian_map_deserialize: ref object at %d reference NULL pair at: %d.",i,ref_index);
    		}
        	//printf("XXX:hessian_map_deserialize: ref: %d value: %s", ref, hessian_getclassname(pair->value));
    	}
    	if (llist_add(self->map,pair) != LLIST_OK) {
			log_error("hessian_map_deserialize: can't add map pair<key,value> to pairs list.");
			// FIXME: delete list content! see dtor...
			llist_delete(refs);
    		llist_delete(self->map);
    		return HESSIAN_ERROR;
    	}
    }

    llist_delete(refs);
    return HESSIAN_OK;
}

// value can be NULL
int hessian_map_add(hessian_object_t * object, hessian_object_t * key, hessian_object_t * value) {
    hessian_map_t * self= object;
    if (self == NULL) {
		log_error("hessian_map_add: NULL object pointer.");
    	return HESSIAN_ERROR;
    }
    const hessian_class_t * class= hessian_getclass(object);
    if (class == NULL) {
		log_error("hessian_map_add: NULL class descriptor.");
    	return HESSIAN_ERROR;
    }
    if (class->type != HESSIAN_MAP) {
		log_error("hessian_map_add: wrong class type: %d.",class->type);
    	return HESSIAN_ERROR;
    }
    map_pair_t * pair= map_pair_create(key,value);
    if (pair == NULL) {
		log_error("hessian_map_add: can't create map pair<key,value>.");
    	return HESSIAN_ERROR;
    }
    if (llist_add(self->map,pair) != LLIST_OK) {
		log_error("hessian_map_add: can't add map pair<key,value> to list.");
		free(pair);
    	return HESSIAN_ERROR;
    }
    return HESSIAN_OK;
}

/**
 * Sets the optional Hessian map type.
 */
int hessian_map_settype(hessian_object_t * object, const char * type) {
    hessian_map_t * self= object;
    if (self == NULL) {
		log_error("hessian_map_settype: NULL object pointer.");
    	return HESSIAN_ERROR;
    }
    const hessian_class_t * class= hessian_getclass(object);
    if (class == NULL) {
		log_error("hessian_map_settype: NULL class descriptor.");
    	return HESSIAN_ERROR;
    }
    if (class->type != HESSIAN_MAP) {
		log_error("hessian_map_settype: wrong class type: %d.",class->type);
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
    		log_error("hessian_map_settype: can't allocate type (%d chars).",(int)type_l);
        	return HESSIAN_ERROR;
        }
        strncpy(self->type,type,type_l);
    }
    else {
    	self->type= NULL;
    }
    return HESSIAN_OK;
}

const char * hessian_map_gettype(const hessian_object_t * object) {
    const hessian_map_t * self= object;
    if (self == NULL) {
		log_error("hessian_map_gettype: NULL object pointer.");
    	return NULL;
    }
    const hessian_class_t * class= hessian_getclass(object);
    if (class == NULL) {
		log_error("hessian_map_gettype: NULL class descriptor.");
    	return NULL;
    }
    if (class->type != HESSIAN_MAP) {
		log_error("hessian_map_gettype: wrong class type: %d.",class->type);
    	return NULL;
    }
    return self->type;

}

// returns HESSIAN_ERROR (-1) on error.
size_t hessian_map_length(const hessian_object_t * object) {
    const hessian_map_t * self= object;
    if (self == NULL) {
		log_error("hessian_map_length: NULL object pointer.");
    	return HESSIAN_ERROR;
    }
    const hessian_class_t * class= hessian_getclass(object);
    if (class == NULL) {
		log_error("hessian_map_length: NULL class descriptor.");
    	return HESSIAN_ERROR;
    }
    if (class->type != HESSIAN_MAP) {
		log_error("hessian_map_length: wrong class type: %d.",class->type);
    	return HESSIAN_ERROR;
    }
    return llist_length(self->map);
}

hessian_object_t * hessian_map_getkey(const hessian_object_t * object, int index) {
    const hessian_map_t * self= object;
    if (self == NULL) {
		log_error("hessian_map_getkey: NULL object pointer.");
    	return NULL;
    }
    const hessian_class_t * class= hessian_getclass(object);
    if (class == NULL) {
		log_error("hessian_map_getkey: NULL class descriptor.");
    	return NULL;
    }
    if (class->type != HESSIAN_MAP) {
		log_error("hessian_map_getkey: wrong class type: %d.",class->type);
    	return NULL;
    }
    map_pair_t * pair= llist_get(self->map,index);
    if (pair == NULL) {
		log_error("hessian_map_getkey: NULL map pair<key,value> at: %d.",index);
    	return NULL;
    }
    else return pair->key;
}

hessian_object_t * hessian_map_getvalue(const hessian_object_t * object, int index) {
    const hessian_map_t * self= object;
    if (self == NULL) {
		log_error("hessian_map_getkey: NULL object pointer.");
    	return NULL;
    }
    const hessian_class_t * class= hessian_getclass(object);
    if (class == NULL) {
		log_error("hessian_map_getkey: NULL class descriptor.");
    	return NULL;
    }
    if (class->type != HESSIAN_MAP) {
		log_error("hessian_map_getkey: wrong class type: %d.",class->type);
    	return NULL;
    }
    map_pair_t * pair= llist_get(self->map,index);
    if (pair == NULL) {
		log_error("hessian_map_getkey: NULL map pair<key,value> at: %d.",index);
    	return NULL;
    }
    else return pair->value;
}

/**
 * Create a map pair<key,value>. If the value is NULL an Hessian null object is added.
 *
 * @param const hessian_object_t * key not NULL key object.
 * @param const hessian_object_t * value object, can be NULL.
 * @return map_pair_t * pointer to the map pair<key,value> or NULL if an error occurs.
 */
map_pair_t * map_pair_create(hessian_object_t * key, hessian_object_t * value) {
	map_pair_t * pair= calloc(1,sizeof(map_pair_t));
	if (pair == NULL) {
		log_error("map_pair_create: can't allocate map pair.");
		return NULL;
	}
	if (key == NULL) {
		log_error("map_pair_create: NULL key.");
		free(pair);
		return NULL;
	}
	pair->key= key;
	if (value == NULL) {
		pair->value= hessian_create(HESSIAN_NULL);
	}
	else {
		pair->value= value;
	}
	return pair;
}

void map_pair_delete(map_pair_t * pair) {
	if (pair == NULL) return;
	hessian_delete(pair->key);
	hessian_delete(pair->value);
	free(pair);
	pair= NULL;
}

