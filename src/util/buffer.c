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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"
#include "log.h"

/*
 * buffer memory increase policies used by the buffer_ensure_capacity()
 * method:
 *  0: increase buffer size by 1k (1024 bytes)
 *  1: increase buffer size by doubling its size
 *  2: increase buffer size by half of the existing size (default)
 */
#ifndef BUFFER_INCREASE_POLICY
#define BUFFER_INCREASE_POLICY 2
#endif

/*
 * buffer initial size if given size at creation time is < 2
 */
#ifndef BUFFER_INITIAL_SIZE
#define BUFFER_INITIAL_SIZE 16
#endif

/* memory buffer structure */
struct pep_buffer {
    unsigned char * data; /* bytes */
    size_t size; /* allocated size */
    size_t wpos; /* write position */
    size_t rpos; /* read position */
};

/* constructor */
pep_buffer_t * pep_buffer_create(size_t size) {
    pep_buffer_t * buffer= calloc(1,sizeof(struct pep_buffer));
    if (buffer == NULL) {
        pep_log_error("pep_buffer_create: calloc pep_buffer_t failed.");
        return NULL;
    }
    buffer->size= size;
    if (size < 2) {
        /* default initial size */
        buffer->size= (size_t)BUFFER_INITIAL_SIZE;
    }
    buffer->wpos= 0;
    buffer->rpos= 0;
    buffer->data= calloc(buffer->size,sizeof(unsigned char));
    if (buffer->data == NULL) {
        pep_log_error("buffer_create: calloc of %d bytes failed.", (int)buffer->size);
        free(buffer);
        return NULL;
    }
    return buffer;
}

/* destroy */
void pep_buffer_delete(pep_buffer_t * buffer) {
    if (buffer == NULL) return;
    if (buffer->data != NULL) free(buffer->data);
    buffer->data= NULL;
    free(buffer);
    buffer= NULL;
}

/**
 * Ensure that the buffer can store additional size bytes.
 *
 * @return BUFFER_OK or BUFFER_ERROR if an error occurs (null pointer, realloc failed)
 */
static int pep_buffer_ensure_capacity(pep_buffer_t * buffer, size_t size) {
    size_t available, new_size0, new_size1, new_size;
    if (buffer == NULL) {
        pep_log_error("pep_buffer_ensure_capacity: buffer is a NULL pointer.");
        return BUFFER_ERROR;
    }
    available= buffer->size - buffer->wpos;
    if (size > available) {
        unsigned char * tmp_data;
        /* realloc */
        new_size0= buffer->size + (size - available); /* minimum capacity */
#if BUFFER_INCREASE_POLICY == 0
        new_size1= buffer->size + 1024;
#elif BUFFER_INCREASE_POLICY == 1
        new_size1= buffer->size * 2;
#else
        new_size1= buffer->size + (buffer->size / 2) + 1;
#endif
        /* the larger of both is the new size */
        new_size= (new_size1 > new_size0) ? new_size1 : new_size0;
        tmp_data= realloc(buffer->data, new_size);
        if (tmp_data == NULL) {
            pep_log_error("pep_buffer_ensure_capacity: realloc (%d bytes) failed.", (int)new_size);
            free(buffer->data);
            buffer->data= NULL;
            return BUFFER_ERROR;
        }
        buffer->data= tmp_data;
        buffer->size= new_size;
    }
    return BUFFER_OK;
}

size_t pep_buffer_fwrite(pep_buffer_t * buffer, FILE * ostream) {
    if (buffer == NULL || ostream == NULL) {
        pep_log_error("pep_buffer_fwrite: buffer or ostream is a NULL pointer.");
        return BUFFER_ERROR;
    }
    /* TODO: only write unread data [rpos..wpos] or all? */
    return fwrite(buffer->data,sizeof(char),buffer->wpos,ostream);
}


size_t pep_buffer_write(const void * src, size_t size, size_t count, void * _buffer) {
    pep_buffer_t * buffer;
    size_t nbytes;
    if (_buffer == NULL || src == NULL) {
        pep_log_error("pep_buffer_write: src or buffer is a NULL pointer.");
        return BUFFER_ERROR;
    }
    buffer = (pep_buffer_t *)_buffer;
    nbytes = size * count;
    if (pep_buffer_ensure_capacity(buffer, nbytes) != BUFFER_OK) {
        pep_log_error("pep_buffer_write: can't increase buffer capacity by %d bytes.", (int)nbytes);
        return BUFFER_ERROR;
    }
    memcpy(&(buffer->data[buffer->wpos]), src, nbytes);
    buffer->wpos += nbytes;
    return nbytes;
}

