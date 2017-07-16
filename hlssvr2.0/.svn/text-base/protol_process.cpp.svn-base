#include <fstream>
#include <sstream>
#include <vector>

#include <stdio.h>
#include <time.h>

#include "tfc_debug_log.h"
#include "tfc_str.hpp"
#include "tfc_base_str.h"
#include "tfc_base_comm.h"

#include "oi_tea.h"
#include "gFunc.h"

#include "muxsvr.h"
#include "muxsvr_errno.h"
#include "muxsvr_mcd_proc.h"
#include "info_channel.h"
#include "voice_channel.h"
#include "protol_process.h"
#include "http.h"
#include "CP2PBlock.h"

#include <json/value.h>
#include <json/writer.h>
#include <json/reader.h>

#include "../lib/Attr_API.h"

#include "hls/hls_buf.h"
#include "hls/hls_mpegts.h"

using namespace std;
using namespace tfc::base;
using namespace muxsvr;

unsigned short usPorts[3] = {80,8000,443};

CProtol::CProtol(CMCDProc* proc)
    :m_pMCD(proc)
{
    m_strHttpOkRsp2Cdn.append("HTTP/1.1 200 OK\r\n");
    m_strHttpOkRsp2Cdn.append("Server: mobile stream\r\n\r\n");

    m_strHttpFailRsp2Cdn.append("HTTP/1.1 404 File Not Found\r\n");
    m_strHttpFailRsp2Cdn.append("Server: live_stream\r\n\r\n");

    m_strHttpResponse.append("HTTP/1.1 200 OK\r\n");
    //m_strHttpResponse.append("Content-Type: application/octet-stream\r\n");
    m_strHttpResponse.append("Content-Type: application/vnd.apple.mpegurl\r\n");
    m_strHttpResponse.append("Date: %s\r\n");
    m_strHttpResponse.append("Last-Modified: %s\r\n");
    m_strHttpResponse.append("Server: cctalk cdn\r\n");
    m_strHttpResponse.append("Access-Control-Allow-Origin: *\r\n");
    m_strHttpResponse.append("Content-Length: %u\r\n");
    m_strHttpResponse.append("Connection: close\r\n");
    m_strHttpResponse.append("Cache-Control: no-cache\r\n");
    m_strHttpResponse.append("Content-Ranges: bytes 0-%u/%u\r\n");
    m_strHttpResponse.append("\r\n");
    //m_strHttpResponse.append("%s");


    m_strHttpOkRsp2RecordSvr.append("HTTP/1.1 200 OK\r\n");
    m_strHttpOkRsp2RecordSvr.append("Channel: %u\r\n");
    m_strHttpOkRsp2RecordSvr.append("Anchor: %u\r\n");
    m_strHttpOkRsp2RecordSvr.append("Room: %u\r\n\r\n");


    m_strHttpCrossDomainRsp.append("HTTP/1.1 200 OK\r\n");
    m_strHttpCrossDomainRsp.append("Content-Type: application/xml\r\n");
    m_strHttpCrossDomainRsp.append("Date: %s\r\n");
    m_strHttpCrossDomainRsp.append("Last-Modified: %s\r\n");
    m_strHttpCrossDomainRsp.append("Server: cctalk cdn\r\n");
    m_strHttpCrossDomainRsp.append("Access-Control-Allow-Origin: *\r\n");
    m_strHttpCrossDomainRsp.append("Content-Length: %u\r\n");
    m_strHttpCrossDomainRsp.append("Connection: close\r\n");
    m_strHttpCrossDomainRsp.append("Cache-Control: no-cache\r\n");
    m_strHttpCrossDomainRsp.append("Content-Ranges: bytes 0-%u/%u\r\n");
    m_strHttpCrossDomainRsp.append("\r\n");


    m_strHttpCrossDomain.append("<?xml version=\"1.0\"?>\r\n");
    m_strHttpCrossDomain.append("<!DOCTYPE cross-domain-policy SYSTEM \"http://www.macromedia.com/xml/dtds/cross-domain-policy.dtd\">\r\n");
    m_strHttpCrossDomain.append("<cross-domain-policy>\r\n");
    m_strHttpCrossDomain.append("<allow-access-from domain=\"localhost\" to-ports=\"*\"/>\r\n");
    m_strHttpCrossDomain.append("<allow-http-request-headers-from domain=\"localhost\" secure=\"false\" headers=\"*\"/>\r\n ");
    m_strHttpCrossDomain.append("<allow-access-from domain=\"*.csmtalk.vn\" to-ports=\"*\"/>\r\n ");
    m_strHttpCrossDomain.append("<allow-http-request-headers-from domain=\"*.csmtalk.vn\" secure=\"false\" headers=\"*\"/>\r\n ");
    m_strHttpCrossDomain.append("<allow-access-from domain=\"*.zing.vn\" to-ports=\"*\"/>\r\n ");
    m_strHttpCrossDomain.append("<allow-http-request-headers-from domain=\"*.zing.vn\" secure=\"false\" headers=\"*\"/>\r\n ");
    m_strHttpCrossDomain.append("<allow-access-from domain=\"*.csmtalk.com.vn\" to-ports=\"*\"/>\r\n ");
    m_strHttpCrossDomain.append("<allow-http-request-headers-from domain=\"*.csmtalk.com.vn\" secure=\"false\" headers=\"*\"/>\r\n ");
    m_strHttpCrossDomain.append("<allow-access-from domain=\"*.csm.com.vn\" to-ports=\"*\"/>\r\n ");
    m_strHttpCrossDomain.append("<allow-http-request-headers-from domain=\"*.csm.com.vn\" secure=\"false\" headers=\"*\"/>\r\n ");
    m_strHttpCrossDomain.append("<allow-access-from domain=\"*.vcdn.vn\" to-ports=\"*\"/>\r\n ");
    m_strHttpCrossDomain.append("<allow-http-request-headers-from domain=\"*.vcdn.vn\" secure=\"false\" headers=\"*\"/>\r\n ");
    m_strHttpCrossDomain.append("<allow-access-from domain=\"*.talktv.vn\" to-ports=\"*\"/>\r\n ");
    m_strHttpCrossDomain.append("<allow-http-request-headers-from domain=\"*.talktv.vn\" secure=\"false\" headers=\"*\"/>\r\n ");
    m_strHttpCrossDomain.append("<allow-access-from domain=\"*.cctalk.vn\" to-ports=\"*\"/>\r\n ");
    m_strHttpCrossDomain.append("<allow-http-request-headers-from domain=\"*.cctalk.vn\" secure=\"false\" headers=\"*\"/>\r\n ");
    m_strHttpCrossDomain.append("</cross-domain-policy>");

    pCompletFrameBuf = new unsigned char[MAX_FRAME_SZIE];
    assert(NULL != pCompletFrameBuf);

    flvFile = fopen("../html/test.flv","wb");
    if ( flvFile == NULL )
         SUBMCD_DEBUG_P(LOG_ERROR,"CProtol failed to open file\n");

    SUBMCD_DEBUG_P(LOG_ERROR,"CProtol succeed to open file\n");

    flvOrgFile = fopen("../html/testorg.mp4","wb");
}

CProtol::~CProtol(void)
{
    if(pCompletFrameBuf)
    {
        delete []pCompletFrameBuf;
    }
}

void CProtol::process_cs_udp_msg(SConnInfo& oConnInfo, char *pPkg, unsigned int uiPkgLen)
{
    SUserInfo oUserInfo;
    oUserInfo.set_conn(oConnInfo);

    //0x02 0x03
    CVBuffer buf(pPkg, uiPkgLen);

    char cHead;
    buf >> cHead;  

    buf >> oUserInfo.usVer;
    buf >> oUserInfo.usCmd;
    buf >> oUserInfo.usSeq;
    buf >> oUserInfo.uiUin;

    buf >> oUserInfo.ucSubCmd;

    switch(oUserInfo.usCmd)
    {
    case CMD_VOICE:
        {
            switch(oUserInfo.ucSubCmd)
            {
            case 0x35:

                SUBMCD_DEBUG_P(LOG_TRACE,"CProtol::process_cs_udp_msg head: 0x%x, "
                                         "usVer: 0x%x, usCmd: 0x%x, usSeq: %d, uiUin: %u, "
                                         "ucSubCmd: 0x%x, voicesvr_ip: %s\n",
                    cHead,
                    oUserInfo.usVer,
                    oUserInfo.usCmd,
                    oUserInfo.usSeq,
                    oUserInfo.uiUin,
                    oUserInfo.ucSubCmd,
                    ip_to_str(oConnInfo.uiIp).c_str());

                OnRecvAudioData(oUserInfo, buf);
                break;

            default:
                {
                    SUBMCD_DEBUG_P(LOG_TRACE,"CProtol::process_cs_udp_msg unknow ucSubCmd "
                                             "head: 0x%x, usVer: 0x%x, usCmd: 0x%x, usSeq: %d, "
                                             "uiUin: %u, ucSubCmd: 0x%x\n",
                        cHead,
                        oUserInfo.usVer,
                        oUserInfo.usCmd,
                        oUserInfo.usSeq,
                        oUserInfo.uiUin,
                        oUserInfo.ucSubCmd);
                }
                break;
            }
        }
        break;

    default:
        {
            SUBMCD_DEBUG_P(LOG_TRACE,"CProtol::process_cs_udp_msg unknow usCmd "
                                     "head: 0x%x, usVer: 0x%x, usCmd: 0x%x, usSeq: %d, "
                                     "uiUin: %u, ucSubCmd: 0x%x\n",
                cHead,
                oUserInfo.usVer,
                oUserInfo.usCmd,
                oUserInfo.usSeq,
                oUserInfo.uiUin,
                oUserInfo.ucSubCmd);
        }
        break;
    }
}

int CProtol::DoNotifyLiveProgramMgrVideoStauts(const SUserInfo &oUserInfo, unsigned int uiStatus, const SFrameVideo &oFrameVideo)
{
    CVBuffer buf;

    DBPkgHead oDBPkgHead;
    RelayPkgHeadEx2 oRelayHead;
    oRelayHead.shExLen = htons(sizeof(RelayPkgHeadEx2));

    cld_pkg_head o_cld_pkg_head;
    o_cld_pkg_head.command = htons(CMD_LIVEPROGRAMMGR);

    buf << (unsigned short)0;
    buf << (unsigned char)0x0a;

    buf.Append((const char*)&oDBPkgHead,sizeof(oDBPkgHead));
    buf.Append((const char*)&oRelayHead,sizeof(oRelayHead));
    buf.Append((const char*)&o_cld_pkg_head,sizeof(o_cld_pkg_head));

    buf << (unsigned char)SUBCMD_NOTIFY_VIDEOID_STATUS;

    //required uint32 uiStatus			= 1;
    //required uint64 uiUin 			= 2;
    //required uint32 uiRoomId 			= 3;
    //required uint32 uiMainRoomId		= 4;
    //required uint32 uiRoomType		= 5;
    //required uint32 uiVideoId			= 6;
    //required uint32 uiSelfInternalIp	= 7;
    //required uint32 uiSelfExternalIp	= 8;

    SNotifyVideoStatus oNotify;
    oNotify.set_uistatus(uiStatus);
    oNotify.set_uiuin(oUserInfo.uiUin);
    oNotify.set_uiroomid(oUserInfo.uiRoomId);
    oNotify.set_uimainroomid(oUserInfo.uiMainRoomId);
    oNotify.set_uiroomtype(oUserInfo.ucRoomType);
    oNotify.set_uivideoid(oUserInfo.uiVideoId);
    oNotify.set_uiselfinternalip(m_pMCD->_cfg.m_uiSelfInternalIp);
    oNotify.set_uiselfexternalip(0);

    if (true == m_pMCD->_cfg.m_bUseFlashp2p)
    {
        oNotify.set_uibitrate(oFrameVideo.uiBitRate);
    }
    
    string strNotity;
    int ret = oNotify.SerializeToString(&strNotity);

    if(!ret)
    {
        SUBMCD_DEBUG_P(LOG_ERROR, "SerializeToString failed!\n");
        return -1;
    }

    buf.Append(strNotity.c_str(), strNotity.size());
    buf.end();

    buf.setTcpMsgLen(buf.getSize());

    SUBMCD_DEBUG_P(LOG_DEBUG,"CProtol::DoNotifyLiveProgramMgrVideoStauts uin: %u, status: %u, roomid: %u, mainroomid: %u, videoid: %u\n",
        oUserInfo.uiUin,
        uiStatus,
        oUserInfo.uiRoomId,
        oUserInfo.uiMainRoomId,
        oUserInfo.uiVideoId);

    return DoSendToLiveProgrammgr((unsigned char*)buf.getBuffer(),buf.getSize());
}

int CProtol::DoSendToLiveProgrammgr(unsigned char *pPkg, unsigned int uiLen)
{
    if(NULL == pPkg || 0 == uiLen)
    {
        return -1;
    }

    unsigned int uiIp		= m_pMCD->_cfg.m_uiLiveProgramMgrIp;
    unsigned short usPort	= m_pMCD->_cfg.m_usLiveProgramMgrPort;

    return m_pMCD->enqueue_2_dcc2((const char*)pPkg,uiLen,uiIp,usPort);
}

int CProtol::DoNotifyZongHengJiuZhouServer(const SUserInfo& oUserInfo)
{
    //required uint32 uin 			= 1;           	//�û�UIN
    //required uint32 roome_id 		= 2;	    	//����id
    //required uint32 videoid_type 	= 3;  			//������ͣ�1:��Ƶ 2:����Ƶ��
    //required uint32 sessionid		= 4;			//һֱ���ϼ�
    //required uint32 time			= 5;			//��ǰʱ��

    static unsigned int uiSessionId = 0;

    QtPushMessage oPush;

    oPush.set_uin(oUserInfo.uiUin);
    oPush.set_room_id(oUserInfo.uiRoomId);
    oPush.set_videoid_type(AUDIO_WITH_VIDEO);
    oPush.set_sessionid(uiSessionId);
    oPush.set_time(m_pMCD->m_cur_time);

    string strPush;

    bool bRet = oPush.SerializeToString(&strPush);
    if(!bRet)
    {
        SUBMCD_DEBUG_P(LOG_ERROR,"CProtol::DoNotifyZongHengJiuZhouServer error uin: %u, roomid: %u\n", 
            oUserInfo.uiUin,
            oUserInfo.uiRoomId);

        return -1;
    }

    ClientHeader oClientHeader;

    oClientHeader.uiLength		= htonl(sizeof(ClientHeader) + strPush.size());
    oClientHeader.uiUin			= htonl(oPush.uin());
    oClientHeader.ushCmdID		= htons((unsigned short)QT_CMD_QT_PUSH);

    CVBuffer oBuf;
    oBuf.Append((const char*)&oClientHeader,sizeof(ClientHeader));
    oBuf.Append((const char*)strPush.c_str(), strPush.size());

    SUBMCD_DEBUG_P(LOG_DEBUG,"DoNotifyZongHengJiuZhouServer uin: %u, roomid: %u, vid: %u, sessionid: %u\n",
        oUserInfo.uiUin,
        oUserInfo.uiRoomId,
        oUserInfo.uiVideoId,
        uiSessionId);

    uiSessionId++;

    DoSendToZHJZGameSvr((unsigned char*)oBuf.getBuffer(), oBuf.getSize());

    return 0;
}

int CProtol::DoSendToZHJZGameSvr(unsigned char *pPkg, unsigned int uiLen)
{
    if(NULL == pPkg || 0 == uiLen)
    {
        return -1;
    }

    unsigned int uiIp		= m_pMCD->_cfg.m_uiZHJZGameSvrIp;
    unsigned short usPort	= m_pMCD->_cfg.m_usZHJZGameSvrPort;

    return m_pMCD->enqueue_2_dcc2((const char*)pPkg,uiLen,uiIp,usPort);
}

SVoiceChannel* CProtol::allocate_voice_channel(unsigned int uiKey, bool &isNew)
{
    return m_pMCD->m_pDataChannelMgr->allocate_voice_channel(uiKey,isNew);
}

SVoiceChannel* CProtol::get_voice_channel(unsigned int uiKey)
{
	return m_pMCD->m_pDataChannelMgr->get_voice_channel(uiKey);
}

int CProtol::release_voice_channel(unsigned int uiKey)
{
	return m_pMCD->m_pDataChannelMgr->release_voice_channel(uiKey);
}

