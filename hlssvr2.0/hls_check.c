
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>


#include "hls/hls_buf.h"
#include "hls/hls_cache.h"
#include "hls/hls_types.h"
#include "hls/hls_stream.h"
#include "muxsvr_mcd_cfg.h"


int main(int argc, char** argv) {
   int ret;
   char *frag_table;
   char *sess_table;
   uint32_t isize,ikey;
   uint32_t objsize;
   uint32_t used = 0;
   uint32_t cmd = 0;
   muxsvr::CCfgMng  _cfg;

   _cfg.LoadCfg("../../etc/muxsvr_mcd.conf");

   if ( argc < 2 ) {
      printf("print help\n");
      exit(-1);
   }

   cmd = atoi(argv[1]);
   printf("info: cmd=%u \n",cmd);
    
/*
   hls_hashbase_t *base = 0;
   base = (hls_hashbase_t*)malloc(sizeof(*base));
   if ( base == NULL ) {
      printf("Can not init cache ----------\n");
      return -1;
   }
   memset(base,0,sizeof(*base));
   base->hb_pid = 0;
   base->hb_time = HLS_HASHTIME;
   base->hb_len = HLS_HASHLEN;
   base->hb_objsize = objsize;
   hls_hashbase_init(base);
*/

   ikey = HLS_MPEGTS_KEY;
   objsize = sizeof(hls_fragmem_t);
   //isize = HLS_HASHTIME*HLS_HASHLEN*objsize;
   isize = _cfg.m_uiSessionHashTime*_cfg.m_uiSessionHashLen*objsize;
   isize += (4096 - isize%4096);
   printf("fragmen len=%u,time=%u\n",_cfg.m_uiSessionHashLen,_cfg.m_uiSessionHashTime);
   printf("fragmen objsize=%u,isize=%u\n",objsize,isize);
   frag_table = hls_get_shm(ikey, isize, 0666);

   if ( frag_table == NULL ) {
       printf("can not get fragment shared mem\n");
       exit(-1);
   }

   ikey = HLS_SESSION_KEY;
   objsize = sizeof(hls_session_t);
   //isize = HLS_HASHTIME*HLS_HASHLEN*objsize;
   isize = _cfg.m_uiSessionHashTime*_cfg.m_uiSessionHashLen*objsize;
   isize += (4096 - isize%4096);
   printf("session objsize=%u,isize=%u\n",objsize,isize);
   sess_table = hls_get_shm(ikey, isize, 0666);

   if ( sess_table == NULL ) {
       printf("can not get session shared mem\n");
       exit(-1);
   }

   switch(cmd) {
       case 1: // print all info & stats
       {
           hls_fragmem_t *item = 0;
           objsize = sizeof(hls_fragmem_t);

           for (int i=0; i<_cfg.m_uiSessionHashTime; i++) 
           for (int j=0; j<_cfg.m_uiSessionHashLen; j++) {
              item = (hls_fragmem_t*)(frag_table 
                   + i*_cfg.m_uiSessionHashLen*objsize
                   + j*objsize);

              if ( item->videoid != 0 ) {
                  printf("fragment info, hashtime=%u,hashindex=%u,videoid=%u\n", 
                          i,j,item->videoid);
                  used++;
              }

           }

           printf("used item, %u/%u\n", used, _cfg.m_uiSessionHashLen*_cfg.m_uiSessionHashTime);

           break;
       }

       case 4: // print all info & stats
       {
           hls_session_t *item = 0;
           objsize = sizeof(hls_session_t);

           for (int i=0; i<_cfg.m_uiSessionHashTime; i++) 
           for (int j=0; j<_cfg.m_uiSessionHashLen; j++) {
              item = (hls_session_t*)(sess_table 
                   + i*_cfg.m_uiSessionHashLen*objsize
                   + j*objsize);

              if ( item->videoid != 0 ) {
                  printf("session info, hashtime=%u,hashindex=%u,videoid=%u,roomid=%u\n", 
                          i,j,item->videoid,item->roomid);
                  used++;
              }

           }

           printf("used item, %u/%u\n", used, _cfg.m_uiSessionHashLen*_cfg.m_uiSessionHashTime);

           break;
       }

       case 2: // print info of a video stream
       {
           uint32_t videoid = atoi(argv[2]);
           hls_fragmem_t *item = 0;
           hls_session_t *session = 0;

           objsize = sizeof(hls_session_t);
           for (int i=0; i<_cfg.m_uiSessionHashTime; i++) 
           for (int j=0; j<_cfg.m_uiSessionHashLen; j++) {
              session = (hls_session_t*)(sess_table 
                   + i*_cfg.m_uiSessionHashLen*objsize
                   + j*objsize);

              if ( session->videoid == videoid ) {
                  printf("session info, videoid=%u, roomid=%u, timestamp=%lu, current_ts=%lu\n",
                          session->videoid,session->roomid,session->current_ts,time(0));
              }

           }

           objsize = sizeof(hls_fragmem_t);

           for (int i=0; i<_cfg.m_uiSessionHashTime; i++) 
           for (int j=0; j<_cfg.m_uiSessionHashLen; j++) {
              item = (hls_fragmem_t*)(frag_table 
                   + i*_cfg.m_uiSessionHashLen*objsize
                   + j*objsize);

              if ( item->videoid == videoid ) {
                  printf("stream info, videoid=%u,frag_id=%"PRIu64"\n", 
                          item->videoid,item->fragid);
                  printf("playlist: %s\n",item->playlist);

                  printf("fragment info: \n");
                  for (int k=0; k<_cfg.m_uiTsNum*2+1; k++) {
                      hls_mem_t *t = &item->mem[k];
                      printf("  %u: id=%"PRIu64",key=%u,duration=%f,len=%u\n",
                             k,t->id,t->key,t->duration,t->len);
                  }
              }

           }


           break;

       }

       case 3: // scan outdated video stream 
       {

/*
           objsize = sizeof(hls_fragmem_t);
           for (int i=0; i<HLS_HASHTIME; i++) 
           for (int j=0; j<HLS_HASHLEN; j++) {
              item = (hls_fragmem_t*)(frag_table 
                   + i*HLS_HASHLEN*objsize
                   + j*objsize);

              if ( item->videoid == videoid ) {
                  printf("fragment info, hashtime=%u,hashindex=%u,videoid=%u\n", 
                          i,j,item->videoid);
              }

           }
*/
           hls_fragmem_t *item = 0;
           hls_session_t *session = 0;
           objsize = sizeof(hls_session_t);
           time_t now = time(0);
           int total = 0;

           for (int i=0; i<_cfg.m_uiSessionHashTime; i++) 
           for (int j=0; j<_cfg.m_uiSessionHashLen; j++) {
              session = (hls_session_t*)(sess_table 
                   + i*_cfg.m_uiSessionHashLen*objsize
                   + j*objsize);

              if ( session->videoid != 0 && now - session->current_ts > 300 ) {
                  int found_frag = 0;

                  for (int k=0; k<_cfg.m_uiSessionHashTime; k++)
                  for (int l=0; l<_cfg.m_uiSessionHashLen; l++) {
                     item = (hls_fragmem_t*)(frag_table 
                      + k*_cfg.m_uiSessionHashLen*sizeof(hls_fragmem_t)
                      + l*sizeof(hls_fragmem_t));

                     if ( item->videoid == session->videoid ) {
                         found_frag = 1;
                         goto found;
                     }
                  }

found:
                  printf("session info, found=%u, frag=%p,item=%p, session_videoid=%u, frag_videoid=%u, item_videoid=%u\n",
                          found_frag,session->fragmem,item,session->videoid,0/*session->fragmem->videoid*/,item->videoid);

                  if ( session->videoid == item->videoid ) {
                     printf("reset session info, videoid=%u, roomid=%u, timestamp=%lu, current_ts=%lu\n",
                          session->videoid,session->roomid,session->current_ts,time(0));
                     total++;
                     //memset(session,0,sizeof(hls_session_t));
                     //memset(item,0,sizeof(hls_fragmem_t));
                     session->videoid = 0;
                     item->videoid = 0;

                  } else {
                     printf("mismatch session and fragmem, sess_videoid=%u, frag_videoid=%u\n",
                            session->videoid, item->videoid);
                  }
              }

           }
           
           printf("total idle streams: %u\n",total);
           break;

       }

       default:
       {
           printf("unknown command\n");
           break;
       }
   }


   return ret;
}
 
