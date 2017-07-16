/*******************************************************************************************
  FileName:    flv_parser.cpp
  Author:      Arvinhu
  copyright :  Copyright@2013 tencent;
  Date:        6/10/2013
  Description: flv流合成功能的实现
  History:     // 历史修改记录
      <author>   	<time>        <version >   <desc>
      Arvinhu    	2013/10/26     1.0          build this moudle
      yuandongtu	2014/01/06     1.1		  解决视频黑屏和FLASH卡的问题
      yuandongtu	2014/03/26     1.2		  解决不同音频数据mux兼容性问题
*******************************************************************************************/
#include "flv_parser.h"
#include "tfc_debug_log.h"
#include "submcd_log.h"
#include "muxsvr.h"

#include "hls/hls_stream.h"
#include "hls/hls_mpegts.h"

extern unsigned int g_iOrgTotalsize, g_iCount, g_iTotalsize, g_iFlvCount;
static const AVPictureType golomb_to_pict_type[5] = {
    AV_PICTURE_TYPE_P, AV_PICTURE_TYPE_B, AV_PICTURE_TYPE_I,
    AV_PICTURE_TYPE_SP, AV_PICTURE_TYPE_SI
};

void 
flv_hexdump(const char* p,int len, const char* type)
{
   char buf[256];
   int i, j, i0;  

   SUBMCD_DEBUG_P(LOG_ERROR,"---- dump buffer (%s) ---- len=%d\n",type,len);

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

      SUBMCD_DEBUG_P(LOG_ERROR,"%s: %s\n", type, buf);
   }

}

int SFlvParser::parser_header(hls_session_t *s, unsigned char *buf,unsigned int size, FILE* file, 
      unsigned int &uiCurLen, unsigned char ucKeyFrame, unsigned char* adtsheader)
{

    if(NULL == (buf+(size -1)))
    {
        uiCurLen = MAX_FLV_HEAD_LEN;
	SUBMCD_DEBUG_P(LOG_ERROR,"FLVPARSER Error parser_header "
                  "buf NULL size[%u]key[%02x]\n ", size, ucKeyFrame);	
	return -1; 
    }
    if( 0 == m_pflv )
    {
        uiCurLen = MAX_FLV_HEAD_LEN;
        SUBMCD_DEBUG_P(LOG_ERROR,"FLVPARSER Error parser_header "
              "m_pflv NULL size[%u]key[%02x]\n ", size, ucKeyFrame);	
	return -1; 
    }

    int ret = write_header(m_pflv->c);

    if(ret < 0)
    {
        uiCurLen = MAX_FLV_HEAD_LEN;
	 SUBMCD_DEBUG_P(LOG_ERROR,"FLVPARSER Error parser_header write_header fail"
                       " size[%u]key[%02x]\n ", size, ucKeyFrame);	
        return ret;
    }

    SUBMCD_DEBUG_P(LOG_ERROR,"FLVPARSER parser_header: write flv header,"
               " [%u]key[%02x]\n ", size, ucKeyFrame);	


    x264_nal_t_ex nalPkt;
    x264_nal_t_ex nalPacket[3];

    set_script_param(m_pflv, m_nWidth, m_nHeight);

    SUBMCD_DEBUG_P(LOG_ERROR,"FLVPARSER parser_header: write script param,"
               " [%u]key[%02x]\n ", size, ucKeyFrame);	

    //get pps sps parameters
    int pass = 0;
    unsigned int pos = 0;
    do
    {
   	pass = get_nal_buffer(buf,size, &nalPkt, pos, ucKeyFrame);
	if(pass < 0)
	{
	    uiCurLen = MAX_FLV_HEAD_LEN;
	    SUBMCD_DEBUG_P(LOG_ERROR,"FLVPARSER Error parser_header get_nal_buffer not passed,"
                               "size[%u]key[%02x]\n ", size, ucKeyFrame);	
	    break;
	}


	if(nalPkt.i_type == NAL_SPS) {
            nalPacket[0] = nalPkt; //SPS
	    SUBMCD_DEBUG_P(LOG_ERROR,"FLVPARSER parser_header: get NAL_SPS\n");	
	} else  if(nalPkt.i_type == NAL_PPS) {
            nalPacket[1] = nalPkt;//PPS
	    SUBMCD_DEBUG_P(LOG_ERROR,"FLVPARSER parser_header: get NAL_PPS\n");	
	} else  if(nalPkt.i_type == NAL_SEI){//SEI
	    nalPacket[2] = nalPkt;
	    SUBMCD_DEBUG_P(LOG_ERROR,"FLVPARSER parser_header: get NAL_SEI\n");	
	    #define BUFFER_SIZE 1024
	    unsigned char* p = m_pflv->sei;
	    int len = nalPkt.i_payload - nalPkt.bytes_startcode;	
   	    if(!m_pflv->sei)
	 	   p = (unsigned char*)malloc(16*BUFFER_SIZE);
	    /*else if(m_pflv->sei && (m_pflv->sei_len + nalPkt.i_payload) > BUFFER_SIZE)
	   	   p = (unsigned char*)realloc(m_pflv->sei,2*BUFFER_SIZE);
	    else if(m_pflv->sei && (m_pflv->sei_len + nalPkt.i_payload) > 2*BUFFER_SIZE)
	       p = (unsigned char*)realloc(m_pflv->sei,4*BUFFER_SIZE);*/
        
	   m_pflv->sei = p;
		
	   m_pflv->sei[m_pflv->sei_len] = (unsigned char)(len >> 24);
	   m_pflv->sei[m_pflv->sei_len + 1] =  (unsigned char)(len >> 16);
	   m_pflv->sei[m_pflv->sei_len + 2] =  (unsigned char)(len >> 8);
	   m_pflv->sei[m_pflv->sei_len + 3] =  (unsigned char)len;
	   memcpy(m_pflv->sei + m_pflv->sei_len + 4, nalPkt.p_payload+nalPkt.bytes_startcode,len);
	   m_pflv->sei_len += len + 4;
	} else {
	    SUBMCD_DEBUG_P(LOG_ERROR,"FLVPARSER parser_header: unknown data\n");	
        }
    } while (pos <= (size-4));
  	
    ret = write_h264_headers(s, m_pflv, nalPacket);
    if(ret < 0)
    {
        uiCurLen = MAX_FLV_HEAD_LEN;
        SUBMCD_DEBUG_P(LOG_ERROR,"FLVPARSER Error parser_header write_h264_headers "
                                 "fail size[%u]key[%02x]\n ", size, ucKeyFrame);	
        return -1;
    }

    {
	unsigned char objecttype	= 0;
	unsigned char samplerate_index 	= 0;
	unsigned char channels		= 0;
        SUBMCD_DEBUG_P(LOG_ERROR,"FLVPARSER adts headers\n");
	parser_adts(adtsheader, 7, &objecttype, &samplerate_index, &channels);
	if(objecttype == 0 && channels ==0)
	{
		objecttype = 1;
		samplerate_index = 6;
		channels = 1;
	}
    	write_aac_headers(s,m_pflv,objecttype,samplerate_index,channels);
        SUBMCD_DEBUG_P(LOG_ERROR,"FLVPARSER aac headers, objtype=%u,samplerate=%u,channels=1\n ",
                                  objecttype,samplerate_index,channels);
    }
/*
    if(NULL != file)
    {
        int n,fd;
        n = fwrite(m_pflv->c->data, m_pflv->c->d_cur, 1, file);

        fd  = open("./live.flv", O_CREAT|O_WRONLY|O_APPEND, 0644);
        if ( fd < 0 ) {
            SUBMCD_DEBUG_P(LOG_ERROR,"FLVPARSER WriteFlvData failed to open file, ret=%u",fd);
        }
        n = write(fd,m_pflv->c->data, m_pflv->c->d_cur);
        if ( n <= 0 ) {
            SUBMCD_DEBUG_P(LOG_ERROR,"FLVPARSER WriteFlvData failed to open file,"
                     "ret=%u, size=%u\n",n,m_pflv->c->d_cur);
        }
        SUBMCD_DEBUG_P(LOG_ERROR,"FLVPARSER WriteFlvData open file,"
                     " ret=%u, size=%u\n",n,m_pflv->c->d_cur);
    }
*/
    //dumpflv();

    uiCurLen = m_pflv->c->d_cur;
    return ret;
}

