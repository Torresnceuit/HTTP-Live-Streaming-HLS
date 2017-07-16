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
 * @(#)hls_core.c
 */


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <getopt.h>
#include <limits.h>
#include <errno.h>

#include "hls_core.h"
#include "hls_log.h"

void
hls_init_config(hls_config_t *ctx)
{
   memset(ctx,0,sizeof(*ctx));

   // init default values;
   ctx->audio_buffer_size = 0x100000;
   ctx->winfrags = 6;
   ctx->max_fraglen = 2000;
   ctx->fraglen = 2000;
   ctx->slicing = 1;
   ctx->max_audio_delay = 300;

   ctx->keys = 0;
   ctx->sync = 2;
   ctx->type = HLS_TYPE_LIVE;//HLS_TYPE_EVENT;
   ctx->naming = HLS_NAMING_SEQUENTIAL;
   //ctx->nested = 0;
   ctx->granularity = 0;

   ctx->base_url.data = "";
   ctx->base_url.len = strlen("");

   ctx->path.data = "../html/live/";
   ctx->path.len = strlen("../html/live/");

   ctx->name.data = "10548";
   ctx->name.len = strlen("10548");

   return;
}

/*
void 
hls_display_usage()
{
    printf("Usage: hls-segmenter [OPTION]...\n");
    printf("\n");
    printf("HTTP Live Streaming - Create hls segments from video files (h264 & aac).\n");
    printf("\n");
    printf("\t-i, --input FILE             Video file to segment\n");
//    printf("\t-d, --duration SECONDS       Duration of each segment (default: 10 seconds)\n");
    printf("\t-p, --output-dir DIR         Directory to store the HLS segments\n");
    printf("\t-n, --stream-name NAME       Name of the HLS stream\n");
    printf("\t-u, --url-prefix URL         Specify urls of the segments, e.g. http://foo.bar/video/\n");
    printf("\t-c, --num-segment NUMBER     Number of segments to keep on disk\n");
    printf("\t-v, --verbose                Show log\n");
    printf("\t-h, --help                   This help\n");
    printf("\n");
    printf("The output files will be appended DIR/NAME.m3m8, DIR/NAME-1.ts etc\n");
    printf("\n");

    exit(0);
}
*/

void
hls_check_dir(hls_config_t *ctx, char* path)
{
   char zpath[MAX_PATH+1];
   struct stat fi;
   size_t len;

   if ( strlen(path) + 1 > sizeof(zpath) ) {
      fprintf(stderr,"path '%s' is too long\n",path);
      exit(1);
   }

   memset(zpath,0,sizeof(zpath)); 
   snprintf(zpath,MAX_PATH,"%s",path);

   if (stat(zpath, &fi) == -1) {

      if (errno != ENOENT) {
          fprintf(stderr,"failed to get directory info '%s'\n", path);
          exit(1);
      }    

      /* ENOENT */
      if (mkdir(zpath, 0744) == -1) {
          fprintf(stderr,"failed to create directory '%s'\n", path);
          return;
      }    

   } else {

        if (!S_ISDIR(fi.st_mode)) {
            fprintf(stderr,"'%s' exists and is not a directory\n", path);
            exit(1);
        }
   }

   len = strlen(zpath);
   if (zpath[len - 1] == '/') {
       len--;
   }
   else {
      zpath[len] = '/';
   }


   ctx->path.data = (char*)malloc(strlen(zpath));
   ctx->path.len = strlen(zpath);
   memset(ctx->path.data,0,ctx->path.len);
   memcpy(ctx->path.data,zpath,strlen(zpath));

   return;
}

void
hls_parse_args(hls_config_t *ctx, int argc, char** argv)
{
    hls_options_t   options;
    //char           *output_filename;
    //char           *remove_filename;
    //char           *dot;
    int             opt;
    int             longindex;
    char           *endptr;

    static const char *optstring = "i:p:n:u:c:ovh?";
    //static const char *optstring = "i:d:p:n:u:c:ovh?";

    static const struct option longopts[] = { 
        { "input",         required_argument, NULL, 'i' },
        //{ "duration",      required_argument, NULL, 'd' },
        { "output-prefix", required_argument, NULL, 'p' },
        { "stream-name",   required_argument, NULL, 'n' },
        { "url-prefix",    required_argument, NULL, 'u' },
        { "num-segments",  required_argument, NULL, 'c' },
        { "help",          no_argument,       NULL, 'h' },
        { "verbose",       no_argument,       NULL, 'v' },
        { 0, 0, 0, 0 } 
    };  

    memset(&options, 0 ,sizeof(options));

    /* Set some defaults */
    options.segment_duration = 10;
    options.num_segments = 0;

    do {
        opt = getopt_long(argc, argv, optstring, longopts, &longindex );
        switch (opt) {
            case 'i':
                options.input_file = optarg;
                if (!strcmp(options.input_file, "-")) {
                    options.input_file = "pipe:";
                }
                break;
            case 'p':
                options.output_prefix = optarg;
                break;
                
            case 'n':
                options.hls_name = optarg;
                break;
                
            case 'u':
                options.url_prefix = optarg;
                break;
                
            case 'c':
                options.num_segments = strtol(optarg, &endptr, 10);
                if (optarg == endptr || options.num_segments < 0 || options.num_segments >= LONG_MAX) {
                    fprintf(stderr, "Maximum number of ts files (%s) invalid\n", optarg);
                    exit(1);
                }
                ctx->num_segments = options.num_segments;
                break;
                
            case 'h':
                //hls_display_usage();
                break;
            case 'v':
                //g_verbose = 1;
                break;
        }       
    } while (opt != -1);

    /* Check required args where set*/
    if (options.input_file == NULL) {
        fprintf(stderr, "Please specify an input file.\n");
        //hls_display_usage();
        exit(1);
    }
    ctx->infile.data = (char*)malloc(strlen(options.input_file)+1);
    memset(ctx->infile.data,0,strlen(options.input_file)+1);
    memcpy(ctx->infile.data,options.input_file,strlen(options.input_file));
    ctx->infile.len = strlen(options.input_file);

    if (options.hls_name == NULL) {
        fprintf(stderr, "Please specify name of stream.\n");
        //hls_display_usage();
        exit(1);
    }
    ctx->name.data = (char*)malloc(strlen(options.hls_name)+1);
    memset(ctx->name.data,0,strlen(options.hls_name)+1);
    memcpy(ctx->name.data,options.hls_name,strlen(options.hls_name));
    ctx->name.len = strlen(options.hls_name);

    if (options.output_prefix == NULL) {
        options.output_prefix = "./ts.files/";
        fprintf(stderr, "Use default output prefix %s\n","./ts.files/");
    }

    //check dir and trailing '/'
    hls_check_dir(ctx,options.output_prefix);

    if (options.url_prefix == NULL) {
        fprintf(stderr, "Use empty url prefix.\n");
        options.url_prefix="";
        //exit(1);
    }

    ctx->base_url.data = (char*)malloc(strlen(options.url_prefix)+1);
    memset(ctx->base_url.data,0,strlen(options.url_prefix)+1);
    memcpy(ctx->base_url.data,options.url_prefix,strlen(options.url_prefix));
    //ctx->base_url.data = options.url_prefix;
    ctx->base_url.len = strlen(options.url_prefix);

    return;
}



