#ifndef _HLS_HASHBASE_H_
#define _HLS_HASHBASE_H_

#include <stdint.h>

typedef struct hls_hashbase hls_hashbase_t;
struct hls_hashbase {
   uint32_t  hb_pid;
   uint32_t  hb_time;
   uint32_t  hb_len;
   uint32_t  hb_objsize;
   uint32_t *hb_base;
   uint32_t  hb_size;
 
   char     *hb_cache;
};

void
hls_hashbase_init(hls_hashbase_t *base);

int 
hls_get_cache(hls_hashbase_t *base, uint32_t key, size_t size, uint32_t create);

char*
hls_get_shm(uint32_t key, size_t size, int32_t flag);

#endif //_HLS_HASHBASE_H_