int SFlvParser::parser_body_for_video(hls_session_t *s,unsigned char *pucBuf,int iSize, unsigned int &uiCurLen, 
         unsigned char ucKeyFrame, unsigned int uiSeq, unsigned int uiRoomId, unsigned int uiRecordRoom)
{
    int iRet = 0;


    if((NULL == pucBuf)||(0 >= iSize))
    {
	     SUBMCD_DEBUG_P(LOG_TRACE,"SFlvParser::parser_body_for_video invalid size: %d\n ", iSize);
        return -1;
    }


    iRet = write_h264_frame(s,m_pflv, pucBuf, iSize,m_uiCurTimeStamp,
                             ucKeyFrame, uiSeq, uiRoomId, uiRecordRoom);
    //iRet = write_h264_frame(m_pflv, pFrame->pucBuf, pFrame->iSize, time_stamp,pFrame->ucKeyFrame, 
    //                            pFrame->uiSeq, pFrame->uiRoomId, pFrame->uiRecordRoom);
	
    uiCurLen = m_pflv->c->d_cur;

    if(m_uiLastVideoSeq+1 != uiSeq)
    {
        if(m_uiLastVideoSeq != 0)
        {
		SUBMCD_DEBUG_P(LOG_ERROR,"SFlvParser::parser_body_for_video seq mismatch"
                        " last_seq: %u, now_seq: %u roomid[%u]\n", m_uiLastVideoSeq, uiSeq, uiRoomId);
        }
    }

    m_uiLastVideoSeq = uiSeq;
    return iRet;
}

int SFlvParser::parser_body_for_audio(hls_session_t *s,unsigned char *pucBuf,int iSize, 
               unsigned int &uiCurLen, unsigned int uiSeq, unsigned int uiRoomId)
{
    unsigned char ucHighlen = 0, ucMiddlelen = 0, ucLowlen = 0;
    int iAudioFrameLen = 0;
    unsigned int uiCurAudioTime = 0;
    int iDiff = 0;

    if((NULL == (pucBuf+(iSize -1)))||(iSize <= 22))
    {
	 SUBMCD_DEBUG_P(LOG_ERROR,"FLVPARSER Error parser_body_for_audio size[%d]seq[%u]\n ", iSize, uiSeq);	
	 return -1; 
    }
    uiCurAudioTime = m_uiCurOffset + (int)(m_uiCurAudioTimeStamp - m_uiCurTimeStamp);
    flv_buffer *c = m_pflv->c;
    x264_put_byte( c, FLV_TAG_TYPE_AUDIO);
    x264_put_be24( c, 0 );// calculated later
    x264_put_be24( c, uiCurAudioTime);
    x264_put_byte( c, (uiCurAudioTime >> 24)& 0x7F);
    x264_put_be24( c, 0 );//stream id
    unsigned int uiStart = c->d_cur;
    x264_put_byte( c, 0xaf);//aac
    /*aac packet type aac raw*/
    x264_put_byte( c, 1);
    /*Get packet length*/
    ucHighlen      = *(pucBuf+18);
    ucMiddlelen    = *(pucBuf+19);
    ucLowlen       = *(pucBuf+20);
    iAudioFrameLen = ((ucHighlen & 0x03) << 11)|(ucMiddlelen << 3)|(ucLowlen >> 5);

    iDiff          = m_uiCurAudioTimeStamp - m_uiPreAudioTimeStamp;

    if(iAudioFrameLen > iSize-15)
    {
        SUBMCD_DEBUG_P(LOG_ERROR,"SFlvParser::parser_body_for_audio FLVPARSERTestAUDIOLENGTH "
                                 "TOO Larger ERROR[%d][%d]High[%02X]Middle[%02X]Low[%02X]\n", 
            iAudioFrameLen, iSize-15, ucHighlen, ucMiddlelen, ucLowlen);
        iAudioFrameLen = iSize-15;
        printf_buf_data(pucBuf, iSize, 0, uiSeq, 0);
    }

    if(iAudioFrameLen < 7)
    {
        SUBMCD_DEBUG_P(LOG_ERROR,"SFlvParser::parser_body_for_audio FLVPARSERTestAUDIOLENGTH "
                                 "TOO Small ERROR[%d]High[%02X]Middle[%02X]Low[%02X]\n", 
            iAudioFrameLen, ucHighlen, ucMiddlelen, ucLowlen);
        iAudioFrameLen = iSize-15;
        printf_buf_data(pucBuf, iSize, 0, uiSeq, 0);
    }

    if(flv_append_data( c, pucBuf+22, (iAudioFrameLen-7)) < 0)
    {
        SUBMCD_DEBUG_P(LOG_ERROR,"SFlvParser::parser_body_for_audio FLVPARSERERROR flv_append_data "
                                 "audio data fail seq[%u]len[%d]\n", uiSeq, iSize);

        printf_buf_data(pucBuf, iSize, 0, uiSeq, 0);
        return -1;
    }



    if(iDiff < 0)
    {
        if(0 != m_uiPreAudioTimeStamp)
        {
            SUBMCD_DEBUG_P(LOG_TRACE,"SFlvParser::parser_body_for_audio FLVPARSERSendAudio "
                                     "usAACFrameLen[%d]seq[%u]time[%u]-[%u]=[%u]\n",
                iAudioFrameLen, uiSeq,
                m_uiCurAudioTimeStamp, m_uiPreAudioTimeStamp, iDiff);
        }
    }
    unsigned int uiLength = c->d_cur - uiStart;
    rewrite_amf_be24( c, uiLength, uiStart - 10 );
    x264_put_be32( c, 11 + uiLength );// Last tag size
    m_uiPreAudioTimeStamp = m_uiCurAudioTimeStamp;
    
    //hls_audio_handler(s,(char*)c->data+16,c->d_cur-16,uiCurAudioTime);	

    uiCurLen = m_pflv->c->d_cur;
    if(uiSeq != m_uiLastAudioSeq+1)
    {
        if(m_uiLastAudioSeq != 0)
        {
            SUBMCD_DEBUG_P(LOG_ERROR,"SFlvParser::parser_body_for_audio FLVPARSERERROR "
                                     "seq error lastseq[%u]nowseq[%u]roomid[%u]\n", 
                                     m_uiLastAudioSeq, uiSeq, uiRoomId);
        }
    }
    m_uiLastAudioSeq = uiSeq;
    return 0;
}

