#include <cstddef>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/shm.h>
#include <string.h>
#include <errno.h>

#include "hls_cache.h"

void
hls_hashbase_init(hls_hashbase_t *base) {
   int iFlag;
   uint32_t iSqrt;
   uint32_t i, j, k;

   if ( base == NULL )
      return;

   base->hb_base = (uint32_t*)malloc(sizeof(uint32_t)*(base->hb_time));
   if ( base->hb_base == NULL ) {
      printf("malloc failed"); 
      return;
   }

   iSqrt = (uint32_t) sqrt(base->hb_len);
   for(i = base->hb_len, j = 0; j <base->hb_time; i--)
   {
      iFlag = 1;
      for(k = 2; k <= iSqrt; k++)
      {
         if(i % k == 0)
         {
             iFlag = 0;
             break;
         }
      }
      if(iFlag == 1)
      {
         base->hb_base[j] = i;
         if(i<=0)
         {
            printf("HashBase[%d] = %d,HashLen[%d] is too small!\n",
                    j,i,base->hb_len);
            exit(-1);
         }
         j++;
      }
   }
   return;
}

char*
hls_get_shm(uint32_t key, size_t size, int32_t flag)
{
   int id; 
   char *addr;
   
   id = shmget(key, size, flag);

   printf("(1)id==%u,key=%x, size=%zu\n",id,key,size);

   if (id < 0) {
      perror("shmget failed");
      return 0;
   }   

   addr = (char*)shmat(id, NULL, 0);

   printf("(2)id=%u,key=%x, size=%zu\n",id,key,size);

   if ( (int)addr == -1 ) { 
      printf("errno=%d\n",errno);
      perror(" (1) ERROR");
      return 0;
   }   

   return addr;
}

int 
hls_get_cache(hls_hashbase_t *base, uint32_t key, size_t size, uint32_t create)
{
    char* pData = 0;// = usnet_get_shm(key, size, 0666);

    base->hb_size = size;

    pData = hls_get_shm(key, size, 0666);
    if (pData == NULL)
    {
        if(create)
        {
            pData = hls_get_shm(key, size, (0666|IPC_CREAT));
            if(pData == NULL)
            {
                return -1;
            }
            memset(pData, 0, size);
            base->hb_cache = pData;
        }
        else
        {
            perror("ERROR");
            return -2;
        }
    }
    else
    {
        memset(pData, 0, size);
        base->hb_cache = pData;
    }

    return 0;
}

