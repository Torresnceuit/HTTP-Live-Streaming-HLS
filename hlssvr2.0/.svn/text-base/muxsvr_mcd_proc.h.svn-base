#ifndef _MUXSVR_MCD_PROC_H_
#define _MUXSVR_MCD_PROC_H_

#include <string>
#include <map>

#include "tfc_object.h"
#include "tfc_cache_proc.h"
#include "tfc_net_ipc_mq.h"
#include "tfc_net_conn_map.hpp"
#include "tfc_net_ccd_define.h"

#include "Attr_API.h"
#include "submcd_log.h"

#include "muxsvr.h"
#include "muxsvr_errno.h"
#include "muxsvr_mcd_cfg.h"

#include "protol_process.h"

using namespace std;
using namespace muxsvr;

namespace muxsvr
{
    typedef struct
    {
        unsigned int	_magic_num;		//ccd 消息标记 = 123123123
        unsigned int	_reqrsp_type;	//消息号
        unsigned int	_flow;			//主要标识
        unsigned int	_ip;			//消息的ip， port
        unsigned int	_port;
        unsigned int    _uin;
        unsigned char   _room_type;
        unsigned int    _room_id;
        unsigned int    _seq;
        unsigned int    _broadcast_count;
        unsigned int	_other;			//扩展用， 用来记录 send 延时， recv 延时,
    }TUCCDHeader;

    // 缓冲区大小
    const unsigned BUF_SIZE = 4*1024*1024 + sizeof(TUCCDHeader);

    class CMCDProc : public tfc::cache::CacheProc
    {
    public:
        CMCDProc()
            :m_infoChannelMgr(NULL)
            ,m_pDataChannelMgr(NULL)
            ,m_oProtolProcess(NULL)
            ,m_recv_buf(NULL)
            ,m_send_buf(NULL)
        {
            //recvbuf
            m_recv_buf = new char[BUF_SIZE];
            _recv_uccd_header = (TUCCDHeader*)m_recv_buf;
            _recv_buf = m_recv_buf + sizeof(TUCCDHeader);

            m_send_buf = new char[BUF_SIZE];
            _send_uccd_header = (TUCCDHeader*)m_send_buf;
            _send_buf = m_send_buf + sizeof(TUCCDHeader);

            m_infoChannelMgr = new CInfoChannelMgr(this);
            assert(m_infoChannelMgr != NULL);

            m_pDataChannelMgr = new CDataChannelMgr(this);
            assert(m_pDataChannelMgr != NULL);

            m_oProtolProcess = new CProtol(this);
            assert(m_oProtolProcess != NULL);
        }

        virtual ~CMCDProc()
        {
            if(m_recv_buf != NULL) 
			{
				delete[] m_recv_buf;
			}

            if(m_send_buf != NULL) 
			{
				delete[] m_send_buf;
			}

            if(m_infoChannelMgr != NULL) 
            {
                delete m_infoChannelMgr;
            }

            if(m_pDataChannelMgr != NULL) 
			{
				delete m_pDataChannelMgr;
			}
			 
            if(m_oProtolProcess != NULL)
			{
				delete m_oProtolProcess;
			}
        }

        virtual void run(const std::string& conf_file);


        //接收客户端的消息 udp 80端口
        int enqueue_2_ccd(const char* buf, unsigned int buf_size, unsigned int uiFlow);

        //接收客户端的消息 udp 8000端口
        int enqueue_2_ccd2(const char* buf, unsigned int buf_size, unsigned int uiFlow);

        //接收客户端的消息 udp 443 
        int enqueue_2_ccd3(const char* buf, unsigned int buf_size, unsigned int uiFlow);

        //接收客户端的消息 tcp 80端口
        int enqueue_2_ccd4(const char* buf, unsigned int buf_size, unsigned int uiFlow);

        //接收客户端的消息 tcp 8000端口
        int enqueue_2_ccd5(const char* buf, unsigned int buf_size, unsigned int uiFlow);

