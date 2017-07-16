#include <sys/epoll.h>
#include "tfc_cache_proc.h"

using namespace tfc::cache;
using namespace tfc::net;

int CacheProc::init_epoll_4_mq() {
	_epfd = epoll_create(MAX_MQ_NUM);
	if(_epfd < 0)
		return -1;
	else
		return 0;
}
int CacheProc::add_mq_2_epoll(CFifoSyncMQ* mq, disp_func func, void* priv) {
	if(_infonum < MAX_MQ_NUM) {	
		struct epoll_event ev;
		ev.events = EPOLLIN | EPOLLERR;
		ev.data.u32 = _infonum;
		epoll_ctl(_epfd, EPOLL_CTL_ADD, mq->fd(), &ev);

		_mq_info[_infonum]._mq = mq;
		_mq_info[_infonum]._func = func;
		_mq_info[_infonum]._priv = priv;
		_mq_info[_infonum]._active = false;
		_infonum++;
		return 0;
	}
	else 
		return -1;
}
int CacheProc::run_epoll_4_mq() {
	static struct epoll_event epv[MAX_MQ_NUM];
	int eventnum = epoll_wait(_epfd, epv, MAX_MQ_NUM, 1);
	MQInfo* info;
	int i;
	for(i = 0; i < eventnum; ++i) {
		info = &_mq_info[epv[i].data.u32];
		info->_mq->clear_flag();
		info->_func(info->_priv);
		info->_active = true;	
	}
	//由于mq的事件通知通过write fd来触发，并不是100%可靠，所以这里
	//还需要遍历一次未激活的mq，再尝试取出数据
	for(i = 0; i < _infonum; ++i) {
		info = &_mq_info[i];
		if(!info->_active) {
			info->_func(info->_priv);
		}
		else
			info->_active = false;
	}

	return 0;
}