SRoom2VideoId* CProtol::alloc_roomid_2_videoid(unsigned int uiRoomId)
{
    return m_pMCD->m_infoChannelMgr->alloc_roomid_2_videoid(uiRoomId);
}

SRoom2VideoId* CProtol::get_roomid_2_videoid(unsigned int uiRoomId)
{
    return m_pMCD->m_infoChannelMgr->get_roomid2videoid(uiRoomId);
}

int CProtol::OnRecvAudioData(SUserInfo& oUserInfo, CVBuffer& buf)
{
    unsigned int uiUin		= 0;
    unsigned char ucRoomType	= 0;

    buf >> uiUin;
    buf >> ucRoomType;
    buf >> oUserInfo.uiRoomId;

    SRoom2VideoId *pRoom2VideoId = get_roomid_2_videoid(oUserInfo.uiRoomId);

    if(pRoom2VideoId == NULL)
    {
        SUBMCD_DEBUG_P(LOG_DEBUG,"CProtol::OnRecvAudioData get_roomid_2_videoid NULL"
                                 " uin: %u, roomid: %u, seq: %u\n",
            oUserInfo.uiUin,
            oUserInfo.uiRoomId,
            oUserInfo.usSeq);

        return -1;
    }

    SUBMCD_DEBUG_P(LOG_DEBUG,"CProtol::OnRecvAudioData get_roomid_2_videoid "
                             "uin: %u, roomid: %u, videoid: %u, videouin: %u, seq: %u\n",
        oUserInfo.uiUin,
        oUserInfo.uiRoomId,
        pRoom2VideoId->uiVideoId,
        pRoom2VideoId->uiVideoUin,
        oUserInfo.usSeq);


    if(pRoom2VideoId->uiVideoUin != oUserInfo.uiUin)
    {
        SUBMCD_DEBUG_P(LOG_DEBUG,"CProtol::OnRecvAudioData skip not_video_player "
                                 "uin: %u, roomid: %u, videoid: %u, seq: %u\n",
            oUserInfo.uiUin,
            oUserInfo.uiRoomId,
            pRoom2VideoId->uiVideoId,
            oUserInfo.usSeq);

        return -1;
    }


    //uiVideoId means original videoid
    unsigned int uiVoiceChannelKey = pRoom2VideoId->uiVideoId;

    if(0 == uiVoiceChannelKey)
    {
        SUBMCD_DEBUG_P(LOG_ERROR,"CProtol::OnRecvAudioData uiVoiceChannelKey error "
                                 "uin: %u, roomid: %u, uiVoiceChannelKey: %u\n",
            oUserInfo.uiUin,
            oUserInfo.uiRoomId,
            uiVoiceChannelKey);

        return -1;
    }

    bool bCreateFlag = false;
    SVoiceChannel *pVoiceChannel = allocate_voice_channel(uiVoiceChannelKey,bCreateFlag);

    if(pVoiceChannel == NULL)
    {
        SUBMCD_DEBUG_P(LOG_ERROR,"CPrtol::OnRecvAudioInfo allocate_voice_channel error "
                                 "uiChannelKey: %u, uin: %u, roomid: %u, seq: %u\n", 
            uiVoiceChannelKey,
            oUserInfo.uiUin, 
            oUserInfo.uiRoomId,
            oUserInfo.usSeq);

        return -1;
    }

    SUBMCD_DEBUG_P(LOG_DEBUG,"CProtol::OnRecvAudioInfo allocate_voice_channel "
                             "uiCHannelKey: %u, uin: %u, roomid: %u, seq: %u, size: %u\n",
        uiVoiceChannelKey,
        oUserInfo.uiUin,
        oUserInfo.uiRoomId,
        oUserInfo.usSeq,
        buf.RemainSize());

    unsigned  char* pTmp = (unsigned char*)buf.JumpBuffer() + 15;
    if(pTmp[0] == 0xFF && 
		(pVoiceChannel->adtsheader[2] != pTmp[2] || pVoiceChannel->adtsheader[3] != pTmp[3] ) )	
    {
   	memcpy(pVoiceChannel->adtsheader,pTmp,sizeof(pVoiceChannel->adtsheader));
	pVoiceChannel->m_bAudioChanged = true;
    }

    { // hls handler
       char *audioBuf = buf.JumpBuffer();
       unsigned char  ucHighlen = 0, ucMiddlelen = 0, ucLowlen = 0;
       int audioLen = 0;
       unsigned int audioTime = 0;

       ucHighlen      = *(audioBuf+18);
       ucMiddlelen    = *(audioBuf+19);
       ucLowlen       = *(audioBuf+20);
   
       audioLen = ((ucHighlen & 0x03) << 11)|(ucMiddlelen << 3)|(ucLowlen >> 5);
       audioTime = ntohl(*(unsigned int*)(audioBuf + buf.RemainSize() - 5));


       //FIXME: audioLen may be too large 
       /*if ( buf.RemainSize() - 29 < audioLen ) {

           SUBMCD_DEBUG_P(LOG_ERROR,"CProtol::OnRecvAudioInfo wrong buffer length, audioLen=%u, buflen=%u",
                                    audioLen, buf.RemainSize() - 29);
           audioLen = buf.RemainSize() - 29;
       }*/

       if ( pVoiceChannel->hls_session != NULL ) {

          pVoiceChannel->hls_session->current_ts = time(0);

          hls_audio_handler(pVoiceChannel->hls_session,audioBuf+22,audioLen-7,audioTime);

          std::vector<char> buff;

          buff.resize(sizeof(int)+sizeof(pVoiceChannel->hls_session->videoid));
          uint32_t type;
          type = 0;

          memcpy(&buff[0],&type,sizeof(uint32_t));
          memcpy(&buff[0]+sizeof(uint32_t),&(pVoiceChannel->hls_session->videoid),sizeof(uint32_t));
       }
    } 

    return pVoiceChannel->pushBackAudioFrame(oUserInfo, oUserInfo.usSeq, 
             (unsigned char*)buf.JumpBuffer(), buf.RemainSize(),m_pMCD->m_cur_time);
}

unsigned int g_iOrgTotalsize = 0, g_iCount = 0, g_iTotalsize = 0, g_iFlvCount = 0;
bool g_bFirstFile = true;

