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

#include <string.h>
#include "base64.h"

#define NO_LINE_BREAK -1000

/**
 * Base64 codec table (RFC1113)
 */
static const char base64_codec_table[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/**
 * Encodes int_l 8-bit binary bytes as 4 '6-bit' characters (including '=' padding).
 */
static void encodeblock3to4( const unsigned char in[3], int in_l, unsigned char out[4] )
{
    out[0] = base64_codec_table[ in[0] >> 2 ];
    out[1] = base64_codec_table[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
    out[2] = (unsigned char) (in_l > 1 ? base64_codec_table[ ((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6) ] : '=');
    out[3] = (unsigned char) (in_l > 2 ? base64_codec_table[ in[2] & 0x3f ] : '=');
}

/**
 * Base64 encodes the in buffer into the out buffer (without line break).
 */
void pep_base64_encode_buffer( BUFFER * inbuf, BUFFER * outbuf ) {
    pep_base64_encode_buffer_l(inbuf,outbuf,NO_LINE_BREAK);
}

/**
 * Base64 encodes the in buffer into the out buffer.
 */
void pep_base64_encode_buffer_l( BUFFER * inbuf, BUFFER * outbuf, int linesize ) {

    unsigned char in[3], out[4];
    int i= 0, in_l= 0;
    size_t b_out = 0; /* byte written */

    if (linesize != NO_LINE_BREAK && linesize < 4) {
        linesize= BASE64_DEFAULT_LINE_SIZE;
    }

    while( !buffer_eof( inbuf ) ) {
        in_l = 0;
        in[0] = in[1] = in[2] = 0;
        for( i = 0; i < 3; i++ ) {
            int c = buffer_getc( inbuf );
            if ( c != BUFFER_EOF ) {
                in[i] = (unsigned char) c;
                in_l++;
            }
        }
        if( in_l > 0 ) {
            encodeblock3to4( in, in_l, out );
            b_out += buffer_write(out,1,4,outbuf);
        }
        if (linesize != NO_LINE_BREAK) {
            if( b_out >= linesize || buffer_eof( inbuf )) {
                buffer_write("\r\n",1,2,outbuf);
                b_out = 0;
            }
        }
    }
}

/**
 * Decodes 4 '6-bit' characters into 3 8-bit binary bytes.
 */
static void decodeblock4to3( const unsigned char in[4], unsigned char out[3] ) {
    out[0] = (in[0] << 2 | in[1] >> 4);
    out[1] = (in[1] << 4 | in[2] >> 2);
    out[2] = (((in[2] << 6) & 0xc0) | in[3]);
}

/**
 * Base64 decodes the in buffer into the out buffer.
 */
void pep_base64_decode_buffer( BUFFER * inbuf, BUFFER * outbuf ) {
    unsigned char in[4], out[3];
    int c, i, in_l;
    char * p;

    while( !buffer_eof( inbuf ) ) {
        in_l= 0;
        in[0] = in[1] = in[2] = in[3] = 0;
        for( i = 0; i < 4;) {
            c= buffer_getc( inbuf );
            if (c == BUFFER_EOF) break;
            /* drop every char not in table */
            p= strchr(base64_codec_table,c);
            if (p != NULL) {
                /* index of c in base64 table */
                in[i] = p - base64_codec_table;
                in_l++;
                i++;
            }
        }
        if( in_l > 0) {
            decodeblock4to3( in, out );
            for( i = 0; i < in_l - 1; i++ ) {
                buffer_putc( out[i], outbuf );
            }
        }
    }
}

