#ifndef MEM_BASE_HPP_201107042302
#define MEM_BASE_HPP_201107042302

#include <vector>
#include <stdlib.h>

namespace sparrow
{

	/**用于内存对齐的联合*/
	union align_info{
		int (*i)();
		void* p;
		long l;
	};

	/**内存链表结点*/
	class mem_node{
	public:
		mem_node(void) : pnext_(NULL){};
		~mem_node(void){};

		mem_node* next(void) const{return pnext_;};
		void next(mem_node *pnext){pnext_ = pnext;};
	private:
		mem_node *pnext_;
	};

	/**空闲内存链表
	*/
	class mem_free_list{
	public:
		mem_free_list(void) : pnode_(NULL){};
		~mem_free_list(void){pnode_ = NULL;};

		void add(mem_node *pnode){
			if(NULL == pnode) return;
			if(NULL == pnode_){
				pnode_ = pnode;
				pnode_->next(NULL);
			}else{
				pnode->next(pnode_);
				pnode_ = pnode;
			}
		};

		void* remove(void){
			mem_node *pnode = pnode_;
			if(NULL == pnode) return pnode;
			pnode_ = pnode_->next();
			return pnode;
		}

	private:
		mem_node *pnode_;
	};

	class memory_pool
	{
		typedef std::vector<char*>	_arr_mem;

	public:
		memory_pool(const size_t objsize, const size_t count, const size_t inc_count) : 
		objsize_(objsize), inc_count_(inc_count)
		{
			if(objsize_ < sizeof(mem_node)) objsize_ = sizeof(mem_node);
			inc_mem(count);
		};

		void* malloc(void)
		{
			void *p = free_list_.remove();
			if(NULL == p)
			{
				inc_mem(inc_count_);
				p = free_list_.remove();
			}
			return p;
		};
		void free(void *p){
			if(NULL == p) return;
			free_list_.add((mem_node*)p);
		};

	private:
		void inc_mem(const size_t count)
		{
			if(0 == count) return;

			char *mem = (char*)::malloc(objsize_ * count);
			if(NULL == mem) return;
			
			arr_mem_.push_back(mem);

			mem_node *pnode = NULL;
			for(size_t i = 0; i < count; ++ i)
			{
				pnode = new (mem + i * objsize_) mem_node();
				free_list_.add(pnode);
			}
		};

	private:
		_arr_mem		arr_mem_;
		mem_free_list	free_list_;
		size_t			objsize_;
		size_t			inc_count_;
	};

	template<typename T>
	class mem_base
	{
		enum{
			DEF_COUNT = 100,
			INC_COUNT = 100
		};
	public:
		static void* operator new(size_t size, void *p){return p;} 

		static void* operator new(size_t size){
			return mem_pool_.malloc();
		};
		static void operator delete(void *p){
			mem_pool_.free(p);
		};

	private:
		static memory_pool		mem_pool_;
	};
	template<typename T>
	memory_pool mem_base<T>::mem_pool_(sizeof(T), DEF_COUNT, INC_COUNT);
}


#endif

