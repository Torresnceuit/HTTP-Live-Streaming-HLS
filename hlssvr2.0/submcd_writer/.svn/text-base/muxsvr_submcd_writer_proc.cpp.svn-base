#include <iostream>
#include <string>

#include "tfc_debug_log.h"
#include "tfc_net_epoll_flow.h"
#include "tfc_base_comm.h"
#include "tfc_str.hpp"

#include "CCheckFlag.h"
#include "gFunc.h"

#include "../muxsvr.h"
#include "muxsvr_submcd_writer_proc.h"

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

void disp_mcd(void* priv)
{
	CSubMcdWriterProc* app = (CSubMcdWriterProc*)priv;
	app->dispatch_mcd();
}

CSubMcdWriterProc::CSubMcdWriterProc()
    :m_LastTimeDoCleanTimeOutFileHandle(0)
	,m_recv_buf(NULL)
	,m_send_buf(NULL)
	,_mq_mcd_2_submcd_writer(NULL)
{
	m_recv_buf	= new char[BUF_SIZE];
	m_send_buf	= new char[BUF_SIZE];

	assert(m_recv_buf != NULL);
	assert(m_send_buf != NULL);

	m_ll_now_Tick = get_tick_count(m_curTVTime);
}

CSubMcdWriterProc::~CSubMcdWriterProc()
{
	if(m_recv_buf != NULL) 
	{
		delete[] m_recv_buf;
	}

	if(m_send_buf != NULL)
	{
		delete[] m_send_buf;
	}
}

void CSubMcdWriterProc::run(const std::string& conf_file)
{
	init(conf_file);    

	add_mq_2_epoll(_mq_mcd_2_submcd_writer, disp_mcd, this);

	DEBUG_P(LOG_NORMAL, "muxsvr_submcd_writer started!\n");

	m_ll_now_Tick = get_tick_count(m_curTVTime);
    m_cur_time    = m_curTVTime.tv_sec;

    m_LastTimeDoCleanTimeOutFileHandle = m_cur_time;

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
				DEBUG_P(LOG_NORMAL, "(%s:%d) muxsvr_submcd_writer recv signal, quit!\n", __FILE__, __LINE__);
				break;
			}
		}

		uiLoopTimeCount++;

		m_ll_now_Tick   = get_tick_count(m_curTVTime);
        m_cur_time      = m_curTVTime.tv_sec;

		if (uiLoopTimeCount >= 500)
		{
			uiLoopTimeCount = 0;
			dispatch_timeout();
		}

		//监控mq事件并运行相应的回调函数
		run_epoll_4_mq();
	}
}

void CSubMcdWriterProc::dispatch_timeout()
{
    if(abs(int(m_cur_time - m_LastTimeDoCleanTimeOutFileHandle)) > 60)
    {
        m_oFileHandleMgr.DoCleanTimeOutFileHandle();

        m_LastTimeDoCleanTimeOutFileHandle = m_cur_time; 
    }
}

void CSubMcdWriterProc::dispatch_mcd()
{
	unsigned int data_len, flow;
	int ret;
	unsigned int uiMaxCount = 0;
	do
	{
		uiMaxCount++;
		//mcd的日志相对比其他进程多，每次多取点提高性能
		if (uiMaxCount >= 1000)
		{
			break;
		}

		ret = _mq_mcd_2_submcd_writer->try_dequeue(m_recv_buf, BUF_SIZE, data_len, flow);//	每个消息块＝dwCmd + dwVideoId + dwLen + msg
		if (ret < 0)
		{
			DEBUG_P(LOG_ERROR, "CSubMcdWriterProc::dispatch_mcd (%s:%d)dequeue msg failded!\n", __FILE__, __LINE__);
			break;
		}

		if (data_len <= sizeof(int))
		{
			break;
		}

		if (data_len >= BUF_SIZE)
		{
			continue;
		}

		m_recv_buf[data_len] = 0;

		unsigned int uiCmd      = *((int *)m_recv_buf);
        unsigned int uiVideoId  = *((int*)m_recv_buf + sizeof(unsigned int)); 
        unsigned int uiLen      = *((int*)m_recv_buf + sizeof(unsigned int) + sizeof(unsigned int));

		if(data_len < 3 * sizeof(unsigned int) + uiLen)
		{
			DEBUG_P(LOG_ERROR, "CSubMcdWriterProc::dispatch_mcd datalen shorter than required format len: %u\n", data_len);

			return;
		}

        switch(uiCmd)
        {
        case SUBMCD_WRITE:
            {
                m_oFileHandleMgr.writeFile((unsigned char*)(m_recv_buf + 3*sizeof(unsigned int)), uiLen, uiVideoId);
            }
            break;
        case SUBMCD_CLOSE:
            {
                m_oFileHandleMgr.shutDownFile(uiVideoId);
            }
            break;
        default:
            break;
        }
		
	} while(ret == 0 && data_len > 0);
}

void CSubMcdWriterProc::init(const string &conf_file)
{
	m_oCfg.LoadCfg(conf_file);

	init_log();
	init_ipc();

	signal(SIGUSR1, sigusr1_handle);
	signal(SIGUSR2, sigusr2_handle);
}

void CSubMcdWriterProc::init_log()
{
	TLogPara *log_para = &(m_oCfg._log_para);

	int ret = DEBUG_OPEN(log_para->log_level_,
						log_para->log_type_,
						log_para->path_, 
						log_para->name_prefix_,
						log_para->max_file_size_, 
						log_para->max_file_no_);

	assert(ret >= 0);
}

void CSubMcdWriterProc::init_ipc()
{
	_mq_mcd_2_submcd_writer  = _mqs["mq_mcd_2_submcd_writer"];

	assert(_mq_mcd_2_submcd_writer != NULL);
}

int CSubMcdWriterProc::check_flags()
{	
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
		return new CSubMcdWriterProc();
	}
}
