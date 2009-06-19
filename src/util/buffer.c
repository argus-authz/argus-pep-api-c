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
 * $Id$
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/buffer.h"
#include "util/log.h"

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

// memory buffer structure
struct buffer {
    unsigned char * data; // bytes
    size_t size; // allocated size
    size_t wpos; // write position
    size_t rpos; // read position
};

// constructor
BUFFER * buffer_create(size_t size) {
    BUFFER * buffer= calloc(1,sizeof(BUFFER));
	if (buffer == NULL) {
    	log_error("buffer_create: calloc BUFFER failed.");
		return NULL;
	}
    buffer->size= size;
    if (size < 2) {
        // default initial size
        buffer->size= (size_t)BUFFER_INITIAL_SIZE;
    }
    buffer->wpos= 0;
    buffer->rpos= 0;
    buffer->data= calloc(buffer->size,sizeof(unsigned char));
	if (buffer->data == NULL) {
    	log_error("buffer_create: calloc of %d bytes failed.", (int)buffer->size);
		free(buffer);
		return NULL;
	}
    return buffer;
}

// destroy
void buffer_delete(BUFFER * buffer) {
	if (buffer == NULL) return;
	if (buffer->data != NULL) free(buffer->data);
    free(buffer);
    buffer= NULL;
}

/**
 * Ensure that the buffer can store additional size bytes.
 *
 * @return BUFFER_OK or BUFFER_ERROR if an error occurs (null pointer, realloc failed)
 */
static int buffer_ensure_capacity(BUFFER * buffer, size_t size) {
	if (buffer == NULL) {
    	log_error("buffer_ensure_capacity: buffer is a NULL pointer.");
		return BUFFER_ERROR;
	}
    size_t available= buffer->size - buffer->wpos;
    //printf("XXX:buffer_check_capacity: available: %ld, needed: %ld\n", available, size);
    if (size > available) {
        // realloc
        size_t new_size0= buffer->size + (size - available); // minimum capacity
#if BUFFER_INCREASE_POLICY == 0
        size_t new_size1= buffer->size + 1024;
#elif BUFFER_INCREASE_POLICY == 1
        size_t new_size1= buffer->size * 2;
#else
        size_t new_size1= buffer->size + (buffer->size / 2) + 1;
#endif
        // the larger of both is the new size
        size_t new_size= (new_size1 > new_size0) ? new_size1 : new_size0;
        //printf("XXX:buffer_ensure_capacity: realloc old size: %ld, new size: %ld\n", buffer->size, new_size);
        buffer->data= realloc(buffer->data, new_size);
        if (buffer->data == NULL) {
        	log_error("buffer_ensure_capacity: realloc (%d bytes) failed.", (int)new_size);
        	return BUFFER_ERROR;
        }
        buffer->size= new_size;
    }
    return BUFFER_OK;
}

size_t buffer_fwrite(BUFFER * buffer, FILE * ostream) {
	if (buffer == NULL || ostream == NULL) {
    	log_error("buffer_fwrite: buffer or ostream is a NULL pointer.");
		return BUFFER_ERROR;
	}
    // TODO: only write unread data [rpos..wpos] or all?
    return fwrite(buffer->data,sizeof(char),buffer->wpos,ostream);
}


size_t buffer_write(const void * src, size_t size, size_t count, void * _buffer) {
	if (_buffer == NULL || src == NULL) {
    	log_error("buffer_write: src or buffer is a NULL pointer.");
		return BUFFER_ERROR;
	}
    BUFFER * buffer = (BUFFER *)_buffer;
    size_t nbytes = size * count;
    //printf("XXX:buffer_write: %ld bytes, size: %ld, count: %ld\n", nbytes, size, count);
    if (buffer_ensure_capacity(buffer, nbytes) != BUFFER_OK) {
    	log_error("buffer_write: can't increase buffer capacity by %d bytes.", (int)nbytes);
    	return BUFFER_ERROR;
    }
    memcpy(&(buffer->data[buffer->wpos]), src, nbytes);
    buffer->wpos += nbytes;
    //printf("XXX:buffer_write: wpos: %ld, rpos: %ld, size: %ld\n", buffer->wpos, buffer->rpos, buffer->size);
    return nbytes;
}

size_t buffer_fread(BUFFER * buffer, FILE * istream) {
	if (buffer == NULL || istream == NULL) {
    	log_error("buffer_fread: buffer or istream is a NULL pointer.");
		return BUFFER_ERROR;
	}
	size_t nbytes= 0;
	int c;
	while((c=fgetc(istream)) != EOF) {
		buffer_putc(c,buffer);
		nbytes++;
	}
	return nbytes;
}


