#ifndef SOCKET_T_HPP_201205231905
#define SOCKET_T_HPP_201205231905


namespace sparrow
{
	class socket
	{
	public:
		enum{
			ST_UNKOWN		= 0,
			ST_TCP_ACCEPTOR	= 1,
			ST_TCP_S		= 2,
			ST_TCP_C		= 3,
			ST_UDP			= 4
		};
	public:
		socket(void) : id_(0), handle_(0), type_(ST_UNKOWN)
		{};
		virtual ~socket(void)
		{
		};

		int id(void) const{return id_;};
		void id(const int i){id_ = i;};

		char type(void) const {return type_;};

		int get_handle(void) const {return handle_;};

		virtual void close(void) = 0;

	protected:
		int			id_;
		int			handle_;
		char		type_;
	};
}

#endif

