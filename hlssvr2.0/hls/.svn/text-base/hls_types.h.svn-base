/*
 * Copyright (c) 2015 Jackie Dinh <jackiedinh8@gmail.com>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  1 Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  2 Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution.
 *  3 Neither the name of the <organization> nor the 
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY 
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * @(#)hls_types.h
 */


#ifndef _HLS_TYPES_H_
#define _HLS_TYPES_H_

#include <stddef.h>

struct hls_str {
    size_t      len;
    char        *data;
};


typedef struct hls_stream_ctx hls_stream_ctx_t;
typedef struct hls_session hls_session_t;
typedef struct hls_str hls_str_t;

#define HLS_MPEGTS_KEY 0x40001
#define HLS_SESSION_KEY 0x40002
#define HLS_HASHTIME 5
#define HLS_HASHLEN  60

#define HLS_MAX_TS_LEN (300*1024)

#define O_BINARY 0
#define hls_open_file(name, mode, create, access)\
    open((const char *) name, mode|create|O_BINARY, access)

#define MAX_PATH               4096
#define HLS_BUFSIZE            (1024*1024)

#define HLS_TYPE_LIVE  1
#define HLS_TYPE_EVENT 2

#define HLS_NAMING_SEQUENTIAL  1
#define HLS_NAMING_TIMESTAMP   2
#define HLS_NAMING_SYSTEM      3

#define cpymem(dst, src, n)   (((char *) memcpy(dst, src, n)) + (n))
#define hls_movemem(dst, src, n)   (((char *) memmove(dst, src, n)) + (n))

#endif // _HLS_TYPES_H_
