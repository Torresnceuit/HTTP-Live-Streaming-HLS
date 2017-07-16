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
 * @(#)hls_utils.c
 */


#include "hls_utils.h"
#include "hls_log.h"

void print_msg(const char *msg, int len)
{
  int i, j;
  char tmp[4], buf[4086]={0};
  int print_len = 0;  
  print_len = len < 18 ? len : 18;

  for(i=0,j=1; i<print_len; i++,j++) {
    sprintf(tmp, "%02X", (unsigned char)msg[i]);
    strcat(buf, tmp);
    strcat(buf, " ");
  }    

  fprintf(stderr, "print_msg: str=%s\n ", buf);

}

void print_msg_full(const char *msg, int len)
{
  int i, j;
  char tmp[4], buf[4086]={0};
  int print_len = 0;  
  print_len = len;

  for(i=0,j=1; i<print_len; i++,j++) {
    sprintf(tmp, "%02X", (unsigned char)msg[i]);
    strcat(buf, tmp);
    strcat(buf, " ");
  }    

  fprintf(stderr, "print_msg: str=%s\n ", buf);

}

void print_msg_extra(const char *msg, int len, char* annotation)
{
  int i, j;
  char tmp[4], buf[4086]={0};
  for(i=0,j=1; i<len; i++,j++) {
    sprintf(tmp, "%02X", (unsigned char)msg[i]);
    strcat(buf, tmp);
    strcat(buf, " ");
  }    

  fprintf(stderr, "print_msg:len=%d, str=%s\n ",len, buf);

}

void 
hexdump(char* p,int len, const char* type)
{
   char buf[256];
   int i, j, i0;  

   ERROR("---- dump buffer (%s) ---- len=%d",type,len);

   for (i = 0; i < len; ) {  
      memset(buf, sizeof(buf), ' ');
      sprintf(buf, "%5d: ", i);  

      i0 = i;   
      for (j=0; j < 16 && i < len; i++, j++) 
         sprintf(buf+7+j*3, "%02x ", (uint8_t)(p[i]));

      i = i0;  
      for (j=0; j < 16 && i < len; i++, j++) 
         sprintf(buf+7+j + 48, "%c",
            isprint(p[i]) ? p[i] : '.');

      ERROR("%s: %s", type, buf);
   }

}


void *             
rmemcpy(void *dst, const void* src, size_t n)
{                  
    char     *d, *s;
    
    d = (char*)dst;
    s = (char*)src + n - 1;
            
    while(s >= (char*)src) {
        *d++ = *s--;
    }   
        
    return dst;
}       



