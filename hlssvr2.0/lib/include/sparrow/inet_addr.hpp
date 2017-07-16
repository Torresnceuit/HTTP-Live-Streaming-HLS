#ifndef INET_ADDR_HPP_201202211528
#define INET_ADDR_HPP_201202211528

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>

using std::string;

namespace sparrow
{
	class inet_address
	{
	public:
		inet_address(void) : ip_(htonl(INADDR_ANY)), port_(0){};
		inet_address(const string &ip, const uint16_t port) : port_(port){
			ip_	= ::inet_addr(ip.c_str());
		};
		
		inet_address(const unsigned long ip, const uint16_t port) :
		ip_(ip), port_(port){};

		inet_address(const uint16_t &port) : port_(port){
			ip_ = htonl(INADDR_ANY);
		};

		void set_ip(const string &ip){
			ip_ = ::inet_addr(ip.c_str());
		};
		string get_ip(void) const {
			in_addr addr;
			addr.s_addr = ip_;
			return string(::inet_ntoa(addr));
		};
		void set_netn_ip(const unsigned long ip){
			ip_ = ip;
		};
		unsigned long get_netn_ip(void) const {return ip_;};

		void set_port(const uint16_t port){port_ = port;};
		uint16_t get_port(void) const {return port_;};

		inet_address& operator=(const inet_address &rhs){
			if(this == &rhs) return *this;
			ip_		= rhs.ip_;
			port_	= rhs.port_;
			return *this;
		};

	private:
		unsigned long	ip_;
		uint16_t		port_;
	};
}


#endif