int SFlvParser::get_slice_type(unsigned char* buffer,unsigned int datasize)
{
	int nal_type = 0;
	unsigned char slice_type = 0;
	x264_nal_t_ex nalPkt;  
   	 int pass = 0;
    	unsigned int pos = 0;
		
  	 do
   	{
  		 pass = get_nal_buffer(buffer,datasize, &nalPkt, pos, 0);
		 if(pass < 0)
		{
		    SUBMCD_DEBUG_P(LOG_ERROR,"FLVPARSER Error parser_header get_nal_buffer 1 fail size[%u]\n ", datasize);	
		    return 0;
		}
		nal_type = nalPkt.p_payload[nalPkt.bytes_startcode] & 0x1F; 

		//获取slice type
		if(nal_type == NAL_SLICE   || nal_type == NAL_DPA)
		{
			unsigned char value =  nalPkt.p_payload[nalPkt.bytes_startcode + 1]; 	// 这里假定first_mb_in_slice值为0，否则nal_type计算会出现错误
			unsigned char shift = value;
			if(!(value & 0x80))	{
				SUBMCD_DEBUG_P(LOG_ERROR,"FLVPARSER Error get_slice_type error,first_mb_in_slice value is not zero \n");	
				return -1;
			}
			value <<= 1;
			if(value & 0x80) 	slice_type = 0;
			else if(value & 0x40){
				shift = value << 2;
				slice_type = shift >> 7; 
				slice_type += 1;
			}
			else if(value & 0x20){
				shift = value << 3;
				slice_type = shift >> 6;
				slice_type += 3;
			}
			else if(value & 0x10){
				shift = value << 4;
				slice_type = shift >> 5;
				slice_type += 7;
			}
			else	{
				continue;
			}
			if(slice_type >= 5)
				slice_type -= 5;
			break;
		}
		else if(nal_type ==NAL_IDR_SLICE)
		{
			slice_type = 2;
			break;
		}
   	 }while(1);

	slice_type = (int)golomb_to_pict_type[slice_type]; 
	return slice_type;	
 }
 
int SFlvParser::get_nal_buffer(unsigned char* buffer,unsigned int datasize,
            x264_nal_t_ex *nalPacket,unsigned int& prepos, unsigned char ucKeyFrame)
{
   int bytes_startcode = 4;
  
   if(NULL == (buffer+(datasize -1)))
   {
       SUBMCD_DEBUG_P(LOG_ERROR,"FLVPARSER Error get_nal_buffer"
                  " buf NULL size[%u]key[%02x]\n ", datasize, ucKeyFrame);	
       return -1; 
   }
   if(int(datasize - prepos) < 4)
   {
       SUBMCD_DEBUG_P(LOG_ERROR,"FLVPARSER Error get_nal_buffer "
              "size[%u]prepos[%u]key[%02x]\n ", datasize, prepos, ucKeyFrame);	
       return -1;
   }

   if(buffer[prepos] != 0x00 || buffer[prepos+1] != 0x00)
   	return -1;
   if(buffer[prepos+2] == 0x01)
   	bytes_startcode = 3;

   SUBMCD_DEBUG_P(LOG_DEBUG,"FLVPARSER: get_nal_buffer, "
              "bytes_startcode=%u,size=%u,prepos=%u,key=%02x\n", 
              bytes_startcode, datasize, prepos, ucKeyFrame);	

    unsigned int pos = prepos + bytes_startcode;

    if(pos >= datasize - bytes_startcode)
    {
        pos = datasize;
        nalPacket->i_payload = pos - prepos;
        nalPacket->p_payload = &buffer[prepos];
        nalPacket->i_type = buffer[prepos + bytes_startcode] & 0x1F;	//ucKeyFrame == 1 ? 5 : 1;
        nalPacket->bytes_startcode = bytes_startcode;
        prepos = pos;
        return 1;
    }

    while(pos < datasize - bytes_startcode)
    {
        //if(buffer[pos]   == 0x00 && buffer[pos+1] == 0x00 && buffer[pos+2] == 0x00 && buffer[pos+3] == 0x01)
        //    break;
        //else if(buffer[pos]   == 0x00 && buffer[pos+1] == 0x00 && buffer[pos+2] == 0x01)
        //    break;
	 //else  if(buffer[pos]   == 0x00 && buffer[pos+1] == 0x00 && buffer[pos+2] == 0x00)
        //    break;
	    if(buffer[pos]   == 0x00 && buffer[pos+1] == 0x00 &&
	    	(buffer[pos+2] == 0x01 || (buffer[pos+2] == 0x00 && buffer[pos+3] == 0x01)) )
    		break;

        pos ++;
    }


    if( pos == datasize - bytes_startcode)
    {
        pos = datasize;
        nalPacket->i_payload = pos - prepos;
        nalPacket->p_payload = &buffer[prepos];
        nalPacket->i_type = buffer[prepos+bytes_startcode] & 0x1F; //ucKeyFrame == 1 ? 5 : 1;
        nalPacket->bytes_startcode = bytes_startcode;
        prepos = pos;
        return 1;
    }

    nalPacket->i_payload = pos - prepos;
    nalPacket->p_payload = &buffer[prepos];
    nalPacket->bytes_startcode = bytes_startcode;
    nalPacket->i_type = buffer[prepos+ bytes_startcode ] & 0x1F; //ucKeyFrame == 1 ? 5 : 1;

    prepos = pos;

    return 0;
}

