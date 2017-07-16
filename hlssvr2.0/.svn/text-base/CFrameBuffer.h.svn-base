#ifndef _CFRAMEBUFFER_H
#define _CFRAMEBUFFER_H

#include <string>
#include <sstream>

#include "CHash.h"

#include "muxsvr.h"

using std::string;
using std::stringstream;
using namespace muxsvr;

class CFrameBuffer
{
    enum
    {
        MAX_VIDEO_JUMP      = 100,
        MAX_VIDEO_TIEMOUT   = 800,//ms
    };

public:
    CFrameBuffer()
		:m_uiFrameDataStartIndex(0)
		,m_uiWriteSeq(0)
		,m_uiReadSeq(0)
		,m_bNeedReset(false)
		,m_uiReceiveFrameNum(0)
		,m_uiCompleteFrameNum(0)
		,m_uiOverTimeFrameNum(0)
		,m_uiCircleLen(0)
		,m_uiMaxCircleLen(0)
		,m_uiMinCircleLen(0)
		,m_uiMaxFrameSize(0)
    {
		memset(m_pFrameDataBuff,0,G_UIFRAMEDATABUFFSZIE);	
    }

    int push_back(const SUserInfo &oUserInfo, const SFrameVideo &oFrameVideo, unsigned long long ullTime);

    int get_frame_buffer(
        const SFrameVideoItem **ppFrameVideoItemBuffer, 
        unsigned int *puiFrameVideoItemBufferSize, 
        const char **pFrameDataBuff, 
        unsigned int *puiFrameDataBuffSize);

	int get_frame_buffer_in_order(
		const SFrameVideoItem **ppFrameVideoItemBuffer, 
		const char **pFrameDataBuff,
		unsigned long long ullTime);

    string to_string(const string &tab) const 
    {
        stringstream ss;
        ss << tab << "m_uiReceiveFrameNum: " << m_uiReceiveFrameNum << "\n"
            << tab << "m_uiCompleteFrameNum: " << m_uiCompleteFrameNum << "\n"
            << tab << "m_uiOverTimeFrameNum: " << m_uiOverTimeFrameNum << "\n"
            << tab << "m_uiCircleLen: " << m_uiCircleLen << "\n"
            << tab << "m_uiMaxCircleLen: " << m_uiMaxCircleLen << "\n"
            << tab << "m_uiMinCircleLen: " << m_uiMinCircleLen << "\n"
            << tab << "m_uiMaxFrameSize: " << m_uiMaxFrameSize
            ;
        return ss.str();
    };

    unsigned int getReceiveFrameNum() const
    {
        return m_uiReceiveFrameNum;
    }

    unsigned int getCompleteFrameNum() const
    {
        return m_uiCompleteFrameNum;
    }

    unsigned int getOverTimeFrameNum() const
    {
        return m_uiOverTimeFrameNum;
    }

    unsigned int getCircleLen() const
    {
        return m_uiCircleLen;
    }

    unsigned int getMaxCircleLen() const
    {
        return m_uiMaxCircleLen;
    }

    unsigned int getMinCircleLen() const
    {
        return m_uiMinCircleLen;
    }

    unsigned int getMaxFrameSize() const
    {
        return m_uiMaxFrameSize;
    }

	unsigned int getWriteSeq() const
	{
		return m_uiWriteSeq;
	}

	unsigned int getReadSeq() const
	{
		return m_uiReadSeq;
	}

	void clearData()
	{
		m_uiFrameDataStartIndex = 0;
		m_uiWriteSeq			= 0;
		m_uiReadSeq				= 0;

		m_uiReceiveFrameNum		= 0;
		m_uiCompleteFrameNum	= 0;
		m_uiOverTimeFrameNum	= 0;
		m_uiCircleLen			= 0;
		m_uiMaxCircleLen		= 0;
		m_uiMinCircleLen		= 0;
		m_uiMaxFrameSize		= 0;

		m_bNeedReset			= true;

		memset(m_pFrameDataBuff, 0 ,G_UIFRAMEDATABUFFSZIE);
	}

	bool needResetFlag() const
	{
		return m_bNeedReset;
	}

	void flipNeedReset()
	{
		if(m_bNeedReset)
		{
			m_bNeedReset = false;
		}
	}

	unsigned int getRWDistance() const
	{
		unsigned int uiDistance  = 0;

		if(m_uiWriteSeq > m_uiReadSeq)
		{
			uiDistance = m_uiWriteSeq - m_uiReadSeq;
		}

		return uiDistance;
	}

	float getAverageRWDistance() const
	{
		float fAverageDis = 0.0;

		if(m_uiCurDisIndex > MAX_DIS_ARRAY_NUM)
		{
			for(unsigned int i=0; i<MAX_DIS_ARRAY_NUM; i++)
			{
				fAverageDis += (float)m_uiDisArray[i];
			}

			fAverageDis = fAverageDis / MAX_DIS_ARRAY_NUM;
		}

		return fAverageDis;
	}

private:
    //保存视频帧的索引信息，最多保存FRAME_ITEM_NUM帧
    CHash<SFrameVideoItem, FRAME_ITEM_NUM> m_oFrameVideoItemHash;

private:
    //m_pFrameDataBuff写入指针
    unsigned int m_uiFrameDataStartIndex;

    unsigned int m_uiWriteSeq;
    unsigned int m_uiReadSeq;

	bool m_bNeedReset;

    //保存视频帧数据
    char m_pFrameDataBuff[G_UIFRAMEDATABUFFSZIE];

	unsigned int m_uiCurDisIndex;
	unsigned int m_uiDisArray[MAX_DIS_ARRAY_NUM];

private:
    //存视频帧统计信息
    unsigned int m_uiReceiveFrameNum;
    unsigned int m_uiCompleteFrameNum;
    unsigned int m_uiOverTimeFrameNum;

    unsigned int m_uiCircleLen;		//m_pFrameDataBuff有效数据长度
    unsigned int m_uiMaxCircleLen;
    unsigned int m_uiMinCircleLen;
    unsigned int m_uiMaxFrameSize;	//保存收到的最大帧长
};

#endif
