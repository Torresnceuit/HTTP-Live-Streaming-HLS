#include <iostream>
#include <string>

#include "tfc_debug_log.h"
#include "tfc_net_epoll_flow.h"
#include "tfc_base_comm.h"
#include "tfc_str.hpp"

#include "CCheckFlag.h"
#include "gFunc.h"
#include "muxsvr_mcd_proc.h"

#include "hls/hls_mpegts.h"
#include "hls/hls_stream.h"

using namespace tfc::net;
using namespace tfc;
using namespace tfc::cache;
using namespace tfc::base;
using namespace muxsvr;

CCheckFlag check_flag;

void sigusr1_handle(int sig_val)
{
    check_flag.set_flag(FLG_CTRL_RELOAD);
    signal(SIGUSR1, sigusr1_handle);
}

void sigusr2_handle(int sig_val)
{
    check_flag.set_flag(FLG_CTRL_QUIT);
    signal(SIGUSR2, sigusr2_handle);
}

void disp_ccd(void* priv)
{
    CMCDProc* app = (CMCDProc*)priv;
    app->dispatch_ccd();
}

void disp_ccd2(void* priv)
{
    CMCDProc* app = (CMCDProc*)priv;
    app->dispatch_ccd2();
}

void disp_ccd3(void* priv)
{
    CMCDProc* app = (CMCDProc*)priv;
    app->dispatch_ccd3();
}

void disp_ccd4(void* priv)
{
    CMCDProc* app = (CMCDProc*)priv;
    app->dispatch_ccd4();
}

void disp_ccd5(void* priv)
{
    CMCDProc* app = (CMCDProc*)priv;
    app->dispatch_ccd5();
}

void disp_ccd6(void* priv)
{
    CMCDProc* app = (CMCDProc*)priv;
    app->dispatch_ccd6();
}

void disp_dcc(void* priv)
{
    CMCDProc* app = (CMCDProc*)priv;
    app->dispatch_dcc();
}

void disp_dcc2(void* priv)
{
    CMCDProc* app = (CMCDProc*)priv;
    app->dispatch_dcc2();
}

void CMCDProc::run(const std::string& conf_file)
{
    init(conf_file);    

    add_mq_2_epoll(_mq_ccd_2_mcd, disp_ccd, this);
    add_mq_2_epoll(_mq_ccd2_2_mcd, disp_ccd2, this);
    add_mq_2_epoll(_mq_ccd3_2_mcd, disp_ccd3, this);

    add_mq_2_epoll(_mq_ccd4_2_mcd, disp_ccd4, this);
    add_mq_2_epoll(_mq_ccd5_2_mcd, disp_ccd5, this);
    add_mq_2_epoll(_mq_ccd6_2_mcd, disp_ccd6, this);

    add_mq_2_epoll(_mq_dcc_2_mcd, disp_dcc, this);
    add_mq_2_epoll(_mq_dcc2_2_mcd, disp_dcc2, this);

    DEBUG_P(LOG_NORMAL, "muxsvr_mcd started!\n");

    m_ll_now_Tick = get_tick_count(m_curTVTime);
    m_cur_time = m_curTVTime.tv_sec;

    m_LastTickDoCleanInvalidVoiceChannel	= m_cur_time + 1;
    m_LastTickDoCleanTimeOutVideoId			= m_cur_time + 2;
	m_LastTickDoCleanTimeOutRoom2VideoId	= m_cur_time + 3;
	m_LastTickDoNotifyVideoMgr				= m_cur_time + 4;


    unsigned int uiLoopTimeCount = 0;
    unsigned int uiCheckFlagsCount = 0;

    for (;;)
    {
        uiCheckFlagsCount++;

        if (uiCheckFlagsCount >= 100)
        {
            uiCheckFlagsCount = 0;
            if (check_flags() > 0)
            {
                DEBUG_P(LOG_NORMAL, "(%s:%d)muxsvr_mcd recv signal, quit!\n", __FILE__, __LINE__);
                break;
            }
        }

        uiLoopTimeCount++;

        m_ll_now_Tick = get_tick_count(m_curTVTime);
        m_cur_time = m_curTVTime.tv_sec;

        if (uiLoopTimeCount >= 200)
        {
            uiLoopTimeCount = 0;
            dispatch_timeout();
        }

        run_epoll_4_mq();
    }
}

void CMCDProc::dispatch_timeout()
{
    if (abs(m_cur_time - m_LastTickDoCleanInvalidVoiceChannel) >= 30)
    {
        if (NULL != m_pDataChannelMgr)
        {
            m_pDataChannelMgr->DoCleanInvalidVoiceChannel();
        }

        m_LastTickDoCleanInvalidVoiceChannel = m_cur_time;  
    }

    if (abs(m_cur_time - m_LastTickDoCleanTimeOutVideoId) >= 30)
    {
        m_infoChannelMgr->DoCleanTimeOutVideoId();
        m_LastTickDoCleanTimeOutVideoId	= m_cur_time;  
    } 

	if(abs(m_cur_time - m_LastTickDoCleanTimeOutRoom2VideoId) >= 30)
	{
		m_infoChannelMgr->DoCleanTimeOutRoomId2VideoId();
		m_LastTickDoCleanTimeOutRoom2VideoId= m_cur_time;
	}
}