int SFlvParser::write_header(flv_buffer *c)
{
    if( 0 == c )
        return -1;

    x264_put_tag( c, "FLV" ); // Signature
    x264_put_byte( c, 1 );    // Version
    x264_put_byte( c, 5 );    // Video + Audio
    //x264_put_byte( c, 1 );    // Video + Audio
    x264_put_be32( c, 9 );    // DataOffset
    x264_put_be32( c, 0 );    // PreviousTagSize0

    return 0;
}

int SFlvParser::set_script_param( flv_hnd_t *handle, double i_width, double i_height)
{
    flv_hnd_t *p_flv = handle;
    flv_buffer *c = p_flv->c;
    /* Tag Type "script data" */
    x264_put_byte( c, FLV_TAG_TYPE_META ); 

    int start = c->d_cur;
    x264_put_be24( c, 0 ); // data length
    x264_put_be24( c, 0 ); // timestamp
    x264_put_be32( c, 0 ); // reserved
    x264_put_byte( c, AMF_DATA_TYPE_STRING );
    x264_put_amf_string( c, "onMetaData" );
    x264_put_byte( c, AMF_DATA_TYPE_MIXEDARRAY );
    x264_put_be32( c, 12 );

    x264_put_amf_string( c, "width" );
    x264_put_amf_double( c, i_width );

    x264_put_amf_string( c, "height" );
    x264_put_amf_double( c, i_height );

    x264_put_amf_string( c, "framerate" );

    p_flv->i_framerate_pos = c->d_cur + c->d_total + 1;
    x264_put_amf_double( c, m_nFps); // written at end of encoding
    x264_put_amf_string( c, "videocodecid" );
    x264_put_amf_double( c, FLV_CODECID_H264 );

    x264_put_amf_string( c, "duration" );
    p_flv->i_duration_pos = c->d_cur + c->d_total + 1;
    x264_put_amf_double( c, 0.00 ); // written at end of encoding

    x264_put_amf_string( c, "filesize" );
    p_flv->i_filesize_pos = c->d_cur + c->d_total + 1;
    x264_put_amf_double( c, 0.00 ); // written at end of encoding

    x264_put_amf_string( c, "videodatarate" );
    p_flv->i_bitrate_pos = c->d_cur + c->d_total + 1;
    x264_put_amf_double( c, m_nBitrate); // written at end of encoding
   
    x264_put_amf_string( c, "audiodatarate" );
    x264_put_amf_double( c, 32.00 );
    
    x264_put_amf_string( c, "audiosamplerate" );
    x264_put_amf_double( c, 48000.00 );
    
    x264_put_amf_string( c, "audiosamplesize" );
    x264_put_amf_double( c, 16.00 );

    x264_put_amf_string( c, "stereo" );
    x264_put_amf_double( c, 1.00 );

    x264_put_amf_string( c, "audiocodecid" );
    x264_put_amf_double( c, 10.00 );
    
    x264_put_amf_string( c, "" );
    x264_put_byte( c, AMF_END_OF_OBJECT );
    unsigned int uiLength = c->d_cur - start;
    rewrite_amf_be24( c, uiLength - 10, start );
    x264_put_be32( c, uiLength + 1 );// tag length       
    
    return 0;
}

int SFlvParser::write_h264_headers(hls_session_t *s, flv_hnd_t *handle, x264_nal_t_ex *p_nal )
{
    int ret =0;
    int header_start;

    flv_hnd_t *p_flv = handle;
    flv_buffer *c = p_flv->c;
   
    header_start = c->d_cur;

    int sps_size = p_nal[0].i_payload;
    int pps_size = p_nal[1].i_payload;
    if((sps_size <= 4)||(pps_size <= 4))
    {
        SUBMCD_DEBUG_P(LOG_ERROR, "FLVPARSER ERROR write_h264_headers"
                  " sps_size[%d]pps_size[%d]\n", sps_size, pps_size);
        return -1;
    }
    //   int sei_size = p_nal[2].i_payload;
    // SEI
    /* It is within the spec to write this as-is but for
    * mplayer/ffmpeg playback this is deferred until before the first frame */
    /*
    p_flv->sei = malloc( sei_size );
    if( !p_flv->sei )
    return -1;
    p_flv->sei_len = sei_size;
    memcpy( p_flv->sei, p_nal[2].p_payload, sei_size );
    */
    // SPS
    unsigned char *sps = p_nal[0].p_payload + 4;
    if(NULL == sps)
    {
        SUBMCD_DEBUG_P(LOG_ERROR, "FLVPARSER sps NULL write_h264_headers"
                       " sps_size[%d]pps_size[%d]\n", sps_size, pps_size);
        return -1;
    }
    if(NULL == sps+3)
    {
        SUBMCD_DEBUG_P(LOG_ERROR, "FLVPARSER sps NULL1 write_h264_headers"
                    " sps_size[%d]pps_size[%d]\n", sps_size, pps_size);
        return -1;
    }
    x264_put_byte( c, FLV_TAG_TYPE_VIDEO );
    x264_put_be24( c, 0 ); // rewrite later
    x264_put_be24( c, 0 ); // timestamp
    x264_put_byte( c, 0 ); // timestamp extended
    x264_put_be24( c, 0 ); // StreamID - Always 0
    p_flv->start = c->d_cur; // needed for overwriting length

    x264_put_byte( c, 7 | FLV_FRAME_KEY ); // Frametype and CodecID
    x264_put_byte( c, 0 ); // AVC sequence header
    x264_put_be24( c, 0 ); // composition time

    // copy to hls from here.
    x264_put_byte( c, 1 );      // version
    x264_put_byte( c, sps[1] ); // profile
    x264_put_byte( c, sps[2] ); // profile
    x264_put_byte( c, sps[3] ); // level
    x264_put_byte( c, 0xff );   // 6 bits reserved (111111) + 2 bits nal size length - 1 (11)
    x264_put_byte( c, 0xe1 );   // 3 bits reserved (111) + 5 bits number of sps (00001)

    //dumpflv();

    x264_put_be16( c, sps_size - 4 );
    ret = flv_append_data( c, sps, sps_size - 4 );

    SUBMCD_DEBUG_P(LOG_ERROR, "FLVPARSER write_h264_headers sps dump,"
                  " sps_size[%d]pps_size[%d]\n", sps_size, pps_size);
    //dumpflv();

    if(ret < 0)
    {
        SUBMCD_DEBUG_P(LOG_ERROR, "FLVPARSER flv_append_data1 ERROR write_h264_headers,"
                    " sps_size[%d]pps_size[%d]\n", sps_size, pps_size);
        return ret;
    }

    // PPS
    x264_put_byte( c, 1 ); // number of pps
    x264_put_be16( c, pps_size - 4 );
    ret = flv_append_data( c, p_nal[1].p_payload + 4, pps_size - 4 );

    SUBMCD_DEBUG_P(LOG_ERROR, "FLVPARSER write_h264_headers pps dump,"
                  " sps_size[%d]pps_size[%d]\n", sps_size, pps_size);


    if(ret < 0)
    {
        SUBMCD_DEBUG_P(LOG_ERROR, "FLVPARSER flv_append_data2 ERROR write_h264_headers"
              " sps_size[%d]pps_size[%d]\n", sps_size, pps_size);
        return ret;
    }

    SUBMCD_DEBUG_P(LOG_ERROR, "FLVPARSER write_h264_headers avc_header=%u,cur_len=%u,videoid=%u\n", 
                  header_start,c->d_cur, s == NULL ? 0 : s->videoid);

    hls_handle_h264_header(s, (char*)c->data + header_start + 16, 
                c->d_cur - header_start - 16);
                       
    // rewrite data length info
    unsigned length = c->d_cur - p_flv->start;
    rewrite_amf_be24( c, length, p_flv->start - 10 );
    x264_put_be32( c, length + 11 ); // Last tag size
    //CHECK( flv_flush_data( c ) );

    SUBMCD_DEBUG_P(LOG_ERROR, "FLVPARSER write_h264_headers rewrite length dump,"
                  " sps_size[%d]pps_size[%d]\n", sps_size, pps_size);

    return sps_size + pps_size;
}

