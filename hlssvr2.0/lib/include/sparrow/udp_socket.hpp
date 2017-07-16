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
		
		/*����udp socket
		@return 0 �ɹ� <0ʧ��
		*/
		int create(void);

		/**����udp socket ���󶨵�һ�����ص�ַ
		*@param localaddr ���ص�ַ
		*@return 0 �ɹ� ����ʧ��
		*/
		int create(const inet_address &localaddr);

		/**��udp socket�󶨵�һ�����ص�ַ
		*@param localaddr ���ص�ַ
		*@return 0 �ɹ� ����ʧ��
		*/
		int bind(const inet_address &localaddr);

		int set_sbuf_len(const size_t len);
		int get_sbuf_len(void);

		int set_rbuf_len(const size_t len);
		int get_rbuf_len(void);

		/*��������
		*@param buf ��Ҫ���͵�����
		*@param len ���ݳ���
		*@param dst Ŀ���ַ
		*@param timeout ��ʱʱ��(����) =0 ������ <0 ���� >0 ��ʱ
		*@return >=0 �ɹ� ���ط��͵����ݳ��� < 0ʧ��
		*/
		int send_to(const void *buf, const size_t len, const inet_address &dst, const int timeout = -1);

		/*��������
		*@param buf �������ݻ�����
		*@param len ����������
		*@param dst ����Դ��ַ
		*@param timeout ��ʱʱ��(����) =0 ������ <0 ���� >0 ��ʱ
		*@param return >=0 �ɹ� ���ط��͵����ݳ��� < 0ʧ��
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

