#ifndef _SUBMCD_LOG_CFG_H_
#define _SUBMCD_LOG_CFG_H_

#include <vector>
#include <string>

#include "tfc_base_config_file.h"
#include "tfc_debug_log.h"

using std::string;

namespace muxsvr
{
    class CSubMcdLogCfgMng
    {
    public:	
        CSubMcdLogCfgMng();
        ~CSubMcdLogCfgMng();
        int LoadCfg(const std::string& conf_file);
        int ReloadCfg();

    public:
        TLogPara _log_para;

        //以下存储为主机字节序
        unsigned int m_uiSelfExternalIp;    //本机外网ip
        unsigned int m_uiSelfInternalIp;    //本机内网ip

        string m_strExternalIP;
        string m_strInternalIP;

    private:	
        std::string _path;   
    };
}

#endif