int SFlvParser::parser_adts(const unsigned char* adtsheader, int len, 
          unsigned char *objecttype, unsigned char *samplerate_index, unsigned char *channels)
{
	assert(adtsheader && len >=7);

	*objecttype			= (adtsheader[2] >> 6) & 0x03;
	*samplerate_index	= (adtsheader[2] >> 2) & 0x0F;
	*channels			=  adtsheader[2] & 0x01;
	*channels  			<<=2 ;
	*channels			|= (adtsheader[3] >> 6) & 0x03;
	SUBMCD_DEBUG_P(LOG_ERROR,"write_aac_headers adts header %02x %02x %02x %02x %02x %02x %02x\n", 
		adtsheader[0],adtsheader[1],adtsheader[2],adtsheader[3],adtsheader[4],adtsheader[5],adtsheader[6]);
	return 0;
}

int SFlvParser::write_aac_headers(hls_session_t *s, flv_hnd_t *handle, unsigned char objecttype, 
                                   unsigned char samplerate_index, unsigned char channels)
{
    flv_hnd_t *p_flv = handle;
    flv_buffer *c = p_flv->c;
    unsigned char byte[5] = {0};
    int header_len = 0;

   //规范Object编码器profile级别
    //if(objecttype == 1 && channels > 1)
   // 		objecttype = 2;
    //if(objecttype >= 3)
//		objecttype = 1;

    objecttype++;
    byte[0] 	|= (objecttype<< 3);
    byte[0]  	|= (samplerate_index >> 1); // &0x0e
    byte[1] 	|= (samplerate_index << 7);
    byte[1] 	|= (channels << 3);
    header_len = 2;
    if(objecttype == 2)
    {
 	byte[2]  = 0x2B;
	byte[3]  = 0xE5;
    	byte[4]  = 0x0;
        header_len += 3;
    }
	
    x264_put_byte( c, FLV_TAG_TYPE_AUDIO);
    x264_put_be24( c, 0 );// calculated later

    x264_put_be24( c, 0);
    x264_put_byte( c, 0);
    x264_put_be24( c, 0 );//stream id
    unsigned int uiStart = c->d_cur;
    x264_put_byte( c, 0xaf);//aac
    x264_put_byte( c, 0);
	
    x264_put_byte(c,byte[0]);
    x264_put_byte(c,byte[1]);
    if(objecttype == 2)
    {
	x264_put_byte(c,byte[2]);
    	x264_put_byte(c,byte[3]);
    	x264_put_byte(c,byte[4]);
    }
	
    unsigned int uiLength = c->d_cur - uiStart;

    SUBMCD_DEBUG_P(LOG_ERROR, "write_aac_headers aac encode params %02x %02x %02x %02x %02x"
                              " objecttype[%d] samplerate_index[%d] channels[%d], length=%u\n", 
			byte[0], byte[1],byte[2],byte[3],byte[4],objecttype,samplerate_index,channels,uiLength);

    hls_handle_aac_header(s,(char*)byte,header_len);

    rewrite_amf_be24( c, uiLength, uiStart - 10 );
    x264_put_be32( c, uiLength + 11 ); // Last tag size
    return 0;
}

int SFlvParser::printf_buf_data(unsigned char *pucBuf,int iSize, unsigned char ucKeyFrame, unsigned int uiSeq, int iPos)
{
   int i = 0, iStrLen = 0, iMaxLen = FLV_PRINT_MAX_SIZE - 10;
   memset(m_strPrintBuf, 0, FLV_PRINT_MAX_SIZE);
   char *pTemp = NULL;
   SUBMCD_DEBUG_P(LOG_ERROR, "FLVPARSERFrameTimePrintBuf key[%02X]seq[%u]size[%d]\n", ucKeyFrame, uiSeq, iSize);
   if(NULL == (pucBuf+(iSize -1)))
   {
       SUBMCD_DEBUG_P(LOG_ERROR,"FLVPARSER Error printf_buf_data buf NULL size[%u]\n ", iSize);	
       return -1; 
   }
   for(i=iPos; i<iSize; i++)
   {
        pTemp = m_strPrintBuf + iStrLen;
        if((i > 0)&&(0 == i%NUM_CHAR_HEX_LINE))
        {
            iStrLen += snprintf(pTemp, FLV_PRINT_MAX_SIZE, "0X%02X\n ", (unsigned char)(*(pucBuf+i)));
        }
        else
        {
            iStrLen += snprintf(pTemp, FLV_PRINT_MAX_SIZE, "0X%02X ", (unsigned char)(*(pucBuf+i)));
        }
        if(iStrLen >= iMaxLen)
        {
            break;
        }
   }
   SUBMCD_DEBUG_P(LOG_ERROR, m_strPrintBuf);
   return 0;
}

