
#include "tfc_net_epoll_flow.h"

using namespace tfc::net;

//////////////////////////////////////////////////////////////////////////

int CEPollFlow::create(size_t iMaxFD)
{
	_maxFD = iMaxFD;
	_fd = epoll_create(1024);
	if(_fd < 0)
		return errno ? -errno : _fd;
	_events = new epoll_event[iMaxFD];
	return 0;
}

CEPollFlowResult CEPollFlow::wait(int iTimeout)
{
	errno = 0;
	int nfds = epoll_wait(_fd, _events, _maxFD, iTimeout);
	if (nfds < 0)
	{
		nfds = 0;
		if (errno != EINTR)
			return CEPollFlowResult(_events, nfds, errno);
	}
	return CEPollFlowResult(_events, nfds);;
}

int CEPollFlow::ctl(int fd, unsigned flow, int epollAction, int flag)
{
	assert(_fd != -1);
	epoll_event ev;
	ev.data.u64 = (unsigned long long)fd + (((unsigned long long) flow) << 32 );
	ev.events = flag;

	errno = 0;
	int ret = epoll_ctl(_fd, epollAction, fd, &ev);
	if (ret < 0)
		return errno ? -errno : ret;
	return 0;
}

unsigned CEPollFlowResult::iterator::flow()
{
	if (_res._events == NULL)
		return ULONG_MAX;
	
	unsigned long long u64 = 0; u64 = _res._events[_index].data.u64;
	unsigned flow = (u64 >> 32);
	return flow;
}

int CEPollFlowResult::iterator::fd()
{
	if (_res._events == NULL)
		return -1;
	
	unsigned long long u64 = 0; u64 = _res._events[_index].data.u64;
	int fd = (u64 & 0x00000000ffffffff);
	return fd;
}

//////////////////////////////////////////////////////////////////////////
///:~
