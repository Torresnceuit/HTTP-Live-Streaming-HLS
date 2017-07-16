#ifndef _FORMAT_FLV_H
#define _FORMAT_FLV_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef unsigned long long int uint64_t;

//===================================flv_bytestream=================================
/* offsets for packed values */
#define FLV_AUDIO_SAMPLESSIZE_OFFSET 1
#define FLV_AUDIO_SAMPLERATE_OFFSET  2
#define FLV_AUDIO_CODECID_OFFSET     4

#define FLV_VIDEO_FRAMETYPE_OFFSET   4

/* bitmasks to isolate specific values */
#define FLV_AUDIO_CHANNEL_MASK    0x01
#define FLV_AUDIO_SAMPLESIZE_MASK 0x02
#define FLV_AUDIO_SAMPLERATE_MASK 0x0c
#define FLV_AUDIO_CODECID_MASK    0xf0

#define FLV_VIDEO_CODECID_MASK    0x0f
#define FLV_VIDEO_FRAMETYPE_MASK  0xf0

#define AMF_END_OF_OBJECT         0x09

enum
{
	FLV_HEADER_FLAG_HASVIDEO = 1,
	FLV_HEADER_FLAG_HASAUDIO = 4,
};

enum
{
	FLV_TAG_TYPE_AUDIO = 0x08,
	FLV_TAG_TYPE_VIDEO = 0x09,
	FLV_TAG_TYPE_META  = 0x12,
};

enum
{
	FLV_MONO   = 0,
	FLV_STEREO = 1,
};

enum
{
	FLV_SAMPLESSIZE_8BIT  = 0,
	FLV_SAMPLESSIZE_16BIT = 1 << FLV_AUDIO_SAMPLESSIZE_OFFSET,
};

enum
{
	FLV_SAMPLERATE_SPECIAL = 0, /**< signifies 5512Hz and 8000Hz in the case of NELLYMOSER */
	FLV_SAMPLERATE_11025HZ = 1 << FLV_AUDIO_SAMPLERATE_OFFSET,
	FLV_SAMPLERATE_22050HZ = 2 << FLV_AUDIO_SAMPLERATE_OFFSET,
	FLV_SAMPLERATE_44100HZ = 3 << FLV_AUDIO_SAMPLERATE_OFFSET,
};

enum
{
	FLV_CODECID_MP3 = 2 << FLV_AUDIO_CODECID_OFFSET,
	FLV_CODECID_AAC = 10<< FLV_AUDIO_CODECID_OFFSET,
};

enum
{
	FLV_CODECID_H264 = 7,
};

enum
{
	FLV_FRAME_KEY   = 1 << FLV_VIDEO_FRAMETYPE_OFFSET | 7,
	FLV_FRAME_INTER = 2 << FLV_VIDEO_FRAMETYPE_OFFSET | 7,
};

typedef enum
{
	AMF_DATA_TYPE_NUMBER      = 0x00,
	AMF_DATA_TYPE_BOOL        = 0x01,
	AMF_DATA_TYPE_STRING      = 0x02,
	AMF_DATA_TYPE_OBJECT      = 0x03,
	AMF_DATA_TYPE_NULL        = 0x05,
	AMF_DATA_TYPE_UNDEFINED   = 0x06,
	AMF_DATA_TYPE_REFERENCE   = 0x07,
	AMF_DATA_TYPE_MIXEDARRAY  = 0x08,
	AMF_DATA_TYPE_OBJECT_END  = 0x09,
	AMF_DATA_TYPE_ARRAY       = 0x0a,
	AMF_DATA_TYPE_DATE        = 0x0b,
	AMF_DATA_TYPE_LONG_STRING = 0x0c,
	AMF_DATA_TYPE_UNSUPPORTED = 0x0d,
} AMFDataType;

typedef struct flv_buffer
{
	unsigned char *data;
	unsigned int d_cur;
	unsigned int d_max;
	unsigned long long int d_total;

	flv_buffer(unsigned char *pData, unsigned int uiSize)
	{
		free();
		
		if(pData == NULL || uiSize ==0 )
		{
			return;
		}

		data = pData;
		d_max	= uiSize;
	}

	~flv_buffer()
	{
		free();
	}

	//int open(unsigned char *ucChunkBuf, unsigned int uiChunkSize)
	//{
	//	if(NULL == ucChunkBuf || uiChunkSize ==0 )
	//	{
	//		return -1;
	//	}

	//	data = ucChunkBuf;
	//	d_max= uiChunkSize;

	//	return 0;
	//}

	void free()
	{
		data = 0;
		d_cur =0;
		d_max =0;
		d_total=0;
	}

} flv_buffer;

//===================================flv_parser=================================

#define CHECK(x)\
	do {\
	if( (x) < 0 )\
	return -1;\
	} while( 0 )

#ifndef _x264_nal_t
typedef struct _x264_nal_t_ex
{
	unsigned char *p_payload;
	int i_payload;
	unsigned i_type;
	int bytes_startcode;
}x264_nal_t_ex;
#endif

