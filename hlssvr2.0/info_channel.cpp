#include <map>

#include "tfc_debug_log.h"
#include "tfc_base_comm.h"

#include "submcd_log.h"
#include "gFunc.h"

#include "muxsvr_mcd_proc.h"
#include "info_channel.h"

#include "hls/hls_stream.h"

using namespace std;
using namespace tfc::base;
using namespace muxsvr;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SVideoIdMgr::SVideoIdMgr()
    :m_oVideoIdInfo()
	,m_oHash(m_oVideoIdInfo, THash::GETFUNC(&SVideoIdInfo::get_func), THash::INSFUNC(&SVideoIdInfo::ins_func), THash::VALFUNC(&SVideoIdInfo::val_fun))
{

}

SVideoIdInfo* SVideoIdMgr::ins(const SVideoIdInfo& oVideoIdInfo)
{
    SVideoIdInfo* pVideoIdInfo = m_oHash.ins(oVideoIdInfo.uiVideoId, oVideoIdInfo);
    return pVideoIdInfo;
}

SRoomId2VideoIdMgr::SRoomId2VideoIdMgr()
	:m_oRoom2VideoId()
	,m_oHash(m_oRoom2VideoId, THash::GETFUNC(&SRoom2VideoId::get_func),THash::INSFUNC(&SRoom2VideoId::ins_func),THash::VALFUNC(&SRoom2VideoId::val_fun))
{

}

