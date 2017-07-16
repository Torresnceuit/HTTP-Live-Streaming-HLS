#ifndef _H264_TO_TS_H
#define _H264_TO_TS_H

#include <string.h>
#include <string>
#include <sstream>

#include "tstype.h"
#include "../comm/tfc_debug_log.h"
#include "../pub/submcd_log.h"
#include "../muxsvr.h"

using namespace std;
using namespace muxsvr;

const unsigned int	TS_CHUNK_NUM = 5;
const unsigned int	TS_FILE_DURATION = 6;

typedef struct SM3u8Buffer
{
    unsigned int	m_uiIndex;
    unsigned int	m_uiStartTime;
    unsigned int	m_uiEndTime;
    unsigned char   m_ucVirgin;

    unsigned char*	m_ucMemBase;
	unsigned char*  m_ucMemTs;
    unsigned int	m_uiMemLen;

    unsigned int	m_uiWriteOffset;

    unsigned char   m_ucFlag;

    SM3u8Buffer()
    {
        memset(this, 0, sizeof(SM3u8Buffer));
    }

    int open(unsigned char* ucBuf, unsigned int uiBufLen,unsigned int uiIndex)
    {
        if(NULL == ucBuf || 0 == uiBufLen)
        {
            return -1;
        }

        m_ucMemBase		= ucBuf;
		m_ucMemTs		= m_ucMemBase + sizeof(SCdnM3u8Header);

        m_uiMemLen		= uiBufLen - sizeof(SCdnM3u8Header);
        m_uiIndex		= uiIndex;

        return 0;
    }

    int writeBuf(unsigned char *ucBuf, unsigned int uiBufLen, unsigned int uiTime)
    {
        if(NULL == ucBuf || 0 == uiBufLen || uiBufLen > m_uiMemLen)
        {
        	SUBMCD_DEBUG_P(LOG_ERROR,"CProtol::264tots writeBuf ucBuf is null:%d uiBufLen %u m_uiMemLen %u\n",
        			NULL == ucBuf,
        			uiBufLen,
        			m_uiMemLen);
            return -1;
        }

        //超过x秒钟,重置为0
        if( uiTime - m_uiEndTime > TS_FILE_DURATION)
        {
            m_uiWriteOffset = 0;
            m_uiStartTime	= 0;
            m_uiEndTime		= 0;
            m_ucVirgin      = 0;
			m_ucFlag		= 0;

            SUBMCD_DEBUG_P(LOG_TRACE,"CProtol::264tots reset buff index: %u, writeOffset: %u, uiBufLen: %u, maxMemLen: %u, uiTimeLag: %u\n",
                m_uiIndex,
                m_uiWriteOffset,
                uiBufLen,
                m_uiMemLen,
                uiTime - m_uiStartTime);
        }

        //在x秒内,数据长度大于最大可用长度,需要调大内存
        if(m_uiWriteOffset + uiBufLen > m_uiMemLen)
        {		
            SUBMCD_DEBUG_P(LOG_ERROR,"CProtol::264tots corrput buff index: %u, writeOffset: %u, uiBufLen: %u, maxMemLen: %u, uiTimeLag: %u\n",
                m_uiIndex,
                m_uiWriteOffset,
                uiBufLen,
                m_uiMemLen,
                uiTime - m_uiStartTime);

            m_uiWriteOffset = 0;
            m_uiStartTime	= 0;
            m_uiEndTime		= 0;
            m_ucVirgin      = 0;
			m_ucFlag		= 0;
        }

        //记录起始时间
        if(0 == m_uiStartTime)
        {
            m_uiStartTime = uiTime;
        }

        //更新结束时间
        m_uiEndTime	= uiTime;

        SUBMCD_DEBUG_P(LOG_TRACE,"CProtol::264tots write buff index: %u, writeOffset: %u, uiBufLen: %u, maxMemLen: %u, uiTimeLag: %u\n",
            m_uiIndex,
            m_uiWriteOffset,
            uiBufLen,
            m_uiMemLen,
            uiTime - m_uiStartTime);

        //拷贝数据
        memcpy(m_ucMemTs + m_uiWriteOffset, ucBuf, uiBufLen);

        //写指针偏移
        m_uiWriteOffset += uiBufLen;

        return 0;
    }

}SM3u8Buffer;