typedef struct flv_hnd_t
{
	flv_buffer *c;
	//CircularChunkBuffer *c;

	unsigned char *sei;
	int sei_len;

	unsigned int i_fps_num;
	unsigned int i_fps_den;
	unsigned int i_framenum;

	unsigned int i_framerate_pos;
	unsigned int i_duration_pos;
	unsigned int i_filesize_pos;
	unsigned int i_bitrate_pos;

	unsigned char b_write_length;
	long long int i_prev_dts;
	long long int i_prev_cts;
	long long int i_delay_time;
	long long int i_init_delta;
	int i_delay_frames;

	double d_timebase;
	int b_vfr_input;
	int b_dts_compress;

	unsigned start;

	flv_hnd_t(unsigned char *pData, unsigned int uiSize)
	{
		sei = 0;
		memset(this,0,sizeof(flv_hnd_t));
		c = new flv_buffer(pData,uiSize);
	}

	void flush_c()
	{
		if(c)
		{
			c->d_total += c->d_cur;

			c->d_cur = 0;
		}
	}

	~flv_hnd_t()
	{
		if(c)
		{
			delete c;

			c=0;
		}
	}

} flv_hnd_t;



typedef struct SFlvBuffer
{
public:
    SFlvBuffer(unsigned char *pData, unsigned int uiSize)
    {
        ucData          = NULL;
        uiCurSize       = 0;
        uiMaxSize       = 0;
        ullTotalSize    = 0;

        if(pData == NULL || uiSize ==0 )
        {
            return;
        }

        ucData      = pData;
        uiMaxSize   = uiSize;
    }

    ~SFlvBuffer()
    {
        
    }

    int flushBuffer()
    {
        ullTotalSize   += uiCurSize;
        uiCurSize       = 0;
        return 0;
    }

    unsigned char       *ucData;
    unsigned int        uiCurSize;
    unsigned int        uiMaxSize;
    unsigned long long  ullTotalSize;

}SFlvBuffer;
class flvaudio_bytestream
{
public:
    flvaudio_bytestream(void){};

    ~flvaudio_bytestream(void){};

    int flv_append_data( SFlvBuffer *pFlvBuffer, unsigned char *ucChunkBuf, unsigned int uiChunkSize )
    {
        if(pFlvBuffer->uiCurSize + uiChunkSize > pFlvBuffer->uiMaxSize)
        {
            return -1;
        }
        memcpy(pFlvBuffer->ucData + pFlvBuffer->uiCurSize, ucChunkBuf, uiChunkSize );
        pFlvBuffer->uiCurSize += uiChunkSize;
        return 0;
    };

    void rewrite_amf_be24( SFlvBuffer *pFlvBuffer, unsigned int uiLength, unsigned int uiStart )
    {
        *(pFlvBuffer->ucData + uiStart + 0) = uiLength >> 16;
        *(pFlvBuffer->ucData + uiStart + 1) = uiLength >> 8;
        *(pFlvBuffer->ucData + uiStart + 2) = uiLength >> 0;
    };

    void put_byte( SFlvBuffer *pFlvBuffer, unsigned char ucByte )
    {
        flv_append_data( pFlvBuffer, &ucByte, 1 );
    };

    void put_be32( SFlvBuffer *pFlvBuffer, unsigned int uiVal )
    {
        put_byte( pFlvBuffer, uiVal >> 24 );
        put_byte( pFlvBuffer, uiVal >> 16 );
        put_byte( pFlvBuffer, uiVal >> 8 );
        put_byte( pFlvBuffer, uiVal );
    };

    void put_be16( SFlvBuffer *pFlvBuffer, unsigned short usVal )
    {
        put_byte( pFlvBuffer, usVal >> 8 );
        put_byte( pFlvBuffer, usVal );
    };

    void put_be24( SFlvBuffer *pFlvBuffer, unsigned int uiVal )
    {
        put_be16( pFlvBuffer, uiVal >> 8 );
        put_byte( pFlvBuffer, uiVal );
    };

    void put_tag( SFlvBuffer *pFlvBuffer, const char *cTag )
    {
        while( *cTag )
        put_byte( pFlvBuffer, *cTag++ );
    };

    void put_amf_string( SFlvBuffer *pFlvBuffer, const char *cStr )
    {
		if(NULL == cStr)
		{
			return;
		}

        unsigned short usLen = strlen(cStr);
        put_be16( pFlvBuffer, usLen );
        flv_append_data( pFlvBuffer, ( unsigned char*)cStr, usLen );
    };

    void put_be64( SFlvBuffer *pFlvBuffer, uint64_t val )
    {
        put_be32( pFlvBuffer, val >> 32 );
        put_be32( pFlvBuffer, val );
    };

    uint64_t dbl2int( double value )
    {
        return *(uint64_t*)(&value);
    };

    void put_amf_double( SFlvBuffer *c, double d )
    {
        put_byte( c, AMF_DATA_TYPE_NUMBER );
        put_be64( c, dbl2int( d ) );
    };
};
#define FLV_BUFSIZE (1024 * 1024)

#endif