void CMCDProc::dispatch_ccd()
{
    unsigned int data_len, flow;
    int ret;
    unsigned int uiMaxCount = 0;
    do
    {
        uiMaxCount++;
        if (uiMaxCount >= 10000)
        {
            return;
        }

        ret = _mq_ccd_2_mcd->try_dequeue(_recv_buf, BUF_SIZE, data_len, flow);
        if (ret < 0)
        {
            SUBMCD_DEBUG_P(LOG_ERROR, "(%s:%d)dequeue msg failded!\n", __FILE__, __LINE__);
            return;
        }

        if (data_len <=0)
        {
            return;
        }

        TCCDHeader* ccd_header = (TCCDHeader*)_recv_buf;

        SConnInfo oConnInfo;

        oConnInfo.uiFlow = flow;
        oConnInfo.uiIp   = ccd_header->_ip;
        oConnInfo.usPort = ccd_header->_port;
        oConnInfo.uiSrcType = SOCKET_UDP_80;
        oConnInfo.uiHelloTime = m_cur_time;

        //SUBMCD_DEBUG_P(LOG_TRACE, "dispatch_ccd ip: %s, port: %u, flow: %u, msglen: %u\n", 
        //                    ip_to_str(oConnInfo.uiIp).c_str(),
        //                    oConnInfo.usPort,
        //                    oConnInfo.uiFlow,
        //                    data_len - sizeof(TCCDHeader));

        process_cs_udp_msg(oConnInfo, _recv_buf+sizeof(TCCDHeader), data_len-sizeof(TCCDHeader));
    }while(ret == 0 && data_len > 0);
}

void CMCDProc::dispatch_ccd2()
{
    unsigned int data_len, flow;
    int ret;
    unsigned int uiMaxCount = 0;
    do
    {
        uiMaxCount++;
        if (uiMaxCount >= 10000)
        {
            return;
        }

        ret = _mq_ccd2_2_mcd->try_dequeue(_recv_buf, BUF_SIZE, data_len, flow);//total_len+flow+msg
        if (ret < 0)
        {
            SUBMCD_DEBUG_P(LOG_ERROR, "CMCDProc::dispatch_ccd2 (%s:%d)dequeue msg failded!\n", __FILE__, __LINE__);
            return;
        }

        if (data_len <=0)
        {
            return;
        }

        TCCDHeader* ccd_header = (TCCDHeader*)_recv_buf;

        SConnInfo oConnInfo;

        oConnInfo.uiFlow = flow;
        oConnInfo.uiIp   = ccd_header->_ip;
        oConnInfo.usPort = ccd_header->_port;
        oConnInfo.uiSrcType = SOCKET_UDP_8000;
        oConnInfo.uiHelloTime = m_cur_time;

        //SUBMCD_DEBUG_P(LOG_TRACE, "dispatch_ccd ip: %s, port: %u, flow: %u, msglen: %u\n", 
        //    ip_to_str(oConnInfo.uiIp).c_str(),
        //    oConnInfo.usPort,
        //    oConnInfo.uiFlow,
        //    data_len - sizeof(TCCDHeader));

        process_cs_udp_msg(oConnInfo, _recv_buf+sizeof(TCCDHeader), data_len-sizeof(TCCDHeader));
    }while(ret == 0 && data_len > 0);
}

void CMCDProc::dispatch_ccd3()
{
    unsigned int data_len, flow;
    int ret;
    unsigned int uiMaxCount = 0;
    do
    {
        uiMaxCount++;
        if (uiMaxCount >= 10000)
        {
            return;
        }

        ret = _mq_ccd3_2_mcd->try_dequeue(_recv_buf, BUF_SIZE, data_len, flow);
        if (ret < 0)
        {
            SUBMCD_DEBUG_P(LOG_ERROR, "(%s:%d)dequeue msg failded!\n", __FILE__, __LINE__);
            return;
        }

        if (data_len <=0)
        {
            return;
        }

        TCCDHeader* ccd_header = (TCCDHeader*)_recv_buf;

        SConnInfo oConnInfo;

        oConnInfo.uiFlow = flow;
        oConnInfo.uiIp   = ccd_header->_ip;
        oConnInfo.usPort = ccd_header->_port;
        oConnInfo.uiSrcType = SOCKET_UDP_443;
        oConnInfo.uiHelloTime = m_cur_time;

        //SUBMCD_DEBUG_P(LOG_TRACE, "dispatch_ccd ip: %s, port: %u, flow: %u, msglen: %u\n", 
        //    ip_to_str(oConnInfo.uiIp).c_str(),
        //    oConnInfo.usPort,
        //    oConnInfo.uiFlow,
        //    data_len - sizeof(TCCDHeader));

        process_cs_udp_msg(oConnInfo, _recv_buf+sizeof(TCCDHeader), data_len-sizeof(TCCDHeader));
    }while(ret == 0 && data_len > 0);
}

