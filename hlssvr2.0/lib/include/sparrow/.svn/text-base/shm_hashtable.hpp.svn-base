#ifndef SHM_HASH_TABLE_HPP_201204131511
#define SHM_HASH_TABLE_HPP_201204131511

#include "boost/unordered_map.hpp"
#include "boost/cstdint.hpp"
#include "shm_managed.hpp"
#include "sv_shm.hpp"
#include "list.hpp"
#include "shm_allocator.hpp"
#include "interprocess_synch.hpp"

using boost::uint32_t;

//实现共享内存中的hashtable

namespace sparrow
{
namespace container
{
	template<typename K, 
		typename T, 
		typename key_op
	>
	class shm_hashtable
	{
	private:
		typedef shm_hashtable<K, T, key_op> this_type;

		typedef T	node_type;

		typedef boost::unordered_map<K, node_type*>	_ht_type;
		typedef typename _ht_type::iterator			ht_iterator;
		//typedef typename _ht_type::const_iterator	const_ht_iterator;

	public:
		typedef shm_allocator<node_type, mem_mgr>	alloc_type;
		typedef list<node_type, alloc_type>			list_type;

		class iterator
		{	
		public:
			iterator(void){};
			iterator(const iterator &rhs) : htiter_(rhs.htiter_){};
			iterator(const ht_iterator &iter) : htiter_(iter){};

			iterator& operator=(const iterator &rhs){
				if(this == &rhs) return *this;
				htiter_	= rhs.htiter_;
				return *this;
			};

			T& operator*(){
				return *(htiter_->second);
			};

			T* operator->(){
				return htiter_->second;
			};

			iterator& operator++(void){
				++htiter_;
				return *this;
			};
			iterator operator++(int){
				ht_iterator ret = htiter_;
				++(*this);
				return iterator(ret);
			};

			bool operator==(const iterator &rhs){
				return (htiter_ == rhs.htiter_);
			};
			bool operator!=(const iterator &rhs){
				return (htiter_ != rhs.htiter_);
			};


		private:
			ht_iterator	htiter_;	

			template<typename Kt, 
			typename Tt, 
			typename key_opt
			>
			friend class shm_hashtable;
		};

	private:
		struct ht_info
		{
			ipc_mutex				lock;
			uint32_t				mod_cnt;
			offset_ptr<list_type>	htlist;

			ht_info(void) : mod_cnt(0){};
		};
	public:
		shm_hashtable(void) : phti_(NULL){};
		shm_hashtable(const this_type &rhs) : phti_(rhs.phti_){
			if(phti_ == NULL) return;
			ipc_mutex::scoped_lock guard(phti_->lock);
			load_from_shm();
		};
		shm_hashtable(ht_info *phti) : phti_(phti)
		{
			if(phti_ == NULL) return;
			ipc_mutex::scoped_lock guard(phti_->lock);
			load_from_shm();
		}
		~shm_hashtable(void){
			ht_.clear();
		};

		template<typename shmmd_type>
		static this_type create_or_open(const string &name, shmmd_type &shmmd){
			string hti		= name + "_hti";
			ht_info *phti = shmmd. template find<ht_info>(hti);
			if(NULL == phti){
				alloc_type alloc(shmmd.get_memory_manager());
				list_type *plist = shmmd. template construct<list_type>()(alloc);
				phti = shmmd. template construct<ht_info>(hti)();
				phti->htlist = plist;
			}
			return this_type(phti);
		};

		this_type& operator=(const this_type &rhs){
			if(this == &rhs) return *this;
			phti_	= rhs.phti_;
			if(phti_ != NULL){
				ipc_mutex::scoped_lock guard(phti_->lock);
				load_from_shm();
			}
			return *this;
		};

		bool is_invalid(void) const {return (phti_ == NULL);};

