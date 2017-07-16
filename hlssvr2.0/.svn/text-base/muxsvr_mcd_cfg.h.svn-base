#ifndef _MUXSVR_MCD_CFG_H_
#define _MUXSVR_MCD_CFG_H_

#include "tfc_base_config_file.h"
#include "tfc_debug_log.h"
#include <vector>
#include <string>

using std::string;

namespace muxsvr
{
    class CCfgMng
    {
    public:	
        CCfgMng();
        ~CCfgMng();
        int LoadCfg(const std::string& conf_file);
        int ReloadCfg();

    public:
        TLogPara _log_para;

        //���´洢Ϊ�����ֽ���
        unsigned int m_uiSelfExternalIp;    //��������ip
        unsigned int m_uiSelfInternalIp;    //��������ip

        string m_strExternalIP;
        string m_strInternalIP;

        unsigned int m_uiVideoIdHashKey;
        unsigned int m_uiVideoIdHashLen;
        unsigned int m_uiVideoIdHashTime;

        unsigned int m_uiCdnFlow80HashKey;
        unsigned int m_uiCdnFlow80HashLen;
        unsigned int m_uiCdnFlow80HashTime;

        unsigned int m_uiCdnFlow8000HashKey;
        unsigned int m_uiCdnFlow8000HashLen;
        unsigned int m_uiCdnFlow8000HashTime;

        unsigned int m_uiCdnFlow443HashKey;
        unsigned int m_uiCdnFlow443HashLen;
        unsigned int m_uiCdnFlow443HashTime;

        unsigned int m_uiVoiceChannelHashKey;
        unsigned int m_uiVoiceChannelHashLen;
        unsigned int m_uiVoiceChannelHashTime;

        unsigned int m_uiVoiceChannelIdHashKey;
        unsigned int m_uiVoiceChannelIdHashLen;
        unsigned int m_uiVoiceChannelIdHashTime;

		unsigned int m_uiRoomId2VideoIdHashKey;
		unsigned int m_uiRoomId2VideoIdHashLen;
		unsigned int m_uiRoomId2VideoIdHashTime;

		unsigned int m_uiLiveProgramMgrIp;
		unsigned short m_usLiveProgramMgrPort;

		unsigned int m_uiIsSendToRecordSvr;
		std::vector<unsigned int> m_uiRecordSvrIp;
		std::vector<unsigned short> m_usRecordSvrPort;

		unsigned int m_uiIsSendToRtmpSvr;
		std::vector<unsigned int> m_uiRtmpSvrIp;
		std::vector<unsigned short> m_usRtmpSvrPort;

		bool m_bUseZHJZNotity;
		unsigned int m_uiZHJZGameSvrIp;
		unsigned short m_usZHJZGameSvrPort;

        bool m_bUseQuickPushToCdn;
        unsigned int m_uiBackFrameNum;

		bool m_bUserVideoIdAsVoiceChannelIndex;
		bool m_bUseAutoSeqReset;
        unsigned int m_uiSubRoomRecord;

        unsigned int m_uiBlockLen;
        bool m_bUseFlashp2p;

		unsigned int m_uiSessionHashTime;
		unsigned int m_uiSessionHashLen;
		unsigned int m_uiTsNum;
		unsigned int m_uiTsDuration;
		unsigned int m_uiMaxAudioDelay;

        //unsigned int m_uiCacheSvrIP;
        //unsigned short m_uiCacheSvrPort;
		std::vector<unsigned int> m_uiCacheSvrIp;
		std::vector<unsigned short> m_usCacheSvrPort;

    private:	
        std::string _path;   
    };
}

#endif