int CProtol::OnVideoFrame(const SUserInfo &oUserInfo, 
    SVideoIdInfo &oVideoIdInfo, const SFrameVideo &oFrameVideo)
{
    //����roomid -> videoid ӳ�䣬����videoid������ͨ��ʹ��
    SRoom2VideoId *pRoom2VideoId = alloc_roomid_2_videoid(oUserInfo.uiRoomId);

    if(NULL == pRoom2VideoId)
    {
        SUBMCD_DEBUG_P(LOG_ERROR, "CProtol::OnVideoFrame alloc_roomid_2_videoid return NULL uin:"
                                  " %u, roomid: %u, videoid: %u, seq: %u\n",
            oUserInfo.uiUin,
            oUserInfo.uiRoomId,
            oUserInfo.uiVideoId,
            oFrameVideo.uiFrameSeq);

        return -1;
    }

    SUBMCD_DEBUG_P(LOG_DEBUG, "CProtol::OnVideoFrame alloc_roomid_2_videoid"
                              " uin: %u, roomid: %u, videoid: %u, seq: %u\n",
        oUserInfo.uiUin,
        oUserInfo.uiRoomId,
        oUserInfo.uiVideoId,
        oFrameVideo.uiFrameSeq);

    pRoom2VideoId->updateRoomId2VideoIdInfo(oUserInfo.uiOriginalVideoId, 
                             oUserInfo.uiUin, (unsigned int)m_pMCD->m_cur_time);

    unsigned int uiVoiceChannelKey = oUserInfo.uiOriginalVideoId;

    if (0 == uiVoiceChannelKey)
    {
        SUBMCD_DEBUG_P(LOG_ERROR,"CProtol::OnVideoFrame uiVoiceChannelKey error,"
                                 " uin: %u, roomid: %u, roomtype: %u, uiVoiceChannelKey: %u\n",
            oUserInfo.uiUin,
            oUserInfo.uiRoomId,
            oUserInfo.ucRoomType,
            uiVoiceChannelKey);

        return -1;
    }

    bool isNewChannel = false;
    SVoiceChannel *pVoiceChannel = allocate_voice_channel(uiVoiceChannelKey,isNewChannel);

    if (NULL == pVoiceChannel)
    {
        SUBMCD_DEBUG_P(LOG_ERROR,"CProtol::OnVideoFrame allocate_voice_channel failed,"
                                 " uin: %u, roomid: %u, roomtype: %u, uiVoiceChannelKey: %u\n",
            oUserInfo.uiUin,
            oUserInfo.uiRoomId,
            oUserInfo.ucRoomType,
            uiVoiceChannelKey);

        return -1;
    }

    //if ( isNewChannel ) {
    if ( pVoiceChannel->hls_session == NULL ) {
        // Init hls stream;
        pVoiceChannel->hls_session = hls_create_session(oUserInfo.uiVideoId,
                                                        m_pMCD->_cfg.m_uiTsDuration,
                                                        m_pMCD->_cfg.m_uiMaxAudioDelay);
        //pVoiceChannel->hls_session = hls_create_session(oUserInfo.uiRoomId);

        if ( pVoiceChannel->hls_session == NULL ) {
           SUBMCD_DEBUG_P(LOG_ERROR,"CProtol::OnVideoFrame failed to create hls session,"
                                 "ret=%u,uin:%u,roomid:%u,roomtype:%u, uiVoiceChannelKey:%u\n",
               pVoiceChannel->hls_session == NULL? 1 : 0,
               oUserInfo.uiUin,
               oUserInfo.uiRoomId,
               oUserInfo.ucRoomType,
               uiVoiceChannelKey);
          
            return -1;
        }

        pVoiceChannel->hls_session->hMCD =(void*)m_pMCD;

        pVoiceChannel->hls_session->roomid = oUserInfo.uiRoomId;

       
        SUBMCD_DEBUG_P(LOG_ERROR,"CProtol::OnVideoFrame create hls session,"
                                 "ret=%u,uin:%u,roomid:%u,roomtype:%u, uiVoiceChannelKey:%u, videoid:%u\n",
               pVoiceChannel->hls_session == NULL? 1 : 0,
               oUserInfo.uiUin,
               oUserInfo.uiRoomId,
               oUserInfo.ucRoomType,
               uiVoiceChannelKey,
               oUserInfo.uiVideoId);
    } else {
        if ( pVoiceChannel->hls_session->videoid != oUserInfo.uiVideoId ) {
              SUBMCD_DEBUG_P(LOG_ERROR,"CProtol::OnVideoFrame use old hls session,"
                                 "old_videoid=%u,uin:%u,roomid:%u,roomtype:%u, uiVoiceChannelKey:%u, videoid:%u\n",
              pVoiceChannel->hls_session->videoid,
              oUserInfo.uiUin,
              oUserInfo.uiRoomId,
              oUserInfo.ucRoomType,
              uiVoiceChannelKey,
              oUserInfo.uiVideoId);

              //hls_session_t *oldSession = pVoiceChannel->hls_session;


              pVoiceChannel->hls_session = hls_create_session(oUserInfo.uiVideoId,m_pMCD->_cfg.m_uiTsDuration,
                                                              m_pMCD->_cfg.m_uiMaxAudioDelay);
              
              if ( pVoiceChannel->hls_session == NULL ) {
                 SUBMCD_DEBUG_P(LOG_ERROR,"CProtol::OnVideoFrame failed to create hls session,"
                                 "ret=%u,uin:%u,roomid:%u,roomtype:%u, uiVoiceChannelKey:%u\n",
                     pVoiceChannel->hls_session == NULL? 1 : 0,
                     oUserInfo.uiUin,
                     oUserInfo.uiRoomId,
                     oUserInfo.ucRoomType,
                     uiVoiceChannelKey);
          
                  return -1;
              }




              pVoiceChannel->hls_session->hMCD =(void*)m_pMCD;

              //copy avc_header
              //hls_copy_session(oldSession, pVoiceChannel->hls_session);

              pVoiceChannel->hls_session->roomid = oUserInfo.uiRoomId;


        }
    }
    //}



    pVoiceChannel->hls_session->current_ts = time(0);

    SUBMCD_DEBUG_P(LOG_TRACE,"CProtol::OnVideoFrame allocate_voice_channel"
                             " uin: %u, roomid: %u, uiChannelId: %u, uiChannelKey: %u, videoid: %u\n",
        oUserInfo.uiUin,
        oUserInfo.uiRoomId,
        pVoiceChannel->getChannelId(),
        uiVoiceChannelKey,
        oUserInfo.uiVideoId);

    //����ͨ����Ϣ��VideoIdInfo��Ϣ
    pVoiceChannel->setUserInfo(oUserInfo.uiUin, oUserInfo.uiRoomId, 
                       oUserInfo.ucRoomType, oUserInfo.uiVideoId, oUserInfo.usVer);
    pVoiceChannel->setCreateTime(m_pMCD->m_ll_now_Tick);

    //�л�����ʱ, ��10s��Ϊ��ʱ��cdn�������·���flvͷ��
    SVideoIdInfo *pVideoIdInfo = UpdateVideoIdInfo(oUserInfo.uiUin, oUserInfo.uiRoomId, 
                      oUserInfo.uiMainRoomId, oUserInfo.uiVideoId, pVoiceChannel->getChannelId());

    SUBMCD_DEBUG_P(LOG_TRACE,"CProtol::OnVideoFrame check log,"
                                 "old_videoid=%u,uin:%u,roomid:%u,roomtype:%u, uiVoiceChannelKey:%u, videoid:%u\n",
              pVoiceChannel->hls_session->videoid,
              oUserInfo.uiUin,
              oUserInfo.uiRoomId,
              oUserInfo.ucRoomType,
              uiVoiceChannelKey,
              oUserInfo.uiVideoId);

    if(NULL == pVideoIdInfo)
    {
        SUBMCD_DEBUG_P(LOG_ERROR,"CProtol::OnVideoFrame UpdateVideoIdInfo fail,"
                                 " uin: %u, roomid: %u, videoid: %u\n",
            oUserInfo.uiUin,
            oUserInfo.uiRoomId,
            oUserInfo.uiVideoId);

        return -1;
    }

    pVoiceChannel->pushBackVideoFrame(oUserInfo, oFrameVideo, m_pMCD->m_ll_now_Tick);

    SUBMCD_DEBUG_P(LOG_TRACE, "CProtol::OnVideoFrame pushBackVideoFrame,"
                              " uin: %u, roomid: %u, seq: %u, uiVideoId: %u\n",      
        oUserInfo.uiUin,
        oUserInfo.uiRoomId,
        oFrameVideo.uiFrameSeq,
        oUserInfo.uiVideoId);

    unsigned int uiNumOfFrames = 1;

    float fCurAverageRWDistance		= pVoiceChannel->getAverageRWDistance();
    unsigned int uiCurRWDistance	= pVoiceChannel->getRWDistance();

    if(fCurAverageRWDistance > 10.0 && uiCurRWDistance >10)
    {
        uiNumOfFrames = 10;
    }

    //����Ƶ֡���ʱ��׷��һ��


    SUBMCD_DEBUG_P(LOG_TRACE,"CProtol::OnVideoFrame check log, numframe=%u\n",uiNumOfFrames);
    for(unsigned int i=0; i<uiNumOfFrames; i++)
    {
        const SFrameVideoItem *pFrameVideoItem	= NULL;
        const char *pFrameVideoItemBuffer		= NULL;

        int iRet = 0;

        int ret = pVoiceChannel->getFrameBufferInOrder(&pFrameVideoItem,
                                     &pFrameVideoItemBuffer,m_pMCD->m_ll_now_Tick);

        if(ret < 0)
        {
            SUBMCD_DEBUG_P(LOG_TRACE,"CProtol::OnVideoFrame getFrameBufferInOrder,"
                                     " ret: %d, uin: %u, uiWriteSeq: %u, uiReadSeq: %u, roomid: %u, videoid: %u\n",
                ret,
                oUserInfo.uiUin,
                oFrameVideo.uiFrameSeq,
                pVoiceChannel->getVideoReadSeq(),
                oUserInfo.uiRoomId,
                oUserInfo.uiVideoId);

            return -1;
        }

        SUBMCD_DEBUG_P(LOG_DEBUG,"CProtol::OnVideoFrame getFrameBufferInOrder,"
                                 " uin: %u, roomid: %u, videoid: %u, uiWriteSeq: %u,"
                                 " uiReadSeq: %u, uiMemSeq: %u, len: %u\n",
            oUserInfo.uiUin,
            oUserInfo.uiRoomId,
            oUserInfo.uiVideoId,
            oFrameVideo.uiFrameSeq,
            pVoiceChannel->getVideoReadSeq(),
            pFrameVideoItem->uiFrameSeq,
            pFrameVideoItem->uiFrameSize);

        if(pFrameVideoItem->uiFrameSize == 0)
        {
            SUBMCD_DEBUG_P(LOG_TRACE,"CProtol::OnVideoFrame getFrameBufferInOrder, size=0"
                                     " uin: %u, roomid: %u, videoid: %u\n",
                oUserInfo.uiUin,
                oUserInfo.uiRoomId,
                oUserInfo.uiVideoId);

            return -1;
        }

        unsigned char *pData = NULL;

        if(pFrameVideoItem->uiStart + pFrameVideoItem->uiFrameSize <= G_UIFRAMEDATABUFFSZIE)
        {
            pData	= (unsigned char *)(pFrameVideoItemBuffer + pFrameVideoItem->uiStart);
        }
        else
        {
            unsigned int uiFirstCopyLen = G_UIFRAMEDATABUFFSZIE - pFrameVideoItem->uiStart;

            memcpy(pCompletFrameBuf, pFrameVideoItemBuffer + pFrameVideoItem->uiStart, uiFirstCopyLen);
            memcpy(pCompletFrameBuf + uiFirstCopyLen, pFrameVideoItemBuffer, pFrameVideoItem->uiFrameSize - uiFirstCopyLen);

            pData	= pCompletFrameBuf;
        }

        unsigned int uiFlvOffset	= 0;
        unsigned int uiFrameLen		= pFrameVideoItem->uiFrameSize;

        //��ʼ������ͷ
        if(1 == pFrameVideoItem->ucKeyFrame && 
           (!pVoiceChannel->m_flv_parser.b_sps_pps_init 
             || pVoiceChannel->m_oFrameBuffer.needResetFlag() 
             || pVoiceChannel->m_bAudioChanged))
        {	
            pVoiceChannel->m_flv_parser.open_flv(oFrameVideo.uiWidth,oFrameVideo.uiHeight,
                                oFrameVideo.uiBitRate,oFrameVideo.uiFps);

            SUBMCD_DEBUG_P(LOG_ERROR, "CProtol::OnVideoFrame open_flv,"
                                      " uin: %u, roomid: %u, videoid: %u,"
                                      " width: %u, height: %u, bitrate: %u, fps: %u\n", 
                oUserInfo.uiUin,
                oUserInfo.uiRoomId,
                oUserInfo.uiVideoId,
                oFrameVideo.uiWidth,
                oFrameVideo.uiHeight,
                oFrameVideo.uiBitRate,
                oFrameVideo.uiFps); 

            pVoiceChannel->m_flv_parser.parser_header(pVoiceChannel->hls_session,pData,uiFrameLen,
                     flvFile/*NULL*/, uiFlvOffset,pFrameVideoItem->ucKeyFrame,pVoiceChannel->adtsheader);
 
          
            pVoiceChannel->hls_session->fps = oFrameVideo.uiFps;

	        pVoiceChannel->m_bAudioChanged = false;
            pVoiceChannel->m_flv_parser.b_sps_pps_init = true;
            pVoiceChannel->m_oFrameBuffer.flipNeedReset();

            //֪ͨLiveProgramMgr��Ƶ����
            DoNotifyLiveProgramMgrVideoStauts(oUserInfo, VIDEO_START, oFrameVideo);

            pVoiceChannel->m_LastTickVideoIdUpdateTime = m_pMCD->m_cur_time;

            if(uiFlvOffset < MAX_FLV_HEAD_LEN)
            {
                memcpy(pVoiceChannel->m_cdnFlvHead.flvHeadBuf, pVoiceChannel->m_flv_parser.getBuffer(), uiFlvOffset);
                pVoiceChannel->m_cdnFlvHead.m_uiFlvHeaderLen = uiFlvOffset;


                ReleayFlvHead2HttpReq(oUserInfo, oVideoIdInfo, *pVoiceChannel);
            }
            else
            {

                pVoiceChannel->m_flv_parser.b_sps_pps_init = false;

                SUBMCD_DEBUG_P(LOG_ERROR, "CProtol::OnVideoFrame flv_header need more space,"
                                          " uin: %u, roomid: %u, videoid: %u, len: %u\n", 
                    oUserInfo.uiUin,
                    oUserInfo.uiRoomId,
                    oUserInfo.uiVideoId,
                    uiFlvOffset);
                pVoiceChannel->m_flv_parser.printf_buf_data(pData, uiFrameLen, 
                         pFrameVideoItem->ucKeyFrame, pFrameVideoItem->uiFrameSeq, 0);
            }
            pVoiceChannel->m_flv_parser.flush();
        }

        if(!pVoiceChannel->m_flv_parser.b_sps_pps_init)
        {
                SUBMCD_DEBUG_P(LOG_ERROR, "CProtol::OnVideoFrame sps pps not init,"
                                          " uin: %u, roomid: %u, videoid: %u\n", 
                    oUserInfo.uiUin,
                    oUserInfo.uiRoomId,
                    oUserInfo.uiVideoId);
            continue;
        }

        //����֪ͨLiveProgramMgr videoid����,10s
        if(abs((int)(m_pMCD->m_cur_time - pVoiceChannel->m_LastTickVideoIdUpdateTime)) > 10)
        {
            DoNotifyLiveProgramMgrVideoStauts(oUserInfo,VIDEO_HEART_BEAT, oFrameVideo);

            DoNotifyZongHengJiuZhouServer(oUserInfo);

            pVoiceChannel->m_LastTickVideoIdUpdateTime = m_pMCD->m_cur_time;
        }

        unsigned char *pAudioData = NULL;
        unsigned int uiAudioSeq   = 0;
        unsigned int uiAudioSize  = 0;
        unsigned int uiAudioTime  = 0;

        pVoiceChannel->m_flv_parser.m_uiCurTimeStamp = pFrameVideoItem->uiFrameTime;

        SUBMCD_DEBUG_P(LOG_TRACE,"CProtol::OnVideoFrame parser_body_for_video"
            " uin: %u, roomid: %u, videoid: %u, uiFrameTime: %u, seq: %u, key: 0x%x, len: %u\n",
            oUserInfo.uiUin,
            oUserInfo.uiRoomId,
            oUserInfo.uiVideoId,
            pFrameVideoItem->uiFrameTime, 
            pFrameVideoItem->uiFrameSeq, 
            pFrameVideoItem->ucKeyFrame, 
            uiFrameLen);

            /*video handle*/
        iRet = pVoiceChannel->m_flv_parser.parser_body_for_video(pVoiceChannel->hls_session,
                                       pData, uiFrameLen, uiFlvOffset, 
                                       pFrameVideoItem->ucKeyFrame, pFrameVideoItem->uiFrameSeq, 
                                       oUserInfo.uiRoomId, m_pMCD->_cfg.m_uiSubRoomRecord);

        if(iRet != 0)
        {
            SUBMCD_DEBUG_P(LOG_ERROR,"CProtol::OnVideoFrame parser_body_for_video fail"
                " uin: %u, roomid: %u, videoid: %u, uiFrameTime: %u, seq: %u, key: 0x%x, len: %u\n",
                oUserInfo.uiUin,
                oUserInfo.uiRoomId,
                oUserInfo.uiVideoId,
                pFrameVideoItem->uiFrameTime, 
                pFrameVideoItem->uiFrameSeq, 
                pFrameVideoItem->ucKeyFrame, 
                uiFrameLen);

            return -1;
        }

        if(pVoiceChannel->m_flv_parser.getFlvOffset() < 11)
        {
            SUBMCD_DEBUG_P(LOG_TRACE,"CProtol::OnVideoFrame parser_body_for_video getFlvOffset < 11"
                                     " uin: %u, roomid: %u, videoid: %u, len: %u\n", 
                oUserInfo.uiUin,
                oUserInfo.uiRoomId,
                oUserInfo.uiVideoId,
                pVoiceChannel->m_flv_parser.getFlvOffset());           
        }
        else
        {
            if((0x09 != *(pVoiceChannel->m_flv_parser.m_pflv->c->data))&&
               (0x08 !=*(pVoiceChannel->m_flv_parser.m_pflv->c->data)))
            {

                SUBMCD_DEBUG_P(LOG_TRACE,"CProtol::OnVideoFrame FLVPARSERWriteflvfileError "
                        "[%u][%02X][%02X][%02X][%02X][%02X][%02X][%02X][%02X][%02X][%02X][%02X]\n ", 
                    pVoiceChannel->m_flv_parser.m_pflv->c->d_cur,
                    *(pVoiceChannel->m_flv_parser.m_pflv->c->data),
                    *(pVoiceChannel->m_flv_parser.m_pflv->c->data+1),
                    *(pVoiceChannel->m_flv_parser.m_pflv->c->data+2),
                    *(pVoiceChannel->m_flv_parser.m_pflv->c->data+3),
                    *(pVoiceChannel->m_flv_parser.m_pflv->c->data+4),
                    *(pVoiceChannel->m_flv_parser.m_pflv->c->data+5),
                    *(pVoiceChannel->m_flv_parser.m_pflv->c->data+6),
                    *(pVoiceChannel->m_flv_parser.m_pflv->c->data+7),
                    *(pVoiceChannel->m_flv_parser.m_pflv->c->data+8),
                    *(pVoiceChannel->m_flv_parser.m_pflv->c->data+9),
                    *(pVoiceChannel->m_flv_parser.m_pflv->c->data+10));
            }
        }

        unsigned char *pFlvTagData	= pVoiceChannel->m_flv_parser.getTagBuffer();
        unsigned int uiFlvLen	= pVoiceChannel->m_flv_parser.getFlvOffset();

        if((NULL != flvFile)&&
           (m_pMCD->_cfg.m_uiSubRoomRecord == oUserInfo.uiRoomId))
        {
           int n,fd;

           fd  = open("../html/live.flv", O_CREAT|O_WRONLY|O_APPEND, 0644);
           if ( fd < 0 ) {
               SUBMCD_DEBUG_P(LOG_ERROR,"FLVPARSER WriteFlvData failed to open file, ret=%u\n",fd);
           }

           SUBMCD_DEBUG_P(LOG_ERROR,"FLVPARSER WriteFlvData g_iOrgTotalsize=%u,g_iTotalsize=%u\n",g_iOrgTotalsize, g_iTotalsize);
           if(g_iOrgTotalsize >0)
           {
                if(0 == g_iTotalsize)
                {								
                    n = write(fd,(unsigned char *)(pVoiceChannel->m_cdnFlvHead.flvHeadBuf),pVoiceChannel->m_cdnFlvHead.m_uiFlvHeaderLen);
                    if ( n <= 0 ) {
                       SUBMCD_DEBUG_P(LOG_ERROR,"FLVPARSER WriteFlvData failed to write file 2,"
                            "ret=%u, size=%u\n",n,pVoiceChannel->m_cdnFlvHead.m_uiFlvHeaderLen);
                    }
 
                    fwrite((unsigned char *)(pVoiceChannel->m_cdnFlvHead.flvHeadBuf), 1, 
                                    pVoiceChannel->m_cdnFlvHead.m_uiFlvHeaderLen, flvFile);
                    g_iTotalsize += pVoiceChannel->m_cdnFlvHead.m_uiFlvHeaderLen;

                    SUBMCD_DEBUG_P(LOG_ERROR, "FLVPARSER WriteFlvData cnt=%u,headerlen=%u,total_size=%d,org_total_size=%d\n", 
                         g_iFlvCount, pVoiceChannel->m_cdnFlvHead.m_uiFlvHeaderLen, g_iTotalsize, g_iOrgTotalsize);

                    g_iFlvCount++;			
                }

                n = write(fd,pFlvTagData+sizeof(tagFLVHEAD),uiFlvLen);
                if ( n <= 0 ) {
                    SUBMCD_DEBUG_P(LOG_ERROR,"FLVPARSER WriteFlvData failed to write file 2,"
                        "ret=%u, size=%u\n",n,uiFlvLen);
                }

                fwrite(pFlvTagData+sizeof(tagFLVHEAD), 1, uiFlvLen, flvFile);

                g_iTotalsize += uiFlvLen;
                SUBMCD_DEBUG_P(LOG_ERROR, "FLVPARSER WriteFlvData cnt=%u,datalen=%u,total_size=%d,org_total_size=%d\n", 
                      g_iFlvCount, uiFlvLen, g_iTotalsize, g_iOrgTotalsize);
                g_iFlvCount++;
           }

        }
        
/*
        if (true == m_pMCD->_cfg.m_bUseFlashp2p)
        {
            RelayVideoFrame2HttpReqWithBlock(oUserInfo, 
                oVideoIdInfo, 
                *pVoiceChannel,
                pFlvTagData,
                uiFlvLen+sizeof(tagFLVHEAD),
                pFrameVideoItem->ucKeyFrame,
                pFrameVideoItem->uiFrameTime);
        }
        else
        {
            RelayVideoFrame2HttpReq(oUserInfo, 
                oVideoIdInfo, 
                *pVoiceChannel,
                pFlvTagData,
                uiFlvLen+sizeof(tagFLVHEAD),
                pVoiceChannel->m_uiCdnSeqNum++,
                pFrameVideoItem->ucKeyFrame);
        }
*/
        pVoiceChannel->m_flv_parser.flush();

        //jump out of while
        /*audio handle*/
        while(1)
        {
            ret = pVoiceChannel->getAudioFrameInOrder(uiAudioSeq, &pAudioData, uiAudioSize, uiAudioTime, m_pMCD->m_cur_time);

            if(ret < 0)
            {
                /*audio packet buf is empty*/
                SUBMCD_DEBUG_P(LOG_TRACE,"CProtol::OnVideoFrame getAudioFrameInOrder "
                         "ret: %d, uin: %u, roomid: %u, videoid: %u, uiReadSeq: %u, uiWriteSeq: %u\n", 
                    ret, 
                    oUserInfo.uiUin,
                    oUserInfo.uiRoomId,
                    oUserInfo.uiVideoId,
                    pVoiceChannel->m_oAudioBuffer.getReadSeq(),
                    pVoiceChannel->m_oAudioBuffer.getWriteSeq());

                break;
            }

            SUBMCD_DEBUG_P(LOG_TRACE,"CProtol::OnVideoFrame getAudioFrameInOrder "
                             "ret: %d, uin: %u, roomid: %u, videoid: %u, uiReadSeq: %u, "
                             "uiWriteSeq: %u, uiAudioTime: %u, uiAudioSeq: %u, uiAudioSize: %u\n", 
                ret, 
                oUserInfo.uiUin,
                oUserInfo.uiRoomId,
                oUserInfo.uiVideoId,
                pVoiceChannel->m_oAudioBuffer.getReadSeq(),
                pVoiceChannel->m_oAudioBuffer.getWriteSeq(),
                uiAudioTime,
                uiAudioSeq,
                uiAudioSize);

            if((NULL != pAudioData)&&(uiAudioSize > 27))
            {
                //skip slow audio packet, 10s֮ǰ����Ƶ��ݶ���
                if(uiAudioTime + 10000 < pVoiceChannel->m_flv_parser.m_uiCurTimeStamp)
                {
                    SUBMCD_DEBUG_P(LOG_TRACE, "CProtol::OnVideoFrame FLVPARSER skip damn slow audio "
                                              " uin: %u, roomid: %u, videoid: %u, audio_time: %u, "
                                              "video_time: %u, audio_seq: %u, audio_size: %u\n",
                        oUserInfo.uiUin,
                        oUserInfo.uiRoomId,
                        oUserInfo.uiVideoId,
                        uiAudioTime,
                        pVoiceChannel->m_flv_parser.m_uiCurTimeStamp, 
                        uiAudioSeq, 
                        uiAudioSize);

                    continue;
                }

                /*audio handle*/
                pVoiceChannel->m_flv_parser.m_uiCurAudioTimeStamp = uiAudioTime;
                iRet = pVoiceChannel->m_flv_parser.parser_body_for_audio(pVoiceChannel->hls_session,
                                   pAudioData, uiAudioSize, uiFlvOffset, uiAudioSeq, oUserInfo.uiRoomId);

                if(0 != iRet)
                {
                    SUBMCD_DEBUG_P(LOG_ERROR, "CProtol::OnVideoFrame FLVPARSER parser_body_for_audio fail, "
                                              "uin: %u, roomid: %u, videoid: %u, size: %u, seq: %u\n", 
                        oUserInfo.uiUin,
                        oUserInfo.uiRoomId,
                        oUserInfo.uiVideoId,
                        uiAudioSize, 
                        uiAudioSeq);

                    continue;
                }

                if(pVoiceChannel->m_flv_parser.getFlvOffset() < 11)
                {

                    SUBMCD_DEBUG_P(LOG_TRACE,"CProtol::OnVideoFrame FLVPARSERLengthError "
                                             "uin: %u, roomid: %u, videoid: %u, len: %u\n", 
                        oUserInfo.uiUin,
                        oUserInfo.uiRoomId,
                        oUserInfo.uiVideoId,
                        pVoiceChannel->m_flv_parser.getFlvOffset());           
                }
                else
                {
                    if((0x09 != *(pVoiceChannel->m_flv_parser.m_pflv->c->data))&&(0x08 !=*(pVoiceChannel->m_flv_parser.m_pflv->c->data)))
                    {

                        SUBMCD_DEBUG_P(LOG_TRACE,"CProtol::OnVideoFrame FLVPARSERWriteflvfileError "
                                                 "[%u][%02X][%02X][%02X][%02X][%02X][%02X][%02X][%02X][%02X][%02X][%02X]\n ", 
                            pVoiceChannel->m_flv_parser.m_pflv->c->d_cur,
                            *(pVoiceChannel->m_flv_parser.m_pflv->c->data),
                            *(pVoiceChannel->m_flv_parser.m_pflv->c->data+1),
                            *(pVoiceChannel->m_flv_parser.m_pflv->c->data+2),
                            *(pVoiceChannel->m_flv_parser.m_pflv->c->data+3),
                            *(pVoiceChannel->m_flv_parser.m_pflv->c->data+4),
                            *(pVoiceChannel->m_flv_parser.m_pflv->c->data+5),
                            *(pVoiceChannel->m_flv_parser.m_pflv->c->data+6),
                            *(pVoiceChannel->m_flv_parser.m_pflv->c->data+7),
                            *(pVoiceChannel->m_flv_parser.m_pflv->c->data+8),
                            *(pVoiceChannel->m_flv_parser.m_pflv->c->data+9),
                            *(pVoiceChannel->m_flv_parser.m_pflv->c->data+10));
                    }
                }

                unsigned char *pFlvTagData	= pVoiceChannel->m_flv_parser.getTagBuffer();
                unsigned int uiFlvLen	= pVoiceChannel->m_flv_parser.getFlvOffset();

                if((NULL != flvFile)&&(m_pMCD->_cfg.m_uiSubRoomRecord == oUserInfo.uiRoomId))
                {
                    int n,fd;

                    fd  = open("../html/live.flv", O_CREAT|O_WRONLY|O_APPEND, 0644);
                    if ( fd < 0 ) {
                        SUBMCD_DEBUG_P(LOG_ERROR,"FLVPARSER WriteFlvData failed to open file, ret=%u\n",fd);
                    }

                    if(g_iOrgTotalsize >0)
                    {
                        if(0 == g_iTotalsize)
                        {								

                            n = write(fd,(unsigned char *)(pVoiceChannel->m_cdnFlvHead.flvHeadBuf),
                                       pVoiceChannel->m_cdnFlvHead.m_uiFlvHeaderLen);
                            if ( n <= 0 ) {
                                SUBMCD_DEBUG_P(LOG_ERROR,"FLVPARSER WriteFlvData failed to write file 1,"
                                     "ret=%u, size=%u\n",n,pVoiceChannel->m_cdnFlvHead.m_uiFlvHeaderLen);
                            }

                            fwrite(pVoiceChannel->m_cdnFlvHead.flvHeadBuf, 1, pVoiceChannel->m_cdnFlvHead.m_uiFlvHeaderLen, flvFile);

                            g_iTotalsize += pVoiceChannel->m_cdnFlvHead.m_uiFlvHeaderLen;

                            SUBMCD_DEBUG_P(LOG_TRACE, "FLVPARSERWriteFlvData[%u][%u][%d][%d]\n", 
                                                      g_iFlvCount, pVoiceChannel->m_cdnFlvHead.m_uiFlvHeaderLen, 
                                                      g_iTotalsize, g_iOrgTotalsize);
                            g_iFlvCount++;			

                        }

                        n = write(fd,(unsigned char *)(pFlvTagData+sizeof(tagFLVHEAD)), uiFlvLen);
                               
                        if ( n <= 0 ) {
                            SUBMCD_DEBUG_P(LOG_ERROR,"FLVPARSER WriteFlvData failed to write file 1,"
                                 "ret=%u, size=%u\n",n,pVoiceChannel->m_cdnFlvHead.m_uiFlvHeaderLen);
                        }

                        fwrite(pFlvTagData+sizeof(tagFLVHEAD), 1, uiFlvLen, flvFile);

                        g_iTotalsize += uiFlvLen;
                        SUBMCD_DEBUG_P(LOG_TRACE, "FLVPARSER WriteFlvData audio [%u][%u][%d][%d]\n", 
                                                  g_iFlvCount, uiFlvLen, g_iTotalsize, g_iOrgTotalsize);
                        g_iFlvCount++;

                    }
                }
/*
                if (true == m_pMCD->_cfg.m_bUseFlashp2p)
                {
                    RelayVideoFrame2HttpReqWithBlock(oUserInfo, 
                        oVideoIdInfo, 
                        *pVoiceChannel,
                        pFlvTagData,
                        uiFlvLen+sizeof(tagFLVHEAD),
                        0,
                        pFrameVideoItem->uiFrameTime);
                }
                else
                {
                    RelayVideoFrame2HttpReq(oUserInfo, 
                        oVideoIdInfo, 
                        *pVoiceChannel,
                        pFlvTagData,
                        uiFlvLen+sizeof(tagFLVHEAD),             
                        pVoiceChannel->m_uiCdnSeqNum++,
                        0);
                }
*/
                pVoiceChannel->m_flv_parser.flush();

                /*1s�Ժ����Ƶ����ݲ�����*/
                if(uiAudioTime > pVoiceChannel->m_flv_parser.m_uiCurTimeStamp + 1000)
                {
                    SUBMCD_DEBUG_P(LOG_TRACE, "CProtol::OnVideoFrame FLVPARSER audio pause and wait "
                                              "uin: %u, roomid: %u, videoid: %u, audio_time: %u, "
                                              "video_time: %u, audio_seq: %u, video_seq: %u\n", 
                        oUserInfo.uiUin,
                        oUserInfo.uiRoomId,
                        oUserInfo.uiVideoId,
                        uiAudioTime,
                        pVoiceChannel->m_flv_parser.m_uiCurTimeStamp, 
                        uiAudioSeq, 
                        uiAudioSize);
                    break;
                }
            }
            else
            {
                SUBMCD_DEBUG_P(LOG_TRACE,"CProtol::OnVideoFrame FLVPARSER get audio data error "
                                         "uin: %u, roomid: %u, videoid: %u, size %u\n", 
                    oUserInfo.uiUin,
                    oUserInfo.uiRoomId,
                    oUserInfo.uiVideoId,
                    uiAudioSize);
            }
        }//end of while

        if(0 == pVoiceChannel->m_flv_parser.m_uiCurOffset)	
        {
            SUBMCD_DEBUG_P(LOG_ERROR,"ERROR FLVPARSER Reset, uin: %u, roomid: %u, seq: %u, videoid: %u\n", 
                oUserInfo.uiUin,
                oUserInfo.uiRoomId,
                oFrameVideo.uiFrameSeq,
                oUserInfo.uiVideoId);
        }
        
    }

    return 0;
}

