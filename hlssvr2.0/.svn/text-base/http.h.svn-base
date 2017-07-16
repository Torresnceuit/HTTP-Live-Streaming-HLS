#ifndef _MUXSVR_HTTP_H_
#define _MUXSVR_HTTP_H_

#include <string>
#include <map>

//////////////////////////////////////////////////////////////////////////
//
//	http include 3 parts:
//		headline
//		header
//		body
//
//////////////////////////////////////////////////////////////////////////

namespace muxsvr
{
    enum en_http_request_type
    {
        C_HTTP_REQ_TYPE_GET = 10001,
        C_HTTP_REQ_TYPE_POST = 10002,
    };

    enum en_http_request_object_type
    {
        C_HTTP_REQ_TYPE_CGI = 20001,
        C_HTTP_REQ_TYPE_STATIC_PAGE = 20002,
    };

    enum en_http_request_error
    {
        C_HTTP_REQ_ERR_PARSE_TOO_MUCH_DATA	= 1000001,
        C_HTTP_ERR_INVALID_REQUEST_LINE	= 1000002,
    };

    enum
    {
        C_HTTP_PROTOCOL_VER_0_9 = 40009,
        C_HTTP_PROTOCOL_VER_1_0 = 40010,
        C_HTTP_PROTOCOL_VER_1_1 = 40011,
    };

    enum
    {
        C_HTTP_HEADER_ERROR_INVALID_FORMAT	= 120001,
        C_HTTP_HEADER_ERROR_ASSENBLY_INCOMPLETE	= 120002,
        C_HTTP_HEADER_ERROR_BUFF_NOT_ENOUGH	= 120003,
    };

    class CHttpFunc
    {
    public:
        //	return non-zero for error
        //	return 0 on success, if line_len == 0, no line-feed('\n'), return
        int get_line(const char* data, unsigned data_len, unsigned& line_len, unsigned& tail_len);
    };

    class CHttpRequestLine
    {
    public:
        CHttpRequestLine();
        ~CHttpRequestLine();

        bool full() const {return _full;}
        int infuse(const char* data, unsigned data_len, unsigned& skip_len);
        int parse(const char *data, unsigned data_len, en_http_request_type &enMethod, std::string &path, std::string &file, std::string &arg);
        
        //获取url 里面的参数： path/file?arg
        int parse_uri(const char* data, unsigned data_len, std::string& path, std::string& file, std::string& arg);

        int req_type() const {return _req_type;}
        std::string path() const {return _path;}	//	/xx/yy/zz/qq.xxx
        std::string argument() const {return _arg;}		//	xx=oo&yy=qq&zz=pp
        int version() const {return _ver;}

        bool _full;
        std::string _line;
        int _req_type;
        std::string _path;
        std::string _file;
        std::string _arg;
        int _ver;

    private:
        int get_first_word(const char* data, unsigned data_len, unsigned& word_len);

        //把 url 里 %xx 转成 buf内容
        int decode_string(const char* data, unsigned data_len, char* buf, unsigned buf_size, unsigned& result_len);

    private:
        CHttpFunc oHttpFunc;
    };

    class CHttpHeader
    {
    public:
        CHttpHeader():_full(false){}
        ~CHttpHeader(){}

        bool full() const {return _full;}
        //解析一段data， 返回头部长度 skip_len
        int infuse(const char* data, unsigned length, unsigned& skip_len);

        //得到属性值
        std::string get(const std::string& name)
        {
            std::map<std::string, std::string>::const_iterator it
                = _head_attribute.find(name);
            if (it == _head_attribute.end())
            {
                return "";
            }
            else
            {
                return it->second;
            }
        }

        //输出所有属性和值
        int output(char* buf, unsigned buff_size, unsigned& data_len)
        {
            if (!_full)
                return -C_HTTP_HEADER_ERROR_ASSENBLY_INCOMPLETE;

            unsigned offset = 0;
            for(std::map<std::string, std::string>::const_iterator it
                = _head_attribute.begin()
                ; it != _head_attribute.end()
                ; it++)
            {
                if (offset + it->first.size() + it->second.size() + 4 > buff_size)
                    return -C_HTTP_HEADER_ERROR_BUFF_NOT_ENOUGH;
                int size = snprintf(buf+offset, buff_size - offset - 1, "%s: %s\r\n"
                    , it->first.c_str(), it->second.c_str());
                offset += size;
            }
            data_len = offset;
            return 0;
        }

        //	fourth, find the "name: value"
        //	for(;;) find the ':', set name = substr(o,pos)
        //	if ":" , set value = substr(pos+1, line_len - tail_len - pos)
        int get_name_value(const char* data, unsigned data_len, unsigned& name_len, unsigned& value_offset, unsigned& value_len);

        //	http head
        bool _full;
        std::map<std::string, std::string> _head_attribute;

    private:
        CHttpFunc oHttpFunc;
    };

    class CReqBody
    {
    public:
        CReqBody(){_content_length = 0xffffffff; _infused_length = 0;}
        ~CReqBody(){}

        bool full(){return _content_length <= _infused_length;}
        int set_len(unsigned len){_content_length = len; return 0;}

        int infuse(const char* data, unsigned data_len, unsigned& skip_len);

        unsigned _content_length;
        unsigned _infused_length;
    };

    class CHttpReq
    {
    public:
        CHttpReq()
        {
            _inited = false;
            _parsed_len = 0;
        }

        ~CHttpReq(){}

        int input(const void* data, unsigned data_len);
        bool inited() const{return  _inited;}
        unsigned parsed_len() const{return  _parsed_len;}

    private: 
        CHttpRequestLine _req_line; //	first part, headline
        CHttpHeader _header;        //	second part, header
        CReqBody _body;             //	third part, body

    private:
        bool _inited;
        unsigned _parsed_len;
    };

    enum en_http_response_error
    {
        C_HTTP_ERR_RSP_PARSE_TOO_MUCH_DATA = -1000011,
        C_HTTP_ERR_INVALID_RESPONSE_LINE = -1000012,
    };

    class CHttpRspLine
    {
    public:
        CHttpRspLine()
        {
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

    private:
        CHttpFunc oHttpFunc;
    };

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

#endif//_MUXSVR_HTTP_H_
