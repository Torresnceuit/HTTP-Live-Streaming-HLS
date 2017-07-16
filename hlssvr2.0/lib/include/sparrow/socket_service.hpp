#ifndef SOCKET_SERVICE_HPP_201205231843
#define SOCKET_SERVICE_HPP_201205231843

#include "epoll_reactor.hpp"
#include "tcp_socket.hpp"
#include "udp_socket.hpp"
#include "hashtable.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <memory>

using std::auto_ptr;

namespace sparrow
{	
	/*class handler
	{
	public:
		void connected(tcp_socket &sock);
		void input(tcp_socket &sock);
		void exception(tcp_socket &sock);
		void closed(tcp_socket &sock);

		void input(udp_socket &sock);
		void exception(udp_socket &sock);
	};
	*/
	////////////////////////////////////////////////////////
	template<typename handler>
	class socket_service
	{
		typedef socket_service<handler> this_type;
	public:
		socket_service(handler &hdl) : 
		phdl_(&hdl),
		rt_hdl_(this)
		{
			rt_.set_event_handler(rt_hdl_);
		};

		int create_tcp_server(const inet_address &listenaddr, const int serid = 0){
			auto_ptr<tcp_socket> acceptor(new tcp_socket());
			 

			acceptor->type_ = socket::ST_TCP_ACCEPTOR;
			acceptor->id(serid);

			int fd = ::socket(AF_INET, SOCK_STREAM, 0);
			if(0 > fd) return -1;
			acceptor->handle_ = fd;

			int opt = 1;
			setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

			sockaddr_in	inaddr;
			bzero(&inaddr, sizeof(inaddr));
			inaddr.sin_family	= AF_INET;
			inaddr.sin_port		= htons(listenaddr.get_port());
			inaddr.sin_addr.s_addr = listenaddr.get_netn_ip();

			if(bind(fd, (sockaddr*)&inaddr, sizeof(inaddr)) < 0) {
				return -10;
			}
			acceptor->local_addr_ = listenaddr;

			if(listen(fd, 65535) < 0) return -20;
			
			if(fcntl(fd, F_SETFL, O_NONBLOCK) < 0) return -30;

			if(rt_.add_sock(*acceptor) < 0) return -40;

			acceptor->is_closed_ = false;

			acceptor.release();
			
			return 0;
		};

		bool add_socket(tcp_socket *psock){
			if(NULL == psock) return false;

			socket *ps = (socket*)psock;
			sockets_[ps] = ps;

			if(!rt_.add_sock(*psock)){
				psock->close();
				return false;
			}
			return true;
		};
		bool add_socket(udp_socket *psock){
			if(NULL == psock) return false;

			socket *ps = (socket*)psock;
			sockets_[ps] = ps;

			if(!rt_.add_sock(*psock)){
				psock->close();
				return false;
			}
			return true;
		}
		/*
		@param timeout >0 超时毫秒数 =0 立即返回 <0 等待直到有数据
		*/
		int run_once(const int timeout = 1){
			return rt_.run_once(timeout);
		};

		void close(void){
			for(_ht_sockets::iterator iter = sockets_.begin(); iter != sockets_.end(); ++ iter)
			{
				socket *ps = iter->second;
				ps->close();
				delete ps;
			}
			sockets_.clear();
		};

	private:
		void input(socket &sock){
			if(sock.type() == socket::ST_UDP){
				udp_socket &udpsock = (udp_socket &)sock;
				input(udpsock);
			}else{
				tcp_socket &tcpsock = (tcp_socket &)sock;
				input(tcpsock);
			}
		};
		void exception(socket &sock){
			if(sock.type() == socket::ST_UDP){
				udp_socket &udpsock = (udp_socket &)sock;
				exception(udpsock);
			}else{
				tcp_socket &tcpsock = (tcp_socket &)sock;
				exception(tcpsock);
			}
		}

		void input(tcp_socket &sock){
			//log_debug << "input sock type=" << sock->type() << log_end;
			if(sock.type() == socket::ST_TCP_ACCEPTOR){
				for(;;){
					sockaddr_in	inaddr;
					int addrlen = sizeof(inaddr);
					bzero(&inaddr, sizeof(inaddr));
					int fd = accept(sock.get_handle(), (sockaddr*)&inaddr, (socklen_t*)&addrlen);
					if(0 > fd) {
						//log_debug << "accept error: " << errno << log_end;
						break;
					}

					tcp_socket *newsock = new tcp_socket();
					newsock->handle_ = fd;
					newsock->type_ = socket::ST_TCP_S;
					newsock->id(sock.id());
					newsock->is_closed_		= false;
					newsock->local_addr_	= sock.local_addr_;
					newsock->remote_addr_	= inet_address(inaddr.sin_addr.s_addr, inaddr.sin_port);

					fcntl(fd, F_SETFL, O_NONBLOCK);
					if(!rt_.add_sock(*newsock)){
						newsock->close();
						continue;
					}
					phdl_->connected(*newsock);
					/*log_debug << "end phdl_ connected" 
						<< " socket type=" << sock->type()
						<< " newsocket type=" << newsock->type()
						<< " tcp_socket::ST_ACCEPTOR = " << tcp_socket::ST_ACCEPTOR
						<< " tcp_socket::ST_S_SOCK = " << tcp_socket::ST_S_SOCK
						<< log_end;*/
				}
			}else if(sock.type() == socket::ST_TCP_S 
				|| sock.type() == socket::ST_TCP_C){
				phdl_->input(sock);
				if(sock.is_closed()) {
					phdl_->closed(sock);
					rt_.del_sock(sock);
					sockets_.erase(&sock);
					delete &sock;
				}
			}
		};

		void exception(tcp_socket &sock){
			phdl_->exception(sock);
		};

		void input(udp_socket &sock){
			phdl_->input(sock);
			if(sock.is_closed()) {
				rt_.del_sock(sock);
				sockets_.erase(&sock);
				delete &sock;
			}
		};

		void exception(udp_socket &sock){
			phdl_->exception(sock);
		};

	private:
		class reactor_hdl
		{
		public:
			reactor_hdl(this_type *p) : pser_(p){}
			void input(socket &sock){pser_->input(sock);};
			void exception(socket &sock){pser_->exception(sock);};

		private:
			this_type	*pser_;
		};

		typedef epoll_reactor<reactor_hdl>	_this_reactor;

		typedef container::hashtable<socket*, socket*>	 _ht_sockets;

	private:
		handler			*phdl_;

		reactor_hdl		rt_hdl_;
		_this_reactor	rt_;

		_ht_sockets		sockets_;
	};

}






#endif