		T& operator[](const K &k){
			if(mod_cnt_ != phti_->mod_cnt) {
				ipc_mutex::scoped_lock guard(phti_->lock);
				load_from_shm();
			}
			iterator iter = find(k);
			if(iter == end()){
				ipc_mutex::scoped_lock guard(phti_->lock);
				T tmp;
				kop_(tmp, k);
				std::pair<iterator, bool> ret = insert_i(tmp);
				if(!ret.second){
					iter = find(k);
				}else{
					iter = ret.first;
				}
			}
			return *iter;
		};

		iterator begin(void){
			if(mod_cnt_ != phti_->mod_cnt) {
				ipc_mutex::scoped_lock guard(phti_->lock);
				load_from_shm();
			}
			return iterator(ht_.begin());
		};
		iterator end(void){
			if(mod_cnt_ != phti_->mod_cnt) {
				ipc_mutex::scoped_lock guard(phti_->lock);
				load_from_shm();
			}
			return iterator(ht_.end());
		};
		
		iterator find(const K &k){
			if(mod_cnt_ != phti_->mod_cnt) {
				ipc_mutex::scoped_lock guard(phti_->lock);
				load_from_shm();
			}
			return iterator(ht_.find(k));
		};

		std::pair<iterator, bool> insert(const T &v){
			ipc_mutex::scoped_lock guard(phti_->lock);
			return insert_i(v);
		};

		iterator erase(iterator &iter){
			if(ht_.end() == iter.htiter_) return iter;

			ipc_mutex::scoped_lock guard(phti_->lock);
			if(mod_cnt_ != phti_->mod_cnt){
				load_from_shm();
			}

			K key = kop_(*iter);

			for(typename list_type::iterator i = phti_->htlist->begin(); i != phti_->htlist->end(); ++i){
				node_type &data = *i;
				K k = kop_(data);
				if(k == key){
					phti_->htlist->erase(i);
					break;
				}
			}

			iterator i_ret(ht_.erase(iter.htiter_));

			++(phti_->mod_cnt);
			mod_cnt_ = phti_->mod_cnt;

			return i_ret;
		};

		iterator erase(const K &k){
			iterator iter(ht_.find(k));
			return erase(iter);
		};

		void clear(void){
			ht_.clear();
			if(NULL == phti_) return;
			ipc_mutex::scoped_lock guard(phti_->lock);
			phti_->htlist->clear();
			++(phti_->mod_cnt);
			mod_cnt_ = phti_->mod_cnt;
		};

		size_t size(void) {
			if(mod_cnt_ != phti_->mod_cnt) {
				ipc_mutex::scoped_lock guard(phti_->lock);
				load_from_shm();
			}
			return ht_.size();
		};

	private:

		std::pair<iterator, bool> insert_i(const T &v){
			K key = kop_(v);
			ht_iterator iter;
			
			if(mod_cnt_ != phti_->mod_cnt){
				load_from_shm();
			}
			iter = ht_.find(key);
			if(ht_.end() != iter){
				return std::make_pair(iterator(ht_.end()), false);
			}
			
			phti_->htlist->push_back(v);
			node_type *pnode = &(phti_->htlist->back());
			if(NULL == pnode){
				return std::make_pair(iterator(ht_.end()), false);
			}
			++(phti_->mod_cnt);
			mod_cnt_ = phti_->mod_cnt;

			std::pair<ht_iterator, bool> ret = ht_.insert(std::make_pair(key, pnode));
			return std::make_pair(iterator(ret.first), ret.second);
		};

		void load_from_shm(void){
			if(NULL == phti_) return;
			ht_.clear();
			for(typename list_type::iterator i = phti_->htlist->begin(); i != phti_->htlist->end(); ++i){
				node_type &data = *i;
				K key = kop_(data);
				ht_.insert(std::make_pair(key, &data));
			}
			mod_cnt_ = phti_->mod_cnt;
		};

	private:
		ht_info		*phti_;
		uint32_t	mod_cnt_;
		_ht_type	ht_;
		key_op		kop_;
	};
}
}



#endif
