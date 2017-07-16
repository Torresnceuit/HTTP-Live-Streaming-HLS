/************************************************************
  FileName:    flv_parser.h
  Author:      Arvinhu
  copyright :  Copyright@2013 tencent;
  Date:        6/10/2013
  Description: flv流合成功能的结构、类、常量定义
  History:     // 历史修改记录
      <author>   <time>        <version >   <desc>
      Arvinhu    2013/10/26     1.0          build this moudle
      yuandongtu 201/12/31    1.2         解决flv播放黑屏bug
***********************************************************/
#ifndef _FORMAT_FLV_PARSER_H
#define _FORMAT_FLV_PARSER_H

#include "flv_bytestream.h"
#include "muxsvr.h"

#include "hls/hls_stream.h"

using namespace muxsvr;

const int UI_FLV_HEADER_SIZE = 8192;
const int FLV_PRINT_MAX_SIZE = 81920;
const int NUM_CHAR_HEX_LINE  = 16; 

enum AVPictureType {
    AV_PICTURE_TYPE_NONE = 0, ///< Undefined
    AV_PICTURE_TYPE_I,     ///< Intra
    AV_PICTURE_TYPE_P,     ///< Predicted
    AV_PICTURE_TYPE_B,     ///< Bi-dir predicted
    AV_PICTURE_TYPE_S,     ///< S(GMC)-VOP MPEG4
    AV_PICTURE_TYPE_SI,    ///< Switching Intra
    AV_PICTURE_TYPE_SP,    ///< Switching Predicted
    AV_PICTURE_TYPE_BI,    ///< BI type
};

/* NAL unit types */
enum {
    NAL_SLICE = 1,
    NAL_DPA,
    NAL_DPB,
    NAL_DPC,
    NAL_IDR_SLICE,
    NAL_SEI,
    NAL_SPS,
    NAL_PPS,
    NAL_AUD,
    NAL_END_SEQUENCE,
    NAL_END_STREAM,
    NAL_FILLER_DATA,
    NAL_SPS_EXT,
    NAL_AUXILIARY_SLICE = 19,
    NAL_FF_IGNORE       = 0xff0f001,
};

void 
flv_hexdump(const char* p,int len, const char* type);

#pragma pack(1)
typedef struct
{
    enum
    {
        TYPE_AAC   = 0x07,
        TYPE_OPUS  = 0x05,
    };

    unsigned char   ucAudioType;
    unsigned short  usAudioHead;
    unsigned short  usTimeStamp;
    unsigned short  usAACFrameLen;
}SQT_AUDIO_HEAD;