SRoom2VideoId* SRoomId2VideoIdMgr::ins(const SRoom2VideoId& oRoom2VideoId)
{
	SRoom2VideoId* pRoom2VideoId = m_oHash.ins(oRoom2VideoId.uiRoomId, oRoom2VideoId);

	return pRoom2VideoId;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SFlow2VideoIdMgr::SFlow2VideoIdMgr()
    :m_oFlow2VideoIdInfo()
    ,m_oHash(m_oFlow2VideoIdInfo, THash::GETFUNC(&SFlow2VideoIdInfo::get_func), THash::INSFUNC(&SFlow2VideoIdInfo::ins_func), THash::INSFUNC(&SFlow2VideoIdInfo::val_fun))
{

}

SFlow2VideoIdInfo* SFlow2VideoIdMgr::ins(const SFlow2VideoIdInfo& oFlow2VideoIdInfo)
{
    SFlow2VideoIdInfo* pCdnFlowInfo = m_oHash.ins(oFlow2VideoIdInfo.uiFlow, oFlow2VideoIdInfo);
    return pCdnFlowInfo;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CInfoChannelMgr::CInfoChannelMgr(CMCDProc* proc)
    :m_pMCD(proc)
{

}

CInfoChannelMgr::~CInfoChannelMgr()
{

}

int CInfoChannelMgr::init()
{
    int ret = 0;

    ret = m_oFlow2VideoId80Mgr.init(m_pMCD->_cfg.m_uiCdnFlow80HashKey, m_pMCD->_cfg.m_uiCdnFlow80HashLen, m_pMCD->_cfg.m_uiCdnFlow80HashTime, true);
    if(ret < 0)
    {
        return -5;
    }

    ret = m_oFlow2VideoId8000Mgr.init(m_pMCD->_cfg.m_uiCdnFlow8000HashKey, m_pMCD->_cfg.m_uiCdnFlow8000HashLen, m_pMCD->_cfg.m_uiCdnFlow8000HashTime, true);
    if(ret < 0)
    {
        return -6;
    }

    ret = m_oFlow2VideoId443Mgr.init(m_pMCD->_cfg.m_uiCdnFlow443HashKey, m_pMCD->_cfg.m_uiCdnFlow443HashLen, m_pMCD->_cfg.m_uiCdnFlow443HashTime, true);
    if(ret < 0)
    {
        return -7;
    }

    ret = m_oVideoIdMgr.init(m_pMCD->_cfg.m_uiVideoIdHashKey, m_pMCD->_cfg.m_uiVideoIdHashLen, m_pMCD->_cfg.m_uiVideoIdHashTime, true);
    if(ret < 0)
    {
        return -8;
    }

	ret = m_oRoomId2VideoIdMgr.init(m_pMCD->_cfg.m_uiRoomId2VideoIdHashKey, m_pMCD->_cfg.m_uiRoomId2VideoIdHashLen, m_pMCD->_cfg.m_uiRoomId2VideoIdHashTime, true);
	if(ret < 0)
	{
		return -9;
	}

    return 0;
}

int CInfoChannelMgr::update_flowinfo_in_flowlist(const SFlowInfo& oFlowInfo, SFlowInfo **ppFlowInfo, SFlowInfo pFlowInfoList[], const unsigned int uiMaxFlowListSize)
{
    if(0==oFlowInfo.uiFlow || NULL==pFlowInfoList || 0==uiMaxFlowListSize)
    {
        return -1;
    }

    bool bAdded = false;

    for(unsigned int i = 0; i < uiMaxFlowListSize; i++)
    {
        if (pFlowInfoList[i].uiFlow == 0)
        {
            if (false == bAdded)
            {
                pFlowInfoList[i].clone(oFlowInfo);
                pFlowInfoList[i].unionFlag.ucIsHaveMediaInfo = false;
                *ppFlowInfo = &pFlowInfoList[i];
                bAdded = true;
            }
        }
        else
        {
            if(pFlowInfoList[i].equal(oFlowInfo))
            {
                if (false == bAdded)
                {
                    pFlowInfoList[i].clone(oFlowInfo);
                    pFlowInfoList[i].unionFlag.ucIsHaveMediaInfo = false;
                    *ppFlowInfo = &pFlowInfoList[i];
                    bAdded = true;
                }
                else
                {
                    //把后面可能重复的删掉
                    pFlowInfoList[i].clear();
                }
            }
        }
    }

    return 0;
}

int CInfoChannelMgr::delete_flowinfo_in_flowlist(const SFlowInfo& oFlowInfo, 
               SFlowInfo pFlowInfoList[], const unsigned int uiMaxFlowListSize)
{
    if(0 == oFlowInfo.uiFlow || NULL == pFlowInfoList)
    {
        return -1;
    }

    for (unsigned int i=0; i < uiMaxFlowListSize; i++)
    {
        if(pFlowInfoList[i].equal(oFlowInfo))
        {
            pFlowInfoList[i].clear();
        }
    }

    return 0;
}

int CInfoChannelMgr::update_flow2videoidinfo(unsigned int uiVideoId, const SFlowInfo &oFlowInfo, 
                SFlow2VideoIdInfo **ppFlow2VideoIdInfo, SFlow2VideoIdMgr &oFlow2VideoIdMgr)
{
    if(0 == uiVideoId || 0 == oFlowInfo.uiFlow || NULL == ppFlow2VideoIdInfo)
    {
        SUBMCD_DEBUG_P(LOG_ERROR, "CInfoChannelMgr::update_flow2videoidinfo failed, "
                                  "flow: %u, uiSrcType: %u, ip: %s, port: %u, uiVideoId: %u\n", 
            oFlowInfo.uiFlow,
            oFlowInfo.uiSrcType,
            ip_to_str(oFlowInfo.uiIp).c_str(),
            oFlowInfo.usPort,
            uiVideoId);

        return -1;
    }

    SFlow2VideoIdInfo oFlow2VideoIdInfo;
    oFlow2VideoIdInfo.clear();
    oFlow2VideoIdInfo.uiFlow = oFlowInfo.uiFlow;
    oFlow2VideoIdInfo.ucFlowType = oFlowInfo.ucFlowType;
    oFlow2VideoIdInfo.uiVideoId = uiVideoId;

    SFlow2VideoIdInfo *pFlow2VideoIdInfo = oFlow2VideoIdMgr.get(oFlow2VideoIdInfo.uiFlow);

    //插入新的
    if (NULL == pFlow2VideoIdInfo)
    {
        pFlow2VideoIdInfo = oFlow2VideoIdMgr.ins(oFlow2VideoIdInfo);
    }
    else
    {
        //videoid -> flow
        SVideoIdInfo *pOldVideoIdInfo = m_oVideoIdMgr.get(pFlow2VideoIdInfo->uiVideoId);

        if(NULL != pOldVideoIdInfo)
        {
            switch(oFlowInfo.ucFlowType)
            {
            case FLOW_TYPE_CDN:
                delete_flowinfo_in_flowlist(oFlowInfo, pOldVideoIdInfo->oCdnFlowList, MAX_CDN_FLOW_NUM);
                break;
            default:
                break;
            }
        }
    }

    if (NULL == pFlow2VideoIdInfo)
    {
        SUBMCD_DEBUG_P(LOG_ERROR, "CInfoChannelMgr::update_flow2videoidinfo ins oFlow2VideoIdInfo failed, "
                                  "flow: %u, uiSrcType: %u, ip: %s, port: %u, uiVideoId: %u\n", 
            oFlowInfo.uiFlow,
            oFlowInfo.uiSrcType,
            ip_to_str(oFlowInfo.uiIp).c_str(),
            oFlowInfo.usPort,
            uiVideoId);

        return -1;
    }

    *ppFlow2VideoIdInfo = pFlow2VideoIdInfo;

    return 0;
}

int CInfoChannelMgr::add_video_flow(unsigned int uiVideoId, const SFlowInfo& oFlowInfo, 
                  SVideoIdInfo **ppVideoIdInfo, SFlowInfo **ppFlowInfo)
{
    //flow -> flow2videoid contains videoid -> videoidinfo contains flowlist
    //三个接收进程,flow可能会一样,所以分开管理
    //将flow和videoid映射关系插入到SFlow2VideoIdMgr

    SFlow2VideoIdInfo *pFlow2VideoIdInfo = NULL;
    int ret = -1;

    switch(oFlowInfo.uiSrcType)
    {
    case SOCKET_TCP_80:
        {
            ret = update_flow2videoidinfo(uiVideoId, oFlowInfo, 
                                &pFlow2VideoIdInfo, m_oFlow2VideoId80Mgr);
        }
        break;

    case SOCKET_TCP_8000:
        {
            ret = update_flow2videoidinfo(uiVideoId, oFlowInfo, 
                                &pFlow2VideoIdInfo, m_oFlow2VideoId8000Mgr);
        }
        break;

    case SOCKET_TCP_443:
        {
            ret = update_flow2videoidinfo(uiVideoId, oFlowInfo, 
                                &pFlow2VideoIdInfo, m_oFlow2VideoId443Mgr);
        }
        break;

    default:
        return -1;
    }

    if(ret!=0 || NULL==pFlow2VideoIdInfo)
    {
        SUBMCD_DEBUG_P(LOG_ERROR,"CInfoChannelMgr::add_video_flow update_flow2videoidinfo fail, "
                                 "uiVideoId: %u, uiFlow: %u, pCdnFlow2VideoIdInfo: 0x%x\n",
            uiVideoId,
            oFlowInfo.uiFlow,
            (unsigned int)pFlow2VideoIdInfo);
        return -1;
    }

    /*
    flow  + videoid = SFlow2VideoIdInfo SFlow2VideoIdMgr_80     
    flow  + videoid = SFlow2VideoIdInfo --> SFlow2VideoIdMgr_8000 --> videoid --> SVideoIdMgr --> SVideoIdInfo --> SFlowInfo         
    flow  + videoid = SFlow2VideoIdInfo SFlow2VideoIdMgr_443
    */

    //更新SVideoInfo信息
    SVideoIdInfo oVideoIdInfo;
    oVideoIdInfo.clear();
    oVideoIdInfo.uiVideoId = uiVideoId;

    SVideoIdInfo *pVideoIdInfo = m_oVideoIdMgr.get(uiVideoId);

    if (NULL == pVideoIdInfo)
    {
        pVideoIdInfo = m_oVideoIdMgr.ins(oVideoIdInfo);
    }

    if (NULL == pVideoIdInfo)
    {
        pFlow2VideoIdInfo->clear();

        //Todo,加入monitor报警

        SUBMCD_DEBUG_P(LOG_ERROR, "CInfoChannelMgr::add_video_flow ins oVideoIdInfo failed, "
                                  "flow: %u, uiSrcType: %u, ip: %s, port: %u, uiVideoId: %u\n", 
            oFlowInfo.uiFlow,
            oFlowInfo.uiSrcType,
            ip_to_str(oFlowInfo.uiIp).c_str(),
            oFlowInfo.usPort,
            uiVideoId);

        return -1;
    }

    ret = -1;
    *ppFlowInfo = NULL;

    switch(oFlowInfo.ucFlowType)
    {
    case FLOW_TYPE_CDN:
        {
            ret = update_flowinfo_in_flowlist(oFlowInfo, ppFlowInfo, 
                              pVideoIdInfo->oCdnFlowList, MAX_CDN_FLOW_NUM);
        }
        break;
    default:
        break;
    }

    if (0!=ret || NULL==*ppFlowInfo)
    {
        pFlow2VideoIdInfo->clear();

        SUBMCD_DEBUG_P(LOG_ERROR, "CInfoChannelMgr::add_video_flow ins oVideoIdInfo_uiFlow failed, "
                                  "flow: %u, uiSrcType: %u, ip: %s, port: %u, uiVideoId: %u\n", 
            oFlowInfo.uiFlow,
            oFlowInfo.uiSrcType,
            ip_to_str(oFlowInfo.uiIp).c_str(),
            oFlowInfo.usPort,
            uiVideoId);

        return -1;
    }

    *ppVideoIdInfo = pVideoIdInfo;

    SUBMCD_DEBUG_P(LOG_DEBUG, "CInfoChannelMgr::add_video_flow flow: %u, "
                              "uiSrcType: %u, ip: %s, port: %u, uiVideoId: %u\n", 
        oFlowInfo.uiFlow,
        oFlowInfo.uiSrcType,
        ip_to_str(oFlowInfo.uiIp).c_str(),
        oFlowInfo.usPort,
        uiVideoId);

    return 0;
}

int CInfoChannelMgr::del_flow(const SConnInfo& oConnInfo)
{
    SFlow2VideoIdInfo *pFlow2VideoIdInfo = NULL;

    switch(oConnInfo.uiSrcType)
    {
    case SOCKET_TCP_80:
        pFlow2VideoIdInfo = m_oFlow2VideoId80Mgr.get(oConnInfo.uiFlow);
        break;
    case SOCKET_TCP_8000:
        pFlow2VideoIdInfo = m_oFlow2VideoId8000Mgr.get(oConnInfo.uiFlow);
        break;
    case SOCKET_TCP_443:
        pFlow2VideoIdInfo = m_oFlow2VideoId443Mgr.get(oConnInfo.uiFlow);
        break;
    default:
        break;
    }

    if (NULL == pFlow2VideoIdInfo)
    {
        SUBMCD_DEBUG_P(LOG_DEBUG, "CInfoChannelMgr::del_flow pFlow2VideoIdInfo NULL. flow: %u, ip: %s, port: %u, uiSrcType: %u\n", 
            oConnInfo.uiFlow,
            ip_to_str(oConnInfo.uiIp).c_str(), 
            oConnInfo.usPort,
            oConnInfo.uiSrcType);

        return -1;
    }

    unsigned int uiVideoId = pFlow2VideoIdInfo->uiVideoId;
    unsigned char ucFlowType = pFlow2VideoIdInfo->ucFlowType;
    pFlow2VideoIdInfo->clear();

    SVideoIdInfo *pVideoIdInfo = m_oVideoIdMgr.get(uiVideoId);

    if (NULL == pVideoIdInfo)
    {
        SUBMCD_DEBUG_P(LOG_ERROR, "CInfoChannelMgr::del_flow pVideoIdInfo NULL. flow: %u, ip: %s, port: %u, uiSrcType: %u, uiVideoId: %u\n", 
            oConnInfo.uiFlow,
            ip_to_str(oConnInfo.uiIp).c_str(),
            oConnInfo.usPort,
            oConnInfo.uiSrcType,
            uiVideoId);

        return -1;
    }

    switch (ucFlowType)
    {
    case FLOW_TYPE_CDN:
        for (unsigned int i=0; i<MAX_CDN_FLOW_NUM; i++)
        {
            if (pVideoIdInfo->oCdnFlowList[i].uiFlow == oConnInfo.uiFlow
                && pVideoIdInfo->oCdnFlowList[i].uiSrcType == oConnInfo.uiSrcType)
            {
                pVideoIdInfo->oCdnFlowList[i].clear();
            }
        }

        break;

    default:
        break;
    }

    SUBMCD_DEBUG_P(LOG_DEBUG, "CInfoChannelMgr::del_flow flow: %u, ip: %s, port: %u, uiSrcType: %u, uiVideoId: %u\n", 
        oConnInfo.uiFlow,
        ip_to_str(oConnInfo.uiIp).c_str(),
        oConnInfo.usPort,
        oConnInfo.uiSrcType,
        uiVideoId);

    return 0;
}

SVideoIdInfo * CInfoChannelMgr::get_video_id(unsigned int uiVideoId)
{
    return m_oVideoIdMgr.get(uiVideoId);
}

SVideoIdInfo * CInfoChannelMgr::add_video_id(unsigned int uiVideoId)
{
    return m_oVideoIdMgr.add(uiVideoId);
}

SRoom2VideoId* CInfoChannelMgr::alloc_roomid_2_videoid(unsigned int uiRoomId)
{
	SRoom2VideoId *pRoom2VideoId = get_roomid2videoid(uiRoomId);

	if(NULL == pRoom2VideoId)
	{
		pRoom2VideoId = add_roomid2videoid(uiRoomId);
	}

	return pRoom2VideoId;
}

SRoom2VideoId* CInfoChannelMgr::get_roomid2videoid(unsigned int uiRoomId)
{
	return m_oRoomId2VideoIdMgr.get(uiRoomId);
}

SRoom2VideoId* CInfoChannelMgr::add_roomid2videoid(unsigned int uiRoomId)
{
	return m_oRoomId2VideoIdMgr.add(uiRoomId);
}

bool CInfoChannelMgr::DoCleanTimeOutVideoId()
{
    SUBMCD_DEBUG_P(LOG_COLOR, "CInfoChannelMgr::DoCleanTimeOutVideoId--------------------begin\n");

    TVideoIdInfoList oList;
    val_videoid(oList);

    SVideoIdInfo* pVideoIdInfo = NULL;

    unsigned int uiVideoIdNum = 0;
    unsigned int uiTimeOutVideoIdNum = 0;

    unsigned int uiVideoIdCount = 0;
    unsigned int uiMaxTotalVideoIdNum = m_pMCD->_cfg.m_uiVideoIdHashLen*m_pMCD->_cfg.m_uiVideoIdHashTime;

	unsigned int uiCdnConnNum = getCdnConnNum();

	Attr_API_Set(328426,uiCdnConnNum);

    for (TVideoIdInfoList::iterator it = oList.begin(); it != oList.end(); it++)
    {
        uiVideoIdCount++;
        if (uiVideoIdCount >uiMaxTotalVideoIdNum)
        {
            break;
        }

        pVideoIdInfo = *it;

        if (NULL == pVideoIdInfo)
        {
            continue;
        }

        uiVideoIdNum++;

        if (pVideoIdInfo->uiTouchTime + 10 < (unsigned int)m_pMCD->m_cur_time)
        {
            uiTimeOutVideoIdNum++;

            for (unsigned int i=0; i<MAX_CDN_FLOW_NUM; i++)
            {
                if (pVideoIdInfo->oCdnFlowList[i].uiFlow != 0)
                {
                    SFlow2VideoIdInfo *pFlow2VideoIdInfo = NULL;

                    switch(pVideoIdInfo->oCdnFlowList[i].uiSrcType)
                    {
                    case SOCKET_TCP_80:
                        pFlow2VideoIdInfo = m_oFlow2VideoId80Mgr.get(pVideoIdInfo->oCdnFlowList[i].uiFlow);
                        break;
                    case SOCKET_TCP_8000:
                        pFlow2VideoIdInfo = m_oFlow2VideoId8000Mgr.get(pVideoIdInfo->oCdnFlowList[i].uiFlow);
                        break;
                    case SOCKET_TCP_443:
                        pFlow2VideoIdInfo = m_oFlow2VideoId443Mgr.get(pVideoIdInfo->oCdnFlowList[i].uiFlow);
                        break;
                    default:
                        break;
                    }

                    if (NULL == pFlow2VideoIdInfo)
                    {
                        continue;
                    }

                    pFlow2VideoIdInfo->clear();
                }
            }

			SUserInfo oUserInfo;
			oUserInfo.uiUin			= pVideoIdInfo->uiUin;
			oUserInfo.uiRoomId		= pVideoIdInfo->uiRoomId;
			oUserInfo.uiMainRoomId	= pVideoIdInfo->uiMainRoomId;
			oUserInfo.uiVideoId		= pVideoIdInfo->uiVideoId;
			oUserInfo.ucRoomType	= pVideoIdInfo->ucRoomType;

			if(0 == oUserInfo.uiUin || 0 == oUserInfo.uiRoomId || 0 == oUserInfo.uiMainRoomId)
			{
				SUBMCD_DEBUG_P(LOG_ERROR, "CInfoChannelMgr::DoCleanTimeOutVideoId found zero userinfo, uin: %u, roomid: %u, mainroomid: %u, vid: %u\n",
					oUserInfo.uiUin,
					oUserInfo.uiRoomId,
					oUserInfo.uiMainRoomId,
					oUserInfo.uiVideoId);

				return -1;
			}

            SFrameVideo oFrameVideo;
			m_pMCD->m_oProtolProcess->DoNotifyLiveProgramMgrVideoStauts(oUserInfo,VIDEO_STOP,oFrameVideo);
            SUBMCD_DEBUG_P(LOG_ERROR, "CInfoChannelMgr::DoCleanTimeOutVideoId clean timeout video, videoid=%d, touchtime=%u, curtime=%u\n",
                                    pVideoIdInfo->uiVideoId, pVideoIdInfo->uiTouchTime, m_pMCD->m_cur_time );

            hls_clean_timeout_stream(pVideoIdInfo->uiVideoId);

            pVideoIdInfo->clear();
        }
    }

    SUBMCD_DEBUG_P(LOG_COLOR, "CInfoChannelMgr::DoCleanTimeOutVideoId uiVideoIdNum: %u, uiTimeOutVideoIdNum: %u\n", uiVideoIdNum, uiTimeOutVideoIdNum);
    SUBMCD_DEBUG_P(LOG_COLOR, "CInfoChannelMgr::DoCleanTimeOutVideoId++++++++++++++++++++end\n");



    return true;
}

bool CInfoChannelMgr::DoCleanTimeOutRoomId2VideoId()
{
	SUBMCD_DEBUG_P(LOG_COLOR, "CInfoChannelMgr::DoCleanTimeOutRoomId2VideoId--------------------begin\n");

	TRoom2VideoIdList oList;

	unsigned int uiRoomIdNumCount	= 0;
	unsigned int uiRoomIdNum		= 0;
	unsigned int uiTimeOutRoomIdNum = 0;

	val_roomid2videoid(oList);

	SRoom2VideoId* pRoom2VideoId = NULL;

	unsigned int uiMaxRoomId2VideoIdNum = m_pMCD->_cfg.m_uiRoomId2VideoIdHashLen * m_pMCD->_cfg.m_uiRoomId2VideoIdHashTime;

	for(TRoom2VideoIdList::iterator it = oList.begin(); it != oList.end(); it++)
	{
		uiRoomIdNumCount++;

		if (uiRoomIdNumCount >uiMaxRoomId2VideoIdNum)
		{
			break;
		}

		pRoom2VideoId = *it;

		if (NULL == pRoom2VideoId)
		{
			continue;
		}

		uiRoomIdNum++;

		if(pRoom2VideoId->uiTouchTime + 5 < (unsigned int)m_pMCD->m_cur_time)
		{
			uiTimeOutRoomIdNum++;

			SUBMCD_DEBUG_P(LOG_COLOR,"CInfoChannelMgr::DoCleanTimeOutRoomId2VideoId uiRoomId: %u, uiVideoId: %u, time: %s\n",
				pRoom2VideoId->uiRoomId,
				pRoom2VideoId->uiVideoId,
				t2s(pRoom2VideoId->uiTouchTime).c_str());

			pRoom2VideoId->clear();
		}
	}


	SUBMCD_DEBUG_P(LOG_COLOR, "CInfoChannelMgr::DoCleanTimeOutRoomId2VideoId uiRoomId: %u, uiTimeOutRoomIdNum: %u\n", uiRoomIdNum, uiTimeOutRoomIdNum);
	SUBMCD_DEBUG_P(LOG_COLOR, "CInfoChannelMgr::DoCleanTimeOutRoomId2VideoId++++++++++++++++++++end\n");
	return true;

}