void CMCDProc::dispatch_ccd4()
{
    unsigned int data_len, flow;
    int ret;
    unsigned int uiMaxCount = 0;
    do
    {
        uiMaxCount++;
        if (uiMaxCount >= 10000)
        {
            return;
        }

        ret = _mq_ccd4_2_mcd->try_dequeue(_recv_buf, BUF_SIZE, data_len, flow);
        if (ret < 0)
        {
            SUBMCD_DEBUG_P(LOG_ERROR, "(%s:%d)dequeue tcp msg failded!\n", __FILE__, __LINE__);
            return;
        }

        if (data_len <=0)
        {
            return;
        }

        if (data_len < CCD_EVENT_HEADER_LEN)
        {
            continue;
        }

        TCCDEventHeader* ccd_header = (TCCDEventHeader*) _recv_buf;

        if(ccd_header->_magic_num != CCD_EVENT_MAGIC_NUM)
        {
            SUBMCD_DEBUG_P(LOG_ERROR, "dequeue msg failed, no ccd event header!\n");
            continue;
        }

        if(ccd_header->_reqrsp_type == ccd_rsp_error)
        {
            SUBMCD_DEBUG_P(LOG_ERROR, "[%s][%d][dispatch_ccd] ccd rsp error\n", __FILE__, __LINE__);
        }

        if (ccd_rsp_connect == ccd_header->_reqrsp_type)
        {
            continue;
        }

        if (ccd_rsp_stat_conn_overload_count == ccd_header->_reqrsp_type)
        {
            continue;
        }

        if(ccd_rsp_disconnect == ccd_header->_reqrsp_type)
        {
            SConnInfo oConnInfo;

            oConnInfo.uiFlow        = flow;
            oConnInfo.uiIp          = ccd_header->_ip;
            oConnInfo.usPort        = ccd_header->_port;
            oConnInfo.uiSrcType     = SOCKET_TCP_80;
            oConnInfo.uiHelloTime   = m_cur_time;

            SUBMCD_DEBUG_P(LOG_TRACE, "CMCDProc::dispatch_ccd4 tcp ccd_rsp_disconnect,"
                                      " uiSrcType: %u, ip: %s, port: %u, flow: %u\n", 
                oConnInfo.uiSrcType,
                ip_to_str(oConnInfo.uiIp).c_str(),
                oConnInfo.usPort,
                oConnInfo.uiFlow);

            del_flow(oConnInfo);

            continue;
        }

        if (ccd_rsp_timeout_close == ccd_header->_reqrsp_type)
        {
            SConnInfo oConnInfo;

            oConnInfo.uiFlow        = flow;
            oConnInfo.uiIp          = ccd_header->_ip;
            oConnInfo.usPort        = ccd_header->_port;
            oConnInfo.uiSrcType     = SOCKET_TCP_80;
            oConnInfo.uiHelloTime   = m_cur_time;

            SUBMCD_DEBUG_P(LOG_TRACE, "CMCDProc::dispatch_ccd4 tcp ccd_rsp_timeout_close,"
                                      " uiSrcType: %u, ip: %s, port: %u, flow: %u\n", 
                oConnInfo.uiSrcType,
                ip_to_str(oConnInfo.uiIp).c_str(),
                oConnInfo.usPort,
                oConnInfo.uiFlow);

            del_flow(oConnInfo);

            continue;
        }

        if(	ccd_header->_reqrsp_type == ccd_rsp_send_nearly_ok 
            || ccd_header->_reqrsp_type == ccd_rsp_send_ok)
        {
            continue;
        }

        if(ccd_header->_reqrsp_type != ccd_rsp_data)
        {
            //SUBMCD_DEBUG_P(LOG_ERROR, "ccd event: %u, ignore.\n", ccd_header->_reqrsp_type);
            continue;
        }

        SConnInfo oConnInfo;

        oConnInfo.uiFlow = flow;
        oConnInfo.uiIp   = ccd_header->_ip;
        oConnInfo.usPort = ccd_header->_port;
        oConnInfo.uiSrcType = SOCKET_TCP_80;
        oConnInfo.uiHelloTime = m_cur_time;

        //SUBMCD_DEBUG_P(LOG_TRACE, "dispatch_ccd4 ip: %s, port: %u, flow: %u, data_len: %u, msg_len: %u\n", 
        //    ip_to_str(oConnInfo.uiIp).c_str(),
        //    oConnInfo.usPort,
        //    oConnInfo.uiFlow,
        //    data_len,
        //    data_len - sizeof(TCCDEventHeader));


        if (data_len <= sizeof(TCCDEventHeader))
        {
            continue;
        }

        process_ss_tcp_msg(oConnInfo, _recv_buf+sizeof(TCCDEventHeader), data_len-sizeof(TCCDEventHeader));
    }while(ret == 0 && data_len > 0);
}

void CMCDProc::dispatch_ccd5()
{
    unsigned int data_len, flow;
    int ret;
    unsigned int uiMaxCount = 0;
    do
    {
        uiMaxCount++;
        if (uiMaxCount >= 10000)
        {
            return;
        }

        ret = _mq_ccd5_2_mcd->try_dequeue(_recv_buf, BUF_SIZE, data_len, flow);
        if (ret < 0)
        {
            SUBMCD_DEBUG_P(LOG_ERROR, "(%s:%d)dequeue http msg failded!\n", __FILE__, __LINE__);
            return;
        }

        if (data_len <=0)
        {
            return;
        }

        if (data_len < CCD_EVENT_HEADER_LEN)
        {
            continue;
        }

        TCCDEventHeader* ccd_header = (TCCDEventHeader*) _recv_buf;

        if(ccd_header->_magic_num != CCD_EVENT_MAGIC_NUM)
        {
            SUBMCD_DEBUG_P(LOG_ERROR, "dequeue msg failed, no ccd event header!\n");
            continue;
        }

        if(ccd_header->_reqrsp_type == ccd_rsp_error)
        {
            SUBMCD_DEBUG_P(LOG_ERROR, "[%s][%d][dispatch_ccd] ccd rsp error\n", __FILE__, __LINE__);
        }

        if (ccd_rsp_connect == ccd_header->_reqrsp_type)
        {
            continue;
        }

        if (ccd_rsp_stat_conn_overload_count == ccd_header->_reqrsp_type)
        {
            continue;
        }

        if(ccd_rsp_disconnect == ccd_header->_reqrsp_type)
        {
            SConnInfo oConnInfo;

            oConnInfo.uiFlow        = flow;
            oConnInfo.uiIp          = ccd_header->_ip;
            oConnInfo.usPort        = ccd_header->_port;
            oConnInfo.uiSrcType     = SOCKET_TCP_8000;
            oConnInfo.uiHelloTime   = m_cur_time;

            SUBMCD_DEBUG_P(LOG_TRACE, "CMCDProc::dispatch_ccd5 tcp ccd_rsp_disconnect,"
                                      " uiSrcType: %u, ip: %s, port: %u, flow: %u\n", 
                oConnInfo.uiSrcType,
                ip_to_str(oConnInfo.uiIp).c_str(),
                oConnInfo.usPort,
                oConnInfo.uiFlow);

            del_flow(oConnInfo);

            continue;
        }

        if (ccd_rsp_timeout_close == ccd_header->_reqrsp_type)
        {
            SConnInfo oConnInfo;

            oConnInfo.uiFlow        = flow;
            oConnInfo.uiIp          = ccd_header->_ip;
            oConnInfo.usPort        = ccd_header->_port;
            oConnInfo.uiSrcType     = SOCKET_TCP_8000;
            oConnInfo.uiHelloTime   = m_cur_time;

            SUBMCD_DEBUG_P(LOG_TRACE, "CMCDProc::dispatch_ccd5 tcp ccd_rsp_timeout_close,"
                                      " uiSrcType: %u, ip: %s, port: %u, flow: %u\n", 
                oConnInfo.uiSrcType,
                ip_to_str(oConnInfo.uiIp).c_str(),
                oConnInfo.usPort,
                oConnInfo.uiFlow);

            del_flow(oConnInfo);

            continue;
        }

        if(	ccd_header->_reqrsp_type == ccd_rsp_send_nearly_ok 
            || ccd_header->_reqrsp_type == ccd_rsp_send_ok)
        {
            continue;
        }

        if(ccd_header->_reqrsp_type != ccd_rsp_data)
        {
            //SUBMCD_DEBUG_P(LOG_TRACE, "ccd event: %u, ignore.\n", ccd_header->_reqrsp_type);
            continue;
        }

        //_mcd_stat.Inc_Stat(ST_RECV);

        SConnInfo oConnInfo;

        oConnInfo.uiFlow = flow;
        oConnInfo.uiIp   = ccd_header->_ip;
        oConnInfo.usPort = ccd_header->_port;
        oConnInfo.uiSrcType = SOCKET_TCP_8000;
        oConnInfo.uiHelloTime = m_cur_time;

        //SUBMCD_DEBUG_P(LOG_TRACE, "dispatch_ccd5 ip: %s, port: %u, flow: %u, data_len: %u, msg_len: %u\n", 
        //    ip_to_str(oConnInfo.uiIp).c_str(),
        //    oConnInfo.usPort,
        //    oConnInfo.uiFlow,
        //    data_len,
        //    data_len - sizeof(TCCDEventHeader));

        if (data_len <= sizeof(TCCDEventHeader))
        {
            continue;
        }

        process_ss_tcp_msg(oConnInfo, _recv_buf+sizeof(TCCDEventHeader), data_len-sizeof(TCCDEventHeader));
    }while(ret == 0 && data_len > 0);
}