typedef struct SM3u8Info
{
    unsigned int	m_uiTimeBase;
    unsigned int	m_uiTimeCurr;
    unsigned int    m_uiRequestTimes;   //请求次数
    unsigned int    m_uiBaseTsSeq;      //ts起始序号

    unsigned char*  m_ucMemBase;        
    unsigned int    m_uiMaxMemLen;      //内存buf总长度
    unsigned int    m_uiChunkSize;      //一块buf长度

    unsigned int    m_uiLastIndex;

    SM3u8Buffer		m3u8BufArray[TS_CHUNK_NUM];

    unsigned int    m_uiVideoId;        //videoid

    SM3u8Info()
    {
        memset(this, 0 ,sizeof(SM3u8Info));
    }

    int open(unsigned char *ucMemBase, unsigned int uiLen)
    {
        if(NULL == ucMemBase || 0 == uiLen)
        {
            return -1;
        }

        m_ucMemBase     = ucMemBase;
        m_uiMaxMemLen   = uiLen;

        m_uiChunkSize   = m_uiMaxMemLen / TS_CHUNK_NUM;

        for(unsigned int i = 0; i < TS_CHUNK_NUM; i++)
        {
            m3u8BufArray[i].open(m_ucMemBase + i * m_uiChunkSize, m_uiChunkSize,i);
        }

        return 0;
    }

    int writeM3u8(unsigned char *ucBuf, unsigned int uiBufLen, unsigned int uiTime, unsigned int uiVideoId)
    {
        if(NULL == ucBuf || 0 == uiBufLen || uiBufLen > m_uiMaxMemLen)
        {
            return -1;
        }

        if(0 == m_uiTimeBase)
        {
            m_uiTimeBase = uiTime;
        }

        m_uiTimeCurr	= uiTime;

        m_uiVideoId		= uiVideoId;

        //按时间来划分文件
        unsigned int uiIndex = ((uiTime - m_uiTimeBase) / TS_FILE_DURATION) % TS_CHUNK_NUM;

        return m3u8BufArray[uiIndex].writeBuf(ucBuf,uiBufLen,uiTime);
    }

    //获取上一个10s tsbuff
    unsigned char* getPreTsChunkBuf(unsigned int uiTime, unsigned int &uiTsSize, unsigned int &uiRealIndex)
    {
        unsigned char *ucRet = NULL;

        uiTsSize = 0;

		uiRealIndex = (uiTime - m_uiTimeBase) / TS_FILE_DURATION;

		unsigned int uiTsIndex = uiRealIndex % TS_CHUNK_NUM;

        //上一个index
        unsigned int uiPreTsIndex =  (uiTsIndex -1 + TS_CHUNK_NUM) % TS_CHUNK_NUM; 

        SM3u8Buffer &o_temp_m3u8 = m3u8BufArray[uiPreTsIndex];

        //第一次被读取而且非空
        if(0 == o_temp_m3u8.m_ucFlag && 0 != o_temp_m3u8.m_uiWriteOffset)
        {
            o_temp_m3u8.m_ucFlag = 1;

			//填充cdn推送头
			{
				SCdnM3u8Header* o_tmp_m3u8_header = (SCdnM3u8Header*)o_temp_m3u8.m_ucMemBase;

				//推cdn之前把头信息写好  0017 0300 0000 06
				o_tmp_m3u8_header->uiSize		= o_temp_m3u8.m_uiWriteOffset;
				o_tmp_m3u8_header->usHeaderSize = sizeof(SCdnM3u8Header);
				o_tmp_m3u8_header->ucType		= 0x03;
				o_tmp_m3u8_header->uiDuration	= TS_FILE_DURATION;
				o_tmp_m3u8_header->uiIndex		= uiRealIndex;
				o_tmp_m3u8_header->ullTimeStamp	= (unsigned long long)o_temp_m3u8.m_uiStartTime;	
			}

            uiTsSize = o_temp_m3u8.m_uiWriteOffset;

			//从头推
            ucRet = o_temp_m3u8.m_ucMemBase;
            FILE *FOoutPiceTs;
            char path[256];
            memset(path, 0, 256);
            sprintf(path,"../log/video_%u.ts", o_temp_m3u8.m_uiStartTime);

            FOoutPiceTs = fopen(path,"wb");
            if(FOoutPiceTs != NULL)
            {
            	fwrite(ucRet + sizeof(SCdnM3u8Header), 1, uiTsSize - sizeof(SCdnM3u8Header),FOoutPiceTs);
            	fclose(FOoutPiceTs);
            }

        }
        else
        {
        	SUBMCD_DEBUG_P(LOG_TRACE,"CProtol::getPreTsChunkBuf m_ucFlag %u m_uiWriteOffset %u uiPreTsIndex %u\n", o_temp_m3u8.m_ucFlag, o_temp_m3u8.m_uiWriteOffset, uiPreTsIndex);
        }

        return ucRet;
    }

    string to_m3u8_test()
    {
        if(0 == m_uiTimeBase)
        {
            return string("BUFFERING...");
        }

        unsigned int uiTsBaseSeq = (m_uiTimeCurr - m_uiTimeBase) / TS_FILE_DURATION;

        stringstream tmp;
        tmp << "#EXTM3U\r\n";
        tmp << "#EXT-X-TARGETDURATION:" << 2 << "\r\n";
        tmp << "#EXT-X-MEDIA-SEQUENCE:" << uiTsBaseSeq << "\r\n";

        unsigned int i = 0;

        for(i = 0; i < TS_CHUNK_NUM; i++)
        {
            if(0 == m3u8BufArray[i].m_uiStartTime 
                || m3u8BufArray[i].m_uiEndTime - m3u8BufArray[i].m_uiStartTime < TS_FILE_DURATION -1)
            {
                break;
            }

            tmp << "#EXTINF:2,\r\n";
            tmp << m_uiVideoId << ".flv&src=apple_" << uiTsBaseSeq + i << ".ts\r\n";
        }

        unsigned int tmpSize = tmp.str().size();

        stringstream ss;
        ss << "HTTP/1.1 200 OK\r\n";
        ss << "Connection: close \r\n";
        ss << "Content-Length: " << tmpSize << "\r\n";
        ss << "Content-Type: application/x-mpegURL\r\n";
        ss << "Server: qqlive_stream\r\n\r\n";

        return ss.str() + tmp.str();
    }

    //根据自己时钟,返回可用m3u8 list
    string to_m3u8()
    {  
        if(0 == m_uiTimeBase)
        {
            return string("BUFFERING...");
        }

        unsigned int uiTsBaseSeq = (m_uiTimeCurr - m_uiTimeBase) / TS_FILE_DURATION;

        stringstream tmp;
        tmp << "#EXTM3U\r\n";
        tmp << "#EXT-X-TARGETDURATION:" << TS_FILE_DURATION << "\r\n";
        tmp << "#EXT-X-MEDIA-SEQUENCE:" << uiTsBaseSeq << "\r\n";

        unsigned int i = 0;

        for(i = 0; i < TS_CHUNK_NUM; i++)
        {
            if(0 == m3u8BufArray[i].m_uiStartTime)
            {
                break;
            }

            tmp << "#EXTINF:" << TS_FILE_DURATION << ",\r\n";
            tmp << m_uiVideoId << ".flv&src=apple_" << uiTsBaseSeq + i << ".ts\r\n";
        }

        unsigned int tmpSize = tmp.str().size();

        stringstream ss;
        ss << "HTTP/1.1 200 OK\r\n";
        ss << "Connection: close \r\n";
        ss << "Content-Length: " << tmpSize << "\r\n";
        ss << "Content-Type: application/x-mpegURL\r\n";
        ss << "Server: qqlive_stream\r\n\r\n";

        return ss.str() + tmp.str();
    }

    unsigned char* getTsChunkBuf(unsigned int uiTsSeq, unsigned int &uiTsSize)
    {
        uiTsSize = 0;
        unsigned char* ucRet = NULL;

        unsigned int uiTsBaseSeq = (m_uiTimeCurr - m_uiTimeBase) / TS_FILE_DURATION;

        //获取不在m3u8列表中的ts
        if(uiTsSeq < uiTsBaseSeq || uiTsSeq > uiTsBaseSeq + TS_CHUNK_NUM)
        {
            return NULL;
        }

        unsigned int uiIndex = uiTsSeq % TS_CHUNK_NUM;

        SM3u8Buffer &oSM3u8Buff = m3u8BufArray[uiIndex];

        //有写入数据
        if(0 != oSM3u8Buff.m_uiStartTime)
        {
            uiTsSize = oSM3u8Buff.m_uiWriteOffset;

			//从真实数据推
            ucRet	= oSM3u8Buff.m_ucMemTs;
        }

        return ucRet;
    }

}SM3u8Info;

