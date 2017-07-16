
#ifndef _TFC_NET_EPOLL_FLOW_H_
#define _TFC_NET_EPOLL_FLOW_H_

#include <string.h>
#include <errno.h>
#include <sys/epoll.h>
#include <assert.h>
#include <string>
#include <stdexcept>
#include <iostream>

#ifndef ULONG_MAX
#define ULONG_MAX 0xffffffffUL 
#endif

//////////////////////////////////////////////////////////////////////////

namespace tfc
{
	namespace net
	{
		class CEPollFlowResult;
		
		class CEPollFlow
		{
		public:
			CEPollFlow() : _fd(-1), _events(NULL){}
			~CEPollFlow(){if (_events) delete[] _events;}
			
			int create(size_t iMaxFD);
			
			int ctl(int fd, unsigned flow, int epollAction, int flag);
			
			int add(int fd, unsigned flow, int flag)
			{return ctl(fd, flow, EPOLL_CTL_ADD, flag);}
			
			int modify(int fd, unsigned flow, int flag)
			{return ctl(fd, flow, EPOLL_CTL_MOD, flag);}
			
			CEPollFlowResult wait(int iTimeout);
			
			int _fd;
			epoll_event* _events;
			size_t _maxFD;
		};
		
		//////////////////////////////////////////////////////////////////////////
		
		class CEPollFlowResult
		{
		public:
			class iterator;
			friend class CEPollFlow;
			friend class CEPollFlowResult::iterator;
			
			~CEPollFlowResult(){}

			CEPollFlowResult(const CEPollFlowResult& right)
				:_events(right._events)
				, _size(right._size)
				, _errno(right._errno)
			{}

			CEPollFlowResult& operator=(const CEPollFlowResult& right)
			{
				_events = right._events;
				_size = right._size;
				_errno = right._errno;
				return *this;
			}

			iterator begin(){return CEPollFlowResult::iterator(0, *this);}

			iterator end(){return CEPollFlowResult::iterator(_size, *this);}

			int error(){return _errno;}
			
		private:
			CEPollFlowResult(epoll_event* events, size_t size, int error = 0)
				:_events(events), _size(size), _errno(error){}

			bool operator==(const CEPollFlowResult& right);
			
			epoll_event* _events;
			size_t _size;
			int _errno;
			
		public:
			class iterator
			{
				friend class CEPollFlowResult;
				
			public:
				iterator(const iterator& right)
					:_index(right._index), _res(right._res){}

				iterator& operator ++(){_index++; return *this;}

				iterator& operator ++(int){_index++; return *this;}
				
				bool operator ==(const iterator& right)
				{return (_index == right._index && &_res == &right._res);}

				bool operator !=(const iterator& right)
				{return !(_index == right._index && &_res == &right._res);}

				epoll_event* operator->(){return &_res._events[_index];}

				unsigned flow();
				int fd();
				
			private:
				iterator(size_t index, CEPollFlowResult& res)
					: _index(index), _res(res){}
				size_t _index;
				CEPollFlowResult& _res;
			};
		};
	}
}

//////////////////////////////////////////////////////////////////////////
#endif//_TFC_NET_EPOLL_FLOW_H_
///:~

