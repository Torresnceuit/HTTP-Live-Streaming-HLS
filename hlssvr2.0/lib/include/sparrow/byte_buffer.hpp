/**@file byte_buffer.hpp
*@brief 二进制数据缓冲区实现
*@author brandonli
*@date  2011.07.04
*/

#ifndef BYTE_BUFFER_HPP_201107032206
#define BYTE_BUFFER_HPP_201107032206

#include <deque>
#include <vector>
#include <string>
#include <stdint.h>
#include <sstream>
#include "mem_base.hpp"
#include "array.hpp"

namespace sparrow
{
	using std::string;
	using std::stringstream;
	typedef unsigned char	byte;

namespace buffers
{
	struct out_of_range{};
	struct need_more_data{};

	template<class T>
	class type2bytes;

	enum{
		INVALID_BUF_LEN	= 0x7fffffff, /**<无效的缓冲区长度*/
		INVALID_RPOS	= 0x7fffffff  /**<无效的读取位置*/
	};

	/**固定长度的二进制缓冲区编译时确定长度,类似于char[], 使用连续内存*/
	template<size_t N>
	class fixed_buffer
	{
		typedef fixed_buffer<N>	 this_type;
	private:
		fixed_buffer(const this_type &rhs){};
		this_type& operator=(const this_type &rhs){};

	public:
		byte	elems[N];

	public:
		fixed_buffer(void) : rpos_(0), size_(0){memset(elems, 0, sizeof(elems));};

		size_t capacity(void) const {return N;};

		const byte* read_ptr(void) const {return (elems + rpos_);};
		byte* write_ptr(void) {return (elems + size_);};
		size_t rpos(void) const {return rpos_;};
		void rpos(const size_t i) const {rpos_ = i;};
		size_t size(void) const {return size_;};
		void size(const size_t n) {size_ = n;};
		size_t free_size(void) const {return N-size_;};

		byte operator[](const size_t i) const {return elems[i];};
		byte& operator[](const size_t i) {return elems[i];};

		void push_back(const char *data, const size_t len){
			if(len > free_size()) throw out_of_range();
			memcpy(write_ptr(), data, len);
			rpos_ = 0;
			size_ += len;
		};
		void push_back(const byte *data, const size_t len){
			push_back((char*)data, len);
		};

		template<typename buffer>
		size_t push_back_buffer(const buffer &buf, const size_t start = 0, const size_t len = INVALID_BUF_LEN){
			if(start >= buf.size()) return 0;
			size_t wlen = INVALID_BUF_LEN == len ? buf.size() : len;
			if(wlen > buf.size() - start) wlen = buf.size() - start;
			if(wlen > free_size()) throw out_of_range();
			for(size_t i = 0; i < wlen; ++i, ++size_) *(elems + size_) = buf[i + start];
			rpos_ = 0;
			return wlen;
		};

		size_t get(char *dst, const size_t len, const size_t start = INVALID_RPOS) const{
			size_t rpos = rpos_;
			if(INVALID_RPOS != start){
				if(start >= size_) return 0;
				rpos_	= start;
			}
			size_t i = 0;
			for(; i < len && rpos_ < size_; ++ i, ++ rpos_) *(dst + i) = *(elems + rpos_);
			if(INVALID_RPOS != start) rpos_ = rpos;
			return i;
		};
		size_t get(byte *dst, const size_t len, const size_t start = INVALID_RPOS) const {
			return get((char*)dst, len, start);
		};

		template<typename T>
		void push_back(const T &d){
			type2bytes<T>()(*this, d);
		};

		template<typename T>
		T& get(T &ret, const size_t start = INVALID_RPOS) const{
			size_t rpos = rpos_;
			if(INVALID_RPOS != start) rpos_ = start;
			type2bytes<T>()(ret, *this);
			if(INVALID_RPOS != start) rpos_ = rpos;
			return ret;
		};

		void pop_front(const size_t n){
			size_t len = n < size_ ? n : size_;
			for(size_t i = len; i < size_; ++ i) *(elems + i -  len) = *(elems + i);
			size_ -= len;
			rpos_ = 0;
		};
		void clear(void){
			size_ = 0;
			rpos_ = 0;
		};

		string str(void) const {
			stringstream ss;
			ss << "rpos=" << rpos_ << " size=" << size_;
			char tmp[32];
			for(size_t i = 0; i < size_; ++ i){
				sprintf(tmp, "0x%02x ", *(elems + i));
				ss << tmp;
			}
			return ss.str();
		};

