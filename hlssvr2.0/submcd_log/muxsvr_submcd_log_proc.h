#ifndef _SUBMCD_LOG_PROC_H_
#define _SUBMCD_LOG_PROC_H_

#include <sys/time.h>
#include <string>
#include <map>

#include "tfc_object.h"
#include "tfc_cache_proc.h"
#include "tfc_net_ipc_mq.h"
#include "tfc_net_conn_map.hpp"
#include "tfc_net_ccd_define.h"

#include "../lib/Attr_API.h"
#include "gFunc.h"
#include "muxsvr_submcd_log_cfg.h"

using namespace std;

namespace muxsvr
{
    // »º³åÇø´óÐ¡
    const unsigned BUF_SIZE = 4*1024*1024;

    class CStatSubMcdLogProc : public tfc::cache::CacheProc
    {
    public:
        CStatSubMcdLogProc();
        virtual ~CStatSubMcdLogProc();

        virtual void run(const std::string& conf_file);

        timeval m_curTVTime;
        unsigned long long m_ll_now_Tick;

    public:
		void dispatch_mcd();

    protected:
        void dispatch_timeout();
        void init(const std::string &conf_file);
		void init_log();
        void init_ipc();
        int	 check_flags();

    public:
        CSubMcdLogCfgMng m_oCfg;

    protected:
        char* m_recv_buf;
        char* m_send_buf;

        tfc::net::CFifoSyncMQ *_mq_mcd_2_submcd_log;
    };
}

#endif