void CMCDProc::dispatch_ccd6()
{
    unsigned int data_len, flow;
    int ret;
    unsigned int uiMaxCount = 0;
    do
    {
        uiMaxCount++;
        if (uiMaxCount >= 10000)
        {
            return;
        }

        ret = _mq_ccd6_2_mcd->try_dequeue(_recv_buf, BUF_SIZE, data_len, flow);
        if (ret < 0)
        {
            SUBMCD_DEBUG_P(LOG_ERROR, "(%s:%d)dequeue http msg failded!\n", __FILE__, __LINE__);
            return;
        }

        if (data_len <=0)
        {
            return;
        }

        if (data_len < CCD_EVENT_HEADER_LEN)
        {
            continue;
        }

        TCCDEventHeader* ccd_header = (TCCDEventHeader*) _recv_buf;

        if(ccd_header->_magic_num != CCD_EVENT_MAGIC_NUM)
        {
            SUBMCD_DEBUG_P(LOG_FATAL, "dequeue msg failed, no ccd event header!\n");
            continue;
        }

        if(ccd_header->_reqrsp_type == ccd_rsp_error)
        {
            SUBMCD_DEBUG_P(LOG_DEBUG, "[%s][%d][dispatch_ccd] ccd rsp error\n", __FILE__, __LINE__);
        }

        if (ccd_rsp_connect == ccd_header->_reqrsp_type)
        {
            continue;
        }

        if (ccd_rsp_stat_conn_overload_count == ccd_header->_reqrsp_type)
        {
            continue;
        }

        if(ccd_rsp_disconnect == ccd_header->_reqrsp_type)
        {
            SConnInfo oConnInfo;

            oConnInfo.uiFlow        = flow;
            oConnInfo.uiIp          = ccd_header->_ip;
            oConnInfo.usPort        = ccd_header->_port;
            oConnInfo.uiSrcType     = SOCKET_TCP_443;
            oConnInfo.uiHelloTime   = m_cur_time;

            SUBMCD_DEBUG_P(LOG_TRACE, "CMCDProc::dispatch_ccd6 tcp ccd_rsp_disconnect,"
                                      " uiSrcType: %u, ip: %s, port: %u, flow: %u\n", 
                oConnInfo.uiSrcType,
                ip_to_str(oConnInfo.uiIp).c_str(),
                oConnInfo.usPort,
                oConnInfo.uiFlow);

            del_flow(oConnInfo);

            continue;
        }

        if (ccd_rsp_timeout_close == ccd_header->_reqrsp_type)
        {
            SConnInfo oConnInfo;

            oConnInfo.uiFlow        = flow;
            oConnInfo.uiIp          = ccd_header->_ip;
            oConnInfo.usPort        = ccd_header->_port;
            oConnInfo.uiSrcType     = SOCKET_TCP_443;
            oConnInfo.uiHelloTime   = m_cur_time;

            SUBMCD_DEBUG_P(LOG_TRACE, "CMCDProc::dispatch_ccd6 tcp ccd_rsp_timeout_close,"
                                      " uiSrcType: %u, ip: %s, port: %u, flow: %u\n", 
                oConnInfo.uiSrcType,
                ip_to_str(oConnInfo.uiIp).c_str(),
                oConnInfo.usPort,
                oConnInfo.uiFlow);

            del_flow(oConnInfo);

            continue;
        }

        if(	ccd_header->_reqrsp_type == ccd_rsp_send_nearly_ok 
            || ccd_header->_reqrsp_type == ccd_rsp_send_ok)
        {
            continue;
        }

        if(ccd_header->_reqrsp_type != ccd_rsp_data)
        {
            //SUBMCD_DEBUG_P(LOG_TRACE, "ccd event: %u, ignore.\n", ccd_header->_reqrsp_type);
            continue;
        }

        //_mcd_stat.Inc_Stat(ST_RECV);

        SConnInfo oConnInfo;

        oConnInfo.uiFlow = flow;
        oConnInfo.uiIp   = ccd_header->_ip;
        oConnInfo.usPort = ccd_header->_port;
        oConnInfo.uiSrcType = SOCKET_TCP_443;
        oConnInfo.uiHelloTime = m_cur_time;

        //SUBMCD_DEBUG_P(LOG_TRACE, "dispatch_ccd6 ip: %s, port: %u, flow: %u, data_len: %u, msg_len: %u\n", 
        //    ip_to_str(oConnInfo.uiIp).c_str(),
        //    oConnInfo.usPort,
        //    oConnInfo.uiFlow,
        //    data_len,
        //    data_len - sizeof(TCCDEventHeader));

        if (data_len <= sizeof(TCCDEventHeader))
        {
            continue;
        }

        process_ss_tcp_msg(oConnInfo, _recv_buf+sizeof(TCCDEventHeader), data_len-sizeof(TCCDEventHeader));
    }while(ret == 0 && data_len > 0);
}

