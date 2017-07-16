#include "tfc_debug_log.h"

#include "Attr_API.h"

#include "submcd_log.h"

#include "muxsvr_mcd_proc.h"
#include "voice_channel.h"

using namespace muxsvr;

int SVoiceChannel::open(const SVoiceChannel &oVoiceChannel, unsigned int uiChannelId)
{
    memcpy((void *)this, (void *)&oVoiceChannel, sizeof(*this));
    m_uiChannelId = uiChannelId;

    return 0;
}

int SVoiceChannel::pushBackVideoFrame(const SUserInfo &oUserInfo, const SFrameVideo &oFrameVideo, unsigned long long ullTime)
{
    int ret = m_oFrameBuffer.push_back(oUserInfo, oFrameVideo, ullTime);

	if(!ret)
	{
		m_uiPushTime	= ullTime / 1000;
	}

    return ret;
}

int SVoiceChannel::pushBackAudioFrame(const SUserInfo &oUserInfo, unsigned int uiSeq, unsigned char *pData, unsigned int uiSize, unsigned int uiTime)
{
	int ret = m_oAudioBuffer.push_back(oUserInfo, uiSeq,pData,uiSize,uiTime);

	if(!ret)
	{
		m_uiPushTime	= uiTime;
	}

	return ret;
}

SVoiceChannelIdInfo *CDataChannelMgr::CVoiceChannelIdMgr::ins(const SVoiceChannelIdInfo &oVoiceChannelIdInfo)
{
    SUBMCD_DEBUG_P(LOG_DEBUG, "SVoiceChannelIdMgr::ins key: %u\n", oVoiceChannelIdInfo.uiKey);

    SVoiceChannelIdInfo* pVoiceChannelIdInfo = get(oVoiceChannelIdInfo.uiKey);

    if (NULL != pVoiceChannelIdInfo)
    {
        return pVoiceChannelIdInfo;
    }    
    else
    {
        m_oVoiceChannelIdInfo.clear();
        memcpy(&m_oVoiceChannelIdInfo, &oVoiceChannelIdInfo, sizeof(SVoiceChannelIdInfo));

        unsigned int uiXHashTime = 0;
        unsigned int uiYHashLen  = 0;
        SVoiceChannelIdInfo* pVoiceChannelIdInfo = m_oHash.ins(m_oVoiceChannelIdInfo.uiKey, m_oVoiceChannelIdInfo, uiXHashTime, uiYHashLen);

        if (NULL == pVoiceChannelIdInfo)
        {
            return NULL;
        }

        m_oVoiceChannelIdInfo.clear();

        return pVoiceChannelIdInfo;
    }

    return NULL;
}

int CDataChannelMgr::CVoiceChannelIdMgr::init()
{
    return init(m_pMCD->_cfg.m_uiVoiceChannelIdHashKey, m_pMCD->_cfg.m_uiVoiceChannelIdHashLen, m_pMCD->_cfg.m_uiVoiceChannelIdHashTime, true);
}

int CDataChannelMgr::CVoiceChannelAllocMgr::init()
{
    return init(m_pMCD->_cfg.m_uiVoiceChannelHashKey, m_pMCD->_cfg.m_uiVoiceChannelHashLen*m_pMCD->_cfg.m_uiVoiceChannelHashTime,true);
}

SVoiceChannel *CDataChannelMgr::get_voice_channel(unsigned int uiKey)
{
    SVoiceChannelIdInfo *pVoiceChannelIdInfo = NULL;
    pVoiceChannelIdInfo = m_pVoiceChannelIdMgr->get(uiKey);

    if (NULL == pVoiceChannelIdInfo)
    {
        return NULL;
    }

    return m_pVoiceChannelAllocMgr->get_voice_channel(pVoiceChannelIdInfo->uiChannelId);
}