int SFlvParser::diff_sps_printf(unsigned char *pStr, unsigned int uiSize)
{
	static unsigned char strSPSData[256] = {0};
	static unsigned int iSPSLen = 0;
        unsigned int i = 0;
		
        if(NULL == (pStr+(uiSize -1)))
	 {
	    SUBMCD_DEBUG_P(LOG_ERROR,"FLVPARSER Error "
                  "diff_sps_printf buf NULL size[%u]\n ", uiSize);	
	    return -1; 
	 }
        if(0x67 != *pStr)
        {
            return 0;
        }
	 if(uiSize >= 256)
	 {
	     SUBMCD_DEBUG_P(LOG_ERROR, "SPSLENGTHERROR[%u]\n", uiSize);
	     printf_buf_data(pStr, uiSize, 0, 0, 0);
	     return -1;
	 }
	 for(i = 0; i < uiSize; i++)
	 {
	     if(strSPSData[i] != *(pStr+i))
	     {
	         break;
	     }
	 }
	 if((i != uiSize)||(i != iSPSLen))
	 {
	     for(i =0; i <uiSize; i++)
	     {
	         strSPSData[i] = *(pStr+i);
	     }
	     iSPSLen = uiSize;
	     SUBMCD_DEBUG_P(LOG_ERROR, "SPSNEWERROR[%u]\n", uiSize);
	     printf_buf_data(pStr, uiSize, 0, 0, 0);
	     return -2;
	 }
	 return 0;
}

