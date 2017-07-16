#ifndef STR_ALGORITHM_HPP_201208061051
#define STR_ALGORITHM_HPP_201208061051

#include <string>
#include <sstream>
#include <ctype.h>
#include <string.h>

namespace sparrow
{
namespace str_algorithm
{
	using std::string;

	class is_any_of
	{
	public:
		is_any_of(const string &s) : 
		str_(s)
		{};

		bool operator()(const char &c) const 
		{
			if(str_.find(c) == string::npos) return false;
			return true;
		};

	private:
		string str_;
	};

	template<typename _dst_container>
	inline void split(_dst_container &dst, const string &src, const is_any_of &op)
	{
		string item;
		size_t len = strlen(src.c_str());
		for(size_t i = 0; i < len; ++ i)
		{
			char c = src.c_str()[i];
			if(op(c))
			{
				if(item.length() > 0) dst.push_back(item);
				item = "";
			}else{
				item += c;
			}
		}
		if(item.length() > 0) dst.push_back(item);
	};

	struct bad_lexical_cast{};

	template<typename Target, typename Source>
	inline Target lexical_cast(const Source &src)
	{
		std::stringstream ss;
		ss << src;
		if(ss.fail()) throw bad_lexical_cast();

		Target ret;
		ss >> ret;
		if(ss.fail()) throw bad_lexical_cast();
		return ret;
	};

	inline void to_upper(string &src)
	{
		size_t len = strlen(src.c_str());
		for(size_t i = 0; i < len; ++ i)
		{
			src[i] = toupper(src.c_str()[i]);
		}
	};
	inline string to_upper_copy(const string &src)
	{
		string ret;
		size_t len = strlen(src.c_str());
		for(size_t i = 0; i < len; ++ i)
		{
			char c = toupper(src.c_str()[i]);
			ret += c;
		};
		return ret;
	};

	inline void to_lower(string &src)
	{
		size_t len = strlen(src.c_str());
		for(size_t i = 0; i < len; ++ i)
		{
			src[i] = tolower(src.c_str()[i]);
		}
	};
	inline string to_lower_copy(const string &src)
	{
		string ret;
		size_t len = strlen(src.c_str());
		for(size_t i = 0; i < len; ++ i)
		{
			char c = tolower(src.c_str()[i]);
			ret += c;
		}
		return ret;
	};

}
}


#endif