SVoiceChannel *CDataChannelMgr::allocate_voice_channel(unsigned int uiKey, bool &bIsNew)
{
	bIsNew = false;

    SVoiceChannelIdInfo *pVoiceChannelIdInfo = NULL;
    pVoiceChannelIdInfo = m_pVoiceChannelIdMgr->get(uiKey);

    if (NULL == pVoiceChannelIdInfo)
    {
        SVoiceChannelIdInfo oVoiceChannelIdInfo;
        oVoiceChannelIdInfo.clear();
        oVoiceChannelIdInfo.uiKey = uiKey;

        oVoiceChannelIdInfo.uiChannelId = INVALID_VOICECHANNEL_ID;

        pVoiceChannelIdInfo = m_pVoiceChannelIdMgr->ins(oVoiceChannelIdInfo);

        if (NULL == pVoiceChannelIdInfo)
        {
            SUBMCD_DEBUG_P(LOG_ERROR, "CDataChannelAllocMgr::allocate_voice_channel"
                  " ins voiceChannelId failed. key: %u\n", oVoiceChannelIdInfo.uiKey);
            return NULL;
        }
    }

    if (INVALID_VOICECHANNEL_ID == pVoiceChannelIdInfo->uiChannelId)
    {
        unsigned int uiChannelId = m_pVoiceChannelAllocMgr->allocate_voice_channel();

        if (INVALID_VOICECHANNEL_ID == uiChannelId)
        {
            SUBMCD_DEBUG_P(LOG_ERROR, "CDataChannelAllocMgr::allocate_voice_channel "
                      "alloc voiceChannel failed. key: %u\n", pVoiceChannelIdInfo->uiKey);

            //事务回滚操作
            pVoiceChannelIdInfo->clear();
            return NULL;
        }

        pVoiceChannelIdInfo->uiChannelId = uiChannelId;
	    bIsNew = true;
        return m_pVoiceChannelAllocMgr->get_voice_channel(uiChannelId);
    }
    else
    {
        return m_pVoiceChannelAllocMgr->get_voice_channel(pVoiceChannelIdInfo->uiChannelId);
    }

    return NULL;
}

int CDataChannelMgr::release_voice_channel(unsigned int uiKey)
{
    SVoiceChannelIdInfo *pVoiceChannelIdInfo = NULL;
    pVoiceChannelIdInfo = m_pVoiceChannelIdMgr->get(uiKey);

    if (NULL == pVoiceChannelIdInfo)
    {
        return -1;
    }

    m_pVoiceChannelAllocMgr->release_voice_channel(pVoiceChannelIdInfo->uiChannelId);
    pVoiceChannelIdInfo->clear();

    return 0;
}

bool CDataChannelMgr::DoCleanInvalidVoiceChannel()
{
    SUBMCD_DEBUG_P(LOG_DEBUG, "CDataChannelAllocMgr::DoCleanInvalidVoiceChannel--------------------begin\n");
    TVoiceChannelIdInfoList oList;
    val(oList);

    SVoiceChannelIdInfo *pVoiceChannelIdInfo = NULL;
    SVoiceChannel *pVoiceChannel = NULL;

    unsigned int uiVoiceChannelNum = 0;
    unsigned int uiInvalidVoiceChannelNum = 0;

    unsigned int uiVoiceChannelCount = 0;
    unsigned int uiMaxTotalVoiceChannelNum = m_pMCD->_cfg.m_uiVoiceChannelHashLen*m_pMCD->_cfg.m_uiVoiceChannelHashTime;

	unsigned int uiFreeChannelNum = getFreeChannelNum();
	unsigned int uiUsedChannelNum = uiMaxTotalVoiceChannelNum - uiFreeChannelNum;

	Attr_API_Set(328425, uiFreeChannelNum);
	Attr_API_Set(328424, uiUsedChannelNum);

    for (TVoiceChannelIdInfoList::iterator it = oList.begin(); it != oList.end(); it++)
    {
        uiVoiceChannelCount++;
        if (uiVoiceChannelCount > uiMaxTotalVoiceChannelNum)
        {
            break;
        }

        pVoiceChannelIdInfo = *it;

        if (NULL == pVoiceChannelIdInfo)
        {
            continue;
        }

        pVoiceChannel = m_pVoiceChannelAllocMgr->get_voice_channel(pVoiceChannelIdInfo->uiChannelId);

        if (NULL == pVoiceChannel)
        {
            continue;
        }
        
        uiVoiceChannelNum++;

        if ((unsigned int)m_pMCD->m_cur_time > pVoiceChannel->getPushTime()+5)
        {
            release_voice_channel(pVoiceChannelIdInfo->uiKey);
            uiInvalidVoiceChannelNum++;
        }
    }

    Attr_API_Set(198659, uiVoiceChannelNum);  
    SUBMCD_DEBUG_P(LOG_DEBUG, "CDataChannelAllocMgr::DoCleanInvalidVoiceChannel RoomChannelNum: %u, InvalidRoomChannelNum: %u\n", uiVoiceChannelNum, uiInvalidVoiceChannelNum);
    SUBMCD_DEBUG_P(LOG_DEBUG, "CDataChannelAllocMgr::DoCleanInvalidVoiceChannel++++++++++++++++++++end\n");
    return true;
}

