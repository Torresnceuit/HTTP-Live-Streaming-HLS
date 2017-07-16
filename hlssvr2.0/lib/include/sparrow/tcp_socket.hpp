#ifndef TCP_SOCKET_HPP_201202201554
#define TCP_SOCKET_HPP_201202201554

#include <sys/types.h>
#include <sys/socket.h>
#include "inet_addr.hpp"
#include "mem_base.hpp"
#include "socket_t.hpp"

namespace sparrow
{
	template<typename T> class socket_service;

	class tcp_socket : 
		public socket,
		public mem_base<tcp_socket>
	{
	public:
		tcp_socket(void);
		~tcp_socket(void){
			if(handle_ > 0) ::close(handle_);
			handle_ = 0;
		};

		/*
		addr	���ӵ�Ŀ���ַ 
		timeout >0 ��ʱ��������<=0 ����
		return 0 ��ȷ��< 0���� 1 ��ʱ
		*/
		int connect(const inet_address &addr, const int timeout = -1);

		/*
		data ���͵�����
		len  ���ݳ���
		timeout >0 ��ʱ��������<0 ��������  ==0 ������
		
		return >0 ��ȷ ���͵����ݳ���
			   ==0 ���ͳ�ʱ
			   <0 ����
		*/
		int send(const void *data, const size_t len, const int timeout = -1);

		/*
		data �������ݻ�����
		len  ����������
		timeout >0 ��ʱ��������<0 ����  ==0 ������
		
		return >0 ��ȷ �յ������ݳ���
			   ==0 �����ݳ�ʱ
			   <0 ����
		*/
		int recv(void *buf, const size_t len, const int timeout = -1);

		void close(void){
			if(is_closed_) return;
			is_closed_ = true;
			shutdown(handle_, 2);
		};

		bool is_closed(void) const {return is_closed_;};

		const inet_address& get_local_addr(void) const {return local_addr_;};
		const inet_address& get_remote_addr(void) const {return remote_addr_;};

		void set_session(void *ptr){psession_ = ptr;};
		const void* get_session(void) const{return psession_;};
		void* get_session(void) {return psession_;};

	private:
		void connect_success(const inet_address &addr);
		
	private:
		bool		is_closed_;

		inet_address	local_addr_, remote_addr_;

		void		*psession_;

		template<typename T> friend class socket_service;
	};
}

#endif