size_t buffer_read(void * dst, size_t size, size_t count, void * _buffer) {
	if (dst == NULL || _buffer == NULL) {
    	log_error("buffer_fread: dst or buffer is a NULL pointer.");
		return BUFFER_ERROR;
	}
	BUFFER * buffer = (BUFFER *)_buffer;
    size_t nbytes = size * count;
    size_t available = buffer->wpos - buffer->rpos;
    //printf("XXX:buffer_read: available %ld bytes, requested %ld bytes (size: %ld count: %ld)\n", available, nbytes, size, count);
    if (nbytes > available) {
        // copy only available bytes
        nbytes= available;
    }
    //printf("XXX:buffer_read: copy %ld bytes\n", nbytes);
    memcpy(dst, &(buffer->data[buffer->rpos]), nbytes);
    buffer->rpos += nbytes;
    return nbytes;
}

int buffer_eof(BUFFER * buffer) {
	if (buffer == NULL) {
    	log_error("buffer_eof: buffer is a NULL pointer.");
		return TRUE;
	}
    //printf("XXX:buffer_eof: wpos <= rpos? %ld <= %ld, size: %ld\n", buffer->wpos, buffer->rpos, buffer->size);
    return (buffer->wpos <= buffer->rpos) ? TRUE : FALSE;
}

int buffer_getc(BUFFER * buffer) {
	if (buffer == NULL) {
    	log_error("buffer_getc: buffer is a NULL pointer.");
		return BUFFER_EOF;
	}
    //printf("XXX:buffer_getc: wpos: %ld, rpos: %ld, size: %ld\n", buffer->wpos, buffer->rpos, buffer->size);
    if (buffer_eof(buffer))
    	return BUFFER_EOF;
    else {
    	unsigned char c= buffer->data[buffer->rpos];
    	buffer->rpos++;
    	return c;
    }
}

int buffer_ungetc(int c, BUFFER * buffer) {
	if (buffer == NULL) {
    	log_error("buffer_ungetc: buffer is a NULL pointer.");
		return BUFFER_ERROR;
	}
    unsigned char uc= (unsigned char)c;
    if (buffer->rpos == 0) {
    	// shift the whole data buffer by 1
        if (buffer_ensure_capacity(buffer, 1) != BUFFER_OK) {
        	log_error("buffer_ungetc: can't increase buffer capacity by 1 byte.");
        	return BUFFER_ERROR;
        }
        //printf("XXX:buffer_ungetc: shift right: rpos: %ld, wpos: %ld, size: %ld\n", buffer->rpos, buffer->wpos, buffer->size);
    	memmove(&(buffer->data[1]),&(buffer->data[0]), buffer->wpos);
    	buffer->wpos++;
    	buffer->rpos++;
    }
    buffer->rpos--;
    buffer->data[buffer->rpos]= uc;
    return BUFFER_OK;
}


int buffer_putc(int c, BUFFER * buffer) {
	if (buffer == NULL) {
    	log_error("buffer_putc: buffer is a NULL pointer.");
		return BUFFER_ERROR;
	}
    //printf("XXX:buffer_getc: wpos: %ld, rpos: %ld, size: %ld\n", buffer->wpos, buffer->rpos, buffer->size);
    unsigned char uc= (unsigned char)c;
    if (buffer_ensure_capacity(buffer, 1) != BUFFER_OK) {
    	log_error("buffer_putc: can't increase buffer capacity by 1 byte.");
    	return BUFFER_ERROR;
    }
    buffer->data[buffer->wpos++]= uc;
    return c;
}

/**
 * Rewind the buffer read position.
 */
int buffer_rewind(BUFFER * buffer) {
	if (buffer == NULL) {
    	log_error("buffer_rewind: buffer is a NULL pointer.");
		return BUFFER_ERROR;
	}
	buffer->rpos= 0;
	return BUFFER_OK;
}

int buffer_reset(BUFFER * buffer) {
	if (buffer == NULL) {
    	log_error("buffer_reset: buffer is a NULL pointer.");
		return BUFFER_ERROR;
	}
	buffer->rpos= 0;
	buffer->wpos= 0;
	memset(buffer->data,0,buffer->size);
	return BUFFER_OK;
}

size_t buffer_length(BUFFER * buffer) {
	if (buffer == NULL) {
    	log_error("buffer_length: buffer is a NULL pointer.");
		return 0;
	}
	return buffer->wpos - buffer->rpos;
}



