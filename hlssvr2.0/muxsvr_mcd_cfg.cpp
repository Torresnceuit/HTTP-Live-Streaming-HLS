#include <net/if.h>
#include <sys/ioctl.h>
#include <time.h>

#include "tfc_base_str.h"

#include "gFunc.h"
#include "muxsvr_mcd_cfg.h"

using namespace tfc;
using namespace tfc::base;

using namespace muxsvr;

CCfgMng::CCfgMng()
{
	    m_uiSelfExternalIp          = 0;    //��������ip
	    m_uiSelfInternalIp			= 0;//��������ip

	    m_strExternalIP             ="";
	    m_strInternalIP             ="";

	    m_uiVideoIdHashKey          = 0;
	    m_uiVideoIdHashLen          = 0;
	    m_uiVideoIdHashTime         = 0;

	    m_uiCdnFlow80HashKey        = 0;
	    m_uiCdnFlow80HashLen        = 0;
	    m_uiCdnFlow80HashTime       = 0;

	    m_uiCdnFlow8000HashKey      = 0;
	    m_uiCdnFlow8000HashLen      = 0;
	    m_uiCdnFlow8000HashTime     = 0;

	    m_uiCdnFlow443HashKey       = 0;
	    m_uiCdnFlow443HashLen       = 0;
	    m_uiCdnFlow443HashTime      = 0;

	    m_uiVoiceChannelHashKey     = 0;
	    m_uiVoiceChannelHashLen     = 0;
	    m_uiVoiceChannelHashTime    = 0;

	    m_uiVoiceChannelIdHashKey   = 0;
	    m_uiVoiceChannelIdHashLen   = 0;
	    m_uiVoiceChannelIdHashTime  = 0;

	    m_uiRoomId2VideoIdHashKey   = 0;
	    m_uiRoomId2VideoIdHashLen   = 0;
	    m_uiRoomId2VideoIdHashTime  = 0;

	    m_uiLiveProgramMgrIp        = 0;
        m_usLiveProgramMgrPort      = 0;

		m_bUseZHJZNotity			= false;
		m_uiZHJZGameSvrIp			= 0;
		m_usZHJZGameSvrPort			= 0;

	    m_bUseQuickPushToCdn        = 0;
	    m_uiBackFrameNum            = 0;

	    m_bUserVideoIdAsVoiceChannelIndex = 0;
	    m_bUseAutoSeqReset          = 0;

        m_uiBlockLen                = 0;
}

CCfgMng::~CCfgMng()
{
}