int SFlvParser::write_h264_frame(hls_session_t *s,flv_hnd_t *handle, unsigned char* buffer,unsigned int datasize, 
      int time_stamp,unsigned char ucKeyFrame, unsigned int uiSeq, unsigned int uiRoomid, unsigned int uiRecordRoom)
{
    x264_nal_t_ex nalPacket[2];

    //get pps sps parameters
    int pass = 0;
    unsigned int pos = 0; 

    if(NULL == handle) {
        SUBMCD_DEBUG_P(LOG_ERROR,"FLVPARSER Error write_h264_frame handle NULL\n");	
        return -1;
    }

    if(NULL == (buffer+(datasize -1))) {

        SUBMCD_DEBUG_P(LOG_ERROR,"FLVPARSER Error write_h264_frame"
                                 " buf NULL size[%u]\n ", datasize);	
	 return -1; 
    }

    flv_buffer *c = handle->c;
    int offset;
    
    offset = (int)(time_stamp - m_uiPreTimeStamp);

    if(datasize < 4) {

	SUBMCD_DEBUG_P(LOG_ERROR, "h264frameSize ERROR datasize[%u][%02X]seq[%u][%u]\n", 
                              datasize, ucKeyFrame, uiSeq, uiRoomid);
    }
    else {
       if(buffer[0] == 0x00 && buffer[1] == 0x00 
          && buffer[2] == 0x00 && buffer[3] == 0x01) {
        ;

       } else {
          SUBMCD_DEBUG_P(LOG_ERROR, "h264frameHeader ERROR "
                                "datasize[%u][%02X]seq[%u][%02X][%02X][%02X][%02X][%u]\n", 
				datasize, ucKeyFrame, uiSeq, buffer[0], buffer[1], 
                                buffer[2],buffer[3], uiRoomid);
       }

       if((buffer[datasize-4]== 0x00 && buffer[datasize-3] == 0x00 
          && buffer[datasize-2] == 0x00 && buffer[datasize-1] == 0x01)||
	  (buffer[datasize-3] == 0x00 && buffer[datasize-2] == 0x00 
           && buffer[datasize-1] == 0x01)) {

           SUBMCD_DEBUG_P(LOG_ERROR, "h264frameEnder ERROR "
                                "datasize[%u][%02X]seq[%u][%02X][%02X][%02X][%02X][%u]\n", 
				datasize, ucKeyFrame, uiSeq, buffer[datasize-4], 
                                buffer[datasize-3], buffer[datasize-2], 
                                buffer[datasize-1], uiRoomid);
       } else {
         ;
       }
    }

    /*if((offset < 0)||(0 == m_uiPreTimeStamp))
    {
        SUBMCD_DEBUG_P(LOG_ERROR, "SFlvParser::write_h264_frame FLVPARSERFrameTime ERROR"
             " CurTimeStamp[%u]PreTimeStamp[%u][%u]\n", time_stamp, m_uiPreTimeStamp, uiRoomid);
        offset = 0;
    }*/
    
    //m_uiCurOffset += offset;
    m_uiCurOffset = time_stamp;
    // A new frame - write packet header
    x264_put_byte( c, FLV_TAG_TYPE_VIDEO);
    x264_put_be24( c, 0 ); // length, calculated later
    x264_put_be24( c, m_uiCurOffset);// timestamp
    x264_put_byte( c, (m_uiCurOffset >>24)& 0x7F);// timestamp

    //SUBMCD_DEBUG_P(LOG_ERROR, "h264frameEncoder timestamp=%u,%x \n", 
    //        m_uiCurOffset,m_uiCurOffset);
    
    x264_put_be24( c, 0 );// streamid

    handle->start = c->d_cur;

    if(ucKeyFrame == 1)
       x264_put_byte( c, FLV_FRAME_KEY);
    else 
       x264_put_byte( c, FLV_FRAME_INTER);

    x264_put_byte( c, 1 ); // AVC NALU
    
    x264_put_be24( c, 0);    

    if( m_pflv->sei )
    {
        flv_append_data( c, m_pflv->sei, m_pflv->sei_len );
        free( m_pflv->sei );
        //m_pflv->sei_len = 0;
        m_pflv->sei = NULL;
    }

    if(ucKeyFrame == 1)
    {
       pass = get_nal_buffer(buffer,datasize, &nalPacket[0], pos, ucKeyFrame);
       SUBMCD_DEBUG_P(LOG_TRACE, "get nal buffer, pass=%u\n",pass);

       if( (-1 == pass) || (nalPacket[0].i_payload <= nalPacket[0].bytes_startcode) ) {
            //printf_buf_data(buffer, datasize, ucKeyFrame, uiSeq, pos);
	     if(uiRecordRoom == uiRoomid) {

                SUBMCD_DEBUG_P(LOG_ERROR, "FLVPARSER Length invalid FrameSize1 ERROR,"
                                          " datalen[%u]pos[%u]len[%d][%d][%u]\n", datasize, pos, 
					nalPacket[0].i_payload, nalPacket[0].bytes_startcode, g_iOrgTotalsize);
	     }
       } else {
            if(uiRecordRoom == uiRoomid)
            {
	            diff_sps_printf(nalPacket[0].p_payload + nalPacket[0].bytes_startcode, 
                                     nalPacket[0].i_payload - nalPacket[0].bytes_startcode);
            }
            x264_put_be32(c, nalPacket[0].i_payload - nalPacket[0].bytes_startcode);
            if(flv_append_data( c, nalPacket[0].p_payload + nalPacket[0].bytes_startcode, 
                                nalPacket[0].i_payload - nalPacket[0].bytes_startcode) < 0)
            {
                printf_buf_data(buffer, datasize, ucKeyFrame, uiSeq, pos);
                SUBMCD_DEBUG_P(LOG_ERROR, "SFlvParser::write_h264_frame "
                     "FLVPARSERFrameSize1 [%u]ERROR[%u]\n ", nalPacket[0].i_payload, uiRoomid);
                return -1;
            }
        }
        pass = get_nal_buffer(buffer,datasize, &nalPacket[0], pos, ucKeyFrame);
        SUBMCD_DEBUG_P(LOG_DEBUG, "get nal buffer 2, pass=%u\n",pass);
        if((-1 == pass)||(nalPacket[0].i_payload <= nalPacket[0].bytes_startcode))
        {
            //printf_buf_data(buffer, datasize, ucKeyFrame, uiSeq, pos);
            if(uiRecordRoom == uiRoomid)
            {
                SUBMCD_DEBUG_P(LOG_ERROR, "FLVPARSER Length invalid FrameSize2 ERROR, "
                                        "datalen[%u]pos[%u]len[%d][%d][%u]\n", datasize, pos, 
					nalPacket[0].i_payload, nalPacket[0].bytes_startcode, g_iOrgTotalsize);
            }
            //SUBMCD_DEBUG_P(LOG_ERROR, "SFlvParser::write_h264_frame FLVPARSERFrameSize2 ERROR, "
            // "datalen[%u]pos[%u]len[%d][%d][%u]\n", datasize, pos, nalPacket[0].i_payload, iPreFlag, uiRoomid);
        }
        else
        {
            if(uiRecordRoom == uiRoomid) {

	            diff_sps_printf(nalPacket[0].p_payload + nalPacket[0].bytes_startcode, 
                               nalPacket[0].i_payload - nalPacket[0].bytes_startcode);
            }

            x264_put_be32(c, nalPacket[0].i_payload - nalPacket[0].bytes_startcode);

            if(flv_append_data( c, nalPacket[0].p_payload + nalPacket[0].bytes_startcode, 
                      nalPacket[0].i_payload - nalPacket[0].bytes_startcode) < 0)
            {
                printf_buf_data(buffer, datasize, ucKeyFrame, uiSeq, pos);
                SUBMCD_DEBUG_P(LOG_ERROR, "SFlvParser::write_h264_frame, size=%u,roomid=%u\n", 
                                          nalPacket[0].i_payload, uiRoomid);
                return -1;
            }
        }
        pass = get_nal_buffer(buffer,datasize, &nalPacket[1], pos, ucKeyFrame);
        if((-1 == pass)||(nalPacket[1].i_payload <= nalPacket[1].bytes_startcode))
        {
            //printf_buf_data(buffer, datasize, ucKeyFrame, uiSeq, pos);
            //SUBMCD_DEBUG_P(LOG_ERROR, "SFlvParser::write_h264_frame FLVPARSERFrameSize3 ERROR, datalen[%u]pos[%u]len[%d][%d][%u]\n", datasize, pos, nalPacket[1].i_payload, iPreFlag, uiRoomid);
            if(uiRecordRoom == uiRoomid)
	     {
                SUBMCD_DEBUG_P(LOG_ERROR, "FLVPARSER Length invalid FrameSize3 ERROR, "
                                        "datalen[%u]pos[%u]len[%d][%d][%u]\n", datasize, pos, 
					nalPacket[1].i_payload, nalPacket[1].bytes_startcode, g_iOrgTotalsize);
	     }
        }
        else
        {
            if(uiRecordRoom == uiRoomid && nalPacket[1].i_type == NAL_SPS)
            {
	            diff_sps_printf(nalPacket[1].p_payload + nalPacket[1].bytes_startcode, 
                                     nalPacket[1].i_payload - nalPacket[1].bytes_startcode);
            }
            x264_put_be32(c, nalPacket[1].i_payload - nalPacket[1].bytes_startcode);
            if(flv_append_data( c, nalPacket[1].p_payload + nalPacket[1].bytes_startcode, 
                                  nalPacket[1].i_payload - nalPacket[1].bytes_startcode) < 0)
            {
                printf_buf_data(buffer, datasize, ucKeyFrame, uiSeq, pos);
                SUBMCD_DEBUG_P(LOG_ERROR, "SFlvParser::write_h264_frame "
                     "FLVPARSERFrameSize3 [%u]ERROR[%u]\n ", nalPacket[1].i_payload, uiRoomid);
                return -1;
            }
        }
        while(pass == 0)
        {
            pass = get_nal_buffer(buffer,datasize, &nalPacket[0], pos, ucKeyFrame);   
            if((-1 == pass)||(nalPacket[0].i_payload <= nalPacket[0].bytes_startcode))
            {
                //printf_buf_data(buffer, datasize, ucKeyFrame, uiSeq, pos);
                //SUBMCD_DEBUG_P(LOG_ERROR, "SFlvParser::write_h264_frame FLVPARSERFrameSize4 ERROR, "
                //"datalen[%u]pos[%u]len[%d][%d][%u]\n", datasize, pos, nalPacket[0].i_payload, iPreFlag, uiRoomid);

                if(uiRecordRoom == uiRoomid)
		  {
	             SUBMCD_DEBUG_P(LOG_ERROR, "FLVPARSER Length invalid FrameSize4 ERROR, "
                                           "datalen[%u]pos[%u]len[%d][%d][%u]\n", datasize, pos, 
				nalPacket[0].i_payload, nalPacket[0].bytes_startcode, g_iOrgTotalsize);
		  }
            }
            else
            {
                if(uiRecordRoom == uiRoomid) {
		            diff_sps_printf(nalPacket[0].p_payload + nalPacket[0].bytes_startcode, 
                                             nalPacket[0].i_payload - nalPacket[0].bytes_startcode);
	        }

                x264_put_be32(c, nalPacket[0].i_payload - nalPacket[0].bytes_startcode);

                if(flv_append_data( c, nalPacket[0].p_payload + nalPacket[0].bytes_startcode, 
                             nalPacket[0].i_payload - nalPacket[0].bytes_startcode) < 0)
                {
                    printf_buf_data(buffer, datasize, ucKeyFrame, uiSeq, pos);
                    SUBMCD_DEBUG_P(LOG_ERROR, "SFlvParser::write_h264_frame "
                              "FLVPARSERFrameSize4 [%u]ERROR[%u]\n ", nalPacket[0].i_payload, uiRoomid);
                    return -1;
                }
            }
        }
    }
    else
    {
        pass = get_nal_buffer(buffer,datasize, &nalPacket[0], pos, ucKeyFrame);
        if((-1 == pass)||(nalPacket[0].i_payload <= nalPacket[0].bytes_startcode))
        {
            //printf_buf_data(buffer, datasize, ucKeyFrame, uiSeq, pos);
            //SUBMCD_DEBUG_P(LOG_ERROR, "SFlvParser::write_h264_frame FLVPARSERFrameSize5 ERROR, "
            //"datalen[%u]pos[%u]len[%d][%d][%u]\n", datasize, pos, nalPacket[0].i_payload, iPreFlag, uiRoomid);
            if(uiRecordRoom == uiRoomid)
  	     {
	             SUBMCD_DEBUG_P(LOG_ERROR, "FLVPARSER Length invalid FrameSize5 ERROR, "
                                         "datalen[%u]pos[%u]len[%d][%d][%u]\n", datasize, pos, 
				nalPacket[0].i_payload, nalPacket[0].bytes_startcode, g_iOrgTotalsize);
	     }
        }
        else
        {
            if(uiRecordRoom == uiRoomid)
	    {
	        diff_sps_printf(nalPacket[0].p_payload + nalPacket[0].bytes_startcode, 
                     nalPacket[0].i_payload - nalPacket[0].bytes_startcode);
	    }

            x264_put_be32(c, nalPacket[0].i_payload - nalPacket[0].bytes_startcode);

            if(flv_append_data( c, nalPacket[0].p_payload + nalPacket[0].bytes_startcode, 
                   nalPacket[0].i_payload - nalPacket[0].bytes_startcode) < 0)
            {
                printf_buf_data(buffer, datasize, ucKeyFrame, uiSeq, pos);
                SUBMCD_DEBUG_P(LOG_ERROR, "SFlvParser::write_h264_frame "
                   "FLVPARSERFrameSize5 [%u]ERROR[%u]\n ", nalPacket[0].i_payload, uiRoomid);
                return -1;
            }
        }
        while(pass == 0)
        {
            pass = get_nal_buffer(buffer,datasize, &nalPacket[0], pos, ucKeyFrame);    
            if((-1 == pass)||(nalPacket[0].i_payload <= nalPacket[0].bytes_startcode))
            {
                //printf_buf_data(buffer, datasize, ucKeyFrame, uiSeq, pos);
                //SUBMCD_DEBUG_P(LOG_ERROR, "SFlvParser::write_h264_frame FLVPARSERFrameSize6 ERROR, "
                //"datalen[%u]pos[%u]len[%d][%d[%u]]\n", datasize, pos, nalPacket[0].i_payload, iPreFlag, uiRoomid);
                if(uiRecordRoom == uiRoomid)
		{
	             SUBMCD_DEBUG_P(LOG_ERROR, "FLVPARSER Length invalid FrameSize6 ERROR, "
                                          "datalen[%u]pos[%u]len[%d][%d][%u]\n", datasize, pos, 
				nalPacket[0].i_payload, nalPacket[0].bytes_startcode, g_iOrgTotalsize);
		}    
            }
            else
            {
                if(uiRecordRoom == uiRoomid) {

		      diff_sps_printf(nalPacket[0].p_payload + nalPacket[0].bytes_startcode, 
                                          nalPacket[0].i_payload - nalPacket[0].bytes_startcode);
	        }

                x264_put_be32(c, nalPacket[0].i_payload - nalPacket[0].bytes_startcode);

                if(flv_append_data( c, nalPacket[0].p_payload + nalPacket[0].bytes_startcode, 
                              nalPacket[0].i_payload - nalPacket[0].bytes_startcode) < 0)
                {
                    printf_buf_data(buffer, datasize, ucKeyFrame, uiSeq, pos);
                    SUBMCD_DEBUG_P(LOG_ERROR, "SFlvParser::write_h264_frame "
                         "FLVPARSERFrameSize6 [%u]ERROR[%u]\n ", nalPacket[0].i_payload, uiRoomid);
                    return -1;
                }
            }
        }
    }

    if( handle->sei )
    {
        flv_append_data( c, handle->sei, handle->sei_len );
        free( handle->sei );
        handle->sei = NULL;
    }
    
    unsigned length = c->d_cur - handle->start;
    if(handle->start < 10)
    {
        printf_buf_data(buffer, datasize, ucKeyFrame, uiSeq, pos);
        SUBMCD_DEBUG_P(LOG_ERROR, "SFlvParser::write_h264_frame "
              "FLVPARSERFrameSizeERROR10[%u][%u]\n", handle->start, uiRoomid);
        handle->start = 10;
    }

    SUBMCD_DEBUG_P(LOG_TRACE,"hls video handler timestamp=%u,dts=%u\n",m_uiCurTimeStamp,s->dts);

    //hls_video_handler(s,ucKeyFrame,(char*)c->data+16,c->d_cur-16,m_uiCurTimeStamp);
    hls_video_handler(s,ucKeyFrame,(char*)c->data+16,c->d_cur-16,m_uiCurOffset);

    rewrite_amf_be24( c, length, handle->start - 10 );
    x264_put_be32( c, 11 + length ); // Last tag size
    handle->i_framenum++;

    SUBMCD_DEBUG_P(LOG_TRACE, "SFlvParser::write_h264_frame FLVPARSERFrameSize "
                       "MidLength P%uP tagtimeP%dP curtime P%uP lasttime P%uP  [%d] seq[%u] offset[%u]\n", 
                       length, offset, time_stamp, m_uiPreTimeStamp,(int)(time_stamp - m_uiPreTimeStamp), 
                       uiSeq, m_uiCurOffset);

    m_uiPreTimeStamp = m_uiCurTimeStamp;
    SUBMCD_DEBUG_P(LOG_TRACE,"SFlvParser::write_h264_frame "
                            "FLVPARSERVideoFrameTimeLast: %u\n", m_uiPreTimeStamp);

    return 0;
}

