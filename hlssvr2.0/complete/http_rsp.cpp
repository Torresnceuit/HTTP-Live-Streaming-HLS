
#include "tfc_base_str.h"
#include "http_func.h"
#include "http_rsp.h"

using namespace std;
using namespace httpsvr;
using namespace tfc::base;
using namespace httpsvr::func;

//////////////////////////////////////////////////////////////////////////
int CHttpRspLine::infuse(const char* data, unsigned data_len, unsigned& skip_len)
{
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

	if (line_len <= 10)	//"HTTP/1.1 200 OK"
	{
		return 0;	//	no line-feed
	}
	
	if( memcmp(data, "HTTP/", 5) != 0 || data[8] != ' ')
	{
		return C_HTTP_ERR_INVALID_RESPONSE_LINE;
	}

	skip_len = line_len;
	_line = string(data, line_len - tail_len);
	_status_code = atoi(data + 9);
	
	unsigned int i = 0;
	for(i = 9; i<line_len; i++)
	{
		if(data[i]==' ')
			break;
	}
	
	_status_desc = string(data + i + 1, line_len - i - 1);
	_full = true;
	return 0;
}

//////////////////////////////////////////////////////////////////////////

int CHttpRspBody::infuse(const char* data, unsigned data_len, unsigned& skip_len)
{
	if (data_len + _infused_length <= _content_length)
	{
		_infused_length += data_len;
		skip_len = data_len;
	}
	else	//收到的数据多了？
	{
		//fprintf(stderr, "warning, content_length less than real data_len!\n");
		skip_len = _content_length - _infused_length;
		_infused_length = _content_length;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

CHttpRsp::CHttpRsp()
{
	_buf_size = 0;
	_buf = NULL;
	_data_len = 0;

	_inited = false;
	_parsed_len = 0;
	_content_offset = 0;
}

CHttpRsp::~CHttpRsp()
{
	if (_buf)
	{
		delete [] _buf;
	}
}

int CHttpRsp::input(const void* data, unsigned data_len)
{
	//	zeroth, append data
	int ret = append_data(data, data_len);
	if (ret)
	{
	}

	//	first, if req-line full? infuse req-line
	if (!_rsp_line.full())
	{
		unsigned skip_len = 0;
		ret = _rsp_line.infuse(_buf + _parsed_len, _data_len - _parsed_len
			, skip_len);
		if (ret)	//	if fail
		{
			return ret;
		}
		//	skip
		_parsed_len += skip_len;
		if (!_rsp_line.full())	//	if not full
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

int CHttpRsp::append_data(const void* data, unsigned data_len)
{
	if (_buf_size == 0)
	{
		_buf = new char[data_len];
		memcpy(_buf, data, data_len);
		_buf_size = _data_len = data_len;
		return 0;
	}
	else if (_buf_size < _data_len + data_len)
	{
		char* buf = new char[_data_len + data_len];
		memcpy(buf, _buf, _data_len);
		memcpy(buf+_data_len, data, data_len);
		delete [] _buf;

		_buf = buf;
		_buf_size = _data_len + data_len;
		_data_len = _buf_size;
		return 0;
	}
	else
	{
		memcpy(_buf + data_len, data, data_len);
		_data_len += data_len;
		return 0;
	}
}

//////////////////////////////////////////////////////////////////////////