int CProtol::ReleayFlvHead2HttpReq(const SUserInfo &oUserInfo, SVideoIdInfo &oVideoIdInfo, SVoiceChannel &oVoiceChannel)
{
    SVideoIdInfo* pVideoIdInfo = m_pMCD->m_infoChannelMgr->get_video_id(oVideoIdInfo.uiVideoId);

    if (NULL == pVideoIdInfo)
    {
        SUBMCD_DEBUG_P(LOG_ERROR, "CProtol::ReleayFlvHead2HttpReq get_video_id failed,"
                                  " uin: %u, roomid: %u, videoId: %u\n",
            oUserInfo.uiUin,
            oUserInfo.uiRoomId,
            oUserInfo.uiVideoId);

        return -1;
    }

    SUBMCD_DEBUG_P(LOG_TRACE, "CProtol::ReleayFlvHead2HttpReq send to uiVideoId,"
                              " uin: %u, roomid: %u, videoId: %u\n",
        oUserInfo.uiUin,
        oUserInfo.uiRoomId,
        oUserInfo.uiVideoId);

    RelayFlvHead2CdnFlow(oUserInfo, *pVideoIdInfo, oVoiceChannel);
    /*
    if (m_pMCD->_cfg.m_uiIsSendToRecordSvr == 1) {
    	RelayFlvHead2RecordSvr(oUserInfo, *pVideoIdInfo, oVoiceChannel);
    }
    */
    return 0;
}

int CProtol::RelayFlvHead2CdnFlow(const SUserInfo& oUserInfo, SVideoIdInfo &oVideoIdInfo, SVoiceChannel &oVoiceChannel)
{
    if(0 == oVoiceChannel.m_cdnFlvHead.m_uiFlvHeaderLen)
    {
        return -1;
    }

    oVoiceChannel.m_cdnFlvHead.m_txHead.huVersion   = oUserInfo.usVer;
    oVoiceChannel.m_cdnFlvHead.m_txHead.huHeaderSize= sizeof(tagFLVHEAD);
    oVoiceChannel.m_cdnFlvHead.m_txHead.uSize       = oVoiceChannel.m_cdnFlvHead.m_uiFlvHeaderLen;
    oVoiceChannel.m_cdnFlvHead.m_txHead.ucType      = HT_HEADER;
    oVoiceChannel.m_cdnFlvHead.m_txHead.ucKeyFrame  = 0;
    oVoiceChannel.m_cdnFlvHead.m_txHead.uSec        = m_pMCD->m_cur_time;
    oVoiceChannel.m_cdnFlvHead.m_txHead.uSeq        = 0;
    oVoiceChannel.m_cdnFlvHead.m_txHead.uSegId	    = 0;
    oVoiceChannel.m_cdnFlvHead.m_txHead.uCheck      = oVoiceChannel.m_cdnFlvHead.m_uiFlvHeaderLen;

    for (unsigned int i=0; i<MAX_CDN_FLOW_NUM; i++)
    {
        if (0 == oVideoIdInfo.oCdnFlowList[i].uiFlow || 0 == oVideoIdInfo.oCdnFlowList[i].uiSrcType)
        {
            continue;
        }

        SUBMCD_DEBUG_P(LOG_COLOR, "CProtol::RelayFlvHead2CdnFlow send flvhead, "
                                  "uiVideoId: %u, flowIndex: %u, uin: %u, roomid: %u, "
                                  "ip: %s, port: %u, flow: %u, uiSrcType: %u\n", 
            oVideoIdInfo.uiVideoId,
            i,
            oUserInfo.uiUin,
            oUserInfo.uiRoomId,
            ip_to_str(oVideoIdInfo.oCdnFlowList[i].uiIp).c_str(),
            oVideoIdInfo.oCdnFlowList[i].usPort,
            oVideoIdInfo.oCdnFlowList[i].uiFlow,
            oVideoIdInfo.oCdnFlowList[i].uiSrcType);

        int ret = DoSendToFlow(oVideoIdInfo.oCdnFlowList[i], 
                               (const char *)&oVoiceChannel.m_cdnFlvHead.m_txHead, 
                               sizeof(tagFLVHEAD) + oVoiceChannel.m_cdnFlvHead.m_uiFlvHeaderLen);


        if (0 == ret)
        {
            oVideoIdInfo.oCdnFlowList[i].unionFlag.ucIsHaveMediaInfo = 1;
        }
    }

    return 0;
}

int CProtol::RelayFlvHead2RecordSvr(const SUserInfo& oUserInfo, SVideoIdInfo &oVideoIdInfo, SVoiceChannel &oVoiceChannel)
{
    if(0 == oVoiceChannel.m_cdnFlvHead.m_uiFlvHeaderLen)
    {
        return -1;
    }
    char cBuff[1024] = {0};

    int len = snprintf(cBuff + sizeof(tagFLVHEAD), 1024 - sizeof(tagFLVHEAD), m_strHttpOkRsp2RecordSvr.c_str(), oVideoIdInfo.uiVideoId, oVideoIdInfo.uiUin, oVideoIdInfo.uiMainRoomId);

    len = len + sizeof(tagFLVHEAD);

    tagFLVHEAD *pTxHead = (tagFLVHEAD *)cBuff;

	pTxHead->huVersion      = 99;
	pTxHead->huHeaderSize   = sizeof(tagFLVHEAD);
	pTxHead->uSize          = len - sizeof(tagFLVHEAD);
	pTxHead->ucType         = HT_DATA;
	pTxHead->ucKeyFrame 	= TYPE_NORMAL_FRAME;
	pTxHead->uSec           = m_pMCD->m_cur_time;
	pTxHead->uSeq           = 0;
	pTxHead->uSegId         = 0;
	pTxHead->uCheck         = len - sizeof(tagFLVHEAD);

    unsigned int uiPortIndex = oVideoIdInfo.uiVideoId % m_pMCD->_cfg.m_usRecordSvrPort.size();

    for (unsigned int i=0; i<m_pMCD->_cfg.m_uiRecordSvrIp.size(); i++)
    {
    	//send http rsp
    	m_pMCD->enqueue_2_dcc2_with_flow(cBuff, len, m_pMCD->_cfg.m_uiRecordSvrIp[i], 
                       m_pMCD->_cfg.m_usRecordSvrPort[uiPortIndex], oVideoIdInfo.uiVideoId);
    	//send flv head
    	m_pMCD->enqueue_2_dcc2_with_flow((const char *)&oVoiceChannel.m_cdnFlvHead.m_txHead, 
                          sizeof(tagFLVHEAD) + oVoiceChannel.m_cdnFlvHead.m_uiFlvHeaderLen, 
                          m_pMCD->_cfg.m_uiRecordSvrIp[i], m_pMCD->_cfg.m_usRecordSvrPort[uiPortIndex], 
                          oVideoIdInfo.uiVideoId);

        SUBMCD_DEBUG_P(LOG_COLOR, "CProtol::RelayFlvHead2RecordSvr send http_rsp and flv head, "
               "uiVideoId: %u, uin: %u, roomid: %u, ip: %s, port: %u, http_rsp_len: %u, flv_head_len: %u\n",
            oVideoIdInfo.uiVideoId,
            oVideoIdInfo.uiUin,
            oVideoIdInfo.uiMainRoomId,
            ip_to_str(m_pMCD->_cfg.m_uiRecordSvrIp[i]).c_str(),
            m_pMCD->_cfg.m_usRecordSvrPort[uiPortIndex],
            len,
            sizeof(tagFLVHEAD) + oVoiceChannel.m_cdnFlvHead.m_uiFlvHeaderLen);
    }
    
    int fd = open("../html/live.flv",O_CREAT|O_WRONLY|O_TRUNC,0644);

    write(fd,(const char *)&oVoiceChannel.m_cdnFlvHead.m_txHead+sizeof(tagFLVHEAD),
            oVoiceChannel.m_cdnFlvHead.m_uiFlvHeaderLen);

    close(fd);

    g_iTotalsize = 1;

    return 0;
}