//udp
void CMCDProc::dispatch_dcc()
{
    unsigned int data_len, flow;
    int ret;
    unsigned int uiMaxCount = 0;
    do
    {
        uiMaxCount++;
        if (uiMaxCount >= 10000)
        {
            break;
        }

        ret = _mq_dcc_2_mcd->try_dequeue(_recv_buf, BUF_SIZE, data_len, flow);
        if (ret < 0)
        {
            SUBMCD_DEBUG_P(LOG_ERROR, "(%s:%d)_mq_dcc_2_mcd dequeue failed!\n", __FILE__, __LINE__);
            break;
        }
        if (data_len <= 0)
        {
            break;
        }

        TDccHeader *pdcc_hd = (TDccHeader*)_recv_buf;       

        SConnInfo oConnInfo;

        oConnInfo.uiFlow = flow;
        oConnInfo.uiIp   = pdcc_hd->_ip;
        oConnInfo.usPort = pdcc_hd->_port;
        oConnInfo.uiSrcType = SRCTYPE_DCC2;
        oConnInfo.uiHelloTime = m_cur_time;

        SUBMCD_DEBUG_P(LOG_TRACE, "dispatch_dcc ip: %s, port: %u, flow: %u, data_len: %u, msg_len: %u\n", 
            ip_to_str(oConnInfo.uiIp).c_str(),
            oConnInfo.usPort,
            oConnInfo.uiFlow,
            data_len,
            data_len - sizeof(TDccHeader));

        process_ss_udp_msg(oConnInfo, _recv_buf+sizeof(TDccHeader), data_len-sizeof(TDccHeader));
    }while(ret == 0 && data_len > 0);   
}

void CMCDProc::dispatch_dcc2()
{
    unsigned int data_len, flow;
    int ret;
    unsigned int uiMaxCount = 0;
    do
    {
        uiMaxCount++;
        if (uiMaxCount >= 10000)
        {
            break;
        }

        ret = _mq_dcc2_2_mcd->try_dequeue(_recv_buf, BUF_SIZE, data_len, flow);
        if (ret < 0)
        {
            SUBMCD_DEBUG_P(LOG_ERROR, "(%s:%d)_mq_dcc2_2_mcd dequeue failed!\n", __FILE__, __LINE__);
            break;
        }

		if(data_len <= 0)
		{
			break;
		}

        if (data_len <= sizeof(TDccHeader))
        {
			continue;
        }

        TDccHeader *pdcc_hd = (TDccHeader*)_recv_buf;       

        SConnInfo oConnInfo;

        oConnInfo.uiFlow = flow;
        oConnInfo.uiIp   = pdcc_hd->_ip;
        oConnInfo.usPort = pdcc_hd->_port;
        oConnInfo.uiSrcType = SRCTYPE_DCC2;
        oConnInfo.uiHelloTime = m_cur_time;
         
        SUBMCD_DEBUG_P(LOG_TRACE, "dispatch_dcc2 ip: %s, port: %u, flow: %u, data_len: %u, msg_len: %u\n", 
                            ip_to_str(oConnInfo.uiIp).c_str(),
                            oConnInfo.usPort,
                            oConnInfo.uiFlow,
                            data_len,
                            data_len - sizeof(TDccHeader));

        process_ss_tcp_game_msg(oConnInfo, _recv_buf+sizeof(TDccHeader), data_len-sizeof(TDccHeader));
    }while(ret == 0 && data_len > 0);   
}

int CMCDProc::enqueue_2_ccd(const char* buf, unsigned int buf_size, unsigned int uiFlow)
{
    //MutexGuard g(m_ccd_mutex);
    int ret = _mq_mcd_2_ccd->enqueue(m_cur_time, buf, buf_size, uiFlow);

    if (ret < 0)
    {
        SUBMCD_DEBUG_P(LOG_ERROR, "(%s:%d)Enqueue to udp 80 port failed!\n", __FILE__, __LINE__);
        //_mcd_stat.Inc_Stat(ST_FAIL);
        //_mcd_stat.Inc_Errno_Stat(ERROR_ENQUEUE_CCD_FAIL);
        return -1;
    }
    //_mcd_stat.Inc_Stat(ST_REPLY);
    return 0;
}

int CMCDProc::enqueue_2_ccd2(const char* buf, unsigned int buf_size, unsigned int uiFlow)
{
    //MutexGuard g(m_ccd_mutex);
    int ret = _mq_mcd_2_ccd2->enqueue(m_cur_time, buf, buf_size, uiFlow);

    if (ret < 0)
    {
        SUBMCD_DEBUG_P(LOG_ERROR, "(%s:%d)Enqueue to udp 8000 port failed!\n", __FILE__, __LINE__);
        //_mcd_stat.Inc_Stat(ST_FAIL);
        //_mcd_stat.Inc_Errno_Stat(ERROR_ENQUEUE_CCD2_FAIL);
        return -1;
    }
    //_mcd_stat.Inc_Stat(ST_REPLY);
    return 0;
}

