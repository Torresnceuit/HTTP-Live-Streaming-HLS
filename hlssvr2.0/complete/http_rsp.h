
#ifndef _HTTP_SERVER_2_0_HTTP_RSP_H_
#define _HTTP_SERVER_2_0_HTTP_RSP_H_

#include <string>
#include "http_head.h"

//////////////////////////////////////////////////////////////////////////

namespace httpsvr
{
	enum en_http_response_error
	{
		C_HTTP_ERR_RSP_PARSE_TOO_MUCH_DATA		= -1000011,
		C_HTTP_ERR_INVALID_RESPONSE_LINE	    = -1000012,
	};
	
	class CHttpRspLine
	{
	public:
		CHttpRspLine(){
			_status_code = 200;
			_full = false;
		}
		~CHttpRspLine(){}

		bool full() const {return _full;}
		int infuse(const char* data, unsigned data_len, unsigned& skip_len);
		
		bool _full;
		std::string _line;
		unsigned _status_code;
		std::string	 _status_desc;
	};
	
	//身体信息
	class CHttpRspBody	//	attacher class, only mark the offset on another buffer
	{
	public:
		CHttpRspBody(){_content_length = 0xffffffff; _infused_length = 0;}
		~CHttpRspBody(){}

		bool full(){return _content_length <= _infused_length;}
		int set_len(unsigned len){_content_length = len; return 0;}

		int infuse(const char* data, unsigned data_len, unsigned& skip_len);

		unsigned _content_length;
		unsigned _infused_length;
	};
	
	//
	//	react operation: set http protocol version, set status code
	//	set header option
	//	infuse cgi return data --> header --> body
	//	set header option again
	//

	//
	//	the empty line between header and body, decided by body maker
	//	that is to say, if statis page, write "\r\n<html>data</html>"
	//	if cgi, write "SET-COOKIE: aaa=bbb\r\n\r\n<html>data</html>"
	//

	class CHttpRsp
	{
	public:
		CHttpRsp();
		~CHttpRsp();

		//注：不可重入
		int input(const void* data, unsigned data_len);
        bool inited() const{return  _inited;}
        int output(char* buf, unsigned buf_size, unsigned& data_len);
        
    public:
        //original data 原始请求数据
        unsigned _buf_size;
        char* _buf;
        unsigned _data_len;
        
        //	parse procedure variables
        bool _inited;
        unsigned _parsed_len;
        
    public:
        //	first part, headline
        CHttpRspLine _rsp_line;
        //	second part, header
        CHttpHeader _header;
        //	third part, body
        CHttpRspBody _body;
        
        int append_data(const void* data, unsigned data_len);
        unsigned _content_offset;
    };
}

//////////////////////////////////////////////////////////////////////////
#endif//_HTTP_SERVER_2_0_HTTP_RSP_H_
///:~
