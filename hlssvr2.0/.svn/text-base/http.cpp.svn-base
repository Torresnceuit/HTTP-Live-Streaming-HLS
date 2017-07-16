#include "http.h"
#include "tfc_base_str.h"

using namespace std;
using namespace tfc::base;
using namespace muxsvr;

int CHttpFunc::get_line(const char* data, unsigned data_len, unsigned &line_len, unsigned &tail_len)
{
    for(unsigned i = 0; i < data_len; i++)
    {
        if(data[i] == '\n')
        {
            if (i == 0)
            {
                line_len = tail_len = 1;
                return 0;
            }
            else if (data[i-1] == '\r')
            {
                line_len = i+1;
                tail_len = 2;
                return 0;
            }
            else
            {
                line_len = i+1;
                tail_len = 1;
                return 0;
            }
        }
    }
    line_len = data_len;
    tail_len = 0;
    return 0;
}

CHttpRequestLine::CHttpRequestLine()
{
    _full = false;
    _req_type = 0;
    _ver = 0;
}

CHttpRequestLine::~CHttpRequestLine(){}

int CHttpRequestLine::infuse(const char *data, unsigned data_len, unsigned& skip_len)
{
    //	first, get line from data
    if (_full)
    {
        return 0;
    }

    unsigned line_len = 0, tail_len = 0;
    int ret = oHttpFunc.get_line(data, data_len, line_len, tail_len);

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

    // 获取 HTTP/1.1
    char* http_p = strstr(p, " HTTP/");

    if(http_p == NULL || (unsigned)(http_p - p) > left_size)
    {
        if(strstr(p, "\n") != NULL)	//已经换行了, 没找到 HTTP
        {
            return C_HTTP_ERR_INVALID_REQUEST_LINE;
        }
        else
        {
            return 0;
        }
    }

    word_len = http_p - p;

    string uri(p, word_len);
    string path;
    string file;
    string arg;

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

int CHttpRequestLine::parse(const char *data, unsigned data_len, en_http_request_type &enMethod, string &path, string &file, string &arg)
{
    unsigned line_len = 0;
    unsigned tail_len = 0;
    oHttpFunc.get_line(data, data_len, line_len, tail_len);

    if (line_len == 0)
    {
        return C_HTTP_ERR_INVALID_REQUEST_LINE;
    }

    unsigned word_len = 0;

    //	second, get the method from line
    get_first_word(data, line_len, word_len);

    if (word_len == 0)	//	no word
    {
        return C_HTTP_ERR_INVALID_REQUEST_LINE;
    }

    string method(data, word_len);

    if (method == "GET")
    {
        enMethod = C_HTTP_REQ_TYPE_GET;
    }
    else if(method == "POST" || method == "QQPOST")
    {
        enMethod = C_HTTP_REQ_TYPE_POST;
    }
    else
    {
        return C_HTTP_ERR_INVALID_REQUEST_LINE;
    }

    const char *p = data + word_len + 1;
    unsigned left_size = line_len-word_len;

    // 获取 HTTP/1.1
    char* http_p = strstr(p, " HTTP/");

    if(http_p == NULL || (unsigned)(http_p - p) > left_size)
    {
        return C_HTTP_ERR_INVALID_REQUEST_LINE;
    }

    word_len = http_p - p;

    int ret = parse_uri(p, word_len, path, file, arg);

    if (ret)
    {
        return C_HTTP_ERR_INVALID_REQUEST_LINE;
    }

    return 0;
}

int CHttpRequestLine::get_first_word(const char* data, unsigned data_len, unsigned& word_len)
{
    for(unsigned i = 0; i < data_len; i++)
    {
        if(isspace(data[i]))
        {
            word_len = i;
            return 0;
        }
    }
    word_len = 0;
    return 0;
}

int CHttpRequestLine::parse_uri(const char* data, unsigned data_len, std::string& path, std::string& file, std::string& arg)
{
    //	first, find the first "?", left is path
    unsigned uri_end = ULONG_MAX;
    for(unsigned i = 0; i < data_len; i++)
    {
        if (data[i] == '?')
        {
            uri_end = i;
            break;
        }
    }
    if (uri_end == ULONG_MAX)
    {
        uri_end = data_len;
    }

    //	third, find the last '/' in the left
    unsigned file_start = ULONG_MAX;
    for(unsigned i = uri_end; i != 0; i--)
    {
        if (data[i-1] == '/')
        {
            file_start = i-1;
            break;
        }
    }
    if (file_start == ULONG_MAX)
    {
        return -1;
    }
    else
    {
        path = string(data, file_start);
        file = string(data + file_start + 1, uri_end - file_start - 1);
    }

    //	fifth, convert the "%6D" into char arg, in the right
    if (uri_end == data_len)
    {
        arg = "";
    }
    else
    {
        static const unsigned C_MAX_ARG_SIZE = 1<<10;
        char sArg[C_MAX_ARG_SIZE];
        unsigned arg_len = 0;
        int ret = decode_string(data + uri_end + 1, data_len - uri_end - 1, sArg, sizeof(sArg), arg_len);
        if (ret)
        {
            return ret;
        }
        sArg[arg_len] = 0;
        arg = sArg;
    }
    return 0;
}

//把 url 里 %xx 转成 buf内容
int CHttpRequestLine::decode_string(const char* data, unsigned data_len, char* buf, unsigned buf_size, unsigned& result_len)
{
    unsigned j = 0;
    for(unsigned i = 0; i < data_len && j < buf_size; i++, j++)
    {
        if (data[i] == '%')
        {
            if (i+2 >= data_len)
            {
                return -1;	//	cant be decoded
            }
            char sPercentage[3] = {0};
            sPercentage[0] = data[i+1];
            sPercentage[1] = data[i+2];
            buf[j] = (char)strtol(sPercentage, (char **)0, 16);
            i += 2;
        }
        else
        {
            buf[j] = data[j];
        }
    }

    result_len = j;
    return 0;
}

int CHttpHeader::infuse(const char* data, unsigned length, unsigned& skip_len)
{
    const char* p = data;
    skip_len = 0;

    static const unsigned C_MAX_HEAD_LINE = 1<<13;	//	max head line 8192
    for(unsigned i = 0; i < C_MAX_HEAD_LINE; i++)
    {
        //	first, get line from data
        unsigned line_len = 0, tail_len = 0;
        int ret = oHttpFunc.get_line(p, length, line_len, tail_len);
        if (ret)
        {
            return ret;
        }

        //	second, if no more line-feed, incomplete line data
        if (line_len == 0)
        {
            return 0;
        }

        skip_len += line_len;

        //	third, if empty line, mean the head finish
        if (line_len == tail_len)
        {
            _full = true;
            return 0;
        }

        //	offset, len, must be, reliable, otherwise ....
        unsigned name_len = 0, value_offset = 0, value_len = 0;
        ret = get_name_value(p, line_len-tail_len, name_len, value_offset, value_len);
        if (ret)
        {
            return 0;			//不完整的
            //return ret;		//可能有问题
        }
        //	make the upper name
        char* sName = new char[name_len+1]; memset(sName,0,name_len+1);
        for(unsigned j = 0; j < name_len; j++)
        {
            sName[j] = toupper(p[j]);	//全转换成大写
        }
        //sName[name_len] = 0;

        _head_attribute[sName] = string(p + value_offset, value_len);
        delete [] sName;
        p += line_len;
        //tencent-felixzhu-20080814,update buffer len to avoid buffer overflow,begin
        length -= line_len;
        //tencent-felixzhu-20080814,end
    }
    return 0;
}

int CHttpHeader::get_name_value(const char* data, unsigned data_len, unsigned& name_len, unsigned& value_offset, unsigned& value_len)
{
    //	find the separator
    unsigned pos = 0;
    const char* tail = data + data_len;
    for(const char* ch = data; ch != tail; ch++)
    {
        if(*ch == ':')
        {
            pos = ch-data;
            break;
        }
    }

    //	find result
    if (pos == 0)	//	no ':', invalid head
        return C_HTTP_HEADER_ERROR_INVALID_FORMAT;

    //	name
    //static const unsigned C_MAX_NAME_LENGTH = 1<<12;
    //char sName[C_MAX_NAME_LENGTH];
    //for(unsigned i = 0; i < pos; i++)
    //{
    //	sName[i] = toupper(data[i]);
    //}
    //sName[pos] = 0;
    name_len = pos;

    //	value
    for(value_offset = pos+1; !isspace(*(data + value_offset)) && value_offset < data_len; value_offset++)
    {
    }
    value_len = data_len - value_offset;
    return 0;
}

int CReqBody::infuse(const char* data, unsigned data_len, unsigned& skip_len)
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

int CHttpReq::input(const void* data, unsigned data_len)
{
    //完成初始化
    _inited = false;
    _parsed_len = 0;

    int ret = 0;
    unsigned skip_len = 0;

    //	first, if req-line full? infuse req-line
    ret = _req_line.infuse((const char *)data + _parsed_len, data_len - _parsed_len, skip_len);

    if (ret)	//	if fail
    {
        return ret;
    }

    //	skip
    _parsed_len += skip_len;

    //	if not full, return; if full, go ahead
    if (!_req_line.full())	
    {
        return 0;
    }


    //	second, if header full? infuse header
    ret = _header.infuse((const char *)data + _parsed_len, data_len - _parsed_len, skip_len);

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

    //	third, if body full? infuse body
    ret = _body.infuse((const char *)data + _parsed_len, data_len - _parsed_len, skip_len);

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

    _inited = true;

    return 0;
}

int CHttpRspLine::infuse(const char* data, unsigned data_len, unsigned& skip_len)
{
    if (_full)
    {
        return 0;
    }

    unsigned line_len = 0, tail_len = 0;
    int ret = oHttpFunc.get_line(data, data_len, line_len, tail_len);
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
