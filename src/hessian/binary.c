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

/**
 * Method prototypes
 */
static OBJECT_CTOR(hessian_binary);
static OBJECT_DTOR(hessian_binary);
static OBJECT_SERIALIZE(hessian_binary);
static OBJECT_DESERIALIZE(hessian_binary);

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
static hessian_object_t * hessian_binary_ctor (hessian_object_t * object, va_list * ap) {
    hessian_binary_t * self= object;
    size_t length= va_arg(*ap, size_t);
    const char * data = va_arg(*ap, const char *);
    if (self == NULL) {
        pep_log_error("hessian_binary_ctor: NULL object pointer.");
        return NULL;
    }
    if (data == NULL) {
        pep_log_error("hessian_binary_ctor: NULL data parameter 3.");
        return NULL;
    }
    self->length= length;
    self->data= calloc(self->length,sizeof(char));
    if (self->data == NULL) {
        pep_log_error("hessian_binary_ctor: can't allocate data (%d bytes).",(int)self->length);
        return NULL;
    }
    memcpy(self->data,data,length);
    return self;
}

/**
 * Hessian binary destructor.
 */
static int hessian_binary_dtor (hessian_object_t * object) {
    hessian_binary_t * self= object;
    if (self == NULL) {
        pep_log_error("hessian_binary_dtor: NULL object pointer.");
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
static int hessian_binary_deserialize (hessian_object_t * object, int tag, pep_buffer_t * input) {
    hessian_binary_t * self= object;
    const hessian_class_t * class;
    size_t buf_size, buf_l;
    int fully_read;
    pep_buffer_t * buf;
    if (self == NULL) {
        pep_log_error("hessian_binary_deserialize: NULL object pointer.");
        return HESSIAN_ERROR;
    }
    class= hessian_getclass(object);
    if (class == NULL) {
        pep_log_error("hessian_binary_deserialize: NULL class descriptor.");
        return HESSIAN_ERROR;
    }
    if (class->type != HESSIAN_BINARY) {
        pep_log_error("hessian_binary_deserialize: wrong class type: %d.", class->type);
        return HESSIAN_ERROR;
    }
    /* tag is 'x' for chunks and 'X' for final */
    if (tag != class->tag && tag != class->chunk_tag) {
        pep_log_error("hessian_binary_deserialize: wrong tag: %c.", (char)tag);
        return HESSIAN_ERROR;
    }
    /* use a buffer as tmp */
    buf_size= 32;
    if (tag == class->chunk_tag) buf_size= HESSIAN_CHUNK_SIZE;
    buf= pep_buffer_create(buf_size);
    fully_read= FALSE;
    while (!fully_read) {
        /* read the binary length */
        int b16= pep_buffer_getc(input);
        int b8= pep_buffer_getc(input);
        size_t bin_l= (b16 << 8) + b8;
        /* fully read string (chunk) */
        size_t n_bytes= 0;
        while(n_bytes < bin_l) {
            int b= pep_buffer_getc(input);
            pep_buffer_putc(b,buf);
            n_bytes++;
        }
        /* was it final chunk? */
        if (tag == class->chunk_tag) {
            tag= pep_buffer_getc(input);
        }
        else {
            /* tag == class->tag (final) */
            fully_read= TRUE;
        }
    }
    /* copy the buffer into the hessian binary */
    buf_l= pep_buffer_length(buf);
    self->length= buf_l;
    self->data= calloc(self->length,sizeof(char));
    if (self->data == NULL) {
        pep_log_error("hessian_binary_deserialize: can't allocated data (%d bytes).", (int)self->length);
        pep_buffer_delete(buf);
        return HESSIAN_ERROR;
    }
    pep_buffer_read(self->data,sizeof(char),buf_l,buf);
    pep_buffer_delete(buf);
    return HESSIAN_OK;
}

/**
 * hessian_binary serialize method.
 */
static int hessian_binary_serialize (const hessian_object_t * object, pep_buffer_t * output) {
    hessian_binary_t * self= (hessian_object_t *) object;
    const hessian_class_t * class;
    size_t byte_l, pos;
    int b8, b16;
    const char * chunk, * rest;
    if (self == NULL) {
        pep_log_error("hessian_binary_serialize: NULL object pointer.");
        return HESSIAN_ERROR;
    }
    class= hessian_getclass(object);
    if (class == NULL) {
        pep_log_error("hessian_binary_serialize: NULL class descriptor.");
        return HESSIAN_ERROR;
    }
    if (class->type != HESSIAN_BINARY) {
        pep_log_error("hessian_binary_serialize: wrong class type: %d.",(int)(class->type));
        return HESSIAN_ERROR;
    }
    byte_l= self->length;
    pos= 0;
    while (byte_l > HESSIAN_CHUNK_SIZE) {
        /* send binary chunks */
        pep_buffer_putc(class->chunk_tag,output);
        b16= HESSIAN_CHUNK_SIZE >> 8;
        b8= HESSIAN_CHUNK_SIZE & 0x00FF;
        pep_buffer_putc(b16,output);
        pep_buffer_putc(b8,output);
        /* write HESSIAN_CHUNK_SIZE bytes */
        chunk= &(self->data[pos]);
        pep_buffer_write(chunk,1,HESSIAN_CHUNK_SIZE,output);
        pos= pos + HESSIAN_CHUNK_SIZE;
        byte_l= byte_l - HESSIAN_CHUNK_SIZE;
    }

    pep_buffer_putc(class->tag,output);
    b16= byte_l >> 8;
    b8= byte_l & 0x00FF;
    pep_buffer_putc(b16,output);
    pep_buffer_putc(b8,output);
    rest= &(self->data[pos]);
    pep_buffer_write(rest,1,byte_l,output);

    return HESSIAN_OK;
}

/**
 * Returns the byte length of the Hessian binary. returns HESSIAN_ERROR (-1) on error.
 */
size_t hessian_binary_length(const hessian_object_t * obj) {
    const hessian_binary_t * self= obj;
    const hessian_class_t * class;
    if (self == NULL) {
        pep_log_error("hessian_binary_length: NULL object pointer.");
        return HESSIAN_ERROR;
    }
    class= hessian_getclass(obj);
    if (class == NULL) {
        pep_log_error("hessian_binary_length: NULL class descriptor.");
        return HESSIAN_ERROR;
    }
    if (class->type != HESSIAN_BINARY) {
        pep_log_error("hessian_binary_length: wrong class type: %d.", class->type);
        return HESSIAN_ERROR;
    }
    return self->length;
}

/**
 * Returns the data of the Hessian binary.
 */
const char * hessian_binary_getdata(const hessian_object_t * obj) {
    const hessian_binary_t * self= obj;
    const hessian_class_t * class;
    if (self == NULL) {
        pep_log_error("hessian_binary_getdata: NULL pointer.");
        return NULL;
    }
    class= hessian_getclass(obj);
    if (class == NULL) {
        pep_log_error("hessian_binary_getdata: no class descriptor.");
        return NULL;
    }
    if (class->type != HESSIAN_BINARY) {
        pep_log_error("hessian_binary_getdata: wrong class type: %d.", class->type);
        return NULL;
    }
    return self->data;
}
