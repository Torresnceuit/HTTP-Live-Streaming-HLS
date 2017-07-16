#include "tfc_debug_log.h"

#include "submcd_log.h"

#include "CFrameBuffer.h"

#include <stdlib.h>

using namespace muxsvr;

int CFrameBuffer::push_back(const SUserInfo &oUserInfo, const SFrameVideo &oFrameVideo, unsigned long long ullTime)
{
    //帧大小异常
    if (oFrameVideo.uiFrameSize > MAX_FRAME_SZIE)
    {
	SUBMCD_DEBUG_P(LOG_ERROR,"CFrameBuffer::push_back size invalid."
                                 " uin: %u, uiRoomId: %u, uiFrameSeq: %u, uiFrameSize: %u, uiCurSize: %u\n", 
            oUserInfo.uiUin,
            oUserInfo.uiRoomId,
            oFrameVideo.uiFrameSeq, 
            oFrameVideo.uiFrameSize, 
            oFrameVideo.uiCurSize);

        return -1;
    }

	//使用uin做通道索引情况下，单用户两次直播，序号变化异常
    unsigned int uiDistance		= abs((int)(oFrameVideo.uiFrameSeq - m_uiWriteSeq));
    unsigned int uiRWDistacne	= abs((int)(m_uiWriteSeq - m_uiReadSeq));

    if(m_uiWriteSeq !=0 && (uiDistance > MAX_VIDEO_JUMP || uiRWDistacne > FRAME_ITEM_NUM))
    {
		SUBMCD_DEBUG_P(LOG_COLOR,"CFrameBuffer::push_back reset data,"
                                         " uin: %u, roomid: %u, videoid: %u, frame_seq: %u, read: %u, write: %u\n",
			oUserInfo.uiUin,
			oUserInfo.uiRoomId,
			oUserInfo.uiVideoId,
			oFrameVideo.uiFrameSeq,
			m_uiReadSeq,
			m_uiWriteSeq);

		clearData();
    }

    bool bTmpCircleFlag = false;

    SFrameVideoItem &oFrameVideoItem = m_oFrameVideoItemHash.get(oFrameVideo.uiFrameSeq);

    if (1 == oFrameVideoItem.ucHaveData)
    {
        m_uiCircleLen -= oFrameVideoItem.uiFrameSize;
        bTmpCircleFlag = true;
    }

    oFrameVideoItem.clear();

    oFrameVideoItem.uiUin           = oUserInfo.uiUin;
    oFrameVideoItem.usVer           = oUserInfo.usVer;
    oFrameVideoItem.ucHaveData      = 1;
    oFrameVideoItem.uiFrameSeq      = oFrameVideo.uiFrameSeq;
    oFrameVideoItem.ucKeyFrame      = oFrameVideo.ucKeyFrame;
    oFrameVideoItem.uiFrameSize     = oFrameVideo.uiFrameSize;
    oFrameVideoItem.uiCurSize       = oFrameVideo.uiCurSize;
    oFrameVideoItem.uiFrameTime     = oFrameVideo.uiFrameTime;
    oFrameVideoItem.uiFps           = oFrameVideo.uiFps;
    oFrameVideoItem.uiStart         = m_uiFrameDataStartIndex;
    oFrameVideoItem.ullTouchTime	= ullTime;

    m_uiFrameDataStartIndex = (m_uiFrameDataStartIndex + oFrameVideo.uiFrameSize) % G_UIFRAMEDATABUFFSZIE;

    if ((oFrameVideoItem.uiStart + oFrameVideoItem.uiFrameSize) <= G_UIFRAMEDATABUFFSZIE)
    {
        memcpy(m_pFrameDataBuff + oFrameVideoItem.uiStart, oFrameVideo.pFrameData, oFrameVideoItem.uiFrameSize);
    }
    else
    {
        //?????????,????????????
        unsigned int uiBuffTailLen = G_UIFRAMEDATABUFFSZIE - oFrameVideoItem.uiStart;
        memcpy(m_pFrameDataBuff + oFrameVideoItem.uiStart, oFrameVideo.pFrameData, uiBuffTailLen);
        memcpy(m_pFrameDataBuff, oFrameVideo.pFrameData + uiBuffTailLen, oFrameVideoItem.uiFrameSize - uiBuffTailLen);
    }

	m_uiWriteSeq	= oFrameVideo.uiFrameSeq;

	m_uiDisArray[m_uiCurDisIndex++ % MAX_DIS_ARRAY_NUM] = getRWDistance();

	SUBMCD_DEBUG_P(LOG_TRACE,"CPorotl::CFrameBuffer::push_back frame,"
                                 " uin: %u, uiRoomId: %u, frame_seq: %u, uiFrameSize: %u, uiStart: %u\n", 
        oFrameVideoItem.uiUin,
		oUserInfo.uiRoomId,
        oFrameVideoItem.uiFrameSeq, 
        oFrameVideoItem.uiFrameSize, 
        oFrameVideoItem.uiStart);

    //===========================================================================================================================
    //统计
    if (oFrameVideo.uiFrameSize > m_uiMaxFrameSize)
    {
        m_uiMaxFrameSize = oFrameVideo.uiFrameSize;
    }

    m_uiCircleLen += oFrameVideoItem.uiFrameSize;

    if (bTmpCircleFlag == true && m_uiCircleLen > m_uiMaxCircleLen)
    {
        m_uiMaxCircleLen = m_uiCircleLen;
    }

    if (bTmpCircleFlag == true && (m_uiCircleLen < m_uiMinCircleLen || m_uiMinCircleLen == 0))
    {
        m_uiMinCircleLen = m_uiCircleLen;
    }

    m_uiReceiveFrameNum += 1;

    bool bIsComplete = (oFrameVideoItem.uiCurSize == oFrameVideoItem.uiFrameSize) ? true : false;

    if (true == bIsComplete)
    {
        m_uiCompleteFrameNum += 1;
    }
    else
    {
        m_uiOverTimeFrameNum += 1;
    }

    return 0;
}