size_t pep_buffer_fread(pep_buffer_t * buffer, FILE * istream) {
    size_t nbytes= 0;
    int c;
    if (buffer == NULL || istream == NULL) {
        pep_log_error("buffer_fread: buffer or istream is a NULL pointer.");
        return BUFFER_ERROR;
    }
    while((c=fgetc(istream)) != EOF) {
        pep_buffer_putc(c,buffer);
        nbytes++;
    }
    return nbytes;
}


size_t pep_buffer_read(void * dst, size_t size, size_t count, void * _buffer) {
    pep_buffer_t * buffer;
    size_t nbytes, available;
    if (dst == NULL || _buffer == NULL) {
        pep_log_error("buffer_fread: dst or buffer is a NULL pointer.");
        return BUFFER_ERROR;
    }
    buffer = (pep_buffer_t *)_buffer;
    nbytes = size * count;
    available = buffer->wpos - buffer->rpos;
    if (nbytes > available) {
        /* copy only available bytes */
        nbytes= available;
    }
    memcpy(dst, &(buffer->data[buffer->rpos]), nbytes);
    buffer->rpos += nbytes;
    return nbytes;
}

int pep_buffer_eof(pep_buffer_t * buffer) {
    if (buffer == NULL) {
        pep_log_error("buffer_eof: buffer is a NULL pointer.");
        return TRUE;
    }
    return (buffer->wpos <= buffer->rpos) ? TRUE : FALSE;
}

int pep_buffer_getc(pep_buffer_t * buffer) {
    unsigned char c;
    if (buffer == NULL) {
        pep_log_error("buffer_getc: buffer is a NULL pointer.");
        return BUFFER_EOF;
    }
    if (pep_buffer_eof(buffer))
        return BUFFER_EOF;
    else {
        c= buffer->data[buffer->rpos];
        buffer->rpos++;
        return c;
    }
}

int pep_buffer_ungetc(int c, pep_buffer_t * buffer) {
    unsigned char uc;
    if (buffer == NULL) {
        pep_log_error("buffer_ungetc: buffer is a NULL pointer.");
        return BUFFER_ERROR;
    }
    uc= (unsigned char)c;
    if (buffer->rpos == 0) {
        /* shift the whole data buffer by 1 */
        if (pep_buffer_ensure_capacity(buffer, 1) != BUFFER_OK) {
            pep_log_error("buffer_ungetc: can't increase buffer capacity by 1 byte.");
            return BUFFER_ERROR;
        }
        memmove(&(buffer->data[1]),&(buffer->data[0]), buffer->wpos);
        buffer->wpos++;
        buffer->rpos++;
    }
    buffer->rpos--;
    buffer->data[buffer->rpos]= uc;
    return BUFFER_OK;
}


int pep_buffer_putc(int c, pep_buffer_t * buffer) {
    unsigned char uc;
    if (buffer == NULL) {
        pep_log_error("buffer_putc: buffer is a NULL pointer.");
        return BUFFER_ERROR;
    }
    uc= (unsigned char)c;
    if (pep_buffer_ensure_capacity(buffer, 1) != BUFFER_OK) {
        pep_log_error("buffer_putc: can't increase buffer capacity by 1 byte.");
        return BUFFER_ERROR;
    }
    buffer->data[buffer->wpos++]= uc;
    return c;
}

/**
 * Rewind the buffer read position.
 */
int pep_buffer_rewind(pep_buffer_t * buffer) {
    if (buffer == NULL) {
        pep_log_error("buffer_rewind: buffer is a NULL pointer.");
        return BUFFER_ERROR;
    }
    buffer->rpos= 0;
    return BUFFER_OK;
}

int pep_buffer_reset(pep_buffer_t * buffer) {
    if (buffer == NULL) {
        pep_log_error("buffer_reset: buffer is a NULL pointer.");
        return BUFFER_ERROR;
    }
    buffer->rpos= 0;
    buffer->wpos= 0;
    memset(buffer->data,0,buffer->size);
    return BUFFER_OK;
}

size_t pep_buffer_length(pep_buffer_t * buffer) {
    if (buffer == NULL) {
        pep_log_error("buffer_length: buffer is a NULL pointer.");
        return 0;
    }
    return buffer->wpos - buffer->rpos;
}



