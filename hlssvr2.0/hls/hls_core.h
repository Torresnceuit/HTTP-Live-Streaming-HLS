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
 * @(#)hls_core.h
 */


#ifndef _HLS_CORE_H_
#define _HLS_CORE_H_

#include "hls_log.h"
#include "hls_types.h"

typedef struct hls_config hls_config_t;
struct hls_config {
   uint32_t           audio_buffer_size;
   uint32_t           winfrags;
   uint64_t           max_fraglen;
   uint64_t           fraglen;
   uint64_t           slicing;
   uint64_t           max_audio_delay;

   uint32_t           keys;
   uint8_t            type;
   uint8_t            sync;
   uint8_t            cleanup;
   uint8_t            naming;
   uint8_t            nested;
   hls_str_t          infile;
   hls_str_t          base_url;
   hls_str_t          key_url;

   hls_str_t          path;
   hls_str_t          name;
   uint64_t           frags_per_key;
   uint64_t           granularity;

   uint64_t           num_segments;
};

typedef struct hls_options hls_options_t;
struct hls_options {
    char *input_file;
    long segment_duration;
    char *output_prefix;
    char *hls_name;
    char *tmp_m3u8_file;
    char *url_prefix;
    long num_segments;
};

void
hls_init_config(hls_config_t *ctx);

void
hls_parse_args(hls_config_t *ctx, int argc, char** argv);



#endif // _HLS_CORE_H_