	private:
		mutable size_t		rpos_;
		size_t				size_;
	};

	//二进制缓冲区包装器
	class wrapper_buffer
	{
	private:
		wrapper_buffer(const wrapper_buffer &rhs){};
		wrapper_buffer& operator=(const wrapper_buffer &rhs){return *this;};
	public:
		wrapper_buffer(void) : 
		pbuf_(NULL), capacity_(0), rpos_(0), size_(0){};

		wrapper_buffer(const void *ptr, const size_t capacity, const size_t size) : 
		pbuf_((byte*)const_cast<void*>(ptr)), capacity_(capacity), rpos_(0), size_(size){};

		void reset(const void *ptr, const size_t capacity, const size_t size){
			pbuf_		= (byte*)const_cast<void*>(ptr);
			capacity_	= capacity;
			rpos_		= 0;
			size_		= size;
		};

		size_t capacity(void) const {return capacity_;};

		const byte* read_ptr(void) const {return (pbuf_ + rpos_);};
		byte* write_ptr(void) {return (pbuf_ + size_);};
		size_t rpos(void) const {return rpos_;};
		void rpos(const size_t i) const {rpos_ = i;};
		size_t size(void) const {return size_;};
		void size(const size_t n) {size_ = n;};
		size_t free_size(void) const {return capacity_ - size_;};

		byte operator[](const size_t i) const {return *(pbuf_ + i);};
		byte& operator[](const size_t i) {return *(pbuf_ + i);};

		void push_back(const char *data, const size_t len){
			if(len > free_size()) throw out_of_range();
			memcpy(write_ptr(), data, len);
			rpos_ = 0;
			size_ += len;
		};
		void push_back(const byte *data, const size_t len){
			push_back((char*)data, len);
		};

		template<typename buffer>
		size_t push_back_buffer(const buffer &buf, const size_t start = 0, const size_t len = INVALID_BUF_LEN){
			if(start >= buf.size()) return 0;
			size_t wlen = INVALID_BUF_LEN == len ? buf.size() : len;
			if(wlen > buf.size() - start) wlen = buf.size() - start;
			if(wlen > free_size()) throw out_of_range();
			for(size_t i = 0; i < wlen; ++i, ++size_) *(pbuf_ + size_) = buf[i + start];
			rpos_ = 0;
			return wlen;
		};

		size_t get(char *dst, const size_t len, const size_t start = INVALID_RPOS) const{
			size_t rpos = rpos_;
			if(INVALID_RPOS != start){
				if(start >= size_) return 0;
				rpos_	= start;
			}
			size_t i = 0;
			for(; i < len && rpos_ < size_; ++ i, ++ rpos_) *(dst + i) = *(pbuf_ + rpos_);
			if(INVALID_RPOS != start) rpos_ = rpos;
			return i;
		};
		size_t get(byte *dst, const size_t len, const size_t start = INVALID_RPOS) const {
			return get((char*)dst, len, start);
		};

		template<typename T>
		void push_back(const T &d){
			type2bytes<T>()(*this, d);
		};

		template<typename T>
		T& get(T &ret, const size_t start = INVALID_RPOS) const{
			size_t rpos = rpos_;
			if(INVALID_RPOS != start) rpos_ = start;
			type2bytes<T>()(ret, *this);
			if(INVALID_RPOS != start) rpos_ = rpos;
			return ret;
		};

		void pop_front(const size_t n){
			size_t len = n < size_ ? n : size_;
			for(size_t i = len; i < size_; ++ i) *(pbuf_ + i -  len) = *(pbuf_ + i);
			size_ -= len;
			rpos_ = 0;
		};
		void clear(void){
			size_ = 0;
			rpos_ = 0;
		};

		string str(void) const {
			stringstream ss;
			ss << "rpos=" << rpos_ << " size=" << size_;
			char tmp[32];
			for(size_t i = 0; i < size_; ++ i){
				sprintf(tmp, "0x%02x ", *(pbuf_ + i));
				ss << tmp;
			}
			return ss.str();
		};


	private:
		byte				*pbuf_;
		size_t				capacity_;
		mutable size_t		rpos_;
		size_t				size_;
	};

