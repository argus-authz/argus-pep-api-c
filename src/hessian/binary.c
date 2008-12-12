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
 * $Id: binary.c,v 1.1 2008/12/12 11:33:43 vtschopp Exp $
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hessian/hessian.h"

/**
 * Method prototypes
 */
OBJECT_CTOR(hessian_binary);
OBJECT_DTOR(hessian_binary);
OBJECT_SERIALIZE(hessian_binary);
OBJECT_DESERIALIZE(hessian_binary);

/**
 * Initializes and registers the internal Hessian binary class descriptor.
 */
static const hessian_class_t _hessian_binary_descr = {
    HESSIAN_BINARY,
    "hessian.Binary",
    sizeof(hessian_binary_t),
    'B', 'b',
    hessian_binary_ctor,
    hessian_binary_dtor,
    hessian_binary_serialize,
    hessian_binary_deserialize
};
const void * hessian_binary_class = &_hessian_binary_descr;

/**
 * Hessian binary constructor.
 *
 * hessian_object_t * b= hessian_create(HESSIAN_BINARY,
 *                                      (size_t)size,
 *                                      (const char *)data);
 */
hessian_object_t * hessian_binary_ctor (hessian_object_t * object, va_list * ap) {
    hessian_binary_t * self= object;
    if (self == NULL) {
		fprintf(stderr,"ERROR:hessian_binary_ctor: NULL object pointer.\n");
    	return NULL;
    }
    size_t length= va_arg(*ap, size_t);
    const char * data = va_arg(*ap, const char *);
    if (data == NULL) {
		fprintf(stderr,"ERROR:hessian_binary_ctor: NULL data parameter 3.\n");
    	return NULL;
    }
    self->length= length;
    self->data= calloc(self->length,sizeof(char));
    if (self->data == NULL) {
		fprintf(stderr,"ERROR:hessian_binary_ctor: can't allocate data (%d bytes).\n",(int)self->length);
    	return NULL;
    }
    memcpy(self->data,data,length);
    return self;
}

/**
 * Hessian binary destructor.
 */
int hessian_binary_dtor (hessian_object_t * object) {
    hessian_binary_t * self= object;
    if (self == NULL) {
		fprintf(stderr,"ERROR:hessian_binary_dtor: NULL object pointer.\n");
    	return HESSIAN_ERROR;
    }
    if (self->data != NULL) free(self->data);
    self->data= NULL;
    self->length= 0;
    return HESSIAN_OK;
}

/**
 * hessian_binary deserialize method.
 */
int hessian_binary_deserialize (hessian_object_t * object, int tag, BUFFER * input) {
    hessian_binary_t * self= object;
    if (self == NULL) {
		fprintf(stderr,"ERROR:hessian_binary_deserialize: NULL object pointer.\n");
    	return HESSIAN_ERROR;
    }
    const hessian_class_t * class= hessian_getclass(object);
    if (class == NULL) {
    	fprintf(stderr,"ERROR:hessian_binary_deserialize: NULL class descriptor.\n");
    	return HESSIAN_ERROR;
    }
    if (class->type != HESSIAN_BINARY) {
    	fprintf(stderr,"ERROR:hessian_binary_deserialize: wrong class type: %d.\n", class->type);
    	return HESSIAN_ERROR;
    }
    // tag is 'x' for chunks and 'X' for final
    if (tag != class->tag && tag != class->chunk_tag) {
    	fprintf(stderr,"ERROR:hessian_binary_deserialize: wrong tag: %c.\n", (char)tag);
    	return HESSIAN_ERROR;
    }
	// use a buffer as tmp
    size_t buf_size= 32;
    if (tag == class->chunk_tag) buf_size= HESSIAN_CHUNK_SIZE;
	BUFFER * buf= buffer_create(buf_size);
    int fully_read= FALSE;
    while (!fully_read) {
        // read the binary length
		int b16= buffer_getc(input);
		int b8= buffer_getc(input);
		size_t bin_l= (b16 << 8) + b8;
		//printf("XXX:binary_deserialize: %c %ld (0x%0X%0X)\n", tag, bin_l, b16, b8);
		// fully read string (chunk)
		size_t n_bytes= 0;
		while(n_bytes < bin_l) {
			int b= buffer_getc(input);
			buffer_putc(b,buf);
			n_bytes++;
		}
		// was it final chunk?
		if (tag == class->chunk_tag) {
			tag= buffer_getc(input);
		    //printf("XXX:binary_deserialize: one more chunk: %c\n", tag);
		}
		else {
			// tag == class->tag (final)
			fully_read= TRUE;
		}
    }
    // copy the buffer into the hessian binary
    size_t buf_l= buffer_length(buf);
    //printf("XXX:binary_deserialize: buffer length: %ld\n", buf_l);
    self->length= buf_l;
    self->data= calloc(self->length,sizeof(char));
    if (self->data == NULL) {
    	fprintf(stderr,"ERROR:hessian_binary_deserialize: can't allocated data (%d bytes).\n", (int)self->length);
        buffer_delete(buf);
        return HESSIAN_ERROR;
    }
    buffer_read(self->data,sizeof(char),buf_l,buf);
    buffer_delete(buf);
    return HESSIAN_OK;
}