int CProtol::RelayVideoFrame2HttpReq(const SUserInfo &oUserInfo, SVideoIdInfo &oVideoIdInfo, 
                     SVoiceChannel &oVoiceChannel, unsigned char *ucPkg, unsigned int uiPkgLen, 
                     unsigned int uiCdnSeq, unsigned char ucKeyFrame)
{
    SVideoIdInfo* pVideoIdInfo = m_pMCD->m_infoChannelMgr->get_video_id(oVideoIdInfo.uiVideoId);

    if (NULL == pVideoIdInfo)
    {
        SUBMCD_DEBUG_P(LOG_ERROR, "CProtol::RelayVideo2HttpReq get_video_id failed, "
                                  "uin: %u, roomid: %u, roomtype: %u, videoId: %u\n",
            oUserInfo.uiUin,
            oUserInfo.uiRoomId,
            oUserInfo.ucRoomType,
            oUserInfo.uiVideoId);
    }
    else
    {
        SUBMCD_DEBUG_P(LOG_TRACE, "CProtol::RelayVideo2HttpReq send to uiVideoId, "
                                  "uin: %u, roomid: %u, roomtype: %u, videoId: %u\n",
            oUserInfo.uiUin,
            oUserInfo.uiRoomId,
            oUserInfo.ucRoomType,
            oUserInfo.uiVideoId);

        RelayVideoFrame2Cdn(oUserInfo, *pVideoIdInfo, oVoiceChannel, ucPkg, uiPkgLen, uiCdnSeq, ucKeyFrame);

        if (m_pMCD->_cfg.m_uiIsSendToRecordSvr == 1) {
        	RelayVideoFrame2RecordSvr(*pVideoIdInfo, ucPkg, uiPkgLen);
        }
    }

    return 0;
}

int CProtol::RelayVideoFrame2HttpReqWithBlock(const SUserInfo &oUserInfo, SVideoIdInfo &oVideoIdInfo, SVoiceChannel &oVoiceChannel, unsigned char *ucTagPkg, unsigned int uiTagPkgLen, unsigned char ucKeyFrame, unsigned int uiFrameTime)
{
    SVideoIdInfo* pVideoIdInfo = m_pMCD->m_infoChannelMgr->get_video_id(oVideoIdInfo.uiVideoId);

    if (NULL == pVideoIdInfo)
    {
        SUBMCD_DEBUG_P(LOG_ERROR, "CProtol::RelayVideoFrame2HttpReqWithBlock get_video_id failed. uin: %u, roomid: %u, roomtype: %u, videoId: %u\n",
            oUserInfo.uiUin,
            oUserInfo.uiRoomId,
            oUserInfo.ucRoomType,
            oUserInfo.uiVideoId);
    }
    else
    {
        SUBMCD_DEBUG_P(LOG_TRACE, "CProtol::RelayVideoFrame2HttpReqWithBlock send to uiVideoId. uin: %u, roomid: %u, roomtype: %u, videoId: %u\n",
            oUserInfo.uiUin,
            oUserInfo.uiRoomId,
            oUserInfo.ucRoomType,
            oUserInfo.uiVideoId);

        RelayVideoFrame2CdnWithBlock(oUserInfo, *pVideoIdInfo, oVoiceChannel, ucTagPkg, uiTagPkgLen, ucKeyFrame, uiFrameTime);
    }

    return 0;
}

int CProtol::RelayVideoFrame2Cdn(const SUserInfo &oUserInfo, SVideoIdInfo &oVideoIdInfo, SVoiceChannel &oVoiceChannel, unsigned char *ucPkg, unsigned int uiPkgLen, unsigned int uiCdnSeq, unsigned char ucKeyFrame)
{
    tagFLVHEAD *pTxHead = (tagFLVHEAD *)ucPkg;

    //�Ƹ�cdn
    pTxHead->huVersion      = oUserInfo.usVer;
    pTxHead->huHeaderSize   = sizeof(tagFLVHEAD);
    pTxHead->uSize          = uiPkgLen - sizeof(tagFLVHEAD);
    pTxHead->ucType         = HT_DATA;

    if (1 == ucKeyFrame)
    {
        pTxHead->ucKeyFrame = TYPE_KEY_FRAME;
    }
    else
    {
        pTxHead->ucKeyFrame = TYPE_NORMAL_FRAME;
    }

    pTxHead->uSec           = m_pMCD->m_cur_time;
    pTxHead->uSeq           = uiCdnSeq;
    pTxHead->uSegId         = 0;
    pTxHead->uCheck         = uiPkgLen - sizeof(tagFLVHEAD);

    SUBMCD_DEBUG_P(LOG_DEBUG,"CProtol::RelayVideo2Cdn frame. uin: %u, ucKeyFrame:%u, frame_seq: %u, frame_len: %u\n", 
        oUserInfo.uiUin, 
        ucKeyFrame,
        uiCdnSeq, 
        uiPkgLen);


    RelayVideo2CdnFlow(oUserInfo, oVideoIdInfo, oVoiceChannel, ucPkg, uiPkgLen, uiCdnSeq, ucKeyFrame);

    return 0;
}

int CProtol::RelayVideoFrame2CdnWithBlock(const SUserInfo &oUserInfo, SVideoIdInfo &oVideoIdInfo, SVoiceChannel &oVoiceChannel, unsigned char *ucTagPkg, unsigned int uiTagPkgLen, unsigned char ucKeyFrame, unsigned int uiFrameTime)
{
    SCdnBlock *pCdnBlock = &oVoiceChannel.m_oCdnBlock;
    unsigned char *ucPkg = ucTagPkg + sizeof(tagFLVHEAD);
    unsigned int uiPkgLen = uiTagPkgLen - sizeof(tagFLVHEAD);

    if (1 == ucKeyFrame)
    {
        //����ǹؼ�֡���Ȱ�����ݷ��ͳ�ȥ
        if (pCdnBlock->uiDataLen>0)
        {
            tagFLVHEAD *pCdnBlockTxHead = &pCdnBlock->m_txHead;

            //����ݿ���ΪTYPE_KEY_FRAME����Ҫ�ı�pTxHead->ucKeyFrame
            //pTxHead->ucKeyFrame     = TYPE_NORMAL_FRAME;
            pCdnBlockTxHead->huVersion      = oUserInfo.usVer;
            pCdnBlockTxHead->huHeaderSize   = sizeof(tagFLVHEAD);
            pCdnBlockTxHead->uSize          = pCdnBlock->uiDataLen;
            pCdnBlockTxHead->ucType         = HT_DATA;
            //pTxHead->uSec           = 0;
            pCdnBlockTxHead->uSeq           = oVoiceChannel.m_uiCdnSeqNum++;
            pCdnBlockTxHead->uSegId         = 0;
            pCdnBlockTxHead->uCheck         = pCdnBlock->uiDataLen;

            SUBMCD_DEBUG_P(LOG_DEBUG,"CProtol::RelayVideoFrame2CdnWithBlock frame. uin: %u, ucKeyFrame:%u, frame_seq: %u, frame_len: %u\n", 
                oUserInfo.uiUin, 
                pCdnBlockTxHead->ucKeyFrame, 
                pCdnBlockTxHead->uSeq, 
                pCdnBlockTxHead->uSize);

            RelayVideo2CdnFlow(oUserInfo, oVideoIdInfo, oVoiceChannel, (unsigned char *)pCdnBlockTxHead, pCdnBlockTxHead->huHeaderSize+pCdnBlockTxHead->uSize, pCdnBlockTxHead->uSeq, pCdnBlockTxHead->ucKeyFrame);
            oVoiceChannel.m_oP2PBlockIndexBuffer.add(pCdnBlockTxHead->uSeq,pCdnBlockTxHead->uSec);
            memset((char *)pCdnBlock, 0, sizeof(SCdnBlock));

        }

        if (uiPkgLen<m_pMCD->_cfg.m_uiBlockLen)
        {
            tagFLVHEAD *pCdnBlockTxHead = &pCdnBlock->m_txHead;

            //֡��ݳ���С�ڷ�Ƭ���ȣ��ȱ����������Ժ���
            pCdnBlockTxHead->ucKeyFrame = TYPE_KEY_FRAME;
            pCdnBlockTxHead->uSec = uiFrameTime/1000;
            memcpy(pCdnBlock->blockBuf, ucPkg, uiPkgLen);
            pCdnBlock->uiDataLen = uiPkgLen;
        }
        else
        {
            //�ؼ�֡����block����
            tagFLVHEAD *pTxHead = (tagFLVHEAD *)ucTagPkg;

            //�Ƹ�cdn
            pTxHead->huVersion      = oUserInfo.usVer;
            pTxHead->huHeaderSize   = sizeof(tagFLVHEAD);
            pTxHead->uSize          = uiPkgLen;
            pTxHead->ucType         = HT_DATA;
            pTxHead->ucKeyFrame     = TYPE_KEY_FRAME;
            pTxHead->uSec           = uiFrameTime/1000;
            pTxHead->uSeq           = oVoiceChannel.m_uiCdnSeqNum++;;
            pTxHead->uSegId         = 0;
            pTxHead->uCheck         = uiPkgLen;

            SUBMCD_DEBUG_P(LOG_DEBUG,"CProtol::RelayVideoFrame2CdnWithBlock frame. uin: %u, ucKeyFrame:%u, frame_seq: %u, frame_len: %u\n", 
                oUserInfo.uiUin, 
                pTxHead->ucKeyFrame, 
                pTxHead->uSeq, 
                pTxHead->uSize);

            RelayVideo2CdnFlow(oUserInfo, oVideoIdInfo, oVoiceChannel, (unsigned char *)pTxHead, pTxHead->huHeaderSize+pTxHead->uSize, pTxHead->uSeq, pTxHead->ucKeyFrame);
            oVoiceChannel.m_oP2PBlockIndexBuffer.add(pTxHead->uSeq,pTxHead->uSec);
        }
    }
    else
        //�յ������λ�ǹؼ�֡
    {
        if (pCdnBlock->uiDataLen+uiPkgLen<m_pMCD->_cfg.m_uiBlockLen)
        {
            if (pCdnBlock->uiDataLen>0)
            {
                //����ݿ���ΪTYPE_KEY_FRAME����Ҫ�ı�pTxHead->ucKeyFrame
                memcpy(pCdnBlock->blockBuf+pCdnBlock->uiDataLen, ucPkg, uiPkgLen);
                pCdnBlock->uiDataLen += uiPkgLen;
            }
            else
            {
                tagFLVHEAD *pCdnBlockTxHead = &pCdnBlock->m_txHead;

                //֡��ݳ���С�ڷ�Ƭ���ȣ��ȱ����������Ժ���
                pCdnBlockTxHead->ucKeyFrame = TYPE_NORMAL_FRAME;
                pCdnBlockTxHead->uSec = uiFrameTime/1000;
                memcpy(pCdnBlock->blockBuf, ucPkg, uiPkgLen);
                pCdnBlock->uiDataLen = uiPkgLen;
            }
        }
        else
        {
            if (pCdnBlock->uiDataLen>0)
            {
                tagFLVHEAD *pCdnBlockTxHead = &pCdnBlock->m_txHead;

                //����ݿ���ΪTYPE_KEY_FRAME����Ҫ�ı�pTxHead->ucKeyFrame
                //pTxHead->ucKeyFrame     = TYPE_NORMAL_FRAME;
                pCdnBlockTxHead->huVersion      = oUserInfo.usVer;
                pCdnBlockTxHead->huHeaderSize   = sizeof(tagFLVHEAD);
                pCdnBlockTxHead->uSize          = pCdnBlock->uiDataLen;
                pCdnBlockTxHead->ucType         = HT_DATA;
                //pTxHead->uSec           = 0;
                pCdnBlockTxHead->uSeq           = oVoiceChannel.m_uiCdnSeqNum++;
                pCdnBlockTxHead->uSegId         = 0;
                pCdnBlockTxHead->uCheck         = pCdnBlock->uiDataLen;

                SUBMCD_DEBUG_P(LOG_DEBUG,"CProtol::RelayVideoFrame2CdnWithBlock frame. uin: %u, ucKeyFrame:%u, frame_seq: %u, frame_len: %u\n", 
                    oUserInfo.uiUin, 
                    pCdnBlockTxHead->ucKeyFrame, 
                    pCdnBlockTxHead->uSeq, 
                    pCdnBlockTxHead->uSize);

                RelayVideo2CdnFlow(oUserInfo, oVideoIdInfo, oVoiceChannel, (unsigned char *)pCdnBlockTxHead, pCdnBlockTxHead->huHeaderSize+pCdnBlockTxHead->uSize, pCdnBlockTxHead->uSeq, pCdnBlockTxHead->ucKeyFrame);
                oVoiceChannel.m_oP2PBlockIndexBuffer.add(pCdnBlockTxHead->uSeq,pCdnBlockTxHead->uSec);
                memset((char *)pCdnBlock, 0, sizeof(SCdnBlock));
            }

            if (uiPkgLen<m_pMCD->_cfg.m_uiBlockLen)
            {
                tagFLVHEAD *pCdnBlockTxHead = &pCdnBlock->m_txHead;

                //֡��ݳ���С�ڷ�Ƭ���ȣ��ȱ����������Ժ���
                pCdnBlockTxHead->ucKeyFrame = TYPE_NORMAL_FRAME;
                pCdnBlockTxHead->uSec = uiFrameTime/1000;
                memcpy(pCdnBlock->blockBuf, ucPkg, uiPkgLen);
                pCdnBlock->uiDataLen = uiPkgLen;
            }
            else
            {
                //�ؼ�֡����block����
                tagFLVHEAD *pTxHead = (tagFLVHEAD *)ucTagPkg;

                //�Ƹ�cdn
                pTxHead->huVersion      = oUserInfo.usVer;
                pTxHead->huHeaderSize   = sizeof(tagFLVHEAD);
                pTxHead->uSize          = uiPkgLen;
                pTxHead->ucType         = HT_DATA;
                pTxHead->ucKeyFrame     = TYPE_NORMAL_FRAME;
                pTxHead->uSec           = uiFrameTime/1000;
                pTxHead->uSeq           = oVoiceChannel.m_uiCdnSeqNum++;;
                pTxHead->uSegId         = 0;
                pTxHead->uCheck         = uiPkgLen;

                SUBMCD_DEBUG_P(LOG_DEBUG,"CProtol::RelayVideoFrame2CdnWithBlock frame. uin: %u, ucKeyFrame:%u, frame_seq: %u, frame_len: %u\n", 
                    oUserInfo.uiUin, 
                    pTxHead->ucKeyFrame, 
                    pTxHead->uSeq, 
                    pTxHead->uSize);

                RelayVideo2CdnFlow(oUserInfo, oVideoIdInfo, oVoiceChannel, (unsigned char *)pTxHead, pTxHead->huHeaderSize+pTxHead->uSize, pTxHead->uSeq, pTxHead->ucKeyFrame);
                oVoiceChannel.m_oP2PBlockIndexBuffer.add(pTxHead->uSeq,pTxHead->uSec);
            }
        }
    } 
    return 0;
}


