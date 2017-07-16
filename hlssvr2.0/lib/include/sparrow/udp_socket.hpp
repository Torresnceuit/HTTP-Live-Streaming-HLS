#ifndef UDP_SOCKET_HPP_201203021531
#define UDP_SOCKET_HPP_201203021531

#include "inet_addr.hpp"
#include "socket_t.hpp"

namespace sparrow
{
	class udp_socket : public socket
	{
	public:
		udp_socket(void) :
		psession_(NULL)
		{
			type_ = ST_UDP;
		};

		~udp_socket(void){
			close();
		};
		
		/*创建udp socket
		@return 0 成功 <0失败
		*/
		int create(void);

		/**创建udp socket 并绑定到一个本地地址
		*@param localaddr 本地地址
		*@return 0 成功 其它失败
		*/
		int create(const inet_address &localaddr);

		/**将udp socket绑定到一个本地地址
		*@param localaddr 本地地址
		*@return 0 成功 其它失败
		*/
		int bind(const inet_address &localaddr);

		int set_sbuf_len(const size_t len);
		int get_sbuf_len(void);

		int set_rbuf_len(const size_t len);
		int get_rbuf_len(void);

		/*发送数据
		*@param buf 需要发送的数据
		*@param len 数据长度
		*@param dst 目标地址
		*@param timeout 超时时间(毫秒) =0 无阻塞 <0 阻塞 >0 超时
		*@return >=0 成功 返回发送的数据长度 < 0失败
		*/
		int send_to(const void *buf, const size_t len, const inet_address &dst, const int timeout = -1);

		/*接收数据
		*@param buf 接收数据缓冲区
		*@param len 缓冲区长度
		*@param dst 数据源地址
		*@param timeout 超时时间(毫秒) =0 无阻塞 <0 阻塞 >0 超时
		*@param return >=0 成功 返回发送的数据长度 < 0失败
		*/
		int recv_from(void *buf, const size_t len, inet_address &src, const int timeout = -1);

		void close(void){
			if(0 < handle_) ::close(handle_);
			handle_ = 0;
		};

		bool is_closed(void){return !(0 < handle_);};
		
		const inet_address& get_local_addr(void) const {return local_addr_;}

		void set_session(void *p){psession_ = p;};
		void* get_session(void) {return psession_;};
		const void* get_session(void) const {return psession_;};

	
	private:
		inet_address local_addr_;

		void	*psession_;
	};

}



#endif

