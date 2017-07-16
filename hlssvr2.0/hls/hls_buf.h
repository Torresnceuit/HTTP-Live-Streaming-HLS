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
 * @(#)hls_buf.h
 */


#ifndef _HLS_BUFFER_H_
#define _HLS_BUFFER_H_

#include <stdint.h>

#include "hls_types.h"

#define WIN_FRAGMENT_NUM 6
#define MAX_FRAGMENT_NUM (2*WIN_FRAGMENT_NUM+1)
typedef struct hls_mem hls_mem_t;
struct hls_mem {
   uint64_t  id;
   double    duration;
   int32_t   key;
   int32_t   len;
   uint8_t   data[HLS_MAX_TS_LEN];
};

#define HLS_PLAYLIST_LEN 1024
#define HLS_AUDIO_BUF_LEN 0x100000
typedef struct hls_fragmem hls_fragmem_t;
struct hls_fragmem {
   uint32_t  videoid;
   uint64_t  fragid;
   uint32_t  playlist_len;
   char      playlist[HLS_PLAYLIST_LEN];
   char      audiobuf[HLS_AUDIO_BUF_LEN];
   hls_mem_t mem[MAX_FRAGMENT_NUM];
}__attribute__((packed));

typedef struct hls_buf hls_buf_t; 
struct hls_buf {
   char  *pos;
   char  *last;
   char  *start;
   char  *end;
};

typedef struct hls_chain hls_chain_t; 
struct hls_chain {
   hls_buf_t   *buf;
   hls_chain_t *next;
};


void*
hls_alloc(int size);

#endif // _HLS_BUFFER_H_




