
#include "http_func.h"
#include "http_req.h"
#include "tfc_base_str.h"

using namespace std;
using namespace tfc::base;
using namespace httpsvr;
using namespace httpsvr::func;

//////////////////////////////////////////////////////////////////////////

CHttpRequestLine::CHttpRequestLine()
{
	_full = false;
	_req_type = 0;
	_ver = 0;
}

CHttpRequestLine::~CHttpRequestLine(){}

int CHttpRequestLine::infuse(const char* data, unsigned data_len
							 , unsigned& skip_len)
{
	//	first, get line from data
	if (_full)
	{
		return 0;
	}
	unsigned line_len = 0, tail_len = 0;
	int ret = get_line(data, data_len, line_len, tail_len);
	if (ret)
	{
		return ret;
	}

	if (line_len == 0)
	{
		return 0;	//	no line-feed
	}
	
	//	second, get the method from line
	unsigned word_len = 0;
	ret = get_first_word(data, line_len, word_len);
	if (ret)
	{
		return ret;
	}
	
	if (word_len == 0)	//	no word, 
	{
		return C_HTTP_ERR_INVALID_REQUEST_LINE;
	}

	string method(data, word_len);
	int req_type = 0;
	if (method == "GET")
	{
		req_type = C_HTTP_REQ_TYPE_GET;
	}
	else if(method == "POST" || method == "QQPOST")
	{
		req_type = C_HTTP_REQ_TYPE_POST;
	}
	else
	{
		return C_HTTP_ERR_INVALID_REQUEST_LINE;
	}

	const char* p = data + word_len + 1;
	unsigned left_size = line_len-word_len;


	// ��ȡ HTTP/1.1
	char* http_p = strstr(p, " HTTP/");

	if(http_p == NULL || http_p - p > left_size)
	{
		if(strstr(p, "\n") != NULL)	//�Ѿ�������, û�ҵ� HTTP
		{
			return C_HTTP_ERR_INVALID_REQUEST_LINE;
		}
		else
		{
			return 0;
		}
	}

	word_len = http_p - p;

	/*
	//	third, get uri
	ret = get_first_word(p, left_size, word_len);
	if (ret)
	{
		return ret;
	}
	if (word_len == 0)
	{
		return C_HTTP_ERR_INVALID_REQUEST_LINE;
	}
	*/

	string uri(p, word_len), path, file, arg;

	ret = parse_uri(p, word_len, path, file, arg);
	if (ret)
	{
		return ret;
	}

	//fprintf(stderr, "p: %s f: %s a:%s\n", path.c_str(), file.c_str(), arg.c_str());
	p += word_len + 1;
	left_size -= word_len;

	//	fourth, get http version
	ret = get_first_word(p, left_size, word_len);
	if (ret)
	{
		return ret;
	}
	if (word_len == 0)
	{
		return C_HTTP_ERR_INVALID_REQUEST_LINE;
	}
	if (word_len < 8)
	{
		return C_HTTP_ERR_INVALID_REQUEST_LINE;
	}

	//	fifth, HTTP/1.0
	if (strncmp(p, "HTTP/", 5))	//	if not
	{
		return C_HTTP_ERR_INVALID_REQUEST_LINE;
	}

	if (p[6] != '.')
	{
		return C_HTTP_ERR_INVALID_REQUEST_LINE;
	}
	
	unsigned primary_version = from_str<unsigned>(string(p+5,1));
	unsigned secondary_version = from_str<unsigned>(string(p+7, word_len-7));
	int version = 0;
	if (primary_version == 0 && secondary_version == 9)
	{
		version = C_HTTP_PROTOCOL_VER_0_9;
	}
	else if (primary_version == 1 && secondary_version == 0)
	{
		version = C_HTTP_PROTOCOL_VER_1_0;
	}
	else if(primary_version == 1 && secondary_version == 1)
	{
		version = C_HTTP_PROTOCOL_VER_1_1;
	}
	else
	{
		return C_HTTP_ERR_INVALID_REQUEST_LINE;
	}


	//	sixth, set data
	_full = true;
	_line = string(data, line_len - tail_len);
	_req_type = req_type;
	_path = path;
	_file = file;
	_arg = arg;
	_ver = version;
	skip_len = line_len;
	
	return 0;
}

//////////////////////////////////////////////////////////////////////////

CHttpReq::CHttpReq(const void* data, unsigned data_len)
{
	_buf_size = 0;
	_buf = (const char *)data;
	_data_len = data_len;
    _buf_size = data_len;

	_inited = false;
	_parsed_len = 0;
	_content_offset = 0;
}

CHttpReq::~CHttpReq()
{
// 	if (_buf)
// 	{
// 		delete [] _buf;
// 	}
}

int CHttpReq::input(const void* data, unsigned data_len)
{
    //��ɳ�ʼ��
    _buf_size = 0;
    _buf = (const char *)data;
    _data_len = data_len;
    _buf_size = data_len;
    
    _inited = false;
    _parsed_len = 0;
	_content_offset = 0;


    int ret;

	//	first, if req-line full? infuse req-line
	if (!_req_line.full())
	{
		unsigned skip_len = 0;
		ret = _req_line.infuse(_buf + _parsed_len, _data_len - _parsed_len
			, skip_len);
		if (ret)	//	if fail
		{
			return ret;
		}
		//	skip
		_parsed_len += skip_len;
		if (!_req_line.full())	//	if not full
		{
			return 0;
		}
		//	if full, go ahead
	}

	//	second, if header full? infuse header
	if (!_header.full())
	{
		unsigned skip_len = 0;
		ret = _header.infuse(_buf + _parsed_len, _data_len - _parsed_len
			, skip_len);
		if (ret)	//	if fail
		{
			return ret;
		}

		//	skip
		_parsed_len += skip_len;
		if (!_header.full())	//	if not full
		{
			return 0;
		}
		//	if full, go ahead
		//	set content-length
		string s_c_l = _header.get("CONTENT-LENGTH");
		unsigned cont_len = s_c_l.empty() ? 0 : from_str<unsigned>(s_c_l);
		_body.set_len(cont_len);
		_content_offset = _parsed_len;
	}

	//	third, if body full? infuse body
	if(!_body.full())
	{
		unsigned skip_len = 0;
		ret = _body.infuse(_buf + _parsed_len, _data_len - _parsed_len
			, skip_len);
		if (ret)	//	if fail
		{
			return ret;
		}
		//	skip
		_parsed_len += skip_len;
		if (!_body.full())	//	if not full
		{
			return 0;
		}
	}

	_inited = true;
	return 0;
}

//////////////////////////////////////////////////////////////////////////

int CReqBody::infuse(const char* data, unsigned data_len, unsigned& skip_len)
{
	if (data_len + _infused_length <= _content_length)
	{
		_infused_length += data_len;
		skip_len = data_len;
	}
	else	//�յ������ݶ��ˣ�
	{
		//fprintf(stderr, "warning, content_length less than real data_len!\n");
		skip_len = _content_length - _infused_length;
		_infused_length = _content_length;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///:~
