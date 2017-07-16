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
 * @(#)hls_stream.c
 */


#include "hls_stream.h"
#include "hls_bitread.h"
#include "hls_utils.h"

#include "tfc_debug_log.h"
#include "submcd_log.h"
#include "hls_cache.h"

hls_hashbase_t *g_session_base;

int
hls_cache_session_init(uint32_t key, uint32_t hashtime, uint32_t hashlen, int isCreate)
{
   hls_hashbase_t *base;
   uint32_t ikey;
   size_t isize;
   uint32_t objsize = sizeof(hls_session_t);

   g_session_base = 0;
   base = (hls_hashbase_t*)malloc(sizeof(*base));
   if ( base == NULL ) {
      printf("Can not init session cache\n");
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
   

   printf("session ikey=%u,objsize=%u,isize=%u\n",ikey,objsize,isize);

   hls_get_cache(base,ikey,isize,isCreate);

   g_session_base = base;
  
   return 0; 
}

void*
hls_cache_session_get(uint32_t key, uint32_t &isNew)
{
   hls_hashbase_t *base = g_session_base;
   hls_session_t *item = 0;
   char *table = 0;
   //int   value = 0;
   uint32_t   i,j;  

   if ( base == NULL )
      return NULL;

   table = (char*)base->hb_cache;

   for ( i=0; i < base->hb_time; i++ ) { 
      /*
      value = key % base->hb_base[i];
      item = (hls_session_t*)(table
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
      }
      */
      for (j=0; j< base->hb_len; j++) {
         item = (hls_session_t*)(table
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
hls_cache_session_search(uint32_t key) 
{
   hls_hashbase_t *base = g_session_base;
   hls_session_t *item = 0;
   char *table = 0;
   //int   value = 0;
   uint32_t      i,j;  

   if ( base == NULL )
      return NULL;

   table = (char*)base->hb_cache;

   for ( i=0; i < base->hb_time; i++ ) { 
      /*value = key % base->hb_base[i];
      item = (hls_session_t*)(table 
                   + i*base->hb_len*base->hb_objsize
                   + value*base->hb_objsize);
      if ( item->videoid == key ) {
         ERROR("session: found key, key=%u", item->videoid);
         return item;
      }*/
      for( j=0; j<base->hb_len; j++) {
         item = (hls_session_t*)(table
                + i*base->hb_len*base->hb_objsize 
                + j*base->hb_objsize);
         if ( item->videoid == key ) {
            DEBUG("session: found key, key=%u", item->videoid);
            return item;
         }
      }
   }   
   
   ERROR("session: not found key, key=%u", key);
   return NULL;
}


void
hls_cache_session_remove(uint32_t key)
{
   return;
}


hls_session_t*
hls_create_session(int videoid, int duration, int max_audio_delay)
{
   hls_session_t *s;
   uint32_t isNew = 0;

   s = (hls_session_t*)hls_cache_session_get(videoid,isNew);
   if (s == NULL) {
      ERROR("cannot create session, videoid=%u", videoid);
      return NULL;
   }
   memset(s,0,sizeof(*s));

   s->videoid = videoid;

   isNew = 0;
   s->fragmem = (hls_fragmem_t*)hls_cache_mpegts_get(s->videoid,isNew);
   if ( s->fragmem == NULL ) {
       //free(s);
       s->videoid = 0;
       return NULL;
   }
   s->mpegts_ctx.aframe.start = s->fragmem->audiobuf;
   s->mpegts_ctx.aframe.end = s->mpegts_ctx.aframe.start + HLS_AUDIO_BUF_LEN;
   s->mpegts_ctx.aframe.pos = s->mpegts_ctx.aframe.last = s->mpegts_ctx.aframe.start;

   s->audio_buffer_size = HLS_AUDIO_BUF_LEN;
   s->winfrags = WIN_FRAGMENT_NUM;

//#define MAX_DURATION 10000
   //s->max_fraglen = MAX_DURATION;
   //s->fraglen = MAX_DURATION;
   s->max_fraglen = duration;
   s->fraglen = duration;
   s->slicing = 1;
   //s->max_audio_delay = 300;
   s->max_audio_delay = max_audio_delay;

   s->keys = 0;
   s->sync = 2;
   s->type = HLS_TYPE_LIVE;
   s->naming = HLS_NAMING_SEQUENTIAL;
   s->granularity = 0;

   s->codec_ctx.avc_header = &s->codec_ctx.avc_header_buf;
   //s->codec_ctx.avc_header->pos = s->codec_ctx.avc_header_data;

   s->codec_ctx.aac_header = &s->codec_ctx.aac_header_buf;
   //s->codec_ctx.aac_header->pos = s->codec_ctx.aac_header_data;

   return s;
}

void
hls_copy_session(hls_session_t *old, hls_session_t *s)
{
    if ( old == NULL || s == NULL )
        return;

    s->codec_ctx.width = old->codec_ctx.width;
    s->codec_ctx.height = old->codec_ctx.height;
    s->codec_ctx.duration = old->codec_ctx.duration;
    s->codec_ctx.frame_rate = old->codec_ctx.frame_rate;
    s->codec_ctx.video_data_rate = old->codec_ctx.video_data_rate;
    s->codec_ctx.video_codec_id = old->codec_ctx.video_codec_id;
    s->codec_ctx.audio_data_rate = old->codec_ctx.audio_data_rate;
    s->codec_ctx.audio_codec_id = old->codec_ctx.audio_codec_id;
    s->codec_ctx.aac_profile = old->codec_ctx.aac_profile;
    s->codec_ctx.aac_chan_conf = old->codec_ctx.aac_chan_conf;
    s->codec_ctx.aac_sbr = old->codec_ctx.aac_sbr;
    s->codec_ctx.aac_ps = old->codec_ctx.aac_ps;
    s->codec_ctx.avc_profile = old->codec_ctx.avc_profile;
    s->codec_ctx.avc_compat = old->codec_ctx.avc_compat;
    s->codec_ctx.avc_level = old->codec_ctx.avc_level;
    s->codec_ctx.avc_nal_bytes = old->codec_ctx.avc_nal_bytes;
    s->codec_ctx.avc_ref_frames = old->codec_ctx.avc_ref_frames;
    s->codec_ctx.sample_rate = old->codec_ctx.sample_rate;
    s->codec_ctx.sample_size = old->codec_ctx.sample_size;
    s->codec_ctx.audio_channels = old->codec_ctx.audio_channels;
    memcpy(s->codec_ctx.profile,old->codec_ctx.profile,32);
    memcpy(s->codec_ctx.level,old->codec_ctx.level,32);

    int len = old->codec_ctx.avc_header_buf.last - old->codec_ctx.avc_header_buf.start;
    ERROR("copy avc header, len=%u",len);
    if ( 0 < len && len < AVC_HEADER_LEN ) {
       memcpy(s->codec_ctx.avc_header_data,old->codec_ctx.avc_header_data,len);
       s->codec_ctx.avc_header_buf.pos = s->codec_ctx.avc_header_data;
       s->codec_ctx.avc_header_buf.start = s->codec_ctx.avc_header_buf.pos;
       s->codec_ctx.avc_header_buf.last = s->codec_ctx.avc_header_buf.pos + len;
       s->codec_ctx.avc_header = &s->codec_ctx.avc_header_buf;
    }

    len = old->codec_ctx.aac_header_buf.last - old->codec_ctx.aac_header_buf.start;
    ERROR("copy acc header, len=%u",len);
    if ( 0 < len && len < ACC_HEADER_LEN ) {
       memcpy(s->codec_ctx.aac_header_data,old->codec_ctx.aac_header_data,len);
       s->codec_ctx.aac_header_buf.pos = s->codec_ctx.aac_header_data;
       s->codec_ctx.aac_header_buf.start = s->codec_ctx.aac_header_buf.pos;
       s->codec_ctx.aac_header_buf.last = s->codec_ctx.aac_header_buf.pos + len;
       s->codec_ctx.aac_header = &s->codec_ctx.aac_header_buf;
    }

/*
    char            avc_header_data[AVC_HEADER_LEN];
    hls_buf_t       avc_header_buf;
    hls_buf_t      *avc_header;

    char            aac_header_data[ACC_HEADER_LEN];
    hls_buf_t       aac_header_buf;
    hls_buf_t      *aac_header;

    hls_buf_t      *meta;
    uint32_t        meta_version;
*/

}





void
hls_reset_session(hls_session_t *s)
{
   return;
}

void
hls_codec_parse_avc_header(hls_session_t *s, char *buf, uint32_t size)
{
    hls_codec_ctx_t        *ctx =  &s->codec_ctx;
    hls_bitread_t           br;  
    uint32_t                profile_idc, width, height, crop_left, crop_right,
                            crop_top, crop_bottom, frame_mbs_only, n, cf_idc,
                            num_ref_frames, bit;

    //hexdump(buf,size ,"avc");
    ERROR("parsing avc header, videoid=%u", s->videoid);

    hls_bitread_init(&br, buf, buf+size);
    
    /* see syntax of AVCDecoderConfigurationRecord  
       at ISO-14496-15, section 5.2.4.1.1 */
    //hls_bitread_get(&br, 48); 
    hls_bitread_get(&br, 8); 

    ctx->avc_profile = (uint32_t) hls_bitread_8(&br);
    ctx->avc_compat = (uint32_t) hls_bitread_8(&br);
    ctx->avc_level = (uint32_t) hls_bitread_8(&br);

    /* nal bytes */
    ctx->avc_nal_bytes = (uint32_t) ((hls_bitread_8(&br) & 0x03) + 1);

    /* nnals */
    if ((hls_bitread_8(&br) & 0x1f) == 0) { 
        return;
    }    

    /* nal size */
    hls_bitread_get(&br, 16); 

    /* nal type: see ISO-14496-10, section 7.3.1 of NAL unit syntax.
                 forbidden_zero_bit u(1), always zero if nothing wrong.
                 nal_ref_pic u(2), non-zero indicates SPS, PPS, etc.
                 nal_unit_type u(5), for SPS it is equal to 7.*/
    if (hls_bitread_8(&br) != 0x67) {
        return;
    }

    /* SPS*/

    /* profile idc */
    profile_idc = (uint32_t) hls_bitread_get(&br, 8);

    /* flags */
    hls_bitread_get(&br, 8);

    /* level idc */
    hls_bitread_get(&br, 8);

    /* SPS id */
    hls_bitread_golomb(&br);

    /* TODO: check profile_idc format */
    if (profile_idc == 100 || profile_idc == 110 ||
        profile_idc == 122 || profile_idc == 244 || profile_idc == 44 ||
        profile_idc == 83 || profile_idc == 86 || profile_idc == 118)
    {
        /* chroma format idc */
        cf_idc = (uint32_t) hls_bitread_golomb(&br);

        if (cf_idc == 3) {
            /* separate color plane */
            hls_bitread_get(&br, 1);
        }

        /* bit depth luma - 8 */
        hls_bitread_golomb(&br);

        /* bit depth chroma - 8 */
        hls_bitread_golomb(&br);

        /* qpprime y zero transform bypass */
        hls_bitread_get(&br, 1);

        /* seq scaling matrix present */
        if (hls_bitread_get(&br, 1)) {

            for (n = 0; n < (cf_idc != 3 ? 8u : 12u); n++) {

                /* seq scaling list present */
                if (hls_bitread_get(&br, 1)) {

                    /* TODO: scaling_list()
                    if (n < 6) {
                    } else {
                    }
                    */
                }
            }
        }
   }

    /* log2 max frame num */
    hls_bitread_golomb(&br);

    /* pic order cnt type */
    bit = hls_bitread_golomb(&br);
    switch (bit) {
    case 0:

        /* max pic order cnt */
        hls_bitread_golomb(&br);
        break;

    case 1:

        /* delta pic order alwys zero */
        hls_bitread_get(&br, 1);

        /* offset for non-ref pic */
        hls_bitread_golomb(&br);

        /* offset for top to bottom field */
        hls_bitread_golomb(&br);

        /* num ref frames in pic order */
        num_ref_frames = (uint32_t) hls_bitread_golomb(&br);

        for (n = 0; n < num_ref_frames; n++) {

            /* offset for ref frame */
            hls_bitread_golomb(&br);
        }
    }

    /* num ref frames */
    ctx->avc_ref_frames = (uint32_t) hls_bitread_golomb(&br);

    /* gaps in frame num allowed */
    hls_bitread_get(&br, 1);

    /* pic width in mbs - 1 */
    width = (uint32_t) hls_bitread_golomb(&br);

    /* pic height in map units - 1 */
    height = (uint32_t) hls_bitread_golomb(&br);

    /* frame mbs only flag */
    frame_mbs_only = (uint32_t) hls_bitread_get(&br, 1);

    if (!frame_mbs_only) {

        /* mbs adaprive frame field */
        hls_bitread_get(&br, 1);
    }

    /* direct 8x8 inference flag */
    hls_bitread_get(&br, 1);

    /* frame cropping */
    if (hls_bitread_get(&br, 1)) {

        crop_left = (uint32_t) hls_bitread_golomb(&br);
        crop_right = (uint32_t) hls_bitread_golomb(&br);
        crop_top = (uint32_t) hls_bitread_golomb(&br);
        crop_bottom = (uint32_t) hls_bitread_golomb(&br);

    } else {

        crop_left = 0;
        crop_right = 0;
        crop_top = 0;
        crop_bottom = 0;
    }

    ctx->width = (width + 1) * 16 - (crop_left + crop_right) * 2;
    ctx->height = (2 - frame_mbs_only) * (height + 1) * 16 -
                  (crop_top + crop_bottom) * 2;

}

void
hls_codec_parse_aac_header(hls_session_t *s, char *buf, uint32_t size)
{
   hls_codec_ctx_t *ctx = &s->codec_ctx;
   hls_bitread_t    br;
   uint32_t         idx; 
   static uint32_t  aac_sample_rates[] =
                            { 96000, 88200, 64000, 48000,
                              44100, 32000, 24000, 22050,
                              16000, 12000, 11025,  8000,
                               7350,     0,     0,     0 }; 

   //hexdump(buf,size,"aac");
   DEBUG("parsing acc header");

   hls_bitread_init(&br, buf, buf+size);

   //hls_bitread_get(&br, 16); 

   ctx->aac_profile = (uint32_t) hls_bitread_get(&br, 5);
   if (ctx->aac_profile == 31) {
       ctx->aac_profile = (uint32_t) hls_bitread_get(&br, 6) + 32;
   }    

    idx = (uint32_t) hls_bitread_get(&br, 4);
    if (idx == 15) {
        ctx->sample_rate = (uint32_t) hls_bitread_get(&br, 24); 
    } else {
        ctx->sample_rate = aac_sample_rates[idx];
    }    

    ctx->aac_chan_conf = (uint32_t) hls_bitread_get(&br, 4);
    if (ctx->aac_profile == 5 || ctx->aac_profile == 29) {

        if (ctx->aac_profile == 29) {
            ctx->aac_ps = 1;
        }

        ctx->aac_sbr = 1;

        idx = (uint32_t) hls_bitread_get(&br, 4);
        if (idx == 15) {
            ctx->sample_rate = (uint32_t) hls_bitread_get(&br, 24);
        } else {
            ctx->sample_rate = aac_sample_rates[idx];
        }

        ctx->aac_profile = (uint32_t) hls_bitread_get(&br, 5);
        if (ctx->aac_profile == 31) {
            ctx->aac_profile = (uint32_t) hls_bitread_get(&br, 6) + 32;
        }
    }
    /* MPEG-4 Audio Specific Config

       5 bits: object type
       if (object type == 31)
         6 bits + 32: object type
       4 bits: frequency index
       if (frequency index == 15)
         24 bits: frequency
       4 bits: channel configuration

       if (object_type == 5)
           4 bits: frequency index
           if (frequency index == 15)
             24 bits: frequency
           5 bits: object type
           if (object type == 31)
             6 bits + 32: object type
             
       var bits: AOT Specific Config
     */


   return;
}


int32_t
hls_handle_h264_header(hls_session_t *s, char* buf, int len)
{
   hls_codec_ctx_t   *codec_ctx;
   hls_buf_t         *avc_header;

   if ( s == NULL )
      return -1;

   codec_ctx = &s->codec_ctx;

   // get avc info.
   hls_codec_parse_avc_header(s,buf,len);


   /*avc_header = (hls_buf_t*)malloc(sizeof(*avc_header)); 
   if ( avc_header == NULL ) {
      SUBMCD_DEBUG_P(LOG_ERROR,"can not alloc avc_header,videoid=%u,\n",s->videoid);
      return -1;
   }
   memset(avc_header,0,sizeof(*avc_header));
   avc_header->pos = (char*)malloc(len);
   if ( avc_header->pos == NULL ) {
      SUBMCD_DEBUG_P(LOG_ERROR,"can not alloc avc_header pos,videoid=%u,len=%u,\n",s->videoid,len);
      return -2;
   }
   */
   codec_ctx->avc_header = &codec_ctx->avc_header_buf;
   avc_header = codec_ctx->avc_header;
   memset(avc_header,0,sizeof(*avc_header));
   avc_header->pos = codec_ctx->avc_header_data;

   if ( len >= AVC_HEADER_LEN ) {
      SUBMCD_DEBUG_P(LOG_ERROR,"can not alloc avc_header pos,videoid=%u,len=%u,\n",s->videoid,len);
      return -1;
   }
   //memset(avc_header->pos,0,AVC_HEADER_LEN);
   memcpy(avc_header->pos,buf, len);
   avc_header->start = avc_header->pos;
   avc_header->last = avc_header->pos + len;

   //FIXME
   /*if ( codec_ctx->avc_header ) {
      free(codec_ctx->avc_header);
   }
   codec_ctx->avc_header = avc_header;*/


   SUBMCD_DEBUG_P(LOG_ERROR,"avc_profile=%u,avc_compat=%u,"
         "avc_level=%u,avc_nal_bytes=%u,"
         "avc_ref_frames=%u,width=%u,height=%u,videoid=%u\n",
          codec_ctx->avc_profile,
          codec_ctx->avc_compat,
          codec_ctx->avc_level,
          codec_ctx->avc_nal_bytes,
          codec_ctx->avc_ref_frames,
          codec_ctx->width,
          codec_ctx->height,
          s->videoid);

   return 0; 
}

int32_t
hls_handle_aac_header(hls_session_t *s, char* buf, int len)
{
   hls_codec_ctx_t  *codec_ctx =  &s->codec_ctx;
   hls_buf_t        *aac_header;


   // TODO: get codec info.
   codec_ctx->aac_profile = 0;
   codec_ctx->sample_rate = 0;
   codec_ctx->aac_chan_conf = 0;
   codec_ctx->aac_ps = 0;
   codec_ctx->aac_sbr = 0;


   hls_codec_parse_aac_header(s,buf,len);
/*
   aac_header = (hls_buf_t*)malloc(sizeof(*aac_header)); 
   if ( aac_header == NULL )
      return -1;
   memset(aac_header,0,sizeof(*aac_header)); 
   
   aac_header->pos = (char*)malloc(len);
   if ( aac_header->pos == NULL )
      return -2;
   aac_header->start = aac_header->pos;
   aac_header->last = aac_header->pos + len;
   memcpy(aac_header->pos,buf,len);

   if ( codec_ctx->aac_header )
      free(codec_ctx->aac_header);

   codec_ctx->aac_header = aac_header;
*/
   codec_ctx->aac_header = &codec_ctx->aac_header_buf;
   aac_header = codec_ctx->aac_header;
   memset(aac_header,0,sizeof(*aac_header)); 

   aac_header->pos = codec_ctx->aac_header_data;
   if ( len >= ACC_HEADER_LEN ) {
      SUBMCD_DEBUG_P(LOG_ERROR,"acc header len to big, len=%u",len);
      return -2;
   }
   aac_header->start = aac_header->pos;
   aac_header->last = aac_header->pos + len;
   memcpy(aac_header->pos,buf,len);

   SUBMCD_DEBUG_P(LOG_ERROR,"aac_profile=%u,sample_rate=%u,"
           "aac_chan_conf=%u,aac_ps=%u,aac_sbr=%u\n",
           codec_ctx->aac_profile,
           codec_ctx->sample_rate,
           codec_ctx->aac_chan_conf,
           codec_ctx->aac_ps,
           codec_ctx->aac_sbr);

   return 0;
}

hls_session_t*
hls_segment_file(hls_config_t *hls_ctx, const char* file)
{
   return 0;
}

int
hls_copy(hls_session_t *s, void *dst, char **src, size_t n, hls_buf_t *buf)
{
    char    *last;
    //size_t   pn;

    if (buf == NULL) {
        return -1;
    }

    last = buf->last;

    //fprintf(stderr, "hls: read %u byte(s), len=%u, last=%p, *src=%p\n", 
    //                 n, last - *src, last, *src);
    if ((size_t)(last - *src) >= n) {
        if (dst) {
           memcpy(dst, *src, n);
        }

        *src += n;
        return 0;
    }

    //ERROR("error reading buffer of %u byte(s)", n);

    return -1;
}


int
hls_parse_aac_header(hls_session_t *s, uint64_t *objtype,
    uint64_t *srindex, uint64_t *chconf)
{
    hls_codec_ctx_t   *codec_ctx;
    hls_buf_t            *cl;
    char                 *p, b0, b1;

    if ( s == NULL ) {
        ERROR("hls: session is NULL, ret=-1");
    }

    codec_ctx = &s->codec_ctx;

    if ( codec_ctx->aac_header == NULL ) {
       return -1;
    }

    cl = codec_ctx->aac_header;

    p = cl->pos;

    //if (hls_copy(s, NULL, &p, 2, cl) != 0) {
    //    return -1;
    //}

    if (hls_copy(s, &b0, &p, 1, cl) != 0) {
        ERROR("hls: error to copy, ret=-1");
        return -1;
    }

    if (hls_copy(s, &b1, &p, 1, cl) != 0) {
        ERROR("hls: error to copy, ret=-2");
        return -2;
    }

    *objtype = b0 >> 3;
    if (*objtype == 0 || *objtype == 0x1f) {
        ERROR("hls: unsupported adts object type:%llu", *objtype);
        return -1;
    }

    if (*objtype > 4) {

        /*
         * Mark all extended profiles as LC
         * to make Android as happy as possible.
         */

        *objtype = 2;
    }

    *srindex = ((b0 << 1) & 0x0f) | ((b1 & 0x80) >> 7);
    if (*srindex == 0x0f) {
        ERROR("hls: unsupported adts sample rate:%llu", *srindex);
        return -1;
    }

    *chconf = (b1 >> 3) & 0x0f;

    //fprintf(stderr, "hls: aac object_type:%u, sample_rate_index:%u, "
    //               "channel_config:%u\n", *objtype, *srindex, *chconf);

    return 0;
}

void
hls_clean_timeout_stream(uint32_t videoid) {
    hls_session_t *s;
    hls_fragmem_t *m;

    s = (hls_session_t*) hls_cache_session_search(videoid);    
    if ( s != NULL )
        s->videoid = 0;

    m = (hls_fragmem_t*) hls_cache_mpegts_search(videoid);    
    if ( s != NULL )
        m->videoid = 0;

    ERROR("clean video stream, videoid=:%u", videoid);

    return;

}