class H264ToTs
{
	public:
		H264ToTs();
		~H264ToTs();

		int open_ts(const char *pTsOut,unsigned int uiFps);

		int open(unsigned char *ucBuf, unsigned int uiBufMaxLen, unsigned char *ucTsBuf, unsigned int uiTsBufMaxLen)
		{
			if(NULL == ucBuf || 0 == uiBufMaxLen || NULL == ucTsBuf || 0 == uiTsBufMaxLen)
			{
				return -1;
			}

			m_ucH264Buff		= ucBuf;
			m_uiH264BuffMaxLen	= uiBufMaxLen;

			m_ucTsBuff			= ucTsBuf;
			m_uiTsBuffMaxLen	= uiTsBufMaxLen/2;

			m_oSm3u8Info.open(ucTsBuf + uiTsBufMaxLen/2, uiTsBufMaxLen/2);

			return 0;
		}

	//video
        int H2642PES(unsigned char *ucTsFrameBuf, unsigned int uiTsFrameLen);
        int processNalu(unsigned char *ucBuff, unsigned int uiLen, unsigned int &uiOutTsLen, unsigned int uiTime, unsigned int uiVideoId);

	//ts
		int CreatePAT();
		int CreatePMT();
		TsPacketHeader* CreateTsHeader(TsPacketHeader *ts_header, unsigned int uiPid,unsigned char play_init,unsigned char ada_field_C);
		int TsHeader2buffer(TsPacketHeader * ts_header,unsigned char *buffer);