        //接收服务器之间的消息 tcp 443端口
        int enqueue_2_ccd6(const char* buf, unsigned int buf_size, unsigned int uiFlow);

        int enqueue_2_dcc(const char* buf, unsigned int buf_size, unsigned int uiIp, unsigned short usPort);
        int enqueue_2_dcc2(const char* buf, unsigned int buf_size, unsigned int uiIp, unsigned short usPort);
        int enqueue_2_dcc2_with_flow(const char* buf, unsigned int buf_size, unsigned int uiIp, unsigned short usPort, unsigned int uiFlow);

		int enqueue_2_submcd_writer(const char *buf, unsigned int uiLen);

        int mcd_close_dcc_flow(unsigned int uiIp, unsigned short usPort);
        int mcd_close_dcc2_flow(unsigned int uiIp, unsigned short usPort);

    public:
        CCfgMng             _cfg;
        time_t              m_cur_time;
        timeval             m_curTVTime;
        unsigned long long  m_ll_now_Tick;

        time_t m_LastTickDoCleanInvalidVoiceChannel;
        time_t m_LastTickDoCleanTimeOutVideoId;
		time_t m_LastTickDoCleanTimeOutRoom2VideoId;

		time_t m_LastTickDoNotifyVideoMgr;

    public:
        void dispatch_ccd();
        void dispatch_ccd2();
        void dispatch_ccd3();

        void dispatch_ccd4();
        void dispatch_ccd5();
        void dispatch_ccd6();

        void dispatch_dcc();
        void dispatch_dcc2();

    protected:
        void dispatch_timeout();
        void init(const std::string &conf_file);
        void init_log();
        void init_ipc();
        int check_flags();

    protected:
        void process_cs_udp_msg(SConnInfo& oConnInfo, char *pPkg, unsigned int uiPkgLen);
     
        void process_ss_udp_msg(SConnInfo& oConnInfo, char *pPkg, unsigned int uiPkgLen);
        void process_ss_tcp_msg(SConnInfo& oConnInfo, char *pPkg, unsigned int uiPkgLen);
		void process_ss_tcp_game_msg(SConnInfo& oConnInfo, char *pPkg, unsigned int uiPkgLen);

		//void process_http_msg(SConnInfo& oConnInfo, const char *pPkg, unsigned int uiPkgLen);

        void del_flow(const SConnInfo& oConnInfo);
        
    public:
        CInfoChannelMgr     *m_infoChannelMgr;
        CDataChannelMgr     *m_pDataChannelMgr;
        CProtol             *m_oProtolProcess;

    protected:
        TUCCDHeader *_recv_uccd_header;
        TUCCDHeader *_send_uccd_header;

        char *m_recv_buf;
        char *_recv_buf;

        char *m_send_buf;
        char *_send_buf;
        //CMutex m_ccd_mutex;
        TFCObject _tfc_obj;

        tfc::net::CFifoSyncMQ *_mq_ccd_2_mcd, *_mq_mcd_2_ccd;
        tfc::net::CFifoSyncMQ *_mq_ccd2_2_mcd, *_mq_mcd_2_ccd2;
        tfc::net::CFifoSyncMQ *_mq_ccd3_2_mcd, *_mq_mcd_2_ccd3;

        tfc::net::CFifoSyncMQ *_mq_ccd4_2_mcd, *_mq_mcd_2_ccd4;
        tfc::net::CFifoSyncMQ *_mq_ccd5_2_mcd, *_mq_mcd_2_ccd5;
        tfc::net::CFifoSyncMQ *_mq_ccd6_2_mcd, *_mq_mcd_2_ccd6;

        tfc::net::CFifoSyncMQ *_mq_dcc_2_mcd, *_mq_mcd_2_dcc;
        tfc::net::CFifoSyncMQ *_mq_dcc2_2_mcd, *_mq_mcd_2_dcc2;

        tfc::net::CFifoSyncMQ *_mq_mcd_2_submcd_log;

		//tfc::net::CFifoSyncMQ *_mq_mcd_2_submcd_writer;
    };
}

#endif