int CProtol::RelayVideo2CdnFlow(const SUserInfo &oUserInfo, SVideoIdInfo &oVideoIdInfo, SVoiceChannel &oVoiceChannel, unsigned char *pPkg, unsigned int uiPkgLen, unsigned int uiSeq, unsigned char ucKey)
{
    oVideoIdInfo.uiUin         = oUserInfo.uiUin;
    oVideoIdInfo.uiRoomId      = oUserInfo.uiRoomId;
    oVideoIdInfo.ucRoomType    = oUserInfo.ucRoomType;
    oVideoIdInfo.uiTouchTime   = m_pMCD->m_cur_time;

    for (unsigned int i=0; i<MAX_CDN_FLOW_NUM; i++)
    {
        if (0 == oVideoIdInfo.oCdnFlowList[i].uiFlow || 0 == oVideoIdInfo.oCdnFlowList[i].uiSrcType)
        {
            continue;
        }

        if (0 == oVideoIdInfo.oCdnFlowList[i].unionFlag.ucIsHaveMediaInfo)
        {
            //ToDo����flv���ͷ
            if(oVoiceChannel.m_cdnFlvHead.m_uiFlvHeaderLen > 0)
            {			
                oVoiceChannel.m_cdnFlvHead.m_txHead.huVersion	= oUserInfo.usVer;
                oVoiceChannel.m_cdnFlvHead.m_txHead.huHeaderSize= sizeof(tagFLVHEAD);
                oVoiceChannel.m_cdnFlvHead.m_txHead.uSize       = oVoiceChannel.m_cdnFlvHead.m_uiFlvHeaderLen;
                oVoiceChannel.m_cdnFlvHead.m_txHead.ucType      = HT_HEADER;
                oVoiceChannel.m_cdnFlvHead.m_txHead.ucKeyFrame  = 0;
                oVoiceChannel.m_cdnFlvHead.m_txHead.uSec        = m_pMCD->m_cur_time;
                oVoiceChannel.m_cdnFlvHead.m_txHead.uSeq        = 0;
                oVoiceChannel.m_cdnFlvHead.m_txHead.uSegId		= 0;
                oVoiceChannel.m_cdnFlvHead.m_txHead.uCheck      = oVoiceChannel.m_cdnFlvHead.m_uiFlvHeaderLen;

                SUBMCD_DEBUG_P(LOG_TRACE, "CProtol::RelayVideo2CdnFlow send meidaInfo, "
                                          "uiLen: %u, uiSeq: %u, uiVideoId: %u, flowIndex: %u, "
                                          "uin: %u, roomid: %u, roomtype: %d, ip: %s, port: %u, "
                                          "flow: %u, uiSrcType: %u\n", 
                    uiPkgLen,
                    uiSeq,
                    oVideoIdInfo.uiVideoId,
                    i,
                    oUserInfo.uiUin,
                    oUserInfo.uiRoomId,
                    oUserInfo.ucRoomType,
                    ip_to_str(oVideoIdInfo.oCdnFlowList[i].uiIp).c_str(),
                    oVideoIdInfo.oCdnFlowList[i].usPort,
                    oVideoIdInfo.oCdnFlowList[i].uiFlow,
                    oVideoIdInfo.oCdnFlowList[i].uiSrcType);

                int ret = DoSendToFlow(oVideoIdInfo.oCdnFlowList[i], 
                    (const char *)&oVoiceChannel.m_cdnFlvHead.m_txHead, 
                    sizeof(tagFLVHEAD) + oVoiceChannel.m_cdnFlvHead.m_uiFlvHeaderLen);

                if (0 == ret)
                {
                    oVideoIdInfo.oCdnFlowList[i].unionFlag.ucIsHaveMediaInfo = 1;
                }
            }
        }

        SUBMCD_DEBUG_P(LOG_DEBUG, "CProtol::RelayVideo2CdnFlow send frame. uiLen: %u, uiSeq: %u, ucKey: %d, uiVideoId: %u, flowIndex: %u, uin: %u, roomid: %u, roomtype: %u, vid: %u, ip: %s, port: %u, flow: %u, uiSrcType: %u\n", 
            uiPkgLen,
            uiSeq,
            ucKey,
            oVideoIdInfo.uiVideoId,
            i,
            oUserInfo.uiUin,
            oUserInfo.uiRoomId,
            oUserInfo.ucRoomType,
            oUserInfo.uiVideoId,
            ip_to_str(oVideoIdInfo.oCdnFlowList[i].uiIp).c_str(),
            oVideoIdInfo.oCdnFlowList[i].usPort,
            oVideoIdInfo.oCdnFlowList[i].uiFlow,
            oVideoIdInfo.oCdnFlowList[i].uiSrcType);

        oVideoIdInfo.oCdnFlowList[i].uiSendTime = m_pMCD->m_cur_time;
        DoSendToFlow(oVideoIdInfo.oCdnFlowList[i], (const char*)pPkg, uiPkgLen);
    }

    return 0;
}

int CProtol::RelayVideoFrame2RecordSvr(SVideoIdInfo &oVideoIdInfo, unsigned char *ucPkg, unsigned int uiPkgLen) {
	int ret = 0;

	tagFLVHEAD *pTxHead = (tagFLVHEAD *)ucPkg;
	pTxHead->huVersion      = 99;
	pTxHead->huHeaderSize   = sizeof(tagFLVHEAD);
	pTxHead->uSize          = uiPkgLen - sizeof(tagFLVHEAD);
	pTxHead->ucType         = HT_DATA;
	pTxHead->ucKeyFrame 	= TYPE_NORMAL_FRAME;
	pTxHead->uSec           = m_pMCD->m_cur_time;
	pTxHead->uSeq           = 0;
	pTxHead->uSegId         = 0;
	pTxHead->uCheck         = uiPkgLen - sizeof(tagFLVHEAD);

	unsigned int uiPortIndex = oVideoIdInfo.uiVideoId % m_pMCD->_cfg.m_usRecordSvrPort.size();

	for (unsigned int i=0; i<m_pMCD->_cfg.m_uiRecordSvrIp.size(); i++)
	{
		m_pMCD->enqueue_2_dcc2_with_flow((const char*) ucPkg, uiPkgLen, m_pMCD->_cfg.m_uiRecordSvrIp[i], 
                                    m_pMCD->_cfg.m_usRecordSvrPort[uiPortIndex], oVideoIdInfo.uiVideoId);

		SUBMCD_DEBUG_P(LOG_COLOR, "CProtol::RelayVideoFrame2RecordSvr "
                                          "uiVideoId: %u, uin: %u, roomid: %u, ip: %s, port: %u, frame_len: %u\n",
			oVideoIdInfo.uiVideoId,
			oVideoIdInfo.uiUin,
			oVideoIdInfo.uiMainRoomId,
			ip_to_str(m_pMCD->_cfg.m_uiRecordSvrIp[i]).c_str(),
			m_pMCD->_cfg.m_usRecordSvrPort[uiPortIndex],
			uiPkgLen);
	}
        if ( g_iTotalsize == 1 ) {
            int fd = open("../html/live.flv",O_CREAT|O_WRONLY|O_APPEND,0644);

            write(fd,ucPkg+sizeof(tagFLVHEAD), uiPkgLen-sizeof(tagFLVHEAD));
          
            close(fd);
        }

	return ret;
}

SVideoIdInfo* CProtol::UpdateVideoIdInfo(unsigned int uiUin, unsigned int uiRoomId, 
          unsigned int uiMainRoomId, unsigned int uiVideoId, unsigned int uiChannelId)
{
    SVideoIdInfo *pVideoIdInfo = m_pMCD->m_infoChannelMgr->get_video_id(uiVideoId);

    if (NULL == pVideoIdInfo)
    {
        pVideoIdInfo = m_pMCD->m_infoChannelMgr->add_video_id(uiVideoId);
    }

    if (NULL == pVideoIdInfo)
    {
        SUBMCD_DEBUG_P(LOG_ERROR,"CProtol::UpdateVideoIdInfo add videoId failed. uiVideoId: %u, uin: %u, roomid: %u\n", 
            uiVideoId,
            uiUin,
            uiRoomId);

        return NULL;
    }
    else
    {
        pVideoIdInfo->uiUin         = uiUin;
        pVideoIdInfo->uiRoomId      = uiRoomId;
	pVideoIdInfo->uiMainRoomId	= uiMainRoomId;
        pVideoIdInfo->uiChannelId	= uiChannelId;

        pVideoIdInfo->uiTouchTime   = m_pMCD->m_cur_time;

        return pVideoIdInfo;
    }

    return NULL;
}

int CProtol::DoSendToFlow(const SFlowInfo& oVideoFlowInfo, const char* pData, const unsigned int uiSize)
{
    if(NULL == pData || 0 == uiSize )
    {
        return -1;
    }

    int ret = 0;

    switch(oVideoFlowInfo.uiSrcType)
    {
    case SOCKET_TCP_80:
        {
            ret = m_pMCD->enqueue_2_ccd4(pData, uiSize, oVideoFlowInfo.uiFlow); 
        }
        break;
    case SOCKET_TCP_8000:
        {
            ret = m_pMCD->enqueue_2_ccd5(pData, uiSize, oVideoFlowInfo.uiFlow); 
        }
        break;
    case SOCKET_TCP_443:
        {
            ret = m_pMCD->enqueue_2_ccd6(pData, uiSize, oVideoFlowInfo.uiFlow); 
        }
        break;
    default:
        {
            ret = -1;

            SUBMCD_DEBUG_P(LOG_ERROR, "CProtol::DoSendToFlow unknown uiSrcType %u, flow: %u, ip: %s, cdn_port: %u, msglen: %u\n", 
                oVideoFlowInfo.uiSrcType,
                oVideoFlowInfo.uiFlow,
                ip_to_str(oVideoFlowInfo.uiIp).c_str(),
                oVideoFlowInfo.usPort,
                uiSize);
        }
        break;
    }  

    return ret;

}

int CProtol::get_http_flv_type(const char *pPkg, unsigned int uiPkgLen, SHttpRequestPara *pHttpRequestPara)
{
    string strUrl(pPkg, uiPkgLen);

    SUBMCD_DEBUG_P(LOG_DEBUG, "CProtol::get_http_app_type strUrl: %s\n", strUrl.c_str());

    if (strUrl.size() <= 1)
    {
        return APP_INVALID;
    }

    string::size_type file_start = strUrl.find('/');

    if(string::npos == file_start)
    {
        return APP_INVALID;
    }

    SUBMCD_DEBUG_P(LOG_DEBUG, "CProtol::get_http_app_type file_start: %u\n", file_start);

    if (file_start >= strUrl.size()-1)
    {
        return APP_INVALID;
    }

    string::size_type videoId_end = strUrl.find(".flv");

    //maybe cdn request
    if(strUrl.npos != videoId_end)
    {
        SUBMCD_DEBUG_P(LOG_DEBUG, "CProtol::get_http_app_type videoId_end: %u\n", videoId_end);

        if (videoId_end+3 >= strUrl.size()-1)
        {
            return APP_INVALID;
        }

        string strVideoId = strUrl.substr(file_start+1, videoId_end-file_start-1);

        try
        {
            pHttpRequestPara->uiVideoId = from_str<unsigned int>(strVideoId);
        }
        catch(...)
        {
            SUBMCD_DEBUG_P(LOG_ERROR, "CProtol::get_http_app_type get uiVideoId failed.\n");
            return APP_INVALID;
        }

        return APP_CDN;
    }
    //invalid request
    else
    {
        return APP_INVALID;
    }

    return APP_INVALID;
}

int CProtol::get_http_hlsm3u8_type(const char *pPkg, unsigned int uiPkgLen, SHttpRequestPara *pHttpRequestPara)
{
    string strUrl(pPkg, uiPkgLen);

    SUBMCD_DEBUG_P(LOG_DEBUG, "CProtol::get_http_hlsm3u8_type strUrl: %s\n", strUrl.c_str());

    if (strUrl.size() <= 1)
    {
        return APP_INVALID;
    }

    string::size_type file_start = strUrl.find('/');

    if(string::npos == file_start)
    {
        return APP_INVALID;
    }

    SUBMCD_DEBUG_P(LOG_DEBUG, "CProtol::get_http_hlsm3u8_type file_start: %u\n", file_start);

    if (file_start >= strUrl.size()-1)
    {
        return APP_INVALID;
    }

    string::size_type videoId_end = strUrl.find(".m3u8");

    //maybe cdn request
    if(strUrl.npos != videoId_end)
    {
        SUBMCD_DEBUG_P(LOG_DEBUG, "CProtol::get_http_hlsm3u8_type videoId_end: %u\n", videoId_end);

        if (videoId_end+4 >= strUrl.size()-1)
        {
            return APP_INVALID;
        }

        string strVideoId = strUrl.substr(file_start+1, videoId_end-file_start-1);

        try
        {
            pHttpRequestPara->uiVideoId = from_str<unsigned int>(strVideoId);
            SUBMCD_DEBUG_P(LOG_DEBUG, "CProtol::get_http_hlsm3u8_type: uiVideoId=%u\n",
                                            pHttpRequestPara->uiVideoId);
        }
        catch(...)
        {
            SUBMCD_DEBUG_P(LOG_ERROR, "CProtol::get_http_hlsm3u8_type get uiVideoId failed.\n");
            return APP_INVALID;
        }

        return APP_HLS_M3U8;
    }
    //invalid request
    else
    {
        return APP_INVALID;
    }

    return APP_INVALID;
}

int CProtol::get_http_hlsts_type(const char *pPkg, unsigned int uiPkgLen, SHttpRequestPara *pHttpRequestPara)
{
    string strUrl(pPkg, uiPkgLen);

    SUBMCD_DEBUG_P(LOG_DEBUG, "CProtol::get_http_hlsts_type strUrl: %s\n", strUrl.c_str());

    if (strUrl.size() <= 1)
    {
        return APP_INVALID;
    }

    string::size_type file_start = strUrl.find('/');

    if(string::npos == file_start) {
        return APP_INVALID;
    }

    SUBMCD_DEBUG_P(LOG_DEBUG, "CProtol::get_http_hlsts_type file_start: %u\n", file_start);

    if (file_start >= strUrl.size()-1) {
        return APP_INVALID;
    }

    string::size_type streamId_end = strUrl.find(".ts");

    //maybe cdn request
    if(strUrl.npos != streamId_end)
    {
        SUBMCD_DEBUG_P(LOG_DEBUG, "CProtol::get_http_hlsts_type videoId_end: %u\n", streamId_end);

        if (streamId_end+3 >= strUrl.size()-1) {
            return APP_INVALID;
        }

        string strStreamId = strUrl.substr(file_start+1, streamId_end-file_start-1);

        try
        {
            string::size_type id_start = strStreamId.find("-");
            if ( id_start >= strStreamId.size()-1 )
                return APP_INVALID;
            string strVideoId = strStreamId.substr(0,id_start); 
            string strTsId = strStreamId.substr(id_start+1,strStreamId.size()-id_start-1); 

            SUBMCD_DEBUG_P(LOG_DEBUG, "CProtol::get_http_hlsts_type: videoid=%s,tsid=%s\n",
                                strVideoId.c_str(), strTsId.c_str());

            pHttpRequestPara->uiVideoId = from_str<unsigned int>(strVideoId);
            pHttpRequestPara->uiTsId = from_str<unsigned int>(strTsId);

            SUBMCD_DEBUG_P(LOG_DEBUG, "CProtol::get_http_hlsts_type: videoid=%u,tsid=%u\n",
                                pHttpRequestPara->uiVideoId, pHttpRequestPara->uiTsId);
        }
        catch(...)
        {
            SUBMCD_DEBUG_P(LOG_ERROR, "CProtol::get_http_app_type get uiVideoId failed.\n");
            return APP_INVALID;
        }

        return APP_HLS_TS;
    }
    //invalid request
    else
    {
        return APP_INVALID;
    }

    return APP_INVALID;
}

int CProtol::get_http_xml_type(const char *pPkg, unsigned int uiPkgLen, SHttpRequestPara *pHttpRequestPara)
{
    string strUrl(pPkg, uiPkgLen);

    SUBMCD_DEBUG_P(LOG_DEBUG, "CProtol::get_http_xml_type strUrl: %s\n", strUrl.c_str());

    if (strUrl.size() <= 1)
    {
        return APP_INVALID;
    }

    string::size_type file_start = strUrl.find('/');

    if(string::npos == file_start)
    {
        return APP_INVALID;
    }

    SUBMCD_DEBUG_P(LOG_DEBUG, "CProtol::get_http_xml_type file_start: %u\n", file_start);

    if (file_start >= strUrl.size()-1)
    {
        return APP_INVALID;
    }

    string::size_type videoId_end = strUrl.find("crossdomain.xml");

    //maybe cdn request
    if(strUrl.npos != videoId_end)
    {
        SUBMCD_DEBUG_P(LOG_DEBUG, "CProtol::get_http_xml_type crossdomain req\n");
        pHttpRequestPara->uiVideoId = 0;

        return APP_CROSSDOMAIN_XML;
    }
    //invalid request
    else
    {
        return APP_INVALID;
    }

    return APP_INVALID;
}

int CProtol::get_http_app_type(const char *pPkg, unsigned int uiPkgLen, SHttpRequestPara *pHttpRequestPara)
{
    if ( get_http_flv_type(pPkg,uiPkgLen,pHttpRequestPara) != APP_INVALID )
        return APP_CDN;

    if ( get_http_hlsm3u8_type(pPkg,uiPkgLen,pHttpRequestPara) != APP_INVALID )
        return APP_HLS_M3U8;

    if ( get_http_hlsts_type(pPkg,uiPkgLen,pHttpRequestPara) != APP_INVALID )
        return APP_HLS_TS;

    if ( get_http_xml_type(pPkg,uiPkgLen,pHttpRequestPara) != APP_INVALID )
        return APP_CROSSDOMAIN_XML;

    return APP_INVALID;
}

