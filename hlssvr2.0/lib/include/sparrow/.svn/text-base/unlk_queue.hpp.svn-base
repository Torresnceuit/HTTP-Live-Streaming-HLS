#ifndef UNLK_QUEUE_HPP_201202231254
#define UNLK_QUEUE_HPP_201202231254

/*无锁对列
*/
#include "offset_ptr.hpp"
#include <string.h>
#include <memory>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <stdint.h>

namespace sparrow
{
	//////////////////////////////////////////////////////////////////////////
	template<typename T, typename A = std::allocator<T> >
	class unlk_queue
	{
	public:
		typedef	T   value_type;
		typedef A	allocator_type;

	private:
		typedef typename allocator_type::pointer value_pointer;

	public:
		/*
		@param cap 队列容量, 
	    @param a   内存分配器
		*/
		unlk_queue(const uint32_t cap) : 
		capacity_(cap),
		wpos_(0), rpos_(0)
		{
			init();
		};

		unlk_queue(const uint32_t cap, const allocator_type &a) : 
		alloc_(a), 
		capacity_(cap),
		wpos_(0), rpos_(0)
		{
			init();
		};
		
		~unlk_queue(void){
			destroy();
		};

		bool is_invalid(void){
			if(elems_ == (void*)NULL) return true;
			return false;
		}

		size_t size(void) const {
			return (wpos_ >= rpos_ ? wpos_ - rpos_ : (wpos_ + capacity_ * 2) - rpos_);
		};
		size_t free_size(void) const {
			return capacity_ - size();
		};

		bool push(const T &val){
			check();
			if(free_size() == 0) return false;
			uint32_t idx = wpos_ % capacity_;
			*(elems_ + idx) = val;
			++wpos_;
			if(wpos_ >= capacity_ * 2) wpos_ = 0;
			return true;
		};

		bool pop(T &ret){
			check();
			if(size() == 0) return false;
			uint32_t idx = rpos_ % capacity_;
			ret = *(elems_ + idx);
			(*(elems_ + idx)).~value_type();
			new(elems_ + idx) value_type();
			++ rpos_;
			if(rpos_ >= capacity_ * 2) rpos_ = 0;
			return true;
		};

		void clear(void){
			T tmp;
			while(pop(tmp)){}
		};

		void reset(void){
			destroy();
			init();
		};

	private:
		void init(void){
			elems_ = NULL;
			elems_ = alloc_.allocate(capacity_);
			for(uint32_t i = 0; i < capacity_; ++ i){
				alloc_.construct(elems_ + i, value_type());
			}
			wpos_	= 0;
			rpos_	= 0;
		};

		void destroy(void){
			clear();
			if(elems_ != (void*)NULL){
				for(uint32_t i = 0; i < capacity_; ++ i){
					alloc_.destroy(elems_ + i);
				}
				alloc_.deallocate(elems_, capacity_);
			}
			elems_	= value_pointer();
			wpos_	= 0;
			rpos_	= 0;
		};

		void check(void){
			if(size() <= capacity_) return;
			wpos_ = 0;
			rpos_ = 0;
		};
		
	private:
		allocator_type		alloc_;

		const uint32_t		capacity_;
		uint32_t			wpos_, rpos_;
		value_pointer		elems_;
	};
	///////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	///只支持一读一写
	template<typename A> 
	class unlk_queue<char*, A>
	{
		enum{
			L_SIZE	= sizeof(size_t)
		};
	public:
		typedef A	allocator_type;

	public:
		enum{
			FORMAT_ERROR	= -1, //消息格式错误
			NEED_LARGER_RBUF	= -2  //需要更大的接受缓冲区
		};

	private:
		typedef typename allocator_type::pointer elem_pointer;
	public:
		/*
		@param cap 队列容量
	    @param a   内存分配器
		*/
		unlk_queue(const uint32_t cap) : 
		capacity_(cap),
		wpos_(0), rpos_(0)
		{
			init();
		};

		unlk_queue(const uint32_t cap, const allocator_type &a) : 
		alloc_(a), 
		capacity_(cap),
		wpos_(0), rpos_(0)
		{
			init();
		};

		~unlk_queue(void){
			destroy();
		};

		bool is_invalid(void)
		{
			if(elems_ == (void*)NULL) return true;
			return false;
		}

