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

/**********************************
 * Hessian string and xml objects *
 **********************************/

/**
 * Method prototypes
 */
static OBJECT_CTOR(hessian_string);
static OBJECT_DTOR(hessian_string);
static OBJECT_SERIALIZE(hessian_string);
static OBJECT_DESERIALIZE(hessian_string);


/**
 * Initializes and registers the string class.
 */
static const hessian_class_t _hessian_string_descr = {
    HESSIAN_STRING,
    "hessian.String",
    sizeof(hessian_string_t),
    'S', 's',
    hessian_string_ctor,
    hessian_string_dtor,
    hessian_string_serialize,
    hessian_string_deserialize
};
const void * hessian_string_class = &_hessian_string_descr;


/**
 * Hessian UTF8 string constructor.
 */
static hessian_object_t * hessian_string_ctor (hessian_object_t * object, va_list * ap) {
    hessian_string_t * self= object;
    const char * str;
    size_t str_l;
    if (self == NULL) {
        log_error("hessian_string_ctor: NULL object pointer.");
        return NULL;
    }
    str = va_arg( *ap, const char *);
    if (str == NULL) {
        log_error("hessian_string_ctor: NULL string parameter 2.");
        return NULL;
    }
    str_l= strlen(str);
    self->string= calloc(str_l + 1,sizeof(char));
    if (self->string == NULL) {
        log_error("hessian_string_ctor: can't allocate string (%d chars).",(int)str_l);
        return NULL;
    }
    strncpy(self->string,str,str_l);
    return self;
}

/**
 * string destructor.
 */
static int hessian_string_dtor (hessian_object_t * object) {
    hessian_string_t * self= object;
    if (self == NULL) {
        log_error("hessian_string_dtor: NULL object pointer.");
        return HESSIAN_ERROR;
    }
    if (self->string != NULL) {
        free(self->string);
    }
    self->string= NULL;
    return HESSIAN_OK;
}

/**
 * string serialize method.
 */
static int hessian_string_serialize (const hessian_object_t * object, BUFFER * output) {
    const hessian_string_t * self= object;
    const hessian_class_t * class;
    size_t str_l, utf8_l, pos;
    const char * chunk, * rest;
    int b8, b16;
    if (self == NULL) {
        log_error("hessian_string_serialize: NULL object pointer.");
        return HESSIAN_ERROR;
    }
    class= hessian_getclass(object);
    if (class == NULL) {
        log_error("hessian_string_serialize: NULL class descriptor.");
        return HESSIAN_ERROR;
    }
    if (class->type != HESSIAN_STRING && class->type != HESSIAN_XML) {
        log_error("hessian_string_serialize: wrong class type: %d.",class->type);
        return HESSIAN_ERROR;
    }
    str_l= strlen(self->string); /* effective chars (bytes) */
    utf8_l= utf8_strlen(self->string);
    pos= 0;
    /* WARN: number of chars != number of bytes (multi-byte utf8) */
    while (utf8_l > HESSIAN_CHUNK_SIZE) {
        size_t start_pos;
        int n_utf8s;
        /* send utf8 chunks */
        b16= (HESSIAN_CHUNK_SIZE >> 8) & 0x00FF;
        b8= HESSIAN_CHUNK_SIZE & 0x00FF;
        buffer_putc(class->chunk_tag,output);
        buffer_putc(b16,output);
        buffer_putc(b8,output);
        /* write HESSIAN_CHUNK_SIZE utf8 chars */
        chunk= &(self->string[pos]);
        /* number of effective bytes */
        start_pos= pos;
        n_utf8s= 0;
        while( n_utf8s < HESSIAN_CHUNK_SIZE ) {
            int byte= self->string[pos++];
            if ((byte & 0xC0) != 0x80) {
                n_utf8s++;
                if ((byte & 0xE0) == 0xC0) pos++; /* start of the 2-byte seq. */
                else if ((byte & 0xF0) == 0xE0) pos+= 2; /* start of the 3-byte seq. */
                else if ((byte & 0xF0) == 0xF0) pos+= 3; /* start of the 4-byte seq. */
            }
        }
        buffer_write(chunk,1,(pos - start_pos),output);
        utf8_l= utf8_l - HESSIAN_CHUNK_SIZE;
    }

    b16= utf8_l >> 8;
    b8= utf8_l & 0x00FF;
    buffer_putc(class->tag,output);
    buffer_putc(b16,output);
    buffer_putc(b8,output);
    rest= &(self->string[pos]);
    buffer_write(rest,1,(str_l - pos),output);

    return HESSIAN_OK;
}

/**
 * Hessian string deserialize method.
 */
