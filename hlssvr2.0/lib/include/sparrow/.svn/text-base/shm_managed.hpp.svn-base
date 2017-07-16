#ifndef SHM_MANAGED_HPP_201203071835
#define SHM_MANAGED_HPP_201203071835

#include "mem_mgr.hpp"
#include <string.h>
#include <string>
#include "offset_ptr.hpp"
#include "detail.hpp"
#include "interprocess_synch.hpp"

using std::string;

namespace sparrow
{
	template<typename shm_type>
	class shm_managed{
		enum{
			MAX_NAME_LEN	= 127
		};

		struct shm_item_type{

			char				key[MAX_NAME_LEN + 1];
			offset_ptr<int>		val;

			offset_ptr<shm_item_type>	next;

			shm_item_type(void){
				memset(key, 0, sizeof(key));
			};
		};

		struct shm_item_list
		{
			ipc_mutex	lock;
			offset_ptr<shm_item_type> head;
		};

	public:
		shm_managed(void) : 
	    pmgr_(NULL)
		{
		};
		int create(const string &name, std::size_t size){
			int ret = shm_.create(name, size);
			if(0 != ret) return ret;

			char *ptr = (char*)shm_.mem_ptr();
			pmgr_ = new ((void*)ptr) mem_mgr();
			std::size_t len = shm_.mem_size() - sizeof(mem_mgr);
			ptr += sizeof(mem_mgr);

			shm_items_ = new((void*)ptr) shm_item_list();

			len = len - sizeof(shm_item_list);
			ptr += sizeof(shm_item_list);

			pmgr_->init(ptr, len);

			return 0;
		};
		int open(const string &name, const bool readonly = false){
			int ret = shm_.open(name, readonly);
			if(0 != ret) return ret;

			char *ptr = (char*)shm_.mem_ptr();
			pmgr_	= (mem_mgr*)ptr;
			ptr += sizeof(mem_mgr);
			
			shm_items_ = (shm_item_list*)ptr;
			//ptr += sizeof(shm_item_list);

			return 0;
		};

		int open(const shm_type &shm)
		{
			shm_ = shm;

			char *ptr = (char*)shm_.mem_ptr();
			pmgr_	= (mem_mgr*)ptr;
			ptr += sizeof(mem_mgr);

			shm_items_ = (shm_item_list*)ptr;
			//ptr += sizeof(shm_item_list);

			return 0;
		}

		void close(void){
			shm_.close();
		};

		static void remove(const string &name){
			shm_type::remove(name);
		};

		template<typename T>
		construct_proxy<T> construct(const string &name){
			if(strlen(name.c_str()) > MAX_NAME_LEN) return NULL;

			ipc_mutex::scoped_lock guard(shm_items_->lock);

			T *ret = find_i<T>(name);
			if(NULL != ret) return construct_proxy<T>(ret);

			void *ptr = pmgr_->malloc(sizeof(shm_item_type) + sizeof(T));
			if(NULL == ptr) return NULL;
			shm_item_type *pitem = new(ptr) shm_item_type();
			ptr =(void*)((char*)ptr + sizeof(shm_item_type));
			T *val = (T*)ptr;

			strcpy(pitem->key, name.c_str());
			pitem->val = val;

			if(shm_items_->head.get() == NULL){
				shm_items_->head = pitem;
			}else{
				shm_item_type *pi = shm_items_->head.get();
				while(true){
					if(pi->next.get() == NULL){
						pi->next = pitem;
						break;
					}
					pi = pi->next.get();
				}
			}

			return construct_proxy<T>(val);
		};

		template<typename T>
		T* construct_array(const string &name, std::size_t len){
			if(strlen(name.c_str()) > MAX_NAME_LEN) return NULL;

			ipc_mutex::scoped_lock guard(shm_items_->lock);

			T *ret = find_i<T>(name);
			if(NULL != ret) return ret;

			void *ptr = pmgr_->malloc(sizeof(shm_item_type) + sizeof(T) * len);
			if(NULL == ptr) return NULL;
			shm_item_type *pitem = new(ptr) shm_item_type();
			ptr =(void*)((char*)ptr + sizeof(shm_item_type));
			T *val = (T*)ptr;

			strcpy(pitem->key, name.c_str());
			pitem->val = val;

			if(shm_items_->head.get() == NULL){
				shm_items_->head = pitem;
			}else{
				shm_item_type *pi = shm_items_->head.get();
				while(true){
					if(pi->next.get() == NULL){
						pi->next = pitem;
						break;
					}
					pi = pi->next.get();
				}
			}

			for(std::size_t i = 0; i < len; ++i){
				new((void*)(val + i)) T();
			}
			return val;
		};

		template<typename T>
		T* find(const string &name){
			ipc_mutex::scoped_lock guard(shm_items_->lock);
			return find_i<T>(name);
		};

		template<typename T>
		void destroy(const string &name){
			ipc_mutex::scoped_lock guard(shm_items_->lock);

			shm_item_type *pi = shm_items_->head.get();
			shm_item_type *pre = NULL;
			shm_item_type *pret = NULL;
			while(NULL != pi){
				if(name.compare(pi->key) == 0){
					pret = pi;
					if(NULL != pre) pre->next = pi->next;
					break;
				}
				pre = pi;
				pi  = pi->next.get();
			}
			if(NULL == pret) return;
			T *p = (T*)pret->val.get();
			p->~T();
			pret->~shm_item_type();

			pmgr_->free(pret);
		};

		template<typename T>
		construct_proxy<T> construct(void){
			void *ptr = malloc(sizeof(T));
			return construct_proxy<T>((T*)ptr);
		};

		template<typename T>
		T* construct_array(std::size_t len){
			void *ptr = malloc(sizeof(T) * len);
			if(NULL == ptr) return (T*)NULL;
			T *p = (T*)ptr;
			for(std::size_t i = 0; i < len; ++i){
				new((void*)(p + i)) T();
			}
			return p;
		};

		template<typename T>
		void destroy(T *p){
			p->~T();
			free(p);
		};

		void* malloc(std::size_t size){
			return pmgr_->malloc(size);
		};
		void free(void *p){
			pmgr_->free(p);
		};

		mem_mgr& get_memory_manager(void){
			return *pmgr_;
		};

		shm_type& get_shm(void){
			return shm_;
		};

	private:
		template<typename T>
		T* find_i(const string &name){
			shm_item_type *pi = shm_items_->head.get();
			T *ret = NULL;
			while(NULL != pi){
				if(name.compare(pi->key) == 0){
					ret = (T*)pi->val.get();
					break;
				}
				pi = pi->next.get();
			}

			return ret;
		};

	private:
		shm_type		shm_;
		mem_mgr			*pmgr_;

		shm_item_list	*shm_items_;
	};
}








#endif