int CFrameBuffer::get_frame_buffer(
    const SFrameVideoItem **ppFrameVideoItemBuffer, 
    unsigned int *puiFrameVideoItemBufferSize, 
    const char **pFrameDataBuff, 
    unsigned int *puiFrameDataBuffSize)
{
    m_oFrameVideoItemHash.get_buffer(ppFrameVideoItemBuffer,puiFrameVideoItemBufferSize);
    *pFrameDataBuff = m_pFrameDataBuff;
    *puiFrameDataBuffSize = G_UIFRAMEDATABUFFSZIE;
    return 0;
}

int CFrameBuffer::get_frame_buffer_in_order(
        const SFrameVideoItem **ppFrameVideoItemBuffer, 
        const char **pFrameDataBuff,
        unsigned long long ullTime)
{
    if(0 == m_uiReadSeq)
    {
        m_uiReadSeq = m_uiWriteSeq;
    }

    if(m_uiReadSeq >= m_uiWriteSeq)
    {
        return -1;
    }

    SFrameVideoItem &p = m_oFrameVideoItemHash.get(m_uiReadSeq);

	SUBMCD_DEBUG_P(LOG_TRACE,"CFrameBuffer::get_frame_buffer_in_order"
                                 " uin: %u, uiReadSeq: %u, uiMemSeq: %u, uiSize: %u\n",
			p.uiUin,
			m_uiReadSeq,
			p.uiFrameSeq,
			p.uiFrameSize);

	if(p.uiFrameSeq != m_uiReadSeq)
	{
		if(p.ullTouchTime == 0)
		{
			p.ullTouchTime = ullTime;
		}

		if(ullTime - p.ullTouchTime > MAX_VIDEO_TIEMOUT)
		{
			m_uiReadSeq++;
		}

		return -2;
	}

    *ppFrameVideoItemBuffer = &p;
    *pFrameDataBuff = m_pFrameDataBuff;

	p.ullTouchTime	= 0;

	m_uiReadSeq++;

    return 0;
}