/**
 * hessian_binary serialize method.
 */
int hessian_binary_serialize (const hessian_object_t * object, BUFFER * output) {
    hessian_binary_t * self= (hessian_object_t *) object;
    if (self == NULL) {
		fprintf(stderr,"ERROR:hessian_binary_serialize: NULL object pointer.\n");
    	return HESSIAN_ERROR;
    }
    const hessian_class_t * class= hessian_getclass(object);
    if (class == NULL) {
    	fprintf(stderr,"ERROR:hessian_binary_serialize: NULL class descriptor.\n");
    	return HESSIAN_ERROR;
    }
    if (class->type != HESSIAN_BINARY) {
    	fprintf(stderr,"ERROR:hessian_binary_serialize: wrong class type: %d.\n",(int)(class->type));
    	return HESSIAN_ERROR;
    }
    size_t byte_l= self->length;
    size_t pos= 0;
    int b16, b8;
    while (byte_l > HESSIAN_CHUNK_SIZE) {
        // send binary chunks
        buffer_putc(class->chunk_tag,output);
        b16= HESSIAN_CHUNK_SIZE >> 8;
        b8= HESSIAN_CHUNK_SIZE & 0x00FF;
        buffer_putc(b16,output);
        buffer_putc(b8,output);
        // write HESSIAN_CHUNK_SIZE bytes
        const char *chunk= &(self->data[pos]);
        //printf("XXX:binary_serialize: write %d chunk bytes\n", HESSIAN_CHUNK_SIZE);
        buffer_write(chunk,1,HESSIAN_CHUNK_SIZE,output);
        pos= pos + HESSIAN_CHUNK_SIZE;
        byte_l= byte_l - HESSIAN_CHUNK_SIZE;
    }

    buffer_putc(class->tag,output);
    b16= byte_l >> 8;
    b8= byte_l & 0x00FF;
    buffer_putc(b16,output);
    buffer_putc(b8,output);
    const char *rest= &(self->data[pos]);
    //printf("XXX:binary_serialize: write %ld final bytes\n", byte_l);
    buffer_write(rest,1,byte_l,output);

    return HESSIAN_OK;
}

/**
 * Returns the byte length of the Hessian binary. returns HESSIAN_ERROR (-1) on error.
 */
size_t hessian_binary_length(const hessian_object_t * obj) {
    const hessian_binary_t * self= obj;
    if (self == NULL) {
    	fprintf(stderr,"ERROR:hessian_binary_length: NULL object pointer.\n");
    	return HESSIAN_ERROR;
    }
    const hessian_class_t * class= hessian_getclass(obj);
    if (class == NULL) {
    	fprintf(stderr,"ERROR:hessian_binary_length: NULL class descriptor.\n");
    	return HESSIAN_ERROR;
    }
    if (class->type != HESSIAN_BINARY) {
    	fprintf(stderr,"ERROR:hessian_binary_length: wrong class type: %d.\n", class->type);
    	return HESSIAN_ERROR;
    }
    return self->length;
}

/**
 * Returns the data of the Hessian binary.
 */
const char * hessian_binary_getdata(const hessian_object_t * obj) {
    const hessian_binary_t * self= obj;
    if (self == NULL) {
    	fprintf(stderr,"ERROR:hessian_binary_getdata: NULL pointer.\n");
    	return NULL;
    }
    const hessian_class_t * class= hessian_getclass(obj);
    if (class == NULL) {
    	fprintf(stderr,"ERROR:hessian_binary_getdata: no class descriptor.\n");
    	return NULL;
    }
    if (class->type != HESSIAN_BINARY) {
    	fprintf(stderr,"ERROR:hessian_binary_getdata: wrong class type: %d.\n", class->type);
    	return NULL;
    }
    return self->data;
}
