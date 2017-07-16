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
 * @(#)hls_mpegts.c
 */

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include "hls_buf.h"
#include "hls_mpegts.h"
#include "hls_stream.h"
#include "hls_utils.h"
#include "hls_cache.h"
#include "muxsvr_mcd_proc.h"
#include "muxsvr_mcd_cfg.h"

static char hls_mpegts_header[] = {

    /* TS */
    0x47, 0x40, 0x00, 0x10, 0x00,
    /* PSI */
    0x00, 0xb0, 0x0d, 0x00, 0x01, 0xc1, 0x00, 0x00,
    /* PAT */
    0x00, 0x01, 0xf0, 0x01,
    /* CRC */
    0x2e, 0x70, 0x19, 0x05,
    /* stuffing 167 bytes */
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,

    /* TS */
    0x47, 0x50, 0x01, 0x10, 0x00,
    /* PSI */
    0x02, 0xb0, 0x17, 0x00, 0x01, 0xc1, 0x00, 0x00,
    /* PMT */
    0xe1, 0x00,
    0xf0, 0x00,
    0x1b, 0xe1, 0x00, 0xf0, 0x00, /* h264 */
    0x0f, 0xe1, 0x01, 0xf0, 0x00, /* aac */
    /*0x03, 0xe1, 0x01, 0xf0, 0x00,*/ /* mp3 */
    /* CRC */
    0x2f, 0x44, 0xb9, 0x9b, /* crc for aac */
    /*0x4e, 0x59, 0x3d, 0x1e,*/ /* crc for mp3 */
    /* stuffing 157 bytes */
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

/* 700 ms PCR delay */
#define HLS_DELAY  63000


int
hls_flush_audio(hls_session_t *s)
{
    hls_mpegts_ctx_t             *ctx;
    hls_mpegts_frame_t         frame;
    int64_t                       rc;  
    hls_buf_t                      *b;  

    ctx = &s->mpegts_ctx;

    if (ctx == NULL || !ctx->opened) {
        return 0;
    }    

    b = &ctx->aframe;
    
    if (b == NULL || b->pos == b->last) {
        DEBUG("aframe is empty, videoid=%u, pos=%p,last=%p",s->videoid,b->pos,b->last);
        return 0;
    }    

    //ERROR("hls: flush audio buffer, pos=%p,last=%p",b->last,b->pos);

    memset(&frame, 0,sizeof(frame));

    frame.dts = ctx->aframe_pts;
    frame.pts = frame.dts;
    frame.cc = ctx->audio_cc;
    frame.pid = 0x101;
    frame.sid = 0xc0;

    rc = hls_mpegts_write_frame(s, &frame, b);

    if (rc != 0) {
        DEBUG("hls: audio flush failed, videoid=%u", s->videoid);
    }

    ctx->audio_cc = frame.cc;
    b->pos = b->last = b->start;
    //ERROR("hls: audio flush done, videoid=%u", s->videoid);

    return rc;
}


hls_frag_t*
hls_get_frag(hls_session_t *s, int n)
{
   hls_mpegts_ctx_t *ctx = &s->mpegts_ctx;

   return  &ctx->frags[(ctx->frag + n) % (s->winfrags * 2 + 1)];
}

int
hls_get_frag_num(hls_session_t *s, int n)
{
   hls_mpegts_ctx_t *ctx = &s->mpegts_ctx;

   return  (ctx->frag + n) % (s->winfrags * 2 + 1);
}

int
hls_mpegts_close_file(hls_mpegts_file_t *file)
{
   close(file->fd);

   return 0;
}

int
hls_next_frag(hls_session_t *s)
{
   hls_mpegts_ctx_t *hctx = &s->mpegts_ctx;

   if ( hctx->nfrags == s->winfrags ) {
      hctx->frag++;
      s->cleanup = 1;
   } else {
      hctx->nfrags++;
   }

   //ERROR("cleanup: frag=%llu,nfrags=%u,winfrags=%u,cleanup=%u",
   //      hctx->frag,hctx->nfrags,ctx->winfrags,ctx->cleanup);

   return 0;
}


int
hls_write_playlist(hls_session_t *s)
{
    static char       buffer[1024];
    uint64_t          i, max_frag;
    char             *p, *end;
    hls_mpegts_ctx_t *ctx;
    hls_frag_t       *f = NULL;  
    size_t            n;   
 
    if ( s == NULL )
       return -1;

    s->fragmem->playlist_len = 0;

    ctx = &s->mpegts_ctx;

    max_frag = s->fraglen / 1000;

    for (i = 0; i < ctx->nfrags; i++) {
        f = hls_get_frag(s, i);
        if (f->duration > max_frag) {
            max_frag = (uint64_t) (f->duration + .5); 
        }    
    }    

    p = buffer;
    end = p + sizeof(buffer);

    DEBUG("live: writing m3u8 header, videoid=%u", s->videoid); 

    hls_write_fragmem_playlist_clean(s->fragmem);

//#define _HLS_TEST_
#ifdef _HLS_TEST_
    uint64_t frag = 0;
    hls_mem_t *buf;
    for (i = 0; i < ctx->nfrags*2; i++) {
        f = hls_get_frag(s, i-6);

        buf = s->fragmem->mem + (hls_get_frag_num(s,i-6)%MAX_FRAGMENT_NUM);
        if ( buf->key ) {
            frag = buf->id;
            break;
        }
    }

    n = snprintf(p, end - p,
                     "#EXTM3U\n"
                     "#EXT-X-VERSION:3\n"
                     "#EXT-X-MEDIA-SEQUENCE:%llu\n"
                     "#EXT-X-TARGETDURATION:%llu\n",
                     //"#EXT-X-DISCONTINUITY\n",
                     frag, max_frag);

#else
    n = snprintf(p, end - p,
                     "#EXTM3U\n"
                     "#EXT-X-VERSION:3\n"
                     "#EXT-X-MEDIA-SEQUENCE:%llu\n"
                     "#EXT-X-TARGETDURATION:%llu\n",
                     //"#EXT-X-DISCONTINUITY\n",
                     ctx->frag, max_frag);
#endif
    if ( n >= sizeof(buffer) ) {
       ERROR("snprintf failed.\n");
    }
    p += n;


    n = hls_write_fragmem_playlist(s->fragmem,buffer,p-buffer);
    if (n < 0) {
        ERROR("writing fragmem playlist failed: videoid=%u,ret=%d",s->videoid, n);
        return -3;
    }

#ifdef _HLS_TEST_
    int start = 0;
    int cnt = 0;
    for (i = 0; i < ctx->nfrags*2; i++) {
        f = hls_get_frag(s, i-6);

        buf = s->fragmem->mem + (hls_get_frag_num(s,i-6)%MAX_FRAGMENT_NUM);

        if ( buf->key ) 
            start = 1;

        if ( start == 0 )
            continue;
#else
    for (i = 0; i < ctx->nfrags; i++) {
        f = hls_get_frag(s, i);
#endif

        p = buffer;
        end = p + sizeof(buffer);

        if (0 && f->discont) {

            n = snprintf(p, end-p, "#EXT-X-DISCONTINUITY\n");
            if ( n < ((size_t)(end - p))) {
               p += n;
            } else {
               ERROR("hls: writing failed: ret=%d,videoid=%u", n, s->videoid);
               return -1;
            }
        }

        DEBUG("duration=%f,videoid=%u, id=%u\n", f->duration, s->videoid, f->id);
/*
        n = snprintf(p, end-p,
                         "#EXTINF:%.3f,\n"
                         "%s%s%s%llu.ts\n",
                         f->duration, s->base_url.data, name_part.data, sep, f->id);
*/
        n = snprintf(p, end-p,
                         "#EXTINF:%.3f,\n"
                         "%u-%llu.ts\n",
                         f->duration, s->videoid, f->id);
        if ( n < ((size_t)(end - p))) {
           p += n;
        } else {
           ERROR("hls: writing failed: videoid=%u,ret=%d",s->videoid,n);
           return -1;
        }

        DEBUG( "hls: fragment frag=%uL, n=%ui/%ui, duration=%.3f, discont=%i",
                       ctx->frag, i + 1, ctx->nfrags, f->duration, f->discont);
   
        n = hls_write_fragmem_playlist(s->fragmem,buffer,p-buffer);
        if (n < 0) {
            ERROR("writing fragmem playlist failed: videoid=%u,ret=%d",s->videoid, n);
            return -4;
        }
   
#ifdef _HLS_TEST_
//        cnt++;
//        if ( cnt == 6 )
//            break;
#endif
    }

    return 0;
}

void 
hls_send_playlist_to_cache(hls_session_t *s)
{
   std::vector<char> buff;
   CMCDProc *t_MCD;
   int ret = 0;
   uint32_t type = htonl(0);
   uint32_t videoid = 0;
   uint32_t playlist_len = 0;

   if ( s == NULL ) {
      ERROR("session is null");
      return;
   }

   t_MCD = (CMCDProc*)s->hMCD;
   if ( t_MCD == NULL ) {
      ERROR("mcd process is null");
      return;
   }
   
   //ERROR("M3U8 information before: videoID = %u,len = %u",videoid,playlist_len );
   videoid = htonl(s->fragmem->videoid);
   playlist_len = htonl(s->fragmem->playlist_len);

   buff.resize(sizeof(type)+sizeof(videoid)+sizeof(playlist_len)+s->fragmem->playlist_len);
//   if(s->fragmem->playlist_len==0)
//       return -1;
   //ERROR("M3U8 information: videoID = %u,len = %u",videoid,playlist_len );

   memcpy(&buff[0],&type,sizeof(type));
   memcpy(&buff[0]+sizeof(type),&videoid,sizeof(videoid));
   memcpy(&buff[0]+sizeof(type)+sizeof(videoid),&playlist_len,sizeof(playlist_len));
   memcpy(&buff[0]+sizeof(type)+sizeof(videoid)+sizeof(playlist_len),s->fragmem->playlist,s->fragmem->playlist_len);

   //ERROR("HTTP POST REQUST, videoid=%u, len=%u", videoid, playlist_len);       
   string m_strStreamSvrPostHead;
   m_strStreamSvrPostHead.append("POST QQTALKSTREAM / HTTP/1.1 \r\n");
   m_strStreamSvrPostHead.append("Accept: */* \r\n");
   m_strStreamSvrPostHead.append("Content-Type: multipart/binary_data \r\n");
   m_strStreamSvrPostHead.append("Content-Length: ");

   stringstream ss;
   ss << buff.size();
   string strPostData;
   strPostData.append(m_strStreamSvrPostHead);
   strPostData.append(ss.str());
   strPostData.append("\r\n\r\n");
   strPostData.append(&buff[0],buff.size());

   //ERROR("SEND HTTP MSG: %s\n",strPostData.c_str());

   if(t_MCD){

       for (unsigned int i=0; i<t_MCD->_cfg.m_uiCacheSvrIp.size(); i++)
       {

           ret =t_MCD->enqueue_2_dcc2(strPostData.c_str(),strPostData.size(),
                                      t_MCD->_cfg.m_uiCacheSvrIp[i], t_MCD->_cfg.m_usCacheSvrPort[0]);
           //ERROR("SEND M3U8 TO CACHE");
           if ( ret < 0 ) {
              ERROR("Failed to send m3u8 to cachsvr, videoid=%u,ret=%d",videoid,ret);
           }
       }

   }

    return;
}


/**
  * byte ordering function for 64bit variable
  */
uint64_t htonll(uint64_t host_longlong)
{
    int x = 1;
 
    /* little endian */
    if(*(char *)&x == 1)
        return ((((uint64_t)htonl(host_longlong)) << 32) + htonl(host_longlong >> 32));
 
    /* big endian */
    else
        return host_longlong;
}
 
uint64_t ntohll(uint64_t host_longlong)
{
    int x = 1;
 
    /* little endian */
    if(*(char *)&x == 1)
        return ((((uint64_t)ntohl(host_longlong)) << 32) + ntohl(host_longlong >> 32));
 
    /* big endian */
    else
        return host_longlong;
 
}

void
hls_send_ts_to_cache(hls_session_t *s)
{
   int ret = 0;
   static char buff[HLS_MAX_TS_LEN + 1024];
   //std::vector<char> buff;
   CMCDProc *t_MCD;
   uint32_t type = htonl(1);
   uint64_t fragid;
   uint32_t videoid;
   hls_mem_t *mem;
   uint32_t len, total_len;

   if ( s == NULL ) {
      return;
   }

   t_MCD = (CMCDProc*)s->hMCD;
   if ( t_MCD == NULL ) {
      return; 
   }

   fragid = htonll(s->fragmem->fragid);
   videoid = htonl(s->fragmem->videoid);
   mem = &s->fragmem->mem[(s->fragmem->fragid)%MAX_FRAGMENT_NUM];
   len = htonl(mem->len);

   if ( mem->len < 0 || mem->len >HLS_MAX_TS_LEN ) {
       ERROR("wrong mem: videoID=%u,fragid=%llu,len =%u, maxlen=%u",
             videoid,fragid,len,HLS_MAX_TS_LEN);
       return;
   }
   //buff.resize(sizeof(type)+sizeof(videoid)+sizeof(fragid)+sizeof(len)+len);
   //  if(len==0)
   //     return -1;
   //ERROR("TS information: videoID = %u, fragid = %llu, len = %u\n", videoid,fragid,len);

   /*memcpy(&buff[0],&type,sizeof(type));
   memcpy(&buff[0]+sizeof(type),&videoid,sizeof(videoid));
   memcpy(&buff[0]+sizeof(type)+sizeof(videoid),&fragid,sizeof(fragid));
   memcpy(&buff[0]+sizeof(type)+sizeof(videoid)+sizeof(fragid),&len,sizeof(len));
   memcpy(&buff[0]+sizeof(type)+sizeof(videoid)+sizeof(fragid)+sizeof(len),&mem,len);*/

   memcpy(buff,&type,sizeof(type));
   memcpy(buff+sizeof(type),&videoid,sizeof(videoid));
   memcpy(buff+sizeof(type)+sizeof(videoid),&fragid,sizeof(fragid));
   memcpy(buff+sizeof(type)+sizeof(videoid)+sizeof(fragid),&len,sizeof(len));
   memcpy(buff+sizeof(type)+sizeof(videoid)+sizeof(fragid)+sizeof(len),mem,mem->len + 24);

   total_len = sizeof(type)+sizeof(videoid)+sizeof(fragid) + sizeof(len) + mem->len + 24;

   string m_strStreamSvrPostHead;
   m_strStreamSvrPostHead.append("POST QQTALKSTREAM / HTTP/1.1 \r\n");
   m_strStreamSvrPostHead.append("Accept: */* \r\n");
   m_strStreamSvrPostHead.append("Content-Type: multipart/binary_data \r\n");
   m_strStreamSvrPostHead.append("Content-Length: ");
   stringstream ss;
   //ss << buff.size();
   ss << total_len;
   string strPostData;
   strPostData.append(m_strStreamSvrPostHead);
   strPostData.append(ss.str());
   strPostData.append("\r\n\r\n");
   //strPostData.append(&buff[0],buff.size());
   strPostData.append(buff,total_len);

   //hexdump((char*)strPostData.c_str(),128,"TS");

   if(t_MCD){

       for (unsigned int i=0; i<t_MCD->_cfg.m_uiCacheSvrIp.size(); i++)
       {
           ret =t_MCD->enqueue_2_dcc2(strPostData.c_str(),strPostData.size(),
                     t_MCD->_cfg.m_uiCacheSvrIp[i], t_MCD->_cfg.m_usCacheSvrPort[0]);
           if ( ret < 0 ) {
              ERROR("Failed to send ts to cachsvr, videoid=%u,ret=%d",videoid,ret);
           }
       }
   }

   return; 

}

int
hls_close_fragment(hls_session_t *s)
{
   hls_mpegts_ctx_t *ctx;

   if ( s == NULL )
      return -1;

   ctx = &s->mpegts_ctx;

   if ( ctx == NULL || !ctx->opened )
      return 0;
   
   ctx->opened = 0; 
   ERROR("close fragment, videoid=%u, ts=%" PRIu64 "", s->videoid, ctx->frag_ts);

   hls_next_frag(s);

   // update m3u8 file
   hls_write_playlist(s);

   hls_send_playlist_to_cache(s);
   hls_send_ts_to_cache(s);

   return 0;
}

static uint64_t
hls_get_fragment_id(hls_session_t *s, uint64_t ts)
{
    hls_mpegts_ctx_t        *ctx;

    if ( s == NULL )
       return -1;

    ctx = &s->mpegts_ctx;

    return ctx->frag + ctx->nfrags;
}

int
hls_mpegts_write_file(hls_mpegts_file_t *file, char *in, size_t in_size)
{
    ssize_t   rc;

    if (!file->encrypt) {

        rc = write(file->fd, in, in_size);

        if (rc < 0) {
            return -1;
        }

    }
    return 0;
}

int
hls_mpegts_write_header(hls_mpegts_file_t *file)
{
    return hls_mpegts_write_file(file, hls_mpegts_header,
                                      sizeof(hls_mpegts_header));
}

int
hls_mpegts_open_file(hls_mpegts_file_t *file, char *path)
{
    file->fd = open(path, O_CREAT|O_WRONLY|O_TRUNC, 0644);

    if (file->fd < 0) {
        DEBUG( "hls: error creating fragment file, path=%s, ret=%d\n",
                   path, file->fd);
        return -1;
    }   

    file->size = 0;
    file->encrypt = 0;

    if (hls_mpegts_write_header(file) != 0) {
        DEBUG( "hls: error writing fragment header\n");
        close(file->fd);
        return -1;
    }   

    return 0;
}


int
hls_open_fragment(hls_session_t *s, uint64_t ts, uint64_t discont)
{
    hls_mpegts_ctx_t  *ctx;
    hls_frag_t        *f;  
    uint64_t           id;  
    uint64_t           g;   
    int ret;

    if ( s == NULL )
       return -1;

    ctx = &s->mpegts_ctx;

    if (ctx->opened) {
        return 0;
    }    

    ERROR("hls: open fragment, opened=%u, videoid=%u, ts=%" PRIu64 " ",
                  ctx->opened,s->videoid,ts);

    // TODO: before getting a new ts segment, send the current ts to cache server.

    id = hls_get_fragment_id(s, ts); 

    if (s->granularity) {
        g = (uint64_t) s->granularity;
        id = (uint64_t) (id / g) * g; 
    }    

    ctx->opened = 1;

    f = hls_get_frag(s, ctx->nfrags);

    memset(f, 0,sizeof(*f));

    f->active = 1;
    f->discont = discont;
    f->id = id;
    ctx->frag_ts = ts;

    ret = hls_fragmem_open(s->fragmem,id);
    if ( ret < 0 ) {
        ERROR("could not open fragmem, videoid=%u,ret=%d",s->videoid,ret);
        return ret;
    }
    hls_write_fragmem_data(s,s->fragmem,hls_mpegts_header,sizeof(hls_mpegts_header),0);

    /* start fragment with audio to make iPhone happy */
    hls_flush_audio(s);

    return 0;
}

int
hls_frag_set_key_frame(hls_session_t *s)
{
   hls_mpegts_ctx_t *ctx;
   hls_frag_t       *f;

   if ( s == NULL )
      return -1;
   
   ctx = &s->mpegts_ctx;
   f = hls_get_frag(s,ctx->nfrags);
   f->key = 1;

   return 0;
}

int
hls_update_fragments(hls_session_t *s, uint64_t ts, int boundary, int flush_rate)
{
   hls_mpegts_ctx_t *ctx;
   hls_frag_t       *f;
   uint64_t          ts_frag_len;
   uint64_t          same_frag,discont;
   hls_buf_t        *b;
   int64_t           d;
   int force;

   if ( s == NULL )
      return -1;
   
   ctx = &s->mpegts_ctx;

   f = NULL;
   force = 0;
   discont = 1;

   if ( ctx->opened ) {

      f = hls_get_frag(s,ctx->nfrags);
      d = (int64_t)(ts - ctx->frag_ts);

      ERROR("fragment info,videoid=%u, duration=%f, ts=%"PRIu64",frag_ts=%"PRIu64,
               s->videoid,f->duration,ts,ctx->frag_ts);

      if  ( /*(ctx->frag_ts != 0) &&*/ (d > (int64_t) s->max_fraglen*90 ||  d < -90000)) {
         ERROR("enforcing fragment,videoid=%u, d=%"PRId64",ts=%"PRIu64",frag_ts=%"PRIu64,
               s->videoid,d,ts,ctx->frag_ts);
         force = 1;
      } else {
         f->duration = (ts - ctx->frag_ts) / 90000.;
         discont = 0;

         hls_fragmem_update_duration(s->fragmem,f->duration);

      }
   }

   ERROR("fragment info,videoid=%u, boundary=%u, ts=%"PRIu64, s->videoid, boundary, ts);

   switch(s->slicing) {
      case 1: // slicing plain.
            if ( f && f->duration < s->fraglen/1000.) {
               boundary = 0;
            }
            break;

      case 2:  //slicing aligned.

            ts_frag_len = s->fraglen * 90;
            same_frag = ctx->frag_ts / ts_frag_len == ts / ts_frag_len;

            if (f && same_frag) {
                boundary = 0;
            }

            if (f == NULL && (ctx->frag_ts == 0 || same_frag)) {
                ctx->frag_ts = ts;
                boundary = 0;
            }

        break;
   }

   // do we need to create a new ts segment?
   if ( boundary || force ) {
      hls_close_fragment(s);
      ERROR("open fragment, boundary=%u, force=%u, ts=%" PRIu64 " ", boundary, force, ts);
      hls_open_fragment(s,ts,discont);
   }

   b = &ctx->aframe;

   if ( ctx->opened && b && b->last > b->pos && 
        ctx->aframe_pts + (uint64_t)s->max_audio_delay*90/flush_rate < ts ) {
      hls_flush_audio(s);
   }

   return 0;
}



int
hls_audio_handler(hls_session_t *s, char *buf, int len, unsigned int timestamp)
{
   hls_mpegts_ctx_t *ctx;
   hls_codec_ctx_t  *codec_ctx;
   hls_buf_t        *b;
   size_t            bsize;
   uint64_t          pts,est_pts;
   int64_t           dpts;
   char             *p = buf;
   uint64_t          objtype, srindex, chconf, size;
  
   if ( s == NULL ) {
      ERROR("hls session is null");
      return -1;
   } 

   if ( len <= 0 ) {
      ERROR("audio length is wrong, len=%d",len);
      return -1;
   }

   ctx = &s->mpegts_ctx;
   codec_ctx = &s->codec_ctx;

   b = &ctx->aframe;
/*
   if ( b == NULL ) {

      b = (hls_buf_t*)hls_alloc(sizeof(hls_buf_t));
      if ( b == NULL )
         return -1;

      b->start = (char*)hls_alloc(s->audio_buffer_size);
      if ( b->start == NULL )
         return -2;

      b->end = b->start + s->audio_buffer_size;
      b->pos = b->last = b->start;

      ctx->aframe = b;
   }
*/
   size = len + 7; // reserve 7 bytes for headers.
   pts = (uint64_t)timestamp * 90; 

   if ( b->start + size > b->end ) {
      ERROR("too big audio frame, size=%llu",size);
      return 0;
   }

   // TODO: boundary.
   //ERROR("audio frame update, ts=%llu, timestamp=%u",pts, timestamp);
   hls_update_fragments(s,pts,0,2);

   if (b->last + size > b->end) {
        hls_flush_audio(s); 
   } 

   if (b->last + 7 > b->end) {
       ERROR("not enough buffer for audio header, size=%u, len=%u, videoid=%u", 
              size, b->end-b->last,s->videoid);
       //FIX: do temporarily
       b->pos = b->last = b->start;
       return 0;
   }    

   p = b->last;
   b->last += 7;

   /* copy payload */
   bsize = len;//packet->size;
   if (b->last + bsize > b->end) {
       bsize = b->end - b->last;
   }    
   b->last = cpymem(b->last, buf/*packet->data*/, bsize);

   /* make up ADTS header */
   if (hls_parse_aac_header(s, &objtype, &srindex, &chconf) != 0) {
       ERROR("hls: audio aac header error");
       return 0;
   }

   //hexdump(b->pos, b->last-b->pos, "af2");

   /* we have 5 free bytes + 2 bytes of RTMP frame header */
   /* ADTS header, see ISO-13818-7, section 6.2 */

   p[0] = 0xff;
   p[1] = 0xf1;
   p[2] = (char) (((objtype - 1) << 6) | (srindex << 2) |
                    ((chconf & 0x04) >> 2));
   p[3] = (char) (((chconf & 0x03) << 6) | ((size >> 11) & 0x03));
   p[4] = (char) (size >> 3);
   p[5] = (char) ((size << 5) | 0x1f);
   p[6] = 0xfc;

   if (p != b->start) {
       ctx->aframe_num++;
       return 0;
   }

   ctx->aframe_pts = pts;

   //DEBUG( "hls: sync=%u, sample_rate=%u\n",
   //               hctx->sync, codec_ctx->sample_rate);
   if (!s->sync || codec_ctx->sample_rate == 0) {
       return 0;
   }

   /* align audio frames */

   /* TODO: We assume here AAC frame size is 1024
    *       Need to handle AAC frames with frame size of 960 */

   est_pts = ctx->aframe_base + ctx->aframe_num * 90000 * 1024 /
                                 codec_ctx->sample_rate;
   dpts = (int64_t) (est_pts - pts);

   //DEBUG("hls: audio sync dpts=%L (%.5fs)",
   //               dpts, dpts / 90000.);

   if (dpts <= (int64_t) s->sync * 90 &&
       dpts >= (int64_t) s->sync * -90)
   {
       ctx->aframe_num++;
       ctx->aframe_pts = est_pts;
       return 0;
   }

   ctx->aframe_base = pts;
   ctx->aframe_num  = 1;

   DEBUG("hls: audio sync gap dpts=%L (%.5fs)",
                  dpts, dpts / 90000.);

   return 0;
}

int
hls_append_sps_pps(hls_session_t *s, hls_buf_t *out)
{
    hls_codec_ctx_t           *codec_ctx;
    char                         *p;
    //hls_buf_t                    in_buf;
    hls_buf_t                    *in;
    hls_mpegts_ctx_t             *ctx;
    int8_t                          nnals;
    uint16_t                        len, rlen;
    int64_t                       n;

    if ( s == NULL )
       return -1;

    ctx = &s->mpegts_ctx;
    codec_ctx = &s->codec_ctx;

    if (ctx == NULL || codec_ctx == NULL) {
        ERROR("mpegts or codec ctx is NULL, videoid=%u",s->videoid);
        return -1;
    }

    if ( codec_ctx->avc_header == NULL ) {
        ERROR("avc_header is NULL, videoid=%u",s->videoid);
        return -2;
    }

    in = codec_ctx->avc_header;
/*
    in = &in_buf;
    in->pos = codec_ctx->avc_header.pos;
    in->last = codec_ctx->avc_header.last;
    in->start = codec_ctx->avc_header.start;
    in->end = codec_ctx->avc_header.end;
*/
    if (in == NULL) {
        return -1;
    }

    p = in->pos;

    /*
     * Skip bytes:
     * - flv fmt
     * - H264 CONF/PICT (0x00)
     * - 0
     * - 0
     * - 0
     * - version
     * - profile
     * - compatibility
     * - level
     * - nal bytes
     */

    //if (hls_copy(s, NULL, &p, 10, &in) != 0) {
    //    return -1;
    //}

    // ignore the first 5 bytes.
    if (hls_copy(s, NULL, &p, 5, in) != 0) {
        return -1;
    }
    
    /* number of SPS NALs */
    if (hls_copy(s, &nnals, &p, 1, in) != 0) {
        return -1;
    }

    nnals &= 0x1f; /* 5lsb */

    //DEBUG( "hls: SPS number: %u\n", nnals);
    /* SPS */
    for (n = 0; ; ++n) {
        for (; nnals; --nnals) {

            /* NAL length */
            if (hls_copy(s, &rlen, &p, 2, in) != 0) {
                return -1;
            }

            rmemcpy(&len, &rlen, 2);

            //DEBUG( "hls: header NAL length: %u\n", (size_t) len);

            /* AnnexB prefix */
            if (out->end - out->last < 4) {
                ERROR( "hls: too small buffer for header NAL size\n");
                return -1;
            }

            *out->last++ = 0;
            *out->last++ = 0;
            *out->last++ = 0;
            *out->last++ = 1;

            /* NAL body */
            if (out->end - out->last < len) {
                ERROR( "hls: too small buffer for header NAL\n");
                return -1;
            }

            if (hls_copy(s, out->last, &p, len, in) != 0) {
                return -1;
            }

            out->last += len;
        }
        if (n == 1) {
            break;
        }

        /* number of PPS NALs */
        if (hls_copy(s, &nnals, &p, 1, in) != 0) {
            return -1;
        }

        //DEBUG( "hls: PPS number: %u\n", nnals);
    }

    return 0;
}


int
hls_append_aud(hls_session_t *s, hls_buf_t *out)
{
    /* access unit delimiter, see ISO 14496-10, section 7.4.1 */
    static char   aud_nal[] = { 0x00, 0x00, 0x00, 0x01, 0x09, 0xf0 };

    if (out->last + sizeof(aud_nal) > out->end) {
        return -1;
    }

    out->last = cpymem(out->last, aud_nal, sizeof(aud_nal));

    return 0;
}

char *
hls_mpegts_write_pcr(char *p, uint64_t pcr)
{
    *p++ = (u_char) (pcr >> 25);
    *p++ = (u_char) (pcr >> 17);
    *p++ = (u_char) (pcr >> 9);
    *p++ = (u_char) (pcr >> 1);
    *p++ = (u_char) (pcr << 7 | 0x7e);
    *p++ = 0;

    return p;
}

char *
hls_mpegts_write_pts(char *p, uint64_t fb, uint64_t pts)
{
    uint64_t val;

    val = fb << 4 | (((pts >> 30) & 0x07) << 1) | 1;
    *p++ = (char) val;

    val = (((pts >> 15) & 0x7fff) << 1) | 1;
    *p++ = (char) (val >> 8);
    *p++ = (char) val;

    val = (((pts) & 0x7fff) << 1) | 1;
    *p++ = (char) (val >> 8);
    *p++ = (char) val;

    return p;
}

int
hls_mpegts_write_frame(hls_session_t *s, hls_mpegts_frame_t *f, hls_buf_t *b)
{   
    uint64_t  pes_size, header_size, body_size, in_size, stuff_size, flags;
    char      packet[188], *p, *base;
    int64_t   first, rc;
    
   
    //ERROR("mpegts: pid=%x, sid=%lu, pts=%lu, "
    //               "dts=%lu, key=%lu, size=%lu\n",
    //               f->pid, f->sid, f->pts, f->dts,
    //               f->key, (size_t) (b->last - b->pos));

    first = 1;

    /* MPEGTS format */
    while (b->pos < b->last) {
        p = packet;

        f->cc++;

        /* TS header has following format: 
           - first byte: sync byte of pattern 0x47.
           - second & third byte: 3 most sigfi. bits: error, start indicator, 
                          transport priority and 13 lower bits for packet ID.
           - forth byte: 2-bits scrambling control, 
                         2-bits adpation field control, 
                         4-bits continuity counter.
           - optional 4-bytes: adaption field.  */

        *p++ = 0x47;
        *p++ = (char) (f->pid >> 8);

        if (first) {
            p[-1] |= 0x40;
        }

        *p++ = (char) f->pid;
        *p++ = 0x10 | (f->cc & 0x0f); /* no adaptation & payload only */

        if (first) {

            if (f->key) {
                packet[3] |= 0x20; /* adaptation */

                *p++ = 7;    /* size */
                *p++ = 0x50; /* random access + PCR */
                p = hls_mpegts_write_pcr(p, f->dts - HLS_DELAY);
            }

            /* PES header */
            *p++ = 0x00;
            *p++ = 0x00;
            *p++ = 0x01;
            *p++ = (char) f->sid;

            header_size = 5;
            flags = 0x80; /* PTS */

            if (f->dts != f->pts) {
                header_size += 5;
                flags |= 0x40; /* DTS */
            }

            pes_size = (b->last - b->pos) + header_size + 3;

            if (pes_size > 0xffff) {
                pes_size = 0;
            }


            *p++ = (char) (pes_size >> 8);
            *p++ = (char) pes_size;
            *p++ = 0x80; /* H222 */
            *p++ = (char) flags;
            *p++ = (char) header_size;

            p = hls_mpegts_write_pts(p, flags >> 6, f->pts + HLS_DELAY);

            if (f->dts != f->pts) {
                p = hls_mpegts_write_pts(p, 1, f->dts + HLS_DELAY);
            }

            first = 0;
        }
        body_size = (uint64_t) (packet + sizeof(packet) - p);
        in_size = (uint64_t) (b->last - b->pos);

        if (body_size <= in_size) {
            memcpy(p, b->pos, body_size);
            b->pos += body_size;

        } else {
            stuff_size = (body_size - in_size);

            if (packet[3] & 0x20) {

                /* has adaptation */
                base = &packet[5] + packet[4];
                p = hls_movemem(base + stuff_size, base, p - base);
                memset(base, 0xff, stuff_size);
                packet[4] += (u_char) stuff_size;

            } else {

                /* no adaptation */
                packet[3] |= 0x20;
                p = hls_movemem(&packet[4] + stuff_size, &packet[4],
                                p - &packet[4]);

                packet[4] = (char) (stuff_size - 1);
                if (stuff_size >= 2) {
                    packet[5] = 0;
                    memset(&packet[6], 0xff, stuff_size - 2);
                }
            }

            memcpy(p, b->pos, in_size);
            b->pos = b->last;
        }

        rc = hls_write_fragmem_data(s,s->fragmem, packet, sizeof(packet), f->key);
        if (rc < 0) {
            ERROR("writing fragmem failed, videoid=%u, ret=%d",s->videoid,rc);
            return rc;
        }

    }

    return 0;
}

int
hls_video_handler(hls_session_t *s, uint8_t iskey, char *packet, int packet_size, unsigned int timestamp)
{
    hls_mpegts_ctx_t     *ctx;
    hls_codec_ctx_t      *codec_ctx;
    char                 *p;
    uint8_t              ftype, nal_type, src_nal_type;
    uint32_t             len, rlen;
    hls_buf_t            out, tmp_in, *b,*in;
    uint32_t             cts;
    hls_mpegts_frame_t   frame;
    uint32_t             nal_bytes;
    int64_t              aud_sent, sps_pps_sent, boundary;
    static char          buffer[HLS_BUFSIZE];

    if ( s == NULL ) {
       ERROR("session is NULL");
       return -1;
    }

    if ( s->videoid == 0 ) {
       ERROR("video is zero, videoid=%u", s->videoid);
       return -1;
    }

    ctx = &s->mpegts_ctx; 
    codec_ctx = &s->codec_ctx;

    if ( s->fps != 0 )
      s->dts += (1000/s->fps);

    if (ctx == NULL || codec_ctx == NULL 
           || codec_ctx->avc_header == NULL) {
        ERROR("not processed, videoid=%u,ctx=%p,codec_ctx=%p",
              s->videoid,ctx,codec_ctx);
        return 0;
    }

    /* Only H264 is supported */
/*
    if (codec_ctx->video_codec_id != AV_CODEC_ID_H264) {
        ERROR("codec not supported");
        return 0;
    }
*/
    in = &tmp_in;
    memset(in,0,sizeof(*in));
    in->pos = packet;
    in->start = in->pos;
    in->last = in->end = in->pos + packet_size;
    p = in->pos;


#ifdef _FROM_HLS_STREAM_
    if (hls_copy(s, &fmt, &p, 1, in) != 0) {
        return 0;
    }

    /* 1: keyframe (IDR)
     * 2: inter frame
     * 3: disposable inter frame 
       see FLV file format spec 10.1, section E.4.3.1 */

    ftype = (fmt & 0xf0) >> 4;

    /* H264 HDR/PICT */
    if (hls_copy(s, &htype, &p, 1, in) != 0) {
        return -1;
    }

    /* proceed only with PICT */
    if (htype != 1) {
        return 0;
    }

    /* 3 bytes: decoder delay */
    if (hls_copy(s, &cts, &p, 3, in) != 0) {
        return -1;
    }

    cts = ((cts & 0x00FF0000) >> 16) | ((cts & 0x000000FF) << 16) |
          (cts & 0x0000FF00);

#endif //_FROM_HLS_STREAM_

    ftype = iskey; /*packet->flags & AV_PKT_FLAG_KEY*/; //TODO: key frame?
    cts = 0;

    memset(&out, 0,sizeof(out));

    out.start = buffer;
    out.end = buffer + sizeof(buffer);
    out.pos = out.start;
    out.last = out.pos;

    nal_bytes = codec_ctx->avc_nal_bytes;
    aud_sent = 0;
    sps_pps_sent = 0;

    //ERROR("begin while loop, nal_bytes=%u,p=%p,las=%p",nal_bytes,p,in->last);
    while (p < in->last) {

        /* TODO: which docs defines this NAL unit length? */
        if (hls_copy(s, &rlen, &p, nal_bytes, in) != 0) {
            ERROR("faile to copy nal_bytes, videoid=%u", s->videoid);
            return 0;
        }

        len = 0;
        rmemcpy(&len, &rlen, nal_bytes);

        if ( nal_bytes > 4 ) {
           ERROR("info nal_bytes, nal_bytes=%u,videoid=%u", nal_bytes,s->videoid);
        }
        if (len == 0 ) {
            ERROR("failed to copy nal_bytes, nal_bytes=%u,rlen=%u,len=%u,videoid=%u",
                   nal_bytes,rlen,len,s->videoid);
            return -1;
            continue;
        }

        if (hls_copy(s, &src_nal_type, &p, 1, in) != 0) {
            ERROR("faile to copy src_nal_type, videoid=%u", s->videoid);
            return 0;
        }

        nal_type = src_nal_type & 0x1f;

        if (nal_type >= 7 && nal_type <= 9) {
            if (hls_copy(s, NULL, &p, len - 1, in) != 0) {
                ERROR("faile to copy nal_type, videoid=%u", s->videoid);
                return -1;
            }
            //ERROR("wrong nal_type, nal_type=%u,videoid=%u", nal_type,s->videoid);
            continue;
        }

        if (!aud_sent) {
            switch (nal_type) {
                case 1:
                case 5:
                case 6:

                    if (hls_append_aud(s, &out) != 0) {
                        ERROR("error appending AUD NAL");
                    }
                case 9:
                    aud_sent = 1;
                    break;
            }
        }

        switch (nal_type) {
            case 1:
                sps_pps_sent = 0;
                break;
            case 5:
                if (sps_pps_sent) {
                    break;
                }

                if (hls_append_sps_pps(s, &out) != 0) {
                    ERROR("error appenging SPS/PPS NALs");
                }
                sps_pps_sent = 1;
                break;
        }

        /* AnnexB prefix */
        if (out.end - out.last < 5) {
            ERROR("not enough buffer, videoid=%u", s->videoid);
            return 0;
        }

        /* first AnnexB prefix is long (4 bytes) */
        if (out.last == out.pos) {
            *out.last++ = 0;
        }

        /* XXX: insert start prefix? */
        *out.last++ = 0;
        *out.last++ = 0;
        *out.last++ = 1;
        *out.last++ = src_nal_type;

        /* NAL body */

        if (out.end - out.last < (int64_t) len) {
            ERROR("not enough buffer for NAL,len=%u,videoid=%u", len, s->videoid);
            return 0;
        }

        if (hls_copy(s, out.last, &p, len - 1, in) != 0) {
            ERROR("faile to copy,len=%u,videoid=%u", len, s->videoid);
            return 0;
        }

        out.last += (len - 1);

        if ( in->pos == in->last )
           break;
    }

    memset(&frame,0,sizeof(frame));

    frame.cc = ctx->video_cc;
    frame.dts = (uint64_t) timestamp * 90; //h->timestamp * 90;
    frame.pts = frame.dts + cts * 90;
    frame.pid = 0x100;
    frame.sid = 0xe0;
    frame.key = (ftype == 1);

    ERROR("frame dts,videoid=%u, timestamp=%u, dts=%" PRIu64 " ", s->videoid, timestamp, frame.dts);

    /*
     * start new fragment if
     * - we have video key frame AND
     * - we have audio buffered or have no audio at all or stream is closed
     */

    b = &ctx->aframe;
    boundary = frame.key && (codec_ctx->aac_header == NULL || !ctx->opened ||
                             (b && b->last > b->pos));

    s->frame_ts = frame.dts;
    hls_update_fragments(s, frame.dts, boundary, 1);

    if (!ctx->opened) {
        ERROR("stream not open,len=%u,videoid=%u", len, s->videoid);
        return 0;
    }

//    if (frame.key)
//       hls_frag_set_key_frame(s);

    if (hls_mpegts_write_frame(s, &frame, &out) != 0) {
        //ERROR("hls: video frame failed");
    }

    ctx->video_cc = frame.cc;

    return 0;
}


void
hls_handle_stream(hls_session_t *s)
{
/*
   static char       buffer[128];
   static int cnt = 0;
   hls_mpegts_ctx_t     *ctx = s->mpegts_ctx;
   hls_config_t        *conf = (hls_config_t*)s->config
   AVPacket              packet;
   char                 *p, *end;
   int                   fd, n;
   int                   i = 0;

   while(av_read_frame(hctx->pFormatCtx, &packet)>=0) {

      if ( conf->num_segments && conf->num_segments-1 < s->num_segments ) {
         goto out;
      }

      if(packet.stream_index==hctx->audio_idx) {

         hls_audio_handler(s,&packet);

      } else if(packet.stream_index==hctx->video_idx) {

         hls_video_handler(s,&packet);

      } else {
         DEBUG("unknown packet, stream_index=%u",
                packet.stream_index);
         print_avpacket(&packet);
      }

      av_free_packet(&packet);

      //if ( i++ > 10 )
      //   break;
   }
  
   hls_close_fragment(s);
   
out:     
   if ( ctx->record ) {
      DEBUG("finalizing playlist %s ...",ctx->playlist.data);

      fd = hls_open_file(ctx->playlist.data, 
                  O_WRONLY, O_CREAT|O_APPEND, 0644);

      if (fd == -1) {
          ERROR("hls: open failed: %s", &ctx->playlist.data);
          return;
      }    

      p = buffer;
      end = p + sizeof(buffer);

      n = snprintf(p, end - p,"#EXT-X-ENDLIST\n");
      p += n;
      n = write(fd, buffer, p - buffer);
   }
*/
}

hls_hashbase_t *g_base;

int
hls_cache_mpegts_init(uint32_t key, uint32_t hashtime, uint32_t hashlen, int isCreate) {
   hls_hashbase_t *base;
   uint32_t ikey;
   size_t isize;
   uint32_t objsize = sizeof(hls_fragmem_t);

   g_base = 0;
   base = (hls_hashbase_t*)malloc(sizeof(*base));
   if ( base == NULL ) {
      printf("Can not init cache ----------\n");
      return -1;
   }
   memset(base,0,sizeof(*base));
   base->hb_pid = 0;
   base->hb_time = hashtime;
   base->hb_len = hashlen;
   base->hb_objsize = objsize;


   hls_hashbase_init(base);

   ikey = key + base->hb_pid;
   isize = hashtime*hashlen*objsize;

   isize += (4096 - isize%4096);

   printf("fragmen objsize=%u,isize=%u\n",objsize,isize);

   hls_get_cache(base,ikey,isize,isCreate);

   g_base = base;
  
   return 0; 
}

void*
hls_cache_mpegts_get(uint32_t key, uint32_t &isNew) 
{
   hls_hashbase_t *base = g_base;
   hls_fragmem_t *item = 0;
   char *table = 0;
   //int   value = 0;
   uint32_t   i,j;  

   if ( base == NULL )
      return NULL;

   table = (char*)base->hb_cache;

   for ( i=0; i < base->hb_time; i++ ) { 
      /*value = key % base->hb_base[i];
      item = (hls_fragmem_t*)(table
                + i*base->hb_len*base->hb_objsize 
                + value*base->hb_objsize);

      if ( item->videoid == key ) {
         isNew = 0;
         return item;
      }

      if ( item->videoid == 0 ) {
         item->videoid = key;
         isNew = 1;
         return item;
      }*/
      for( j=0; j<base->hb_len; j++) {
         item = (hls_fragmem_t*)(table
                + i*base->hb_len*base->hb_objsize 
                + j*base->hb_objsize);

         if ( item->videoid == key ) {
            isNew = 0;
            return item;
         }

         if ( item->videoid == 0 ) {
            item->videoid = key;
            isNew = 1;
            return item;
         }
      }
   }   
   
   return NULL;
}

void*
hls_cache_mpegts_search(uint32_t key) 
{
   hls_hashbase_t *base = g_base;
   hls_fragmem_t *item = 0;
   char *table = 0;
   //int   value = 0;
   uint32_t      i,j;  

   if ( base == NULL )
      return NULL;

   table = (char*)base->hb_cache;

   for ( i=0; i < base->hb_time; i++ ) { 
      /*value = key % base->hb_base[i];
      item = (hls_fragmem_t*)(table 
                   + i*base->hb_len*base->hb_objsize
                   + value*base->hb_objsize);

      if ( item->videoid == key ) {
         DEBUG("m3u8: found key, key=%u,value=%u", item->videoid,value);
         return item;
      }*/
      for( j=0; j<base->hb_len; j++) {
         item = (hls_fragmem_t*)(table 
                   + i*base->hb_len*base->hb_objsize
                   + j*base->hb_objsize);
         if ( item->videoid == key ) {
            DEBUG("m3u8: found key, key=%u,value=%u", item->videoid,value);
            return item;
         }
      }
   }   
   
   ERROR("m3u8: not found key, key=%u", key);
   return NULL;
}

void
hls_cache_mpegts_remove(uint32_t key) {
   return;
}

int
hls_write_fragmem_playlist_clean(hls_fragmem_t *fragmem) 
{

   if ( fragmem == NULL )
      return -1;

   memset(fragmem->playlist, 0,HLS_PLAYLIST_LEN);
   fragmem->playlist_len = 0;

   return 0;
}


int
hls_write_fragmem_playlist(hls_fragmem_t *fragmem, char* buff, int len) 
{

   if ( fragmem == NULL )
      return -1;

   if (len > (int)(1024 - fragmem->playlist_len) )
      return -2;

   memcpy(fragmem->playlist + fragmem->playlist_len, buff, len);
   fragmem->playlist_len += len;

   return len;
}


int 
hls_write_fragmem_data(hls_session_t *s, hls_fragmem_t *fragmem, char *buff, int len, int key) 
{
   uint64_t   id = fragmem->fragid;
   hls_mem_t *buf = fragmem->mem + (id%MAX_FRAGMENT_NUM);
   
   //ERROR("fragid=%llu, p=%p, buf_id=%u, buf_len=%u, len=%u, ts=%llu, id=%u", 
   //       fragmem->fragid, buf, buf->id, buf->len, len, id, id%13);

   //if ( (256*1024 - buf->len) < len ) 
   if ( (HLS_MAX_TS_LEN - buf->len) < len )  {
      ERROR("not enough buffer, videoid=%u, buf_len=%u, len=%u", 
            fragmem->videoid, buf->len, len);

      hls_close_fragment(s);
      hls_open_fragment(s,s->frame_ts,1);
     
      //update id to get new buffer
      id = hls_get_fragment_id(s,0);
      buf = fragmem->mem + (id%MAX_FRAGMENT_NUM);
   }

   memcpy(buf->data + buf->len,buff,len);
   buf->len += len;
   buf->key |= key;

   return len;
}


int
hls_fragmem_open(hls_fragmem_t *fragmem, uint64_t id )
{
   hls_mem_t *buf = 0;
   
   if ( fragmem == NULL ) {
       ERROR("fragmem is NULL");
       return -1;
   }

   buf = fragmem->mem + (id%MAX_FRAGMENT_NUM);
   fragmem->fragid = id;
   buf->id = id;
   buf->duration = 0;
   buf->len = 0;
   buf->key = 0;
   DEBUG("fragmem open:videoid=%u, ts=%"PRIu64", id=%u", fragmem->videoid, id, id%MAX_FRAGMENT_NUM);
   return 0;
}

void
hls_fragmem_update_duration(hls_fragmem_t *fragmem, double duration)
{
   hls_mem_t *buf = fragmem->mem + (fragmem->fragid%MAX_FRAGMENT_NUM);

   buf->duration = duration;
   return;
}

hls_mem_t*
hls_tsmem_get(hls_fragmem_t *fragmem, uint64_t tsid)
{
   hls_mem_t *buf = fragmem->mem + (tsid%MAX_FRAGMENT_NUM);

   //DEBUG("get tsmem(Tsto): videoid=%u, buf_id=%llu, tsid=%llu, index=%u", 
   //        fragmem->videoid, buf->id, tsid, fragmem->fragid%13);

   if ( buf->id == tsid ) {
      return buf;
   }

   ERROR("get tsmem: mismatch id, buf_id=%llu, tsid=%llu", buf->id, tsid);

   return NULL;
}