static int hessian_string_deserialize (hessian_object_t * object, int tag, BUFFER * input) {
    hessian_string_t * self= object;
    const hessian_class_t * class;
    BUFFER * sb;
    size_t sb_size, sb_l;
    int fully_read;
    if (self == NULL) {
        log_error("hessian_string_deserialize: NULL object pointer.");
        return HESSIAN_ERROR;
    }
    class= hessian_getclass(object);
    if (class == NULL) {
        log_error("hessian_string_deserialize: NULL class descriptor.");
        return HESSIAN_ERROR;
    }
    if (class->type != HESSIAN_STRING && class->type != HESSIAN_XML) {
        log_error("hessian_string_deserialize: wrong class type: %d.",class->type);
        return HESSIAN_ERROR;
    }
    /* tag is 's' for chunks and 'S' for final */
    if (tag != class->tag && tag != class->chunk_tag) {
        log_error("hessian_string_deserialize: invalid tag: %c (%d).",(char)tag,tag);
        return HESSIAN_ERROR;
    }
    /* use a string buffer as tmp */
    sb_size= 32;
    if (tag == class->chunk_tag) sb_size= HESSIAN_CHUNK_SIZE;
    sb= buffer_create(sb_size);
    if (sb == NULL) {
        log_error("hessian_string_deserialize: can't create temp buffer (%d bytes).",(int)sb_size);
        return HESSIAN_ERROR;
    }
    fully_read= FALSE;
    while (!fully_read) {
        /* read the utf8 str length */
        int b16= buffer_getc(input);
        int b8= buffer_getc(input);
        size_t utf8_l= (b16 << 8) + b8;
        /* fully read UTF8 string (chunk) */
        char * utf8= utf8_bgets(utf8_l,input);
        /* FIXME: utf8 == NULL ?? */
        buffer_write(utf8,sizeof(char),strlen(utf8),sb);
        free(utf8);
        /* was it final chunk? */
        if (tag == class->chunk_tag) {
            tag= buffer_getc(input);
        }
        else {
            /* tag == class->tag (final) */
            fully_read= TRUE;
        }
    } /* while */

    /* copy the string buffer into the hessian string */
    sb_l= buffer_length(sb);
    self->string= calloc(sb_l + 1,sizeof(char));
    if (self->string == NULL) {
        log_error("hessian_string_deserialize: can't allocate string (%d chars).", (int)sb_l);
        buffer_delete(sb);
        return HESSIAN_ERROR;
    }
    buffer_read(self->string,sizeof(char),sb_l,sb);
    buffer_delete(sb);
    return HESSIAN_OK;
}

/**
 * Returns the UTF-8 string.
 */
const char * hessian_string_getstring(const hessian_object_t * object) {
    const hessian_string_t * self= object;
    const hessian_class_t * class;
    if (self == NULL) {
        log_error("hessian_string_getstring: NULL object pointer.");
        return NULL;
    }
    class= hessian_getclass(object);
    if (class == NULL) {
        log_error("hessian_string_getstring: NULL class descriptor.");
        return NULL;
    }
    if (class->type != HESSIAN_STRING && class->type != HESSIAN_XML) {
        log_error("hessian_string_getstring: wrong class type: %d.",class->type);
        return NULL;
    }
    return self->string;
}

/**
 * Returns a char array ('\0' terminated) containing utf8_l UTF-8 chars, read from the input BUFFER.
 * You are responsible to free the array.
 *
 * @return a char array pointer or NULL on error.
 */
char * utf8_bgets(size_t utf8_l, BUFFER * input) {
    size_t n_utf8, tmp_l;
    char * utf8;
    /* use a tmp buffer */
    BUFFER * tmp= buffer_create(utf8_l);
    if (tmp == NULL) {
        log_error("utf8_bgets: can't create temp buffer (%d bytes).", (int)utf8_l);
        return NULL;
    }
    n_utf8= 0;
    while(n_utf8 < utf8_l) {
        int byte= buffer_getc(input);
        buffer_putc(byte,tmp);
        if ((byte & 0xC0) == 0xC0) {
            /* utf8 multi-byte char sequence */
            int n_mbyte= 0;
            if ((byte & 0xF0) == 0xC0) n_mbyte= 1; /* c is start of the 2-byte seq. */
            else if ((byte & 0xF0) == 0xE0) n_mbyte= 2; /* c is start of the 3-byte seq. */
            else if ((byte & 0xF0) == 0xF0) n_mbyte= 3; /* c is start of the 4-byte seq. */
            else log_error("utf8_bgets: unknown multi-bytes utf8 sequence: 0x%0X",byte);
            /* read additional bytes */
            while (n_mbyte-- > 0) {
                int mbyte= buffer_getc(input);
                buffer_putc(mbyte,tmp);
            }
        }
        n_utf8++;
    }
    /* alloc the char array */
    tmp_l= buffer_length(tmp);
    utf8= calloc(tmp_l + 1, sizeof(char));
    if (utf8 == NULL) {
        log_error("utf8_bgets: can't allocate string (%d chars).", (int)tmp_l);
        buffer_delete(tmp);
        return NULL;
    }
    /* copy tmp buffer to char array */
    buffer_read(utf8,sizeof(char),tmp_l,tmp);
    buffer_delete(tmp);
    return utf8;
}

