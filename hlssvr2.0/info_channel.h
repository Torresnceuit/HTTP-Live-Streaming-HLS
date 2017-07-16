#ifndef _MUXSVR_INFO_CHANNEL_H
#define _MUXSVR_INFO_CHANNEL_H

#include <map>
#include <list>

#include "buffer.h"
#include "CMultiHash.h"
#include "muxsvr.h"

namespace muxsvr 
{
    class CMCDProc;

    typedef std::list<SVideoIdInfo*>                TVideoIdInfoList;
	typedef std::list<SRoom2VideoId*>				TRoom2VideoIdList;
    typedef std::list<SFlow2VideoIdInfo*>           TFlow2VideoIdInfoList;

    typedef struct SFlow2VideoIdMgr
    {
    public:
        SFlow2VideoIdMgr();
        ~SFlow2VideoIdMgr(){}

        SFlow2VideoIdInfo* ins(const SFlow2VideoIdInfo& oCdnFlowInfo);

        SFlow2VideoIdInfo* get(unsigned int uiFlow)
        {
            m_oFlow2VideoIdInfo.uiFlow = uiFlow;
            return m_oHash.get(uiFlow, m_oFlow2VideoIdInfo);
        }

        int val(TFlow2VideoIdInfoList& oList)
        {
            return m_oHash.val(oList);
        }

        unsigned int valSize() const
        {
            return m_oHash.valSize();
        }

        int del(unsigned int uiFlow)
        {
            m_oFlow2VideoIdInfo.uiFlow = uiFlow;
            return m_oHash.del(uiFlow, m_oFlow2VideoIdInfo);
        }

        int init(unsigned int uiKey, unsigned int uiHashLen, unsigned int uiHashTime, bool bCreate)
        {
            return m_oHash.init(uiKey, uiHashLen, uiHashTime, bCreate);
        }

    public:
        SFlow2VideoIdInfo  m_oFlow2VideoIdInfo;

    private:
        typedef CMultiHash<SFlow2VideoIdInfo> THash;
        THash     m_oHash; 
    }SFlow2VideoIdMgr;

    typedef struct SVideoIdMgr
    {
    public:
        SVideoIdMgr();
        ~SVideoIdMgr(){};

        SVideoIdInfo* ins(const SVideoIdInfo& oVideoIdInfo);

        SVideoIdInfo* get(unsigned int uiVideoId)
        {
            m_oVideoIdInfo.uiVideoId = uiVideoId;
            return m_oHash.get(uiVideoId, m_oVideoIdInfo);
        }

        SVideoIdInfo* add(unsigned int uiVideoId)
        {
            m_oVideoIdInfo.clear();
            m_oVideoIdInfo.uiVideoId = uiVideoId;
            return m_oHash.ins(uiVideoId, m_oVideoIdInfo);
        }

        int val(TVideoIdInfoList& oList)
        {
            return m_oHash.val(oList);
        }

        int del(unsigned int uiVideoId)
        {
            m_oVideoIdInfo.uiVideoId = uiVideoId;
            return m_oHash.del(uiVideoId, m_oVideoIdInfo);
        }

        int init(unsigned int uiKey, unsigned int uiHashLen, unsigned int uiHashTime, bool bCreate)
        {
            return m_oHash.init(uiKey, uiHashLen, uiHashTime, bCreate);
        };

    public:
        SVideoIdInfo  m_oVideoIdInfo;

    private:
        typedef CMultiHash<SVideoIdInfo> THash;
        THash     m_oHash; 
    }SVideoIdMgr;