typedef struct SFlvParser : public flv_bytestream
{
public:
    SFlvParser()
    {
	b_sps_pps_init		= false;
	bInit			= false;
	m_pflv			= NULL;

	memset(&m_oflvTag, 0, sizeof(tagFLVHEAD));

        m_uiPreTimeStamp            = 0;        //上一次时间戳
        m_uiCurTimeStamp            = 0;        //当前时间戳
        m_uiCurOffset               = 0;
        m_uiLastOffset              = 0;
        g_uiAudioBeginTime          = 0;        //当前时间戳
        g_llBeginAudioTimeStamp     = 0;
        m_uiCurAudioTimeStamp       = 0;
        m_uiPreAudioTimeStamp       = 0;
        m_uiCurAudioOffset          = 0;
        m_uiBeginVideoTime          = 0;
        m_uiLastVideoSeq            = 0;
	m_uiLastAudioSeq            = 0;

	m_nWidth                    = 0;
	m_nHeight                   = 0;
	m_nFps                      = 0;
	m_nBitrate                  = 0;

	m_nFpsInit  		    = 0;

	m_dwLastFrameTime           = 0;
	m_dwLastFrameTimeFor1Second = 0;
	m_dwFrameTick               = 0;
	m_nFrameCountForCalcFpsAgv  = 0;

	memset(m_buffer, 0, G_UIFLVCIRCULARSIZE);

        m_strPrintBuf         = new char[FLV_PRINT_MAX_SIZE];
    }

    int open_flv(int nWidth, int nHeight, int nBitrate, int nFps)
    {
        m_nWidth = nWidth;
        m_nHeight = nHeight;
        m_nFps = nFps;
        m_nBitrate = nBitrate;

        m_pflv =new flv_hnd_t(m_buffer,G_UIFLVCIRCULARSIZE);
        return 0;
    }

    void close_flv()
    {
        if(m_pflv)
        {
            delete m_pflv;
            m_pflv = 0;
        }
    }

    void dumpflv()
    {
        if ( m_pflv && m_pflv->c ) {
           flv_hexdump((const char*)m_pflv->c->data,m_pflv->c->d_cur,"flv");
        }
    }

    int parser_header(hls_session_t *s, unsigned char *buf,unsigned int size, FILE* file, 
                       unsigned int &uiCurLen, unsigned char ucKeyFrame,unsigned char* adtsheader);
    int parser_body_for_video(hls_session_t *s,unsigned char *pucBuf,int iSize, unsigned int &uiCurLen, unsigned char ucKeyFrame, unsigned int 
    uiSeq, unsigned int uiRoomid, unsigned int uiRecordRoom);
    int parser_body_for_audio(hls_session_t *s,unsigned char *pucBuf,int iSize, unsigned int &uiCurLen, unsigned int uiSeq, unsigned int uiRoomId);
    int get_nal_buffer(unsigned char* buffer,unsigned int datasize,x264_nal_t_ex *nalPacket,unsigned int& prepos, unsigned char ucKeyFrame);

    int write_header(flv_buffer *c);
    int set_script_param( flv_hnd_t *handle, double i_width, double i_height);

    int write_h264_headers(hls_session_t *s, flv_hnd_t *handle, x264_nal_t_ex *p_nal );
    int write_aac_headers(hls_session_t *s, flv_hnd_t *handle,unsigned char objecttype, 
                         unsigned char samplerate_index, unsigned char channels);

    int write_h264_frame(hls_session_t *s,flv_hnd_t *handle, unsigned char* buffer,unsigned int datasize,
                   int time_stamp, unsigned char ucKeyFrame,unsigned int uiSeq, unsigned int uiRoomid, unsigned int uiRecordRoom);

    int printf_buf_data(unsigned char *pucBuf,int iSize, unsigned char ucKeyFrame, unsigned int uiSeq, int iPos);
    int diff_sps_printf(unsigned char *pStr, unsigned int uiSize);
    int get_slice_type(unsigned char* buffer,unsigned int datasize);
    int parser_adts(const unsigned char* adtsheader, int len, unsigned char *objecttype, unsigned char *samplerate_index, unsigned char *channels);

    int getFlvOffset()
    {
        if(NULL == m_pflv)
        {
            return 0;
        }

        return m_pflv->c->d_cur;
    }

    unsigned char *getBuffer()
    {
        return m_buffer;
    }

    unsigned char *getTagBuffer()
    {
	return (unsigned char*)&m_oflvTag;
    }

    void flush()
    {
        flv_flush_data(m_pflv->c);
    }

    bool b_sps_pps_init;
    bool bInit;
    unsigned char m_buf[4*1024];

public:
    flv_hnd_t* m_pflv;

public:
    unsigned int m_uiPreTimeStamp;        //上一次时间戳
    unsigned int m_uiCurTimeStamp;        //当前时间戳
    unsigned int m_uiLastFramePTS;
    unsigned int m_uiCurOffset;
    unsigned int m_uiLastOffset;
    unsigned int g_uiAudioBeginTime;        //当前时间戳
    unsigned int g_llBeginAudioTimeStamp;
    unsigned int m_uiCurAudioTimeStamp;
    unsigned int m_uiPreAudioTimeStamp;
    unsigned int m_uiCurAudioOffset;
    unsigned int m_uiBeginVideoTime;
    unsigned int m_uiLastVideoSeq;
    unsigned int m_uiLastAudioSeq;	

    tagFLVHEAD   m_oflvTag;
    unsigned char   m_buffer[G_UIFLVCIRCULARSIZE];

    char         *m_strPrintBuf;
private:
    int m_nWidth;
    int m_nHeight;
    int m_nFps;
    int m_nBitrate;

    int m_nFpsInit;
    //这个变量用于记录上一帧时间
    unsigned int m_dwLastFrameTime;
    unsigned int m_dwLastFrameTimeFor1Second;
    unsigned int m_dwFrameTick;
    int m_nFrameCountForCalcFpsAgv;
}SFlvParser;

#pragma pack()

#endif

