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
 * @(#)hls_mpegts.h
 */


#ifndef _HLS_MPEGTS_H_
#define _HLS_MPEGTS_H_

#include <stdint.h>

#include "hls_types.h"
#include "hls_buf.h"

typedef struct hls_mpegts_file hls_mpegts_file_t;
struct hls_mpegts_file {
    int         fd;                     
    unsigned    size;
    char        buf[16];                
    char        iv[16];
    unsigned    encrypt:1;
};

typedef struct hls_mpegts_frame hls_mpegts_frame_t;
struct hls_mpegts_frame {
    uint64_t    pts;
    uint64_t    dts;
    uint64_t    pid;
    uint64_t    sid;            
    uint64_t    cc;             
    unsigned    key:1;
};

typedef struct hls_frag hls_frag_t;
struct hls_frag {
    uint64_t    id;
    //uint64_t    key_id;
    double      duration;
    unsigned    active:1;
    unsigned    discont:1; /* before */
    unsigned    key:1;
};

typedef struct hls_mpegts_ctx hls_mpegts_ctx_t;
struct hls_mpegts_ctx {
    unsigned             opened:1;

    //hls_mpegts_file_t    file;

    uint64_t             frag;
    uint64_t             frag_ts;
    uint32_t             nfrags;
    //hls_frag_t          *frags; /* circular 2 * winfrags + 1 */
    hls_frag_t           frags[MAX_FRAGMENT_NUM]; /* circular 2 * winfrags + 1 */

    uint32_t             audio_cc;
    uint32_t             video_cc;
    uint32_t             key_frags;

    uint64_t             aframe_base;
    uint64_t             aframe_num;
    uint64_t             aframe_pts;
    hls_buf_t            aframe;
};

void
hls_write_mpegts(hls_session_t *s);

int
hls_video_handler(hls_session_t *s, uint8_t iskey, 
                  char *packet, int packet_size,
                      unsigned int timestamp);

int
hls_audio_handler(hls_session_t *s, char *buf, int len, 
                      unsigned int timestamp);

int
hls_mpegts_write_frame(hls_session_t *s,
                       hls_mpegts_frame_t *f, hls_buf_t *b);

int
hls_cache_mpegts_init(uint32_t key, uint32_t hashtime, uint32_t hashlen, int isCreate);

void*
hls_cache_mpegts_get(uint32_t key, uint32_t &isNew);

void*
hls_cache_mpegts_search( uint32_t key);

void
hls_cache_mpegts_remove(uint32_t key);

int
hls_write_fragmem_playlist(hls_fragmem_t *fragmem, char* buff, int len);

int 
hls_write_fragmem_data(hls_session_t *s, hls_fragmem_t *fragmem, char *buff, int len, int key);
//int 
//hls_write_fragmem_data(hls_fragmem_t *fragmem, char *buff, int len);

int
hls_fragmem_open(hls_fragmem_t *fragmem, uint64_t id );

void
hls_fragmem_update_duration(hls_fragmem_t *fragmem, double duration);

hls_mem_t*
hls_tsmem_get(hls_fragmem_t *fragmem, uint64_t tsid);


int
hls_get_frag_num(hls_session_t *s, int n);

int
hls_write_fragmem_playlist_clean(hls_fragmem_t *fragmem);

#endif // _HLS_MPEGTS_H_