int CMCDProc::enqueue_2_ccd3(const char* buf, unsigned int buf_size, unsigned int uiFlow)
{
    //MutexGuard g(m_ccd_mutex);
    int ret = _mq_mcd_2_ccd3->enqueue(m_cur_time, buf, buf_size, uiFlow);

    if (ret < 0)
    {
        SUBMCD_DEBUG_P(LOG_ERROR, "(%s:%d)Enqueue to udp 443 port failed!\n", __FILE__, __LINE__);
        //_mcd_stat.Inc_Stat(ST_FAIL);
        //_mcd_stat.Inc_Errno_Stat(ERROR_ENQUEUE_CCD_FAIL);
        return -1;
    }
    //_mcd_stat.Inc_Stat(ST_REPLY);
    return 0;
}

int CMCDProc::enqueue_2_ccd4(const char* buf, unsigned int buf_size, unsigned int uiFlow)
{
    //MutexGuard g(m_ccd_mutex);
    int ret = _mq_mcd_2_ccd4->enqueue(m_cur_time, buf, buf_size, uiFlow);

    if (ret < 0)
    {
        SUBMCD_DEBUG_P(LOG_ERROR, "(%s:%d)Enqueue to CCD4 failed!\n", __FILE__, __LINE__);
        //_mcd_stat.Inc_Stat(ST_FAIL);
        //_mcd_stat.Inc_Errno_Stat(ERROR_ENQUEUE_CCD4_FAIL);
        return -1;
    }
    //_mcd_stat.Inc_Stat(ST_REPLY);
    return 0;
}

int CMCDProc::enqueue_2_ccd5(const char* buf, unsigned int buf_size, unsigned int uiFlow)
{
    //MutexGuard g(m_ccd_mutex);
    int ret = _mq_mcd_2_ccd5->enqueue(m_cur_time, buf, buf_size, uiFlow);

    if (ret < 0)
    {
        SUBMCD_DEBUG_P(LOG_ERROR, "(%s:%d)Enqueue to CCD5 failed!\n", __FILE__, __LINE__);
        //_mcd_stat.Inc_Stat(ST_FAIL);
        //_mcd_stat.Inc_Errno_Stat(ERROR_ENQUEUE_CCD5_FAIL);
        return -1;
    }
    //_mcd_stat.Inc_Stat(ST_REPLY);
    return 0;
}

int CMCDProc::enqueue_2_ccd6(const char* buf, unsigned int buf_size, unsigned int uiFlow)
{
    //MutexGuard g(m_ccd_mutex);
    int ret = _mq_mcd_2_ccd6->enqueue(m_cur_time, buf, buf_size, uiFlow);

    if (ret < 0)
    {
        SUBMCD_DEBUG_P(LOG_ERROR, "(%s:%d)Enqueue to CCD6 failed!\n", __FILE__, __LINE__);
        //_mcd_stat.Inc_Stat(ST_FAIL);
        //_mcd_stat.Inc_Errno_Stat(ERROR_ENQUEUE_CCD6_FAIL);
        return -1;
    }
    //_mcd_stat.Inc_Stat(ST_REPLY);
    return 0;
}

int CMCDProc::enqueue_2_dcc(const char* buf, unsigned int buf_size, unsigned int uiIp, unsigned short usPort)
{
    //encode
    TDccHeader stDccHeader;
    stDccHeader._ip = uiIp;
    stDccHeader._port = usPort;
    stDccHeader._reqrsp_type = dcc_req_send;

    memcpy(_send_buf,&stDccHeader,sizeof(TDccHeader));
    memcpy(_send_buf + sizeof(TDccHeader), buf, buf_size);

	 unsigned int uiSeq = (uiIp>>16) + (usPort<<16);

    // enqueue to dcc
    int ret = _mq_mcd_2_dcc->enqueue(m_cur_time, _send_buf, sizeof(TDccHeader) + buf_size, uiSeq);
    if (ret != 0)
    {
        //_mcd_stat.Inc_Stat(ST_FAIL);
        //_mcd_stat.Inc_Errno_Stat(ERROR_ENQUEUE_DCC_FAIL);
        SUBMCD_DEBUG_P(LOG_ERROR, "Enqueue to DCC failed! IP:%s, Port:%u, ReqMstLen: %u\n", 
                                  ip_to_str(uiIp).c_str(), usPort, buf_size);
        return -1;
    }

    return 0;
}

int CMCDProc::enqueue_2_dcc2(const char *buf, unsigned int buf_size, unsigned int uiIp, unsigned short usPort)
{
    //encode
    TDccHeader stDccHeader;
    stDccHeader._ip = uiIp;
    stDccHeader._port = usPort;
    stDccHeader._reqrsp_type = dcc_req_send;

    memcpy(_send_buf,&stDccHeader,sizeof(TDccHeader));
    memcpy(_send_buf + sizeof(TDccHeader), buf, buf_size);

    unsigned int uiSeq = ipport_2_flow(uiIp, usPort);

    // enqueue to dcc3
    int ret = _mq_mcd_2_dcc2->enqueue(m_cur_time, _send_buf, sizeof(TDccHeader) + buf_size, uiSeq);
    if (ret != 0)
    {
        //_mcd_stat.Inc_Stat(ST_FAIL);
        //_mcd_stat.Inc_Errno_Stat(ERROR_ENQUEUE_DCC2_FAIL);
        SUBMCD_DEBUG_P(LOG_ERROR, "Enqueue to dcc2 failed! IP:%s, Port:%u, ReqMstLen: %u\n", 
                                  ip_to_str(uiIp).c_str(), usPort, buf_size);
        return -1;
    }

  /*  struct in_addr addr;
    addr.s_addr = ip;
    const char* rs_ip_str = inet_ntoa(addr);*/

    //SUBMCD_DEBUG_P(LOG_DEBUG, "Enqueue to DCC3. IP:%s, Port:%u, ReqMstLen: %u\n", rs_ip_str, port, buf_size);

    return 0;
}

