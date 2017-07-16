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
 * @(#)hls_bitread.h
 */


#ifndef _HLS_BITREAD_H_
#define _HLS_BITREAD_H_

#include <stdint.h>

typedef struct hls_bitread hls_bitread_t;
struct hls_bitread {
    char      *pos;
    char      *last;
    uintptr_t  offs;
    uintptr_t  err;
};


void hls_bitread_init(hls_bitread_t *br, char *pos, char *last);
uint64_t hls_bitread_get(hls_bitread_t *br, uint32_t n);
uint64_t hls_bitread_golomb(hls_bitread_t *br);
void hls_dump_bitread(hls_bitread_t *br);
    
#define hls_bitread_err(br) ((br)->err)
    
#define hls_bitread_eof(br) ((br)->pos == (br)->last)
        
#define hls_bitread_8(br)\
    ((uint8_t) hls_bitread_get(br, 8))
        
#define hls_bitread_16(br)\
    ((uint16_t) hls_bitread_get(br, 16))

#define hls_bitread_32(br)\
    ((uint32_t) hls_bitread_get(br, 32))

#define hls_bitread_64(br)\
    ((uint64_t) hls_bitread_get(br, 64))


#endif // _HLS_BITREAD_H_
