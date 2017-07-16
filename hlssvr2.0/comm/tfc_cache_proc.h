
#ifndef _TFC_CACHE_PROC_H_
#define _TFC_CACHE_PROC_H_

#include <map>
#include <string>

#include "tfc_net_ipc_mq.h"
#include "tfc_cache_access.h"
#include "tfc_diskcache_access.h"

//////////////////////////////////////////////////////////////////////////
using namespace tfc::net;
#define MAX_MQ_NUM		(1<<6)
namespace tfc{namespace cache
{
    //在mq被epoll激活时将调用该类型的回调函数来处理mq消息
    typedef void (*disp_func)(void*);
    typedef struct {
        CFifoSyncMQ* _mq;		//关联mq
        disp_func	_func;		//当关联的mq被epoll激活的时候调用的回调函数
        void* _priv;			//回调函数的自定义参数
        bool _active;			//是否被epoll激活
    }MQInfo;

	class CacheProc
	{
	public:
        CacheProc(): _epfd(-1), _infonum(0){}
        virtual ~CacheProc(){ close(_epfd);}
		virtual void run(const std::string& conf_file) = 0;
		std::map<std::string, tfc::net::CFifoSyncMQ*> _mqs;
		std::map<std::string, tfc::cache::CacheAccess*> _caches;
		std::map<std::string, tfc::diskcache::CacheAccess*> _disk_caches;

        //初始化mq的epoll设施	
        int init_epoll_4_mq();
        //在动态库的run循环中调用此函数可以运行mq的处理函数
        int run_epoll_4_mq();
        //func是处理mq事件的回调函数，一般是CacheProc子类的成员函数，priv一般是CacheProc子类的对象指针
        int add_mq_2_epoll(CFifoSyncMQ* mq, disp_func func, void* priv);
    protected:
        int _epfd;
        MQInfo	_mq_info[MAX_MQ_NUM];
        int _infonum;
	};
}}

//////////////////////////////////////////////////////////////////////////
#endif//_TFC_CACHE_PROC_H_
///:~