int CProtol::DoSend200ok2Client(const SConnInfo& oConnInfo)
{
    SUBMCD_DEBUG_P(LOG_NORMAL, "CProtol::process_cdn_req send m_strHttpOkRsp2Cdn. "
                               "uiSrcType: %u, flow: %u, ip: %s, cdn_port: %u, m_strHttpOkRsp2Cdn: %s\n", 
        oConnInfo.uiSrcType,
        oConnInfo.uiFlow,
        ip_to_str(oConnInfo.uiIp).c_str(),
        oConnInfo.usPort,
        m_strHttpOkRsp2Cdn.c_str());

    rsp2Client(oConnInfo,m_strHttpOkRsp2Cdn.c_str(),m_strHttpOkRsp2Cdn.size());

    return 0;
}

int CProtol::DoSend404Error2Client(const SConnInfo& oConnInfo)
{
    SUBMCD_DEBUG_P(LOG_NORMAL, "CProtol::process_cdn_req send m_strHttpFailRsp2Cdn. "
                               "uiSrcType: %u, flow: %u, ip: %s, cdn_port: %u, m_strHttpFailRsp2Cdn: %s\n", 
        oConnInfo.uiSrcType,
        oConnInfo.uiFlow,
        ip_to_str(oConnInfo.uiIp).c_str(),
        oConnInfo.usPort,
        m_strHttpFailRsp2Cdn.c_str());

    rsp2Client(oConnInfo,m_strHttpFailRsp2Cdn.c_str(),m_strHttpFailRsp2Cdn.size());

    return 0;
}

void 
test_hexdump(char* p,int len, const char* type)
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

int CProtol::DoSendm3u8toClient(const SVideoIdInfo &oVideoIdInfo, const SConnInfo& oConnInfo)
{
    static char    buf[2048];
    int            n = 0;
    hls_fragmem_t *videoHls = NULL;
    char datebuf[64];    
    time_t now;
    struct tm tm;

    SUBMCD_DEBUG_P(LOG_NORMAL, "CProtol::DoSendm3u8toClient: info uiVideoId: %u, uiSrcType: %u, "
                               "flow: %u, ip: %s, cdn_port: %u, m_strHttpOkRsp2Cdn: %s\n", 
        oVideoIdInfo.uiVideoId,
        oConnInfo.uiSrcType,
        oConnInfo.uiFlow,
        ip_to_str(oConnInfo.uiIp).c_str(),
        oConnInfo.usPort,
        m_strHttpOkRsp2Cdn.c_str());


    // lookup for m3u8 according to videoid.
    SUBMCD_DEBUG_P(LOG_NORMAL, "CProtol::DoSendm3u8toClient: get video, uiVideoId: %u\n",
                                   oVideoIdInfo.uiVideoId);
    videoHls = (hls_fragmem_t*)hls_cache_mpegts_search(oVideoIdInfo.uiVideoId);

    if ( videoHls == NULL ) {
       SUBMCD_DEBUG_P(LOG_NORMAL, "CProtol::DoSendm3u8toClient: video not found, uiVideoId: %u\n",
                                   oVideoIdInfo.uiVideoId);
       DoSend404Error2Client(oConnInfo);
       return -1;
    }
    
    SUBMCD_DEBUG_P(LOG_NORMAL, "CProtol::DoSendm3u8toClient: video found, uiVideoId: %u\n",
                                   oVideoIdInfo.uiVideoId);
   
    now = time(0);
    tm = *gmtime(&now);
    n = strftime(datebuf, sizeof datebuf, "%a, %d %b %Y %H:%M:%S %Z", &tm);
    datebuf[n]='\0';

    n = sprintf(buf,m_strHttpResponse.c_str(),datebuf,datebuf,videoHls->playlist_len, 
                    videoHls->playlist_len-1, videoHls->playlist_len);

    SUBMCD_DEBUG_P(LOG_NORMAL, "CProtol::DoSendTstoClient: http header, http_header=%s\n", buf);

    if ( n > 0 ) {
       rsp2Client(oConnInfo,buf,n);
       rsp2Client(oConnInfo,videoHls->playlist,videoHls->playlist_len);
       //test_hexdump(videoHls->playlist,videoHls->playlist_len,"m3u8content");
    } else {
       SUBMCD_DEBUG_P(LOG_NORMAL, "CProtol::DoSendm3u8toClient: response failed, uiVideoId: %u\n",
                                   oVideoIdInfo.uiVideoId);
       DoSend404Error2Client(oConnInfo);
       return -1;
    }

    return 0;
}

int CProtol::DoSendTstoClient(const SHttpRequestPara &oHttpRequestPara, const SConnInfo& oConnInfo)
{
    static char    buf[2048];
    int            n = 0;
    hls_fragmem_t *videoHls = NULL;
    hls_mem_t     *tsmem = NULL;
    char datebuf[64];    
    time_t now;
    struct tm tm;

    SUBMCD_DEBUG_P(LOG_NORMAL, "CProtol::DoSendTstoClient: info uiVideoId: %u, uiSrcType: %u, "
                               "flow: %u, ip: %s, cdn_port: %u, m_strHttpOkRsp2Cdn: %s\n", 
        oHttpRequestPara.uiVideoId,
        oConnInfo.uiSrcType,
        oConnInfo.uiFlow,
        ip_to_str(oConnInfo.uiIp).c_str(),
        oConnInfo.usPort,
        m_strHttpOkRsp2Cdn.c_str());


    // lookup for m3u8 according to videoid.
    SUBMCD_DEBUG_P(LOG_NORMAL, "CProtol::DoSendTstoClient: get video, uiVideoId: %u\n",
                                   oHttpRequestPara.uiVideoId);
    videoHls = (hls_fragmem_t*)hls_cache_mpegts_search(oHttpRequestPara.uiVideoId);

    if ( videoHls == NULL ) {
       SUBMCD_DEBUG_P(LOG_NORMAL, "CProtol::DoSendTstoClient: video not found, uiVideoId: %u",
                                   oHttpRequestPara.uiVideoId);
       DoSend404Error2Client(oConnInfo);
       return -1;
    }

    tsmem = hls_tsmem_get(videoHls,oHttpRequestPara.uiTsId);
    
    if ( tsmem == NULL ) {
       SUBMCD_DEBUG_P(LOG_ERROR, "CProtol::DoSendTstoClient: tsmem not found, uiVideoId: %u, tsid:%u\n",
                                   oHttpRequestPara.uiVideoId,oHttpRequestPara.uiTsId);
       DoSend404Error2Client(oConnInfo);
       return -2;
    }
   
    now = time(0);
    tm = *gmtime(&now);
    n = strftime(datebuf, sizeof datebuf, "%a, %d %b %Y %H:%M:%S %Z", &tm);
    datebuf[n]='\0';

    n = sprintf(buf,m_strHttpResponse.c_str(), datebuf, datebuf, tsmem->len, 
                            tsmem->len-1, tsmem->len);

    SUBMCD_DEBUG_P(LOG_TRACE, "CProtol::DoSendTstoClient: video found, uiVideoId=%u, tsid=%u data_len=%u\n",
                                   oHttpRequestPara.uiVideoId,oHttpRequestPara.uiTsId, tsmem->len);

    SUBMCD_DEBUG_P(LOG_NORMAL, "CProtol::DoSendTstoClient: http header, http_header=%s\n", buf);
    if ( n > 0 ) {
       rsp2Client(oConnInfo,buf,n);
       rsp2Client(oConnInfo,(char*)tsmem->data,tsmem->len);
    } else {
       SUBMCD_DEBUG_P(LOG_ERROR, "CProtol::DoSendTstoClient: response failed, uiVideoId: %u",
                                   oHttpRequestPara.uiVideoId);
       DoSend404Error2Client(oConnInfo);
       return -1;
    }

    return 0;
}

int CProtol::DoSendCrossDomain2Client(const SConnInfo& oConnInfo)
{
    static char    buf[2048];
    int n = 0;
    int len = 0;
    char datebuf[64];    
    time_t now;
    struct tm tm;

    now = time(0);
    tm = *gmtime(&now);
    n = strftime(datebuf, sizeof datebuf, "%a, %d %b %Y %H:%M:%S %Z", &tm);
    datebuf[n]='\0';
    len = m_strHttpCrossDomain.size();

    //n = sprintf(buf,m_strHttpResponse.c_str(), datebuf, datebuf, len, len-1, len);
    n = sprintf(buf,m_strHttpCrossDomainRsp.c_str(), datebuf, datebuf, len, len-1, len);

    SUBMCD_DEBUG_P(LOG_ERROR, "CProtol::process_cdn_req send crossdomain xml "
                               "uiSrcType: %u, flow: %u, ip: %s, cdn_port: %u, size: %u, m_strHttpCrosDomain: %s\n", 
        oConnInfo.uiSrcType,
        oConnInfo.uiFlow,
        ip_to_str(oConnInfo.uiIp).c_str(),
        oConnInfo.usPort,
        m_strHttpCrossDomain.size(),
        m_strHttpCrossDomain.c_str());

    if ( n > 0 ) {
       rsp2Client(oConnInfo,buf,n);
       rsp2Client(oConnInfo,m_strHttpCrossDomain.c_str(),m_strHttpCrossDomain.size());
    } else {
       SUBMCD_DEBUG_P(LOG_ERROR, "CProtol::DoSendCrossDomain2Client: response failed");
       DoSend404Error2Client(oConnInfo);
       return -1;
    }

    return 0;
}


int CProtol::DoSend200ok2Cdn(const SVideoIdInfo &oVideoIdInfo, SFlowInfo &oFlowInfo)
{
    SUBMCD_DEBUG_P(LOG_NORMAL, "CProtol::process_cdn_req send m_strHttpOkRsp2Cdn. "
                               "uiVideoId: %u, uiSrcType: %u, flow: %u, ip: %s, cdn_port: %u, m_strHttpOkRsp2Cdn: %s\n", 
        oVideoIdInfo.uiVideoId,
        oFlowInfo.uiSrcType,
        oFlowInfo.uiFlow,
        ip_to_str(oFlowInfo.uiIp).c_str(),
        oFlowInfo.usPort,
        m_strHttpOkRsp2Cdn.c_str());

    DoSendToFlow(oFlowInfo,m_strHttpOkRsp2Cdn.c_str(),m_strHttpOkRsp2Cdn.size());

    if (true == m_pMCD->_cfg.m_bUseQuickPushToCdn)
    {
        //DoQuickPushToCdn(oVideoIdInfo, oFlowInfo);
    }

    return 0;
}

int CProtol::process_p2p_http_req(const SConnInfo& oConnInfo, const char *pPkg, unsigned int uiPkgLen)
{
    SUBMCD_DEBUG_P(LOG_DEBUG, "CProtol::process_p2p_http_req uiSrcType: %u, flow: %u, ip: %s, port: %u\n", 
        oConnInfo.uiSrcType,
        oConnInfo.uiFlow,
        ip_to_str(oConnInfo.uiIp).c_str(),
        oConnInfo.usPort);

    vector<SVoiceChannel *> vecActiveChannel;
    m_pMCD->m_pDataChannelMgr->get_used_voice_channel(vecActiveChannel);

    SUBMCD_DEBUG_P(LOG_DEBUG, "CProtol::process_p2p_http_req vecActiveChannel.size: %u!\n", vecActiveChannel.size());

    Json::Value root;

    for (vector<SVoiceChannel *>::iterator it=vecActiveChannel.begin(); it!=vecActiveChannel.end(); it++)
    {
        if (false == (*it)->m_oP2PBlockIndexBuffer.haveData())
        {
            continue;
        }
        
        Json::Value channel;
        channel["beginblock"] = (*it)->m_oP2PBlockIndexBuffer.getBeginBlock();
        channel["endblock"] = (*it)->m_oP2PBlockIndexBuffer.getEndBlock();
        channel["beginsec"] = (*it)->m_oP2PBlockIndexBuffer.getBeginSec();
        channel["endsec"] = (*it)->m_oP2PBlockIndexBuffer.getEndSec();

        stringstream ss;
        ss << (*it)->getVideoId();
        string strVideoId = ss.str();
        channel["progid"] = strVideoId;

        root.append(channel);
    }

    Json::FastWriter oWriter;

    string strRoot = oWriter.write(root);
    SUBMCD_DEBUG_P(LOG_DEBUG, "CProtol::process_p2p_http_req strRoot: %s!\n", strRoot.c_str());

    Json::Reader reader;
    Json::Value result;

    if(!reader.parse(strRoot, result))
    {
        SUBMCD_DEBUG_P(LOG_ERROR, "CProtol::process_p2p_http_req parse error!\n");
        return -1;
    }
    else
    {
        const Json::Value channelList = result;
        int relyCount = channelList.size();

        for(int i = relyCount - 1; i >= 0; i--)
        {
            const Json::Value channel = channelList[i];

            unsigned int beginBlock = channel.get("beginblock", 0).asUInt();
            unsigned int endBlock = channel.get("endblock", 0).asUInt();
            unsigned int beginSec = channel.get("beginsec", 0).asUInt();
            unsigned int endSec = channel.get("endsec", 0).asUInt();

            unsigned int channelId = atoll(channel.get("progid", "").asString().c_str());

            SUBMCD_DEBUG_P(LOG_DEBUG, "CProtol::process_p2p_http_req beginBlock: %u, "
                                      "endBlock: %u, beginSec: %u, endSec: %u, channelId: %u\n", 
                beginBlock,
                endBlock,
                beginSec,
                endSec,
                channelId);
        }
    }

    stringstream ss;
    ss << strRoot.size();
    string strContentLen = ss.str();

    string strRsp2Peersvr;

    strRsp2Peersvr.append("HTTP/1.1 200 OK\r\n");
    strRsp2Peersvr.append("Content-Length: ");
    strRsp2Peersvr.append(strContentLen);
    strRsp2Peersvr.append("\r\n\r\n");
    strRsp2Peersvr.append(strRoot);

    SUBMCD_DEBUG_P(LOG_DEBUG, "CProtol::process_p2p_http_req strRsp2Peersvr: %s\n", strRsp2Peersvr.c_str());

    rsp2Client(oConnInfo, strRsp2Peersvr.c_str(), strRsp2Peersvr.size());

    return 0;
}

