#ifndef MEM_MGR_HPP_201203071423
#define MEM_MGR_HPP_201203071423

#include <cstdlib>
#include "interprocess_synch.hpp"
#include "offset_ptr.hpp"

namespace sparrow
{
	class mem_mgr
	{
		typedef std::size_t offset_type;

		struct header{
			offset_type		count;
			offset_type		next;

			header(void) : 
			count(0),
				next(0)
			{
			};
		};

		typedef offset_ptr<char>	ptr_type;

		enum{
			b_len = sizeof(offset_type),
			h_count = sizeof(header)/b_len
		};

	public:
		mem_mgr(void) : 
	    mem_((char*)NULL),
		b_count_(0), b_free_(n_offset),
		free_size_(0)
		{
			
		};

		void init(void *mem, const std::size_t len);

		void* malloc(std::size_t size);
		void free(void *ptr);

		std::size_t free_size(void){return free_size_;};

	private:
		offset_type get_offset(void *ptr){
			char *mem_end = get_ptr(b_count_ - 1);
			if(ptr < (void*)(mem_.get() + b_len) || ptr > (void*)mem_end) return n_offset;
			return (offset_type)(((char*)ptr - mem_.get())/b_len);
		};
		char* get_ptr(offset_type i){
			if(n_offset == i || i >= b_count_) return NULL; 
			return (mem_.get() + (i * b_len));
		};

	private:
		ptr_type		mem_;
		ipc_mutex		lock_;
		offset_type		b_count_;
		offset_type		b_free_;
		std::size_t		free_size_;

	private:
		static const offset_type	n_offset;
	};

}





#endif

