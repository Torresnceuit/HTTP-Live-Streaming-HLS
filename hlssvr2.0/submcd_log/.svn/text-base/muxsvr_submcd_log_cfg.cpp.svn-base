#include <net/if.h>
#include <sys/ioctl.h>
#include <time.h>

#include "tfc_base_str.h"

#include "gFunc.h"
#include "muxsvr_submcd_log_cfg.h"

using namespace tfc;
using namespace tfc::base;
using namespace muxsvr;

CSubMcdLogCfgMng::CSubMcdLogCfgMng()
{
	m_uiSelfExternalIp	= 0;    //云字翌利ip
	m_uiSelfInternalIp	= 0;    //云字坪利ip

	m_strExternalIP		= "";
	m_strInternalIP		= "";
}

CSubMcdLogCfgMng::~CSubMcdLogCfgMng()
{
}

int CSubMcdLogCfgMng::LoadCfg(const std::string& conf_file)
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

    m_uiSelfExternalIp = getipbyif("eth0");
    m_uiSelfInternalIp = getipbyif("eth1");

    m_strExternalIP = ip_to_str(m_uiSelfExternalIp);
    m_strInternalIP = ip_to_str(m_uiSelfInternalIp);

    printf("Server External ip: %s, Internal ip: %s\n", m_strExternalIP.c_str(), m_strInternalIP.c_str());

	return 0;

}

int CSubMcdLogCfgMng::ReloadCfg()
{
    LoadCfg(_path);
    
    return 0;
}