int CCfgMng::LoadCfg(const std::string& conf_file)
{
    _path = conf_file;

    CFileConfig page;
    page.Init(_path);

    _log_para.log_level_ = from_str<int>(page["root\\log\\log_level"]);
    _log_para.log_type_ = from_str<int>(page["root\\log\\log_type"]);
    _log_para.path_ = page["root\\log\\path"];
    _log_para.name_prefix_ = page["root\\log\\name_prefix"];
    _log_para.max_file_size_ = from_str<int>(page["root\\log\\max_file_size"]);
    _log_para.max_file_no_ = from_str<int>(page["root\\log\\max_file_no"]);

    try
    {
        m_uiVideoIdHashKey = from_str<unsigned int>(page["root\\info\\videoidhashkey"]);
    }
    catch(...)
    {
        m_uiVideoIdHashKey = 0x20131001;
    }

    try
    {
        m_uiVideoIdHashLen = from_str<unsigned int>(page["root\\info\\videoidhashlen"]);
    }
    catch(...)
    {
        m_uiVideoIdHashLen = 3000;
    }

    try
    {
        m_uiVideoIdHashTime = from_str<unsigned int>(page["root\\info\\videoidhashtime"]);
    }
    catch(...)
    {
        m_uiVideoIdHashTime = 10;
    }

    try
    {
        m_uiCdnFlow80HashKey = from_str<unsigned int>(page["root\\info\\cdnflow80hashkey"]);
    }
    catch(...)
    {
        m_uiCdnFlow80HashKey = 0x20131002;
    }

    try
    {
        m_uiCdnFlow80HashLen = from_str<unsigned int>(page["root\\info\\cdnflow80hashlen"]);
    }
    catch(...)
    {
        m_uiCdnFlow80HashLen = 10000;
    }

    try
    {
        m_uiCdnFlow80HashTime = from_str<unsigned int>(page["root\\info\\cdnflow80hashtime"]);
    }
    catch(...)
    {
        m_uiCdnFlow80HashTime = 10;
    }

    try
    {
        m_uiCdnFlow8000HashKey = from_str<unsigned int>(page["root\\info\\cdnflow8000hashkey"]);
    }
    catch(...)
    {
        m_uiCdnFlow8000HashKey = 0x20131003;
    }

    try
    {
        m_uiCdnFlow8000HashLen = from_str<unsigned int>(page["root\\info\\cdnflow8000hashlen"]);
    }
    catch(...)
    {
        m_uiCdnFlow8000HashLen = 10000;
    }

    try
    {
        m_uiCdnFlow8000HashTime = from_str<unsigned int>(page["root\\info\\cdnflow8000hashtime"]);
    }
    catch(...)
    {
        m_uiCdnFlow8000HashTime = 10;
    }

    try
    {
        m_uiCdnFlow443HashKey = from_str<unsigned int>(page["root\\info\\cdnflow443hashkey"]);
    }
    catch(...)
    {
        m_uiCdnFlow443HashKey = 0x20131004;
    }

    try
    {
        m_uiCdnFlow443HashLen = from_str<unsigned int>(page["root\\info\\cdnflow443hashlen"]);
    }
    catch(...)
    {
        m_uiCdnFlow443HashLen = 10000;
    }

    try
    {
        m_uiCdnFlow443HashTime = from_str<unsigned int>(page["root\\info\\cdnflow443hashtime"]);
    }
    catch(...)
    {
        m_uiCdnFlow443HashTime = 10;
    }

    try
    {
        m_uiVoiceChannelHashKey = from_str<unsigned int>(page["root\\info\\voicechannelhashkey"]);
    }
    catch(...)
    {
        m_uiVoiceChannelHashKey = 0x20131005;
    }

    try
    {
        m_uiVoiceChannelHashLen = from_str<unsigned int>(page["root\\info\\voicechannelhashlen"]);
    }
    catch(...)
    {
        m_uiVoiceChannelHashLen = 150;
    }

    try
    {
        m_uiVoiceChannelHashTime = from_str<unsigned int>(page["root\\info\\voicechannelhashtime"]);
    }
    catch(...)
    {
        m_uiVoiceChannelHashTime = 150;
    }

    try
    {
        m_uiVoiceChannelIdHashKey = from_str<unsigned int>(page["root\\info\\voicechannelidhashkey"]);
    }
    catch(...)
    {
        m_uiVoiceChannelIdHashKey = 0x20131006;
    }

    try
    {
        m_uiVoiceChannelIdHashLen = from_str<unsigned int>(page["root\\info\\voicechannelidhashlen"]);
    }
    catch(...)
    {
        m_uiVoiceChannelIdHashLen = 20000;
    }

    try
    {
        m_uiVoiceChannelIdHashTime = from_str<unsigned int>(page["root\\info\\voicechannelidhashtime"]);
    }
    catch(...)
    {
        m_uiVoiceChannelIdHashTime = 6;
    }

	try
	{
		m_uiRoomId2VideoIdHashKey	= from_str<unsigned int>(page["root\\info\\roomid2videoidhashkey"]);
	}
	catch(...)
	{
		m_uiRoomId2VideoIdHashKey	= 0x20131007;
	}
    printf("roomid2videoid: key=%u\n",m_uiRoomId2VideoIdHashKey);

	try
	{
		m_uiRoomId2VideoIdHashLen	= from_str<unsigned int>(page["root\\info\\roomid2videoidhashlen"]);
	}
	catch(...)
	{
		m_uiRoomId2VideoIdHashLen = 20000;
	}

	try
	{
		m_uiRoomId2VideoIdHashTime = from_str<unsigned int>(page["root\\info\\roomid2videoidhashtime"]);
	}
	catch(...)
	{
		m_uiRoomId2VideoIdHashTime = 6;
	}

	string strLiveProgramMgrIP;

	try
	{
		strLiveProgramMgrIP		= page["root\\liveprogrammgr\\ip"];
		m_uiLiveProgramMgrIp	= inet_addr(strLiveProgramMgrIP.c_str());
	}
	catch(...)
	{
		m_uiLiveProgramMgrIp = 0;
	}

	try
	{
		m_usLiveProgramMgrPort = from_str<unsigned short>(page["root\\liveprogrammgr\\port"]);
	}
	catch(...)
	{
		m_usLiveProgramMgrPort = 8000;
	}

	try
	{
		m_uiIsSendToRecordSvr = from_str<unsigned int>(page["root\\recordsvr\\is_send_recordsvr"]);
	}
	catch(...)
	{
		m_uiIsSendToRecordSvr = 0;
	}

	try
	{
		m_uiSessionHashLen = from_str<unsigned int>(page["root\\session\\hashlen"]);
	}
	catch(...)
	{
		m_uiSessionHashLen = 90;
	}

	try
	{
		m_uiSessionHashTime = from_str<unsigned int>(page["root\\session\\hashtime"]);
	}
	catch(...)
	{
		m_uiSessionHashTime = 4;
	}

	try
	{
		m_uiTsNum = from_str<unsigned int>(page["root\\session\\tsnum"]);
	}
	catch(...)
	{
		m_uiTsNum = 6;
    }

	try
	{
		m_uiTsDuration = from_str<unsigned int>(page["root\\session\\tsduration"]);
	}
	catch(...)
	{
		m_uiTsDuration = 10;
    }
	try
	{
		m_uiMaxAudioDelay = from_str<unsigned int>(page["root\\session\\maxaudiodelay"]);
	}
	catch(...)
	{
		m_uiMaxAudioDelay = 300;
    }

	printf("CCfgMng::LoadCfg load m_uiTsDuration: %u\n", m_uiTsDuration);
	try
	{
		unsigned int uiCount = 0;
		unsigned int uiMaxCount = 1000000;
		m_uiRecordSvrIp.clear();
		std::vector<std::string> vectIppath = page.GetDomains("root\\recordsvr\\IP");
		for (std::vector<std::string>::const_iterator cit = vectIppath.begin();
			cit != vectIppath.end(); ++cit)
		{
			++uiCount;
			if (uiCount > uiMaxCount)
			{
				break;
			}
			std::string strIP = *cit;
			struct in_addr addr;
			inet_aton(strIP.c_str(), &addr);

			m_uiRecordSvrIp.push_back(addr.s_addr);
			printf("CCfgMng::LoadCfg load m_uiRecordSvrIp host. ip: %s\n", strIP.c_str());
		}
	}
	catch(...)
	{
		m_uiRecordSvrIp.clear();
	}
	try
	{
		unsigned int uiCount = 0;
		unsigned int uiMaxCount = 1000000;
		m_usRecordSvrPort.clear();
		std::vector<std::string> vectPort = page.GetDomains("root\\recordsvr\\Port");
		for (std::vector<std::string>::const_iterator cit = vectPort.begin();
			cit != vectPort.end(); ++cit)
		{
			++uiCount;
			if (uiCount > uiMaxCount)
			{
				break;
			}
			std::string strPort = *cit;
			unsigned short usPort = from_str<unsigned short>(strPort);

			m_usRecordSvrPort.push_back(usPort);
			printf("CCfgMng::LoadCfg load m_usRecordSvrPort Port: %d\n", usPort);
		}
	}
	catch(...)
	{
		m_usRecordSvrPort.clear();
	}

	try
	{
		m_uiIsSendToRtmpSvr = from_str<unsigned int>(page["root\\rtmpsvr\\is_send_rtmpsvr"]);
	}
	catch(...)
	{
		m_uiIsSendToRtmpSvr = 0;
	}
	printf("CCfgMng::LoadCfg load m_uiIsSendToRtmpSvr: %u\n", m_uiIsSendToRtmpSvr);
	try
	{
		unsigned int uiCount = 0;
		unsigned int uiMaxCount = 1000000;
		m_uiRtmpSvrIp.clear();
		std::vector<std::string> vectIppath = page.GetDomains("root\\rtmpsvr\\IP");
		for (std::vector<std::string>::const_iterator cit = vectIppath.begin();
			cit != vectIppath.end(); ++cit)
		{
			++uiCount;
			if (uiCount > uiMaxCount)
			{
				break;
			}
			std::string strIP = *cit;
			struct in_addr addr;
			inet_aton(strIP.c_str(), &addr);

			m_uiRtmpSvrIp.push_back(addr.s_addr);
			printf("CCfgMng::LoadCfg load m_uiRtmpSvrIp host. ip: %s\n", strIP.c_str());
		}
	}
	catch(...)
	{
		m_uiRtmpSvrIp.clear();
	}
	try
	{
		unsigned int uiCount = 0;
		unsigned int uiMaxCount = 1000000;
		m_usRtmpSvrPort.clear();
		std::vector<std::string> vectPort = page.GetDomains("root\\rtmpsvr\\Port");
		for (std::vector<std::string>::const_iterator cit = vectPort.begin();
			cit != vectPort.end(); ++cit)
		{
			++uiCount;
			if (uiCount > uiMaxCount)
			{
				break;
			}
			std::string strPort = *cit;
			unsigned short usPort = from_str<unsigned short>(strPort);

			m_usRtmpSvrPort.push_back(usPort);
			printf("CCfgMng::LoadCfg load m_usRtmpSvrPort Port: %d\n", usPort);
		}
	}
	catch(...)
	{
		m_usRtmpSvrPort.clear();
	}

	try
	{
		if ("true" == page["root\\ZHJZ\\open_notify"])
		{
			m_bUseZHJZNotity = true;
		}
		else
		{
			m_bUseZHJZNotity = false;
		}
	}
	catch(...)
	{
		m_bUseZHJZNotity = false;
	}

	string strZHJZGameSvrIp;
	try
	{
		strZHJZGameSvrIp	= page["root\\ZHJZ\\ip"];
		m_uiZHJZGameSvrIp	= inet_addr(strZHJZGameSvrIp.c_str());
	}
	catch(...)
	{
		m_uiZHJZGameSvrIp	= 0;
	}
	
	try
	{
		m_usZHJZGameSvrPort = from_str<unsigned short>(page["root\\ZHJZ\\port"]);
	}
	catch(...)
	{
		m_usZHJZGameSvrPort = 8080;
	}

	try
	{
		m_uiSubRoomRecord = from_str<unsigned int>(page["root\\subroomrecord\\roomid"]);
	}
	catch(...)
	{
		m_uiSubRoomRecord = 0;
	}

	printf("CCfgMng::LoadCfg load m_uiSubRoomRecord: %u\n", m_uiSubRoomRecord);
    try
    {
        m_uiBackFrameNum = from_str<unsigned int>(page["root\\cdn\\uiBackFrameNum"]);
    }
    catch(...)
    {
        m_uiBackFrameNum = 40;
    }

    try
    {
        if ("true" == page["root\\cdn\\use_quick_push_to_cdn"])
        {
            m_bUseQuickPushToCdn = true;
        }
        else
        {
            m_bUseQuickPushToCdn = false;
        }
    }
    catch(...)
    {
        m_bUseQuickPushToCdn = false;
    }

    try
    {
        if ("true" == page["root\\cdn\\use_flashp2p"])
        {
            m_bUseFlashp2p = true;
        }
        else
        {
            m_bUseFlashp2p = false;
        }
    }
    catch(...)
    {
        m_bUseFlashp2p = false;
    }

    try
    {
        m_uiBlockLen = from_str<unsigned int>(page["root\\flashp2p\\uiBlockLen"]);
    }
    catch(...)
    {
        m_uiBlockLen = 1024*16;
    }

    string strCacheSvrIP;

/*
    try
    {
        strCacheSvrIP = page["root\\cachesvr\\ip"];
        m_uiCacheSvrIP = inet_addr(strCacheSvrIP.c_str());
    }
    catch(...)
    {
        m_uiCacheSvrIP = 0;
    }

    try
    {
        m_uiCacheSvrPort = from_str<unsigned int>(page["root\\cachesvr\\port"]);
    }
    catch(...)
    {
        m_uiCacheSvrPort = 0;
    }
*/
	try
	{
		unsigned int uiCount = 0;
		unsigned int uiMaxCount = 1000000;
		m_uiCacheSvrIp.clear();
		std::vector<std::string> vectIppath = page.GetDomains("root\\cachesvr\\IP");
		for (std::vector<std::string>::const_iterator cit = vectIppath.begin();
			cit != vectIppath.end(); ++cit)
		{
			++uiCount;
			if (uiCount > uiMaxCount)
			{
				break;
			}
			std::string strIP = *cit;
			struct in_addr addr;
			inet_aton(strIP.c_str(), &addr);

			m_uiCacheSvrIp.push_back(addr.s_addr);
			printf("CCfgMng::LoadCfg load m_uiCacheSvrIp host. ip: %s\n", strIP.c_str());
		}
	}
	catch(...)
	{
		m_uiCacheSvrIp.clear();
	}
	try
	{
		unsigned int uiCount = 0;
		unsigned int uiMaxCount = 1000000;
		m_usCacheSvrPort.clear();
		std::vector<std::string> vectPort = page.GetDomains("root\\cachesvr\\Port");
		for (std::vector<std::string>::const_iterator cit = vectPort.begin();
			cit != vectPort.end(); ++cit)
		{
			++uiCount;
			if (uiCount > uiMaxCount)
			{
				break;
			}
			std::string strPort = *cit;
			unsigned short usPort = from_str<unsigned short>(strPort);

			m_usCacheSvrPort.push_back(usPort);
			printf("CCfgMng::LoadCfg load m_usCacheSvrPort Port: %d\n", usPort);
		}
	}
	catch(...)
	{
		m_usCacheSvrPort.clear();
	}


    m_uiSelfExternalIp = getipbyif("eth0");
    m_uiSelfInternalIp = getipbyif("eth1");

    m_strExternalIP = ip_to_str(m_uiSelfExternalIp);
    m_strInternalIP = ip_to_str(m_uiSelfInternalIp);

    printf("Server External ip: %s, Internal ip: %s\n", m_strExternalIP.c_str(), m_strInternalIP.c_str());

    return 0;

}

int CCfgMng::ReloadCfg()
{
    CFileConfig page;
    page.Init(_path);

    LoadCfg(_path);

    return 0;
}
