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
 * @(#)hls_bitread.c
 */


#include <string.h>
#include <stdio.h>

#include "hls_bitread.h"

void
hls_bitread_init(hls_bitread_t *br, char *pos, char *last)
{
    memset(br, 0,sizeof(hls_bitread_t));

    br->pos = pos;
    br->last = last;
}


uint64_t
hls_bitread_get(hls_bitread_t *br, uint32_t n)
{
    uint64_t  v;  
    uintptr_t d;
    uint8_t   c;
    uint8_t   pos_v;

    v = 0;

    while (n) {

        if (br->pos >= br->last) {
            br->err = 1;
            return 0;
        }   

        d = (br->offs + n > 8 ? (uintptr_t) (8 - br->offs) : n); 
        
        pos_v = ((uint8_t)*br->pos >> (8 - br->offs - d));
        c = ((uint8_t) 0xff >> (8 - d)); 

        v <<= d;
        //v += (*br->pos >> (8 - br->offs - d)) & ((char) 0xff >> (8 - d));
        v += pos_v & c;

        br->offs += d;
        n -= d;

        if (br->offs == 8) {
            br->pos++;
            br->offs = 0;
        }
    }
    
    return v;
}

uint64_t
hls_bitread_golomb(hls_bitread_t *br)
{
    uintptr_t  n;

    for (n = 0; hls_bitread_get(br, 1) == 0 && !br->err; n++);

    return ((uint64_t) 1 << n) + hls_bitread_get(br, n) - 1;
}

void
hls_dump_bitread(hls_bitread_t *br)
{
   fprintf(stderr,"pos=%p,last=%p,offs=%u,err=%u\n",
                   br->pos,br->last,br->offs,br->err);
}

