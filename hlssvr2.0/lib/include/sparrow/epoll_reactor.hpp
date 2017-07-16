#ifndef EPOLL_REACTOR_HPP_201202201709
#define EPOLL_REACTOR_HPP_201202201709

#include <sys/epoll.h>
#include <errno.h>
#include "logging.hpp"
#include "socket_t.hpp"

namespace sparrow
{
	/*
	class event_handler
	{
	public:
		void input(shared_ptr<sock_t> &sock);
		void exception(shared_ptr<sock_t> &sock);
	};
	
	*/

	template<typename event_handler>
	class epoll_reactor
	{
		enum{
			DEF_SIZE	= 1024,
			DEF_EVN_COUNT = 1024
		};
	public:
		epoll_reactor(const unsigned int evn_count = DEF_EVN_COUNT) : 
		pevnh_(NULL), pevns_(NULL), evn_count_(evn_count)
		{
			epfd_	= epoll_create(DEF_SIZE);
			pevns_  = new epoll_event[evn_count_];
		};
		~epoll_reactor(void){
			close(epfd_);
			delete [] pevns_;
		}

		void set_event_handler(event_handler &evnh){pevnh_ = &evnh;};

		bool add_sock(socket &sock){
			epoll_event ee;
			ee.data.ptr		= &sock;
			ee.events		= EPOLLIN|EPOLLET;

			int ret = epoll_ctl(epfd_, EPOLL_CTL_ADD, sock.get_handle(), &ee);
			if(0 != ret) {
				log_debug << "epoll_ctl error: " << errno << log_end;
				return false;
			}
			
			return true;
		};
		bool del_sock(const socket &sock){
			epoll_event ee;
			ee.events	= EPOLLIN|EPOLLET;

			int ret = epoll_ctl(epfd_, EPOLL_CTL_DEL, sock.get_handle(), &ee);
			if(0 != ret) {
				return false;
			}

			return true;
		};

		int run_once(const int timeout = 1){
			int e_evns = epoll_wait(epfd_, pevns_, (int)evn_count_, timeout);
			if(0 >= e_evns) return e_evns;
			for(int i = 0; i < e_evns; ++ i){
				epoll_event &evn = *(pevns_ + i);
				socket &sock = *(socket*)evn.data.ptr;
				if(evn.events & EPOLLIN){
					pevnh_->input(sock);
				}else if(evn.events & EPOLLERR){
					pevnh_->exception(sock);
				}
			}
			return e_evns;
		};

	private:
		int				epfd_;
		event_handler	*pevnh_;
		epoll_event		*pevns_;
		unsigned int	evn_count_;		
	};
}

#endif