int CMCDProc::enqueue_2_dcc2_with_flow(const char *buf, unsigned int buf_size, unsigned int uiIp, unsigned short usPort, unsigned int uiFlow)
{
    //encode
    TDccHeader stDccHeader;
    stDccHeader._ip = uiIp;
    stDccHeader._port = usPort;
    stDccHeader._reqrsp_type = dcc_req_send;

    memcpy(_send_buf,&stDccHeader,sizeof(TDccHeader));
    memcpy(_send_buf + sizeof(TDccHeader), buf, buf_size);

    // enqueue to dcc3
    int ret = _mq_mcd_2_dcc2->enqueue(m_cur_time, _send_buf, sizeof(TDccHeader) + buf_size, uiFlow);
    if (ret != 0)
    {
        //_mcd_stat.Inc_Stat(ST_FAIL);
        //_mcd_stat.Inc_Errno_Stat(ERROR_ENQUEUE_DCC2_FAIL);
        SUBMCD_DEBUG_P(LOG_ERROR, "Enqueue to dcc2 failed! IP:%s, Port:%u, ReqMstLen: %u\n", 
                                  ip_to_str(uiIp).c_str(), usPort, buf_size);
        return -1;
    }

  /*  struct in_addr addr;
    addr.s_addr = ip;
    const char* rs_ip_str = inet_ntoa(addr);*/

    //SUBMCD_DEBUG_P(LOG_DEBUG, "Enqueue to DCC3. IP:%s, Port:%u, ReqMstLen: %u\n", rs_ip_str, port, buf_size);

    return 0;
}

int CMCDProc::enqueue_2_submcd_writer(const char *buf, unsigned int uiLen)
{
	//int ret = _mq_mcd_2_submcd_writer->enqueue(m_cur_time, buf, uiLen, 0);

	//if(ret != 0)
	//{
	//	SUBMCD_DEBUG_P(LOG_ERROR,"Enqueue to submcd_writer failed uiLen: %u", uiLen);
	//	return -1;
	//}

	return 0;
}

int CMCDProc::mcd_close_dcc_flow(unsigned int uiIp, unsigned short usPort)
{
    TDccHeader stDccHeader;
    stDccHeader._ip = uiIp;
    stDccHeader._port = usPort;
    stDccHeader._reqrsp_type = dcc_req_disconnect;
    memcpy(_send_buf, &stDccHeader,sizeof(TDccHeader));

    unsigned int uiSeq = ipport_2_flow(uiIp, usPort);
    int ret = _mq_mcd_2_dcc->enqueue(m_cur_time, _send_buf, sizeof(TDccHeader), uiSeq);

    if (ret != 0)
    {
        SUBMCD_DEBUG_P(LOG_ERROR, "Close DCC flow failed! flow:%u, IP:%s, Port:%u\n", uiSeq, ip_to_str(uiIp).c_str(), usPort);
        return -1;
    }

    SUBMCD_DEBUG_P(LOG_DEBUG, "CMCDProc::close_dcc_flow ip: %s, port: %u, flow: %u\n",ip_to_str(uiIp).c_str(), usPort, uiSeq);

    return 0;
}

int CMCDProc::mcd_close_dcc2_flow(unsigned int uiIp, unsigned short usPort)
{
    TDccHeader stDccHeader;
    stDccHeader._ip = uiIp;
    stDccHeader._port = usPort;
    stDccHeader._reqrsp_type = dcc_req_disconnect;
    memcpy(_send_buf, &stDccHeader,sizeof(TDccHeader));

    unsigned int uiSeq = ipport_2_flow(uiIp, usPort);
    int ret = _mq_mcd_2_dcc2->enqueue(m_cur_time, _send_buf, sizeof(TDccHeader), uiSeq);

    if (ret != 0)
    {
        SUBMCD_DEBUG_P(LOG_ERROR, "Close DCC2 flow failed! flow:%u, IP:%s, Port:%u\n", uiSeq, ip_to_str(uiIp).c_str(), usPort);
        return -1;
    }

    SUBMCD_DEBUG_P(LOG_DEBUG, "CMCDProc::close_dcc_flow ip: %s, port: %u, flow: %u\n",ip_to_str(uiIp).c_str(), usPort, uiSeq);

    return 0;
}

void CMCDProc::init(const string &conf_file)
{
    int ret = 0;
    ret = _tfc_obj.init();

    if (ret < 0)
    {
        SUBMCD_DEBUG_P(LOG_ERROR, "tfc object init fail\n");
        SUBMCD_DEBUG_P(LOG_ERROR, "svr shutdown!.\n");
        assert(false);
    }

    _cfg.LoadCfg(conf_file);

    init_log();
    init_ipc();

    ret = m_infoChannelMgr->init();

    if(ret < 0)
    {
		printf("ret=%d, errno=%d\n", ret, errno);
        SUBMCD_DEBUG_P(LOG_ERROR, "m_infoChannelMgr init fail, not enough shared memory\n");
        SUBMCD_DEBUG_P(LOG_ERROR, "svr shutdown!.\n");
        assert(false);
    }

    ret = m_pDataChannelMgr->init();
    if(ret < 0)
    {
	printf("ret=%d, errno=%d\n", ret, errno);
        SUBMCD_DEBUG_P(LOG_ERROR, "m_pDataChannelMgr init fail, not enough shared memory\n");
        SUBMCD_DEBUG_P(LOG_ERROR, "svr shutdown!.\n");
        assert(false);
    }

    //ret = hls_cache_mpegts_init(HLS_MPEGTS_KEY,HLS_HASHTIME,HLS_HASHLEN,1);
    ret = hls_cache_mpegts_init(HLS_MPEGTS_KEY,_cfg.m_uiSessionHashTime,_cfg.m_uiSessionHashLen,1);
    if ( ret < 0 ) {
	printf("ret=%d, errno=%d\n", ret, errno);
        assert(false);
    }
    //ret = hls_cache_session_init(HLS_SESSION_KEY,HLS_HASHTIME,HLS_HASHLEN,1);
    ret = hls_cache_session_init(HLS_SESSION_KEY,_cfg.m_uiSessionHashTime,_cfg.m_uiSessionHashLen,1);
    if ( ret < 0 ) {
	printf("ret=%d, errno=%d\n", ret, errno);
        assert(false);
    }

    signal(SIGUSR1, sigusr1_handle);
    signal(SIGUSR2, sigusr2_handle);
}