		int WriteAdaptive_flags_Head(Ts_Adaptation_field  * ts_adaptation_field);
		int WriteAdaptive_flags_Tail(Ts_Adaptation_field  * ts_adaptation_field);
		int CreateAdaptive_Ts(Ts_Adaptation_field * ts_adaptation_field,unsigned char * pointer_ts,unsigned int AdaptiveLength);
		int PES2TS(TsPes * ts_pes,unsigned int Video_Audio_PID,Ts_Adaptation_field * ts_adaptation_field_Head ,Ts_Adaptation_field * ts_adaptation_field_Tail);

		void clearTsBuf()
		{
			m_uiTsBuffLen = 0;
		}

        unsigned int getTsInBufSize()
        {
            return m_uiH264BuffLen;
        }

        unsigned int getTsOutBufSize()
        {
            return m_uiTsBuffLen;
        }

		string to_m3u8()
		{
			return m_oSm3u8Info.to_m3u8();
		}

		string to_m3u8_test()
		{
			return m_oSm3u8Info.to_m3u8_test();
		}

		unsigned char* getTsNumBuf(unsigned int uiTsSeq, unsigned int &uiTsSize)
		{
			return m_oSm3u8Info.getTsChunkBuf(uiTsSeq, uiTsSize);
		}

    unsigned char* getPreTsChunkBuf(unsigned int uiTime, unsigned int &uiTsSize, unsigned int &uiIndex)
    {
        return m_oSm3u8Info.getPreTsChunkBuf(uiTime, uiTsSize, uiIndex);
    }

public:
    bool b_sps_pps_init;
    SM3u8Info m_oSm3u8Info;

	private:
        NALU_t  m_nal_t;

		unsigned char	*m_ucH264Buff;					//输入h264 buf
        unsigned int	m_uiH264BuffLen;				//输入h264 len
		unsigned int    m_uiH264BuffMaxLen;				//最大h264 长

		unsigned char	*m_ucTsBuff;					//输出tsbuf
		unsigned int	m_uiTsBuffLen;					//输出ts len
		unsigned int    m_uiTsBuffMaxLen;				//输出ts 最大长
		unsigned int	m_uiTsSumLen;					//累计长度

		unsigned int m_uiFps;

		FILE *FOutVideoTs;


	//video
		unsigned long long Videopts;
		unsigned long long Videodts;
		
		unsigned int VideoFrameCount;		                     
		double	Frame_Time;

	//ts
		unsigned char TSbuf[TS_PACKET_SIZE];				
		unsigned int WritePacketNum;

		TsPes   ts_video_pes;
	
		Continuity_Counter continuity_counter;
};

#endif