		size_t size(void) const {
			return (wpos_ >= rpos_ ? wpos_ - rpos_ : (wpos_ + capacity_ * 2) - rpos_);
		};
		size_t free_size() const 
		{
			return (capacity_ - size());
		};

		size_t push(const char *buf, const size_t len){
			check();
			uint32_t freesize = free_size();
			if(freesize < len + L_SIZE) return 0;

			const char *plen	= (const char*)(&len);

			uint32_t idx	 = wpos_ % capacity_;
			size_t   _l		 = std::min(capacity_ - idx, (uint32_t)L_SIZE);
			memcpy(elems_ + idx, plen, _l);
			if(L_SIZE - _l > 0) memcpy(elems_ + 0, plen + _l, L_SIZE - _l);

			uint32_t wpos	= wpos_ + L_SIZE;
			idx	= wpos % capacity_;
			_l  = std::min(capacity_ - idx,  len);
			memcpy(elems_ + idx, buf, _l);
			if(len - _l > 0) memcpy(elems_ + 0, buf + _l, len - _l);

			size_t wlen = len + L_SIZE;
			wpos_ += wlen;
			if(wpos_ >= capacity_ * 2) {
				wpos_ -= (capacity_ * 2);
			}
			return wlen - L_SIZE;
		};
		
		/************************************************************************/
		/*return >= 0 这确， 取出的消息长度  < 0 消息格式错误                                                                    */
		/************************************************************************/
		ssize_t pop(char *dst, const size_t len){
			check();
			uint32_t elem_size = size();
			if(0 == elem_size) return 0;
			
			if(elem_size < L_SIZE) {
				clear();
				return FORMAT_ERROR;
			}

			size_t mlen = 0;
			char *pmlen = (char*)&mlen;
			uint32_t idx	= rpos_ % capacity_;
			uint32_t _l		= std::min(capacity_ - idx, (uint32_t)L_SIZE);
			memcpy(pmlen, elems_ + idx, _l);
			if(L_SIZE - _l > 0) memcpy(pmlen + _l, elems_ + 0, L_SIZE - _l);

			if(mlen > elem_size - L_SIZE){
				clear();
				return FORMAT_ERROR;
			}
			if(len < mlen) {
				return NEED_LARGER_RBUF;
			}

			uint32_t rpos = rpos_ + L_SIZE;
			idx		= rpos % capacity_;
			_l		= std::min(capacity_ - idx, mlen);
			memcpy(dst, elems_ + idx, _l);
			if(mlen - _l > 0) memcpy(dst + _l, elems_ + 0, mlen - _l);

			rpos_ += (mlen + L_SIZE);

			if(rpos_ >= capacity_ * 2) {
				rpos_ -= (capacity_ * 2);
			}

			return (ssize_t)mlen;
		};
		
		void pop(void)
		{
			check();
			uint32_t elem_size = size();
			if(0 == elem_size) return;

			if(elem_size < L_SIZE) {
				clear();
				return;
			}

			size_t mlen = 0;
			char *pmlen = (char*)&mlen;
			uint32_t idx	= rpos_ % capacity_;
			uint32_t _l		= std::min(capacity_ - idx, (uint32_t)L_SIZE);
			memcpy(pmlen, elems_ + idx, _l);
			if(L_SIZE - _l > 0) memcpy(pmlen + _l, elems_ + 0, L_SIZE - _l);

			if(mlen > elem_size - L_SIZE){
				clear();
				return;
			}

			rpos_ += (mlen + L_SIZE);

			if(rpos_ >= capacity_ * 2){
				rpos_ -= (capacity_ * 2);
			}
		}

		void clear(void){
			wpos_	= 0;
			rpos_	= 0;
		};

		void reset(void){
			destroy();
			init();
		};

	private:
		void init(void){
			elems_ = alloc_.allocate(capacity_);
			wpos_	= 0;
			rpos_	= 0;
		};
		void destroy(void){
			clear();
			if(elems_ != (void*)NULL){
				alloc_.deallocate(elems_, capacity_);
			}
			elems_ = elem_pointer();
		};

		void check(void){
			if(size() <= capacity_) return;
			wpos_	= 0;
			rpos_	= 0;
		};
		
	private:
		allocator_type		alloc_;

		const uint32_t		capacity_;
		uint32_t			wpos_, rpos_;
		elem_pointer		elems_;
		
	};

	typedef unlk_queue<char*, std::allocator<char> > byte_unlkqueue;

}

#endif