	template<typename buffer, typename T>
	buffer& operator<<(buffer &buf, const T &data){
		buf.push_back(data);
		return buf;
	}
	template<typename buffer, typename T, size_t N>
	buffer& operator<<(buffer &buf, const T (&data)[N]){
		for(size_t i = 0; i < N; ++ i) buf.push_back(*(data + i));
		return buf;
	}
	template<typename buffer, typename T, typename Allocator>
	buffer& operator<<(buffer &buf, const std::vector<T, Allocator> &data){
		uint32_t len = data.size();
		buf.push_back(len);
		for(uint32_t i = 0; i < len; ++ i) buf.push_back(data.at(i));
		return buf;
	}
	template<typename buffer, typename T, size_t N>
	buffer& operator<<(buffer &buf, const array<T, N> &data){
		uint32_t	len = data.size();
		buf.push_back(len);
		for(uint32_t i = 0; i < len; ++ i) buf.push_back(data[i]);
	}

	template<typename buffer, typename T>
	const buffer& operator>>(const buffer &buf, T &data){
		buf.get(data);
		return buf;
	}
	template<typename buffer, typename T, size_t N>
	const buffer& operator>>(const buffer &buf, T (&data)[N]){
		for(size_t i = 0; i < N; ++ i)  buf.get(*(data + i));
		return buf;
	}
	template<typename buffer, typename T, typename Allocator>
	const buffer& operator>>(const buffer &buf, std::vector<T, Allocator> &data){
		uint32_t len;
		buf.get(len);
		for(uint32_t i = 0; i < len && buf.rpos() < buf.size(); ++i){
			T d;
			buf.get(d);
			data.push_back(d);
		}
		return buf;
	}
	template<typename buffer, typename T, size_t N>
	buffer& operator>>(buffer &buf, array<T, N> &data){
		uint32_t	len = 0;
		buf.get(len);
		uint32_t  i = 0;
		for(; i < len && buf.rpos() < buf.size(); ++ i){
			T &d = data[i];
			buf.get(d);
		}
		data.size(i);
	}

	/**只支持整型数据*/
	template<class T>
	class type2bytes
	{
	public:
		template<typename buffer>
		void operator()(buffer &buf, const T &v) const{
			size_t size = sizeof(T);
			size_t i = 0;
			byte temp[sizeof(int64_t)];
			for(; i < size && i < sizeof(temp); ++ i){
				temp[i] = ((v >> ((size - i - 1) * 8)) & 0xff); 
			}
			buf.push_back(temp, i);
		};
		template<typename buffer>
		void operator()(T &ret, const buffer &buf) const{
			ret = 0;
			size_t size = sizeof(T);
			byte temp[sizeof(int64_t)];

			if(size > sizeof(temp)) size = sizeof(temp);
			if(buf.get(temp, size) != size) throw need_more_data();
			ret = 0;
			for(size_t i = 0; i < size; ++ i){
				ret |= ((T)temp[i] << ((size - i - 1) * 8)); 
			}
		};
	};

	template<>
	class type2bytes<float>
	{
	public:
		template<typename buffer>
		void operator()(buffer &buf, const float &f) const{
			size_t size = sizeof(float);
			byte *p = (byte*)&f;
			buf.push_back(p, size);
		};
		template<typename buffer>
		void operator()(float &ret, const buffer &buf) const{
			ret = 0.0;
			byte *p = (byte*)&ret;
			if(buf.get(p, sizeof(float)) != sizeof(float)) throw need_more_data();
		};
	};

	template<>
	class type2bytes<double>
	{
	public:
		template<typename buffer>
		void operator()(buffer &buf, const double &f) const{
			size_t size = sizeof(double);
			byte *p = (byte*)&f;
			buf.push_back(p, size);
		};
		template<typename buffer>
		void operator()(double &ret, const buffer &buf) const{
			ret = 0.0;
			byte *p = (byte*)&ret;
			if(buf.get(p, sizeof(double)) != sizeof(double)) throw need_more_data();
		};
	};

	template<>
	class type2bytes<string>
	{
	public:
		template<typename buffer>
		void operator()(buffer &buf, const string &str) const{
			uint32_t len = strlen(str.c_str());
			buf.push_back(len);
			buf.push_back(str.c_str(), len);
		};
		template<typename buffer>
		void operator()(string &ret, const buffer &buf) const{
			ret = "";
			uint32_t len;
			buf.get(len);
			for(size_t i = 0; i < buf.size() && i < len; ++ i) {
				char c = 0;
				buf.get(c);
				ret += c;
			};
		};
	};

}
}


#endif