void CMCDProc::init_log()
{
    TLogPara *log_para = &(_cfg._log_para);
    int ret = DEBUG_OPEN(log_para->log_level_, log_para->log_type_,
        log_para->path_, log_para->name_prefix_,
        log_para->max_file_size_, log_para->max_file_no_);
    assert(ret >= 0);
}

void CMCDProc::init_ipc()
{
    _mq_ccd_2_mcd  = _mqs["mq_ccd_2_mcd"];
    _mq_mcd_2_ccd  = _mqs["mq_mcd_2_ccd"];

    _mq_ccd2_2_mcd = _mqs["mq_ccd2_2_mcd"];
    _mq_mcd_2_ccd2 = _mqs["mq_mcd_2_ccd2"];

    _mq_ccd3_2_mcd = _mqs["mq_ccd3_2_mcd"];
    _mq_mcd_2_ccd3 = _mqs["mq_mcd_2_ccd3"];

    _mq_ccd4_2_mcd = _mqs["mq_ccd4_2_mcd"];
    _mq_mcd_2_ccd4 = _mqs["mq_mcd_2_ccd4"];

    _mq_ccd5_2_mcd = _mqs["mq_ccd5_2_mcd"];
    _mq_mcd_2_ccd5 = _mqs["mq_mcd_2_ccd5"];

    _mq_ccd6_2_mcd = _mqs["mq_ccd6_2_mcd"];
    _mq_mcd_2_ccd6 = _mqs["mq_mcd_2_ccd6"];

    _mq_dcc_2_mcd	= _mqs["mq_dcc_2_mcd"];
    _mq_mcd_2_dcc	= _mqs["mq_mcd_2_dcc"];

    _mq_dcc2_2_mcd	= _mqs["mq_dcc2_2_mcd"];
    _mq_mcd_2_dcc2	= _mqs["mq_mcd_2_dcc2"];

    _mq_mcd_2_submcd_log = _mqs["mq_mcd_2_submcd_log"];

	//_mq_mcd_2_submcd_writer = _mqs["mq_mcd_2_submcd_writer"];

    SUBMCD_DEBUG_OPEN(_mq_mcd_2_submcd_log, &m_cur_time);

    assert(_mq_ccd_2_mcd != NULL);
    assert(_mq_mcd_2_ccd != NULL);

    assert(_mq_ccd2_2_mcd != NULL);
    assert(_mq_mcd_2_ccd2 != NULL);

    assert(_mq_ccd3_2_mcd != NULL);
    assert(_mq_mcd_2_ccd3 != NULL);

    assert(_mq_ccd4_2_mcd != NULL);
    assert(_mq_mcd_2_ccd4 != NULL);

    assert(_mq_ccd5_2_mcd != NULL);
    assert(_mq_mcd_2_ccd5 != NULL);

    assert(_mq_ccd6_2_mcd != NULL);
    assert(_mq_mcd_2_ccd6 != NULL);

    assert(_mq_dcc_2_mcd != NULL);
    assert(_mq_mcd_2_dcc != NULL);

    assert(_mq_dcc2_2_mcd != NULL);
    assert(_mq_mcd_2_dcc2 != NULL);

    assert(_mq_mcd_2_submcd_log != NULL);
	//assert(_mq_mcd_2_submcd_writer != NULL);
}

int CMCDProc::check_flags()
{
    if (check_flag.is_flag_set(FLG_CTRL_RELOAD))
    {
        _cfg.ReloadCfg();
        DEBUG_SET_LEVEL(_cfg._log_para.log_level_);
        check_flag.clear_flag();
    }

    if (check_flag.is_flag_set(FLG_CTRL_QUIT))
    {
        check_flag.clear_flag();
        return 1;
    }
    return 0;
}

extern "C"
{
    CacheProc* create_app()
    {
        return new CMCDProc();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMCDProc::process_cs_udp_msg(SConnInfo &oConnInfo, char *pPkg, unsigned int uiPkgLen)
{
	static unsigned int uiPortIndex = 0;
	if (_cfg.m_uiIsSendToRtmpSvr == 1) {
		uiPortIndex = (uiPortIndex + 1) % _cfg.m_usRtmpSvrPort.size();
		for (unsigned int i = 0; i < _cfg.m_uiRtmpSvrIp.size(); i++) {
			enqueue_2_dcc(pPkg, uiPkgLen, _cfg.m_uiRtmpSvrIp[i], _cfg.m_usRtmpSvrPort[uiPortIndex]);
		}
	}
    m_oProtolProcess->process_cs_udp_msg(oConnInfo, pPkg, uiPkgLen);
}

void CMCDProc::process_ss_tcp_msg(SConnInfo& oConnInfo, char *pPkg, unsigned int uiPkgLen)
{
	static unsigned int uiPortIndex = 0;
	if (_cfg.m_uiIsSendToRtmpSvr == 1) {
		uiPortIndex = (uiPortIndex + 1) % _cfg.m_usRtmpSvrPort.size();
		for (unsigned int i = 0; i < _cfg.m_uiRtmpSvrIp.size(); i++) {
			enqueue_2_dcc2(pPkg, uiPkgLen, _cfg.m_uiRtmpSvrIp[i], _cfg.m_usRtmpSvrPort[uiPortIndex]);
		}
	}
	m_oProtolProcess->process_http_msg(oConnInfo, pPkg, uiPkgLen);
	return;
}

void CMCDProc::process_ss_tcp_game_msg(SConnInfo& oConnInfo, char *pPkg, unsigned int uiPkgLen)
{
	m_oProtolProcess->process_game_msg(oConnInfo, pPkg, uiPkgLen);
	return;
}

void CMCDProc::process_ss_udp_msg(SConnInfo& oConnInfo, char *pPkg, unsigned int uiPkgLen)
{
	return;
}

void CMCDProc::del_flow(const SConnInfo& oConnInfo)
{
    m_oProtolProcess->del_flow(oConnInfo);
}


