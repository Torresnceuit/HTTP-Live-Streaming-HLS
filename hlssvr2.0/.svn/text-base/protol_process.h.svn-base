#ifndef _PROTOL_PROCESS_H
#define _PROTOL_PROCESS_H

#include "buffer.h"
#include "muxsvr.h"
#include "info_channel.h"
#include "voice_channel.h"

#include "protos/muxsvr.pb.h"
#include "protos/LiveProgramMgr.pb.h"
#include "protos/game.pb.h"

using namespace muxsvr_protos;
using namespace liveprogrammgr_protos;
using namespace gamesvr_protos;



namespace muxsvr 
{
    class CMCDProc;
    class CProtol
    {
    public:
        CProtol(CMCDProc* proc);
        ~CProtol(void);
    public:
        void process_cs_udp_msg(SConnInfo& oConnInfo, char *pPkg, unsigned int uiPkgLen);
        void process_ss_tcp_msg(SConnInfo& oConnInfo, char *pPkg, unsigned int uiPkgLen);
	void process_http_msg(const SConnInfo& oConnInfo, const char *pPkg, unsigned int uiPkgLen);
	void process_game_msg(const SConnInfo& oConnInfo, const char *pPkg, unsigned int uiPkgLen);

    public:
	int DoNotifyLiveProgramMgrVideoStauts(const SUserInfo &oUserInfo, unsigned int uiStatus, const SFrameVideo &oFrameVideo);
	int DoNotifyZongHengJiuZhouServer(const SUserInfo& oUserInfo);
        int del_flow(const SConnInfo& oConnInfo);

    private:
//============================================================================================================================================
        int process_p2p_http_req(const SConnInfo& oConnInfo, const char *pPkg, unsigned int uiPkgLen);
	int process_cdn_http_req(const SConnInfo& oConnInfo, const char *pPkg, unsigned int uiPkgLen);
	int get_http_flv_type(const char *pPkg, unsigned int uiPkgLen, SHttpRequestPara *pHttpRequestPara);
	int get_http_hlsm3u8_type(const char *pPkg, unsigned int uiPkgLen, SHttpRequestPara *pHttpRequestPara);
	int get_http_hlsts_type(const char *pPkg, unsigned int uiPkgLen, SHttpRequestPara *pHttpRequestPara);
	int get_http_app_type(const char *pPkg, unsigned int uiPkgLen, SHttpRequestPara *pHttpRequestPara);
    int get_http_xml_type(const char *pPkg, unsigned int uiPkgLen, SHttpRequestPara *pHttpRequestPara);
	int DoSend200ok2Cdn(const SVideoIdInfo &oVideoIdInfo, SFlowInfo &oFlowInfo);

        // added by dungdv
        int DoSend200ok2Client(const SConnInfo& oConnInfo);
        int DoSend404Error2Client(const SConnInfo& oConnInfo);
        int DoSendm3u8toClient(const SVideoIdInfo &oVideoIdInfo, const SConnInfo& oConnInfo);
        int DoSendTstoClient(const SHttpRequestPara &oHttpRequestPara, const SConnInfo& oConnInfo);
        int DoSendCrossDomain2Client(const SConnInfo& oConnInfo);

	int OnRecvAudioData(SUserInfo& oUserInfo, CVBuffer& buf);
	int OnRecvVideoPost(const SConnInfo& oConnInfo, const char *pPkg, unsigned int uiPkgLen);
	int OnVideoFrame(const SUserInfo &oUserInfo, SVideoIdInfo &oVideoIdInfo, const SFrameVideo &oFrameVideo);

	int DoSendToLiveProgrammgr(unsigned char *pPkg, unsigned int uiLen);
	int DoSendToZHJZGameSvr(unsigned char *pPkg, unsigned int uiLen);

	int ReleayFlvHead2HttpReq(const SUserInfo &oUserInfo, SVideoIdInfo &oVideoIdInfo, SVoiceChannel &oVoiceChannel);
	int RelayVideoFrame2HttpReq(const SUserInfo &oUserInfo, SVideoIdInfo &oVideoIdInfo, SVoiceChannel &oVoiceChannel, 
                           unsigned char *ucPkg, unsigned int uiPkgLen, unsigned int uiCdnSeq, unsigned char ucKeyFrame);

	int RelayFlvHead2CdnFlow(const SUserInfo& oUserInfo, SVideoIdInfo &oVideoIdInfo, SVoiceChannel &oVoiceChannel);
	int RelayFlvHead2RecordSvr(const SUserInfo& oUserInfo, SVideoIdInfo &oVideoIdInfo, SVoiceChannel &oVoiceChannel);

	int RelayVideoFrame2Cdn(const SUserInfo &oUserInfo, SVideoIdInfo &oVideoIdInfo, SVoiceChannel &oVoiceChannel, 
                              unsigned char *ucPkg, unsigned int uiPkgLen, unsigned int uiCdnSeq, unsigned char ucKeyFrame);

	int RelayVideo2CdnFlow(const SUserInfo& oUserInfo, SVideoIdInfo &oVideoIdInfo, SVoiceChannel &oVoiceChannel, 
                               unsigned char *pPkg, unsigned int uiPkgLen, unsigned int uiSeq, unsigned char ucKey);

	int RelayVideoFrame2RecordSvr(SVideoIdInfo &oVideoIdInfo, unsigned char *ucPkg, unsigned int uiPkgLen);
	int RelayM3u8ToHlsCacheSvr(char *buff, int len);

        int RelayVideoFrame2HttpReqWithBlock(const SUserInfo &oUserInfo, SVideoIdInfo &oVideoIdInfo, 
                            SVoiceChannel &oVoiceChannel, unsigned char *ucTagPkg, unsigned int uiTagPkgLen, 
                            unsigned char ucKeyFrame, unsigned int uiFrameTime);

        int RelayVideoFrame2CdnWithBlock(const SUserInfo &oUserInfo, SVideoIdInfo &oVideoIdInfo, 
                         SVoiceChannel &oVoiceChannel, unsigned char *ucTagPkg, unsigned int uiTagPkgLen, 
                         unsigned char ucKeyFrame, unsigned int uiFrameTime);

        int DoSendToFlow(const SFlowInfo& oVideoFlowInfo, const char* pData, const unsigned int uiSize);
        SVideoIdInfo* UpdateVideoIdInfo(unsigned int uiUin, unsigned int uiRoomId, unsigned int uiMainRoomId, 
                                        unsigned int uiVideoId, unsigned int uiChannelId);

	SVoiceChannel* allocate_voice_channel(unsigned int uiKey, bool &isNew);
	SVoiceChannel* get_voice_channel(unsigned int uiKey);
	int	release_voice_channel(unsigned int uiKey);
	SRoom2VideoId* alloc_roomid_2_videoid(unsigned int uiRoomId);
	SRoom2VideoId* get_roomid_2_videoid(unsigned int uiRoomId);

//============================================================================================================================================

    private:
        int rsp2Client(const SConnInfo &oConnInfo, const char *pData, unsigned int uiSize);

    private:
        CMCDProc	*m_pMCD;
	unsigned char	*pCompletFrameBuf;

    private:
        string			m_strHttpOkRsp2Cdn;
        string          m_strHttpFailRsp2Cdn;
        string			m_strHttpOkRsp2RecordSvr;
        string			m_strHttpResponse;
        string          m_strHttpCrossDomain;
        string          m_strHttpCrossDomainRsp;
	FILE			*flvFile;
	FILE			*flvOrgFile;
        
    };
}
#endif