int CProtol::process_cdn_http_req(const SConnInfo& oConnInfo, const char *pPkg, unsigned int uiPkgLen)
{
    unsigned int uiVideoId = 0;

    SHttpRequestPara oHttpRequestPara;
    oHttpRequestPara.uiFlow = oConnInfo.uiFlow;

    int app_type = get_http_app_type(pPkg, uiPkgLen, &oHttpRequestPara);

    uiVideoId = oHttpRequestPara.uiVideoId;

    if (app_type == APP_INVALID)
    {
        SUBMCD_DEBUG_P(LOG_ERROR, "CProtol::process_http_req get_http_app_type failed. "
                                  "uiSrcType: %u, flow: %u, ip: %s, port: %u\n", 
            oConnInfo.uiSrcType,
            oConnInfo.uiFlow,
            ip_to_str(oConnInfo.uiIp).c_str(),
            oConnInfo.usPort);

        return -1;
    }

    SUBMCD_DEBUG_P(LOG_DEBUG, "CProtol::process_http_req "
                              "uiSrcType: %u, flow: %u, ip: %s, port: %u, app_type: %d\n", 
        oConnInfo.uiSrcType,
        oConnInfo.uiFlow,
        ip_to_str(oConnInfo.uiIp).c_str(),
        oConnInfo.usPort,
        app_type);

    SFlowInfo oVideoFlowInfo;

    oVideoFlowInfo.set_conn(oConnInfo);

    SVideoIdInfo oVideoIdInfo;
    oVideoIdInfo.clear();
    oVideoIdInfo.uiVideoId = uiVideoId;

    SVideoIdInfo *pVideoIdInfo = m_pMCD->m_infoChannelMgr->m_oVideoIdMgr.get(uiVideoId);

    if (NULL == pVideoIdInfo) {
        SUBMCD_DEBUG_P(LOG_ERROR, "CProtol::process_http_req oVideoIdInfo not found, "
                                  "flow: %u, uiSrcType: %u, ip: %s, port: %u, uiVideoId: %u\n", 
                oVideoFlowInfo.uiFlow,
                oVideoFlowInfo.uiSrcType,
                ip_to_str(oVideoFlowInfo.uiIp).c_str(),
                oVideoFlowInfo.usPort,
                uiVideoId);
        /*
        pVideoIdInfo = m_pMCD->m_infoChannelMgr->m_oVideoIdMgr.ins(oVideoIdInfo);

        if (NULL == pVideoIdInfo) {
            SUBMCD_DEBUG_P(LOG_ERROR, "CProtol::process_http_req ins oVideoIdInfo failed, "
                                      "flow: %u, uiSrcType: %u, ip: %s, port: %u, uiVideoId: %u\n", 
                oVideoFlowInfo.uiFlow,
                oVideoFlowInfo.uiSrcType,
                ip_to_str(oVideoFlowInfo.uiIp).c_str(),
                oVideoFlowInfo.usPort,
                uiVideoId);

            return -1;
        }
        */
    }

    switch(app_type)
    {
    case APP_CDN:
        {
            oVideoFlowInfo.unionFlag.ucIsHaveMediaInfo = 0;
            oVideoFlowInfo.ucFlowType = FLOW_TYPE_CDN;
        }
        break;
    case APP_HLS_M3U8:
        {
            oVideoFlowInfo.unionFlag.ucIsHaveMediaInfo = 0;
            oVideoFlowInfo.ucFlowType = FLOW_TYPE_HLS;
            DoSendm3u8toClient(oVideoIdInfo,oConnInfo);
        }
        break;
    case APP_HLS_TS:
        {
            oVideoFlowInfo.unionFlag.ucIsHaveMediaInfo = 0;
            oVideoFlowInfo.ucFlowType = FLOW_TYPE_HLS;
            DoSendTstoClient(oHttpRequestPara,oConnInfo);
        }
        break;

    case APP_CROSSDOMAIN_XML:
        {
            oVideoFlowInfo.unionFlag.ucIsHaveMediaInfo = 0;
            oVideoFlowInfo.ucFlowType = FLOW_TYPE_HLS;
            DoSendCrossDomain2Client(oConnInfo);
        }
        break;
    default:
        {
            SUBMCD_DEBUG_P(LOG_ERROR, "CProtol::process_http_req unknown app_type. "
                                      "uiSrcType: %u, flow: %u, ip: %s, port: %u, app_type: %d\n", 
                oConnInfo.uiSrcType,
                oConnInfo.uiFlow,
                ip_to_str(oConnInfo.uiIp).c_str(),
                oConnInfo.usPort,
                app_type);

            return -1;
        }
        break;
    }

    return 0;
/*
    SVideoIdInfo *pVideoIdInfo = NULL;
    SFlowInfo *pFlowInfo = NULL;
    int ret_add_video_flow = m_pMCD->m_infoChannelMgr->add_video_flow(uiVideoId, oVideoFlowInfo, &pVideoIdInfo, &pFlowInfo);

    if (0 != ret_add_video_flow || NULL == pVideoIdInfo || NULL == pFlowInfo)
    {
        SUBMCD_DEBUG_P(LOG_ERROR, "CProtol::process_http_req add_video_flow failed, "
                                  "uiVideoId: %u, uiSrcType: %u, flow: %u, ip: %s, "
                                  "cdn_port: %u, ret_add_video_flow: %d, pVideoIdInfo: 0x%x, "
                                  "pFlowInfo: 0x%x\n", 
            uiVideoId,
            oConnInfo.uiSrcType,
            oConnInfo.uiFlow,
            ip_to_str(oConnInfo.uiIp).c_str(),
            oConnInfo.usPort,
            ret_add_video_flow,
            pVideoIdInfo,
            pFlowInfo);

        return -1;
    }

    pVideoIdInfo->uiTouchTime = m_pMCD->m_cur_time;

    switch(app_type)
    {
    case APP_CDN:
        {
            DoSend200ok2Cdn(*pVideoIdInfo, *pFlowInfo);
        }
        break;
    case APP_HLS_M3U8:
        {
            SUBMCD_DEBUG_P(LOG_ERROR, "CProtol::process_http_req m3u8 response\n");
            DoSend200ok2Cdn(*pVideoIdInfo, *pFlowInfo);
        }
        break;
    case APP_HLS_TS:
        {
            SUBMCD_DEBUG_P(LOG_ERROR, "CProtol::process_http_req ts response\n");
            DoSend200ok2Cdn(*pVideoIdInfo, *pFlowInfo);
        }
        break;
    default:
        {
            SUBMCD_DEBUG_P(LOG_ERROR, "CProtol::process_http_req unknown app_type, "
                                      "uiSrcType: %u, flow: %u, ip: %s, port: %u, app_type: %d\n", 
                oConnInfo.uiSrcType,
                oConnInfo.uiFlow,
                ip_to_str(oConnInfo.uiIp).c_str(),
                oConnInfo.usPort,
                app_type);

            return -1;
        }
        break;
    }

    return 0;
*/
}

int CProtol::del_flow(const SConnInfo& oConnInfo)
{
    int ret = m_pMCD->m_infoChannelMgr->del_flow(oConnInfo);

    return ret;
}


int CProtol::OnRecvVideoPost(const SConnInfo& oConnInfo, const char *pPkg, unsigned int uiPkgLen)
{
    if (pPkg == NULL || uiPkgLen == 0)
    {
        SUBMCD_DEBUG_P(LOG_ERROR, "CProtol::process_videosvr_http_post pPkg == NULL || uiPkgLen == 0!\n");
        return -1;
    }

    //��֤�Ƿ���post pic����
    char *p = strstr(pPkg,"POST QQTALKSTREAM");

    if (p!=pPkg)
    {
        SUBMCD_DEBUG_P(LOG_ERROR, "CProtol::process_videosvr_http_post [POST QQTALKSTREAM] index error!\n");
        return -1;
    }

    //��ȡContent-Length�ַ�λ��
    string strContentLenBeginTag = "Content-Length:";
    char *pContentLenTagIndex = strstr(pPkg,strContentLenBeginTag.c_str());

    if (pContentLenTagIndex == NULL)
    {
        SUBMCD_DEBUG_P(LOG_ERROR, "CProtol::process_videosvr_http_post can not find strContentLenBeginTag: %s!\n", 
                                  strContentLenBeginTag.c_str());
        return -1;
    }

    //��ȡ���ݳ�����ʼλ��
    char *pContentLenBeginIndex = pContentLenTagIndex + strContentLenBeginTag.size();

    //��ȡ���ݳ��Ƚ���λ��
    string strContentLenEndTag = "\r\n";
    char *pContentLenEndIndex = strstr(pContentLenBeginIndex, strContentLenEndTag.c_str());

    if (pContentLenEndIndex == NULL)
    {
        SUBMCD_DEBUG_P(LOG_ERROR, "CProtol::process_videosvr_http_post can not find strContentLenEndTag: %s!\n", 
                                  strContentLenEndTag.c_str());
        return -1;
    }

    //��ȡ���ݳ���
    string strConTentLen(pContentLenBeginIndex,pContentLenEndIndex-pContentLenBeginIndex);
    unsigned int uiContentLen = 0;
    sscanf(strConTentLen.c_str(),"%u", &uiContentLen);

    if (uiContentLen == 0)
    {
        SUBMCD_DEBUG_P(LOG_ERROR, "CProtol::process_videosvr_http_post uiContentLen == 0!\n");
        return -1;
    }

    //��ȡ������ʼλ��
    string strContentTag = "\r\n\r\n";
    char *pContentTag = strstr(pPkg, strContentTag.c_str());

    if (pContentTag == NULL)
    {
        SUBMCD_DEBUG_P(LOG_ERROR, "CProtol::process_videosvr_http_post can not find strContentTag: %s!\n", 
                                  strContentTag.c_str());
        return -1;
    }

    char *pContent = pContentTag + strContentTag.size();

    unsigned int uiRealContentLen = uiPkgLen - (pContent - pPkg);

    if (uiContentLen != uiRealContentLen)
    {
        SUBMCD_DEBUG_P(LOG_ERROR, "CProtol::process_videosvr_http_post uiContentLen != uiRealContentLen."
                                  " uiContentLen: %u, uiRealContentLen: %u!\n", 
                                  uiContentLen,
                                  uiRealContentLen);

        return -1;
    }

    SMuxFrameInfo oFrameInfo;
    bool bRet = oFrameInfo.ParseFromArray(pContent, uiContentLen);

    if(!bRet)
    {
        SUBMCD_DEBUG_P(LOG_ERROR, "CProtol::process_videosvr_http_post ParseFromArray: invalid content!\n");
        return -1;
    }

    /*
    required uint32 uiUin 	= 1;
    required uint32 uiRoomId 	= 2;
    required uint32 uiMainRoomId = 3; 
    required uint32 uiFrameSeq 	= 4;
    required uint32 uiVideoId	= 5;
    required uint32 uiRoomType  = 6;
    required uint32 uiKeyFrame	= 7;
    required bytes StrMediaInfo = 8;
    required bytes StrFrame 	= 9;
    */

    unsigned int uiUin		= oFrameInfo.uiuin();
    unsigned int uiRoomId	= oFrameInfo.uiroomid();
    unsigned int uiMainRoomId	= oFrameInfo.uimainroomid();
    unsigned int uiFrameSeq	= oFrameInfo.uiframeseq();
    unsigned int uiVideoId	= oFrameInfo.uivideoid();
    //unsigned int uiRoomType	= oFrameInfo.uiroomtype();
    unsigned char ucKeyFrame	= (unsigned char)oFrameInfo.uikeyframe();
    //unsigned char *pStrMedia	= (unsigned char*)oFrameInfo.strmediainfo().c_str();
    unsigned int uiStrMediaLen  = oFrameInfo.strmediainfo().size();
    unsigned char *pFrameInfo	= (unsigned char*)oFrameInfo.strframe().c_str();
    unsigned int uiStrFrameLen	= oFrameInfo.strframe().size();
    unsigned int uiWidth	= oFrameInfo.uiwidth();
    unsigned int uiHeight	= oFrameInfo.uiheight();
    unsigned int uiFps		= oFrameInfo.uifps();
    unsigned int uiBitRate	= oFrameInfo.uibitrate();
    unsigned int uiFrameOrignSize = oFrameInfo.uiframeorignsize();
    unsigned int uiFrameTime	= oFrameInfo.uiframetime();
    unsigned int uiOriginalVideoId = 0;

    if(oFrameInfo.has_uioriginalvideoid())
    {
        uiOriginalVideoId = oFrameInfo.uioriginalvideoid();
    }


    SFrameVideo oFrameVideo;
    oFrameVideo.uiFps		= uiFps;
    oFrameVideo.uiWidth		= uiWidth;
    oFrameVideo.uiHeight	= uiHeight;
    oFrameVideo.uiVideoId	= uiVideoId;
    oFrameVideo.uiBitRate	= uiBitRate;
    oFrameVideo.uiFrameSeq	= uiFrameSeq;
    oFrameVideo.uiFrameSize	= uiStrFrameLen;
    oFrameVideo.uiCurSize	= uiFrameOrignSize;
    oFrameVideo.ucKeyFrame	= ucKeyFrame;
    oFrameVideo.pFrameData	= (char*)pFrameInfo;
    oFrameVideo.uiFrameTime	= uiFrameTime;
    oFrameVideo.uiOriginalVideoId = uiOriginalVideoId;

    SUserInfo oUserInfo;
    oUserInfo.set_conn(oConnInfo);

    oUserInfo.uiUin			= uiUin;
    oUserInfo.uiRoomId		= uiRoomId;
    oUserInfo.uiMainRoomId	= uiMainRoomId;
    oUserInfo.uiVideoId		= uiVideoId;
    oUserInfo.uiOriginalVideoId	= uiOriginalVideoId;

    SUBMCD_DEBUG_P(LOG_DEBUG, "CProtol::process_videosvr_http_post uin: %u, roomid: %u, uiMainRoomId: %u,"
                              " videoid: %u, original_videoid: %u, uiWidth: %u, uiHeight: %u, uiFps: %u,"
                              " uiBitRate: %u, ucMediaLen: %d, uiFrameSeq: %u, uiFrameLen: %u, uiFrameTime: %u\n", 
        uiUin,
        uiRoomId,
        uiMainRoomId,
        uiVideoId,
        uiOriginalVideoId,
        uiWidth,
        uiHeight,
        uiFps,
        uiBitRate,
        uiStrMediaLen,
        uiFrameSeq,
        uiStrFrameLen,
        uiFrameTime);

    SVideoIdInfo oVideoIdInfo;
    oVideoIdInfo.uiUin		= uiUin;
    oVideoIdInfo.uiVideoId	= uiVideoId;
    oVideoIdInfo.uiRoomId	= uiRoomId;

    return OnVideoFrame(oUserInfo, oVideoIdInfo, oFrameVideo);
}

void CProtol::process_http_msg(const SConnInfo& oConnInfo, const char *pPkg, unsigned int uiPkgLen)
{
    if(NULL == pPkg || 0 == uiPkgLen)
    {
        SUBMCD_DEBUG_P(LOG_ERROR, "CProtol::process_http_msg  pPkg == NULL || uiPkgLen == 0!\n");
        return;
    }

    CHttpRequestLine oHttpRequestLine;
    en_http_request_type enMethod;
    string path;
    string file;
    string arg;

    int ret = oHttpRequestLine.parse(pPkg, uiPkgLen, enMethod, path, file, arg);

    //SUBMCD_DEBUG_P(LOG_DEBUG, "CProtol::process_http_msg  path: %s, file: %s, arg: %s, enMethod=%u, ret=%d\n", 
    //                           path.c_str(), file.c_str(), arg.c_str(), enMethod, ret);
    //flv_hexdump(pPkg,uiPkgLen,"process_http");

    if (ret != 0) {
        SUBMCD_DEBUG_P(LOG_ERROR, "CProtol::process_http_msg  parsing http req failed\n");
        return ;
    }
    
    if (enMethod == C_HTTP_REQ_TYPE_GET) {
        process_cdn_http_req(oConnInfo, pPkg, uiPkgLen);
    }
    else if (enMethod == C_HTTP_REQ_TYPE_POST) {
        char *p = strstr(pPkg, "POST QQTALKSTREAM");

        if(p == pPkg) {
            // video data from videosvr
            OnRecvVideoPost(oConnInfo,pPkg,uiPkgLen);
        }
    }
    else {
        SUBMCD_DEBUG_P(LOG_ERROR, "CProtol::process_http_msg  enMethod invalid! enMethod: %u\n", enMethod);
        return;
    }
}

void CProtol::process_game_msg(const SConnInfo& oConnInfo, const char *pPkg, unsigned int uiPkgLen)
{
    if(NULL == pPkg || 0 == uiPkgLen)
    {
        SUBMCD_DEBUG_P(LOG_ERROR, "CProtol::process_game_msg  pPkg == NULL || uiPkgLen == 0!\n");

        return;
    }

    ClientHeader *pClientHead	= (ClientHeader *)pPkg;

    if(ntohs(pClientHead->ushCmdID) == QT_CMD_QT_ACK)
    {
        unsigned int	uiPkgLen	= ntohl(*(unsigned int*)(pClientHead));

        QtAckMessage oAck;

        bool bRet = oAck.ParseFromArray(pPkg + sizeof(ClientHeader), uiPkgLen - sizeof(ClientHeader));

        if(!bRet)
        {
            SUBMCD_DEBUG_P(LOG_ERROR,"CProtol::process_game_msg ParseFromArray");
            return;
        }

        SUBMCD_DEBUG_P(LOG_COLOR,"CProtol::process_game_msg uin: %u, roomid: %u, sessionid: %u\n", 
            oAck.uin(),
            oAck.room_id(),
            oAck.sessionid());
    }

}


int CProtol::rsp2Client(const SConnInfo &oConnInfo, const char *pData, unsigned int uiSize)
{
    switch(oConnInfo.uiSrcType)
    {
    case SOCKET_TCP_80:
        m_pMCD->enqueue_2_ccd4(pData, uiSize, oConnInfo.uiFlow);
        break;
    case SOCKET_TCP_8000:
        m_pMCD->enqueue_2_ccd5(pData, uiSize, oConnInfo.uiFlow);
        break;
    case SOCKET_TCP_443:
        m_pMCD->enqueue_2_ccd6(pData, uiSize, oConnInfo.uiFlow);
        break;
    default:
        return -1;
    }

    return 0;
}