	typedef struct SRoomId2VideoIdMgr
	{
	public:
		SRoomId2VideoIdMgr();
		~SRoomId2VideoIdMgr(){};

		SRoom2VideoId* ins(const SRoom2VideoId& oRoomId2VideoId);

		SRoom2VideoId* get(unsigned int uiRoomId)
		{
			m_oRoom2VideoId.uiRoomId = uiRoomId;
			return m_oHash.get(uiRoomId, m_oRoom2VideoId);
		}

		SRoom2VideoId* add(const unsigned int uiRoomId)
		{
			m_oRoom2VideoId.clear();
			m_oRoom2VideoId.uiRoomId = uiRoomId;

			return m_oHash.ins(uiRoomId, m_oRoom2VideoId);
		}

		int val(TRoom2VideoIdList& oList)
		{
			return m_oHash.val(oList);
		}

		int del(unsigned int uiRoomId)
		{
			m_oRoom2VideoId.uiVideoId = uiRoomId;
			return m_oHash.del(uiRoomId, m_oRoom2VideoId);
		}

		int init(unsigned int uiKey, unsigned int uiHashLen, unsigned int uiHashTime, bool bCreate)
		{
			return m_oHash.init(uiKey, uiHashLen, uiHashTime, bCreate);
		};

	public:
		SRoom2VideoId	m_oRoom2VideoId;

	private:
		typedef CMultiHash<SRoom2VideoId> THash;
		THash	m_oHash;

	}SRoomId2VideoIdMgr;

    class CInfoChannelMgr
    {
    public:
        int update_flowinfo_in_flowlist(const SFlowInfo& oFlowInfo, SFlowInfo **ppFlowInfo, 
                                SFlowInfo pFlowInfoList[], const unsigned int uiMaxFlowListSize);

        int delete_flowinfo_in_flowlist(const SFlowInfo& oFlowInfo, SFlowInfo pFlowInfoList[], 
                                           const unsigned int uiMaxFlowListSize);

        int update_flow2videoidinfo(unsigned int uiVideoId, const SFlowInfo& oFlowInfo, 
                                     SFlow2VideoIdInfo **ppFlow2VideoIdInfo, SFlow2VideoIdMgr &oFlow2VideoIdMgr);

        int add_video_flow(unsigned int uiVideoId, const SFlowInfo& oFlowInfo, 
                             SVideoIdInfo **ppVideoIdInfo, SFlowInfo **ppFlowInfo);

        int del_flow(const SConnInfo& oConnInfo);

        SVideoIdInfo *get_video_id(unsigned int uiVideoId);
        SVideoIdInfo *add_video_id(unsigned int uiVideoId);

	SRoom2VideoId* alloc_roomid_2_videoid(unsigned int uiRoomId);
	SRoom2VideoId* get_roomid2videoid(unsigned int uiRoomId);

    private:
	SRoom2VideoId* add_roomid2videoid(unsigned int uiRoomId);

    public:
        bool DoCleanTimeOutVideoId();
	bool DoCleanTimeOutRoomId2VideoId();

    public:
        CInfoChannelMgr(CMCDProc* proc);
        ~CInfoChannelMgr();

        int init();
        void dispatch_timeout();

        int val_videoid(TVideoIdInfoList& oList)
        {
            return m_oVideoIdMgr.val(oList);
        }

	int val_roomid2videoid(TRoom2VideoIdList& oList)
	{
		return m_oRoomId2VideoIdMgr.val(oList);
	}

	unsigned int getCdnConnNum() const
	{
		return (unsigned int)(m_oFlow2VideoId80Mgr.valSize()
                               + m_oFlow2VideoId8000Mgr.valSize()
                               + m_oFlow2VideoId443Mgr.valSize());
	}

    public:
        SVideoIdMgr         m_oVideoIdMgr;

    private:
        SFlow2VideoIdMgr    m_oFlow2VideoId80Mgr;
        SFlow2VideoIdMgr    m_oFlow2VideoId8000Mgr;
        SFlow2VideoIdMgr    m_oFlow2VideoId443Mgr;

	SRoomId2VideoIdMgr  m_oRoomId2VideoIdMgr;

    private:
        CMCDProc*     m_pMCD;

    };
}

#endif
