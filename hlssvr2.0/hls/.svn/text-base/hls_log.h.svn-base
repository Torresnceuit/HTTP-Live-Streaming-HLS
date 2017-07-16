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
 * @(#)hls_log.h
 */

#ifndef _HLS_LOG_H_
#define _HLS_LOG_H_

#include <string.h>
#include <dirent.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdio.h>
#include <sys/time.h>

#include "tfc_debug_log.h"
#include "submcd_log.h"

enum {           
   HLS_INFO = 0,
   HLS_DEBUG = 1,
   HLS_WARN = 2,
   HLS_ERROR = 3,
   HLS_FATAL = 4
};

//extern int g_verbose;

void 
hls_log(int level, const char* sourcefilename, int line, const char* msg, ...);

#define LOG(level,fmt, ...) \
{  hls_log(level,__FILE__, __LINE__,fmt, ##__VA_ARGS__); }

#define TRACE(_fmt, ...)     do {} while(0)

#define DEBUG(_fmt, ...)     do {} while(0)
/*
#define DEBUG(_fmt, ...)                                 \
   do {                                                      \
      if ( g_verbose )\
      fprintf(stdout,"[DEBUG|%s:%d] " _fmt "\n",\
                    __FUNCTION__, __LINE__, ##__VA_ARGS__);  \
   } while (0);
*/

#define INFO(_fmt, ...)     do {} while(0)
#define WARN(_fmt, ...)     do {} while(0)

//#define ERROR(_fmt, ...)     do {} while(0)
#define ERROR(_fmt, ...)                                 \
   do {                                                      \
      SUBMCD_DEBUG_P(LOG_ERROR,"[%s:%d] " _fmt "\n",\
                    __FUNCTION__, __LINE__, ##__VA_ARGS__);  \
   } while (0);

#define FATAL(_fmt, ...)     do {} while(0)


#endif // _HLS_LOG_H_
