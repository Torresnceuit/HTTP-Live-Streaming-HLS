
#ifndef _HTTP_SVR_HTTP_HEAD_H_
#define _HTTP_SVR_HTTP_HEAD_H_

//////////////////////////////////////////////////////////////////////////
//
//	http include 3 parts:
//		headline
//		header
//		body
//
//////////////////////////////////////////////////////////////////////////

#include <map>
#include <string>

//////////////////////////////////////////////////////////////////////////

namespace httpsvr
{
	enum
	{
			C_HTTP_PROTOCOL_VER_0_9				= 40009,
			C_HTTP_PROTOCOL_VER_1_0			= 40010,
			C_HTTP_PROTOCOL_VER_1_1			= 40011,
	};
	
	enum
	{
			C_HTTP_HEADER_ERROR_INVALID_FORMAT	= 120001,
			C_HTTP_HEADER_ERROR_ASSENBLY_INCOMPLETE	= 120002,
			C_HTTP_HEADER_ERROR_BUFF_NOT_ENOUGH	= 120003,
	};

	class CHttpHeader
	{
	public:
		CHttpHeader():_full(false){}
		~CHttpHeader(){}

		bool full() const {return _full;}
		//����һ��data�� ����ͷ������ skip_len
		int infuse(const char* data, unsigned length, unsigned& skip_len);
		
		//�õ�����ֵ
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

		//����������Ժ�ֵ
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

		//	http head
		bool _full;
		std::map<std::string, std::string> _head_attribute;
	};
}

//////////////////////////////////////////////////////////////////////////
#endif//_HTTP_SVR_HTTP_HEAD_H_
///:~