/**
 * Returns the effective UTF8 string length
 */
size_t utf8_strlen(const char *s) {
    int i = 0;
    size_t j = 0;
    if (s == NULL) {
        log_error("utf8_strlen: NULL string pointer.");
        return 0;
    }
    while (s[i]) {
        if ((s[i] & 0xc0) != 0x80) j++; /* multi byte char */
        i++;
    }
    return j;
}

/* return TRUE iff the byte is part of an UTF8 multi-byte sequence. */
int is_multibyte(const int byte) {
    return ((byte & 0xC0) == 0x80); /* 2nd, 3rd and 4th byte of multi-byte seq. */
/*
    return ((byte & 0xE0) == 0xC0); // start of the 2-byte seq.
    return ((byte & 0xF0) == 0xE0); // start of the 3-byte seq.
    return ((byte & 0xF0) == 0xF0); // start of the 4-byte seq.
*/
}

int is_ascii(const int byte) {
    return (byte < 0x80);
}

/**
 * Returns the UTF-8 length of the Hessian string or 0 on error.
 */
size_t hessian_string_utf8_length(const hessian_object_t * object) {
    const hessian_string_t * self= object;
    const hessian_class_t * class;
    if (self == NULL) {
        log_error("hessian_string_utf8_length: NULL object pointer.");
        return 0;
    }
    class= hessian_getclass(object);
    if (class == NULL) {
        log_error("hessian_string_utf8_length: NULL class descriptor.");
        return 0;
    }
    if (class->type != HESSIAN_STRING && class->type != HESSIAN_XML) {
        log_error("hessian_string_utf8_length: wrong class type: %d.",class->type);
        return 0;
    }
    return utf8_strlen(self->string);
}

/**
 * Returns the byte length of the Hessian string or 0 on error.
 */
size_t hessian_string_length(const hessian_object_t * object) {
    const hessian_string_t * self= object;
    const hessian_class_t * class;
    if (self == NULL) {
        log_error("hessian_string_length: NULL object pointer.");
        return 0;
    }
    class= hessian_getclass(object);
    if (class == NULL) {
        log_error("hessian_string_length: NULL class descriptor.");
        return 0;
    }
    if (class->type != HESSIAN_STRING && class->type != HESSIAN_XML) {
        log_error("hessian_string_length: wrong class type: %d.",class->type);
        return 0;
    }
    return strlen(self->string);
}

/**
 * Returns TRUE iff the Hessian string is exactly the same as str.
 */
int hessian_string_equals(const hessian_object_t * object, const char *str) {
    const hessian_string_t * self= object;
    const hessian_class_t * class;
    size_t string_l;
    if (self == NULL) {
        log_error("hessian_string_equals: NULL object pointer.");
        return HESSIAN_ERROR;
    }
    class= hessian_getclass(object);
    if (class == NULL) {
        log_error("hessian_string_equals: NULL class descriptor.");
        return HESSIAN_ERROR;
    }
    if (class->type != HESSIAN_STRING && class->type != HESSIAN_XML) {
        log_error("hessian_string_equals: wrong class type: %d.",class->type);
        return HESSIAN_ERROR;
    }
    if (self->string == str) {
        return TRUE;
    }
    if (str == NULL) {
        return FALSE;
    }
    string_l= strlen(self->string);
    return (strncmp(self->string, str, string_l) == 0) ? TRUE : FALSE;
}

/**
 * Initializes and registers the HessianXml class.
 */
static const hessian_class_t _hessian_xml_descr = {
    HESSIAN_XML,
    "hessian.Xml",
    sizeof(hessian_xml_t),
    'X', 'x',
    hessian_string_ctor,
    hessian_string_dtor,
    hessian_string_serialize,
    hessian_string_deserialize
};

const void * hessian_xml_class = &_hessian_xml_descr;

size_t hessian_xml_utf8_length(const hessian_object_t * xml) {
    return hessian_string_utf8_length(xml);
}
size_t hessian_xml_length(const hessian_object_t * xml) {
    return hessian_string_length(xml);
}
const char * hessian_xml_getxml(const hessian_object_t *xml) {
    return hessian_string_getstring(xml);
}
