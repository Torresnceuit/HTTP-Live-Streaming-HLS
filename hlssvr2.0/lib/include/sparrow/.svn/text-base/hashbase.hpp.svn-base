#ifndef HASHBASE_HPP_201208161636
#define HASHBASE_HPP_201208161636

#include <memory>
#include <string>
#include "stdint.h"

namespace sparrow
{
namespace container
{
	typedef uint64_t hk_type;

	inline hk_type string_hash(const std::string &str){
		register hk_type ret = 0;
		register const char *p = str.c_str();

		while(*p != 0){
			ret = ret * 31 + (hk_type)*p;
			++p;
		}
		return ret;
	};



	template<typename T>
	class hash
	{
		enum{
			INVALID_RANGE	= 0xffffffff
		};
	public:
		hash(void)
		{};

		hk_type operator()(const T &v, const hk_type m = INVALID_RANGE) const
		{
			if(INVALID_RANGE == m) return (hk_type)v;
			return ((hk_type)v % m);
		};
	};

	template<>
	class hash<std::string>
	{
		enum{
			INVALID_RANGE	= 0xffffffff
		};
	public:
		hk_type operator()(const std::string &str, const hk_type m = INVALID_RANGE) const
		{
			hk_type strhk = string_hash(str);
			if(INVALID_RANGE == m) return (hk_type)strhk;
			return ((hk_type)strhk % m);
		};

	private:
		inline hk_type string_hash(const std::string &str) const
		{
			register hk_type ret = 0;
			register const char *p = str.c_str();

			while(*p != 0){
				ret = ret * 31 + (hk_type)*p;
				++p;
			}
			return ret;
		};
	};

	template<typename T>
	class list_op
	{
	public:
		list_op(void) : 
		head_(T()),  tail_(T())
		{};

		void push_back(T &v)
		{
			if(tail_ == T())
			{
				tail_ = v;
				head_ = tail_;
				head_->pre = T();
				tail_->next = T();
			}else{
				tail_->next = v;
				v->pre = tail_;
				tail_ = v;
				tail_->next = T();
			}
		};
		void push_front(T &v)
		{
			if(head_ == T())
			{
				head_ = v;
				tail_ = head_;
				head_->pre = T();
				tail_->next = T();
			}else{
				head_->pre = v;
				v->next = head_;
				head_ = v;
				head_->pre = T();
			}
		}

		void remove(T &v)
		{
			T pre = v->pre;
			T next = v->next;
			if(pre == T())
			{
				head_ = next;
				if(head_ != T()){
					head_->pre = T();
				}
			}else{
				pre->next = next;
			}

			if(next == T()){
				tail_ = pre;
				if(tail_ != T()){
					tail_->next = T();
				}
			}else{
				next->pre = pre;
			}
			v->pre = T();
			v->next = T();
		}

		void clear(void)
		{
			head_ = T();
			tail_ = head_;
		};

	public:
		T	head_, tail_;
	};

	class hh_cap
	{
	public:
		enum{
			CAP_COUNT	 = 10
		};
	public:
		static size_t get_next_cap(const size_t cap){
			size_t ret = 0;
			for(size_t i = 0; i < CAP_COUNT; ++ i)
			{
				if(cap < caps[i])
				{
					ret = caps[i];
					break;
				}
			}
			if(ret == 0) ret = caps[CAP_COUNT - 1];
			return ret;
		};

		static size_t get_cap(const size_t idx){
			size_t i = idx;
			if(i >= CAP_COUNT) i = CAP_COUNT - 1;
			return caps[i];
		};

		static size_t caps[CAP_COUNT];

		//{1949, 3907, 7817, 15641, 31307, 62617, 125243, 250489, 501001, 1002077}
	};
	/*特征定义*/
	//自动增加容量 不允许重复值
	struct hh_trait_inccap
	{
		bool inc_cap(void) const {return true;};
		bool multi_elem(void) const {return false;};
	};
	//自动增加容量，允许重复值
	struct hh_trait_inccap_multi
	{
		bool inc_cap(void) const {return true;};
		bool multi_elem(void) const {return true;};
	};
	//固定容量，不允许重复值
	struct hh_trait_fixedcap
	{
		bool inc_cap(void) const {return false;};
		bool multi_elem(void) const {return false;};
	};
	//固定容量，允许重复值
	struct hh_trait_fixedcap_multi
	{
		bool inc_cap(void) const {return false;};
		bool multi_elem(void) const {return true;};
	};



	template<typename T,
		typename Trait = hh_trait_inccap,
		typename H = hash<T>,
		typename A = std::allocator<T>
	>
	class hashbase
	{
		typedef typename A::template rebind<char>::other  _char_alloc;
		typedef typename _char_alloc::pointer			  _char_ptr;

		struct elem_type;
		typedef typename A::template rebind<elem_type>::other _elem_type_alloc;
		typedef typename _elem_type_alloc::pointer			   _elem_type_ptr;
		
		struct elem_type
		{
			T		val;
			bool	used;
			_elem_type_ptr pre, next, bknext;

			elem_type(void) :
			val(), used(false),
			pre(_elem_type_ptr()),
			next(_elem_type_ptr()),
			bknext(_elem_type_ptr())
			{};

			void reset(void)
			{
				val.~T();
				new (&val) T();

				used = false;
				pre  = _elem_type_ptr();
				next = pre;
				bknext	= pre;
			}
		};

		struct bucket;
		typedef typename A::template rebind<bucket>::other  _bucket_alloc;
		typedef typename _bucket_alloc::pointer				_bucket_ptr;

		struct elem_mem;
		typedef typename A::template rebind<elem_mem>::other _elem_mem_alloc;
		typedef typename _elem_mem_alloc::pointer		_elem_mem_ptr;

		struct elem_mem{
			_elem_mem_ptr next;

			elem_mem(void) : 
			next(_elem_mem_ptr())
			{};
		};

		struct bucket
		{
			size_t			size;
			_elem_type_ptr	elem;
			
			bucket(void) : 
			size(0), elem(_elem_type_ptr())
			{};
		};

		typedef list_op<_elem_type_ptr>	_elem_list;

		typedef hashbase<T, Trait, H, A> _this_type;

	private:

		enum{
			ALLOC_ELEM_MIN_COUNT   = 16,
			ALLOC_ELEM_MAX_COUNT   = (1024 * 512)/sizeof(elem_type),
			ALLOC_ELEM_COUNT =  ALLOC_ELEM_MAX_COUNT < ALLOC_ELEM_MIN_COUNT ? ALLOC_ELEM_MIN_COUNT : ALLOC_ELEM_MAX_COUNT
		};

	public:
		class iterator
		{
			iterator(const _elem_type_ptr &elemp, const bool f_multi = false) : 
			elemp_(elemp),
			f_multi_(f_multi)
			{
				if(f_multi_ && elemp_ != _elem_type_ptr())
				{
					val_ = elemp_->val;
				}
			};
		public:
			iterator(void) : 
			elemp_(_elem_type_ptr()),
			f_multi_(false),
			val_(T())
			{};
			iterator(const iterator &rhs) : 
			elemp_(rhs.elemp_),
			f_multi_(rhs.f_multi_),
			val_(rhs.val_)
			{};

			T& operator*(void)
			{
				return elemp_->val;
			};
			T* operator->(void)
			{
				return &(elemp_->val);
			};

			iterator& operator=(const iterator &rhs)
			{
				if(this == &rhs) return *this;
				elemp_	= rhs.elemp_;
				f_multi_ = rhs.f_multi_;
				val_	 = rhs.val_;
				return *this;
			};

			bool operator==(const iterator &rhs)
			{
				return elemp_ == rhs.elemp_;
			};
			bool operator!=(const iterator &rhs)
			{
				return !(*this == rhs);
			};

			iterator& operator++(void)
			{
				if(elemp_ == _elem_type_ptr()) return *this;
				if(!elemp_->used){
					elemp_ = _elem_type_ptr();
					return *this;
				}
				if(!f_multi_)
				{
					elemp_ = elemp_->next;
					if(elemp_ != _elem_type_ptr() && !elemp_->used){
						elemp_ = _elem_type_ptr();
					}
				}else{
					elemp_ = elemp_->bknext;
					while(elemp_ != _elem_type_ptr())
					{
						if(!elemp_->used){
							elemp_ = _elem_type_ptr();
							break;
						}
						if(elemp_->val == val_) break;
						elemp_ = elemp_->bknext;
					}
					
				}
				
				return *this;
			};

			iterator operator++(int)
			{
				iterator tmp = *this;
				++(*this);
				return tmp;
			};

		private:
			_elem_type_ptr	elemp_;
			bool			f_multi_;
			T				val_;

			template<typename Tt,
				typename Triatt,
				typename Ht,
				typename At
			>
			friend class hashbase;
		};


	public:
		hashbase(void) : 
		buckets_(_bucket_ptr()),
		bk_cap_(0),
		elem_count_(0),
		elem_mem_(_elem_mem_ptr()),
		elem_pool_(_elem_type_ptr())
		{
			size_t new_bk_cap = hh_cap::get_next_cap(bk_cap_);
			allocate_bucket(new_bk_cap);
		};

		~hashbase(void)
		{
			destroy();
		};

		hashbase(const size_t bkcap) : 
		buckets_(_bucket_ptr()),
		bk_cap_(0),
		elem_count_(0),
		elem_mem_(_elem_mem_ptr()),
		elem_pool_(_elem_type_ptr())
		{
			size_t new_bk_cap = hh_cap::get_cap(bkcap);
			allocate_bucket(new_bk_cap);
		};

		hashbase(const A &alloc) : 
		mem_alloc_(alloc),
		bucket_alloc_(alloc),
		elem_type_alloc_(alloc),
		elem_mem_alloc_(alloc),
		buckets_(_bucket_ptr()),
		bk_cap_(0),
		elem_count_(0),
		elem_mem_(_elem_mem_ptr()),
		elem_pool_(_elem_type_ptr())
		{
			size_t new_bk_cap = hh_cap::get_next_cap(bk_cap_);
			allocate_bucket(new_bk_cap);
		};

		hashbase(const size_t bkcap, const A &alloc) :
		mem_alloc_(alloc),
		bucket_alloc_(alloc),
		elem_type_alloc_(alloc),
		elem_mem_alloc_(alloc),
		buckets_(_bucket_ptr()),
		bk_cap_(0),
		elem_count_(0),
		elem_mem_(_elem_mem_ptr()),
		elem_pool_(_elem_type_ptr())
		{
			size_t new_bk_cap = hh_cap::get_cap(bkcap);
			allocate_bucket(new_bk_cap);
		};

		hashbase(const _this_type &rhs):
		buckets_(_bucket_ptr()),
		bk_cap_(0),
		elem_count_(0),
		elem_mem_(_elem_mem_ptr()),
		elem_pool_(_elem_type_ptr())
		{
			*this = rhs;	
		};

		iterator begin(void){
			return iterator(elem_list_.head_);
		};
		iterator begin(void) const
		{
			return iterator(elem_list_.head_);
		};
		
		iterator end(void)
		{
			return iterator();
		};
		iterator end(void) const
		{
			return iterator();
		};

		_this_type& operator=(const _this_type &rhs)
		{
			if(*this == rhs) return *this;
			for(iterator iter = rhs.begin(); iter != rhs.end(); ++ iter)
			{
				insert(*iter);
			}
			return *this;
		};

		T& operator[](const T &v){
			iterator iter = find(v);
			if(end() == iter)
			{
				iter = insert(v);
			}
			return *iter;
		};

		iterator find(const T &v)
		{
			return iterator(find_i(v), trait_.multi_elem());
		};
		void find(const T &v, iterator &iter)
		{
			iter.elemp_ = find_i(v);
			iter.f_multi_ = trait_.multi_elem();
		};

		iterator find(const T &v) const
		{
			return iterator(find_i(v), trait_.multi_elem());
		};
		void find(const T &v, iterator &iter) const
		{
			iter.elemp_ = find_i(v);
			iter.f_multi_ = trait_.multi_elem();
		};

		iterator insert(const T &v) 
		{
			return iterator(insert_i(v));
		};

		bool erase(const T &v)
		{
			if(!trait_.multi_elem())
			{
				return erase_i(v);
			}else{
				T tmp_v = v;
				return erase_i(tmp_v);
			}
		};

		iterator erase(const iterator &iter)
		{
			if(end() == iter) return iter;
			iterator ret = iter;
			ret.f_multi_ = false;
			while(ret.elemp_->val == iter.elemp_->val)
			{
				++ ret;
				if(end() == ret) break;
			}
			if(!trait_.multi_elem())
			{
				erase_i(iter.elemp_->val);
			}else{
				T tmp_v = iter.elemp_->val;
				erase_i(tmp_v);
			}
			return ret;
		};

		size_t size(void) const {return elem_count_;};

		void clear(void)
		{
			for(size_t i = 0; i < bk_cap_; ++ i)
			{
				_bucket_ptr bkp = buckets_ + i;
				if(bkp->size == 0) continue;

				_elem_type_ptr elemp = bkp->elem;
				_elem_type_ptr elem_next = _elem_type_ptr();
				while(elemp != _elem_type_ptr())
				{
					if(!elemp->used) break;
					elem_next = elemp->bknext;
					free_elem(elemp);
					elemp = elem_next;
				}
				bkp->size = 0;
				bkp->elem = _elem_type_ptr();
			}
			elem_list_.clear();
			elem_count_ = 0;
		};

	private:
		void destroy(void)
		{
			elem_list_.clear();
			elem_count_ = 0;
			
			deallocate_bucket();
			deallocate_elem();

		};

		bool allocate_bucket(void)
		{
			size_t new_bk_cap = hh_cap::get_next_cap(bk_cap_);
			if(new_bk_cap == bk_cap_) return false;
			return allocate_bucket(new_bk_cap);
		};

		bool allocate_bucket(const size_t new_bk_cap)
		{
			_bucket_ptr	new_buckets = bucket_alloc_.allocate(new_bk_cap);
			if(new_buckets == _bucket_ptr()) return false;

			for(size_t i = 0; i < new_bk_cap; ++ i)
			{
				_bucket_ptr bkp = new_buckets + i;
				bucket_alloc_.construct(bkp, bucket());
			}

			move_elems(new_buckets, new_bk_cap, buckets_, bk_cap_);

			_bucket_ptr rm_buckets = buckets_;
			size_t      rm_cap    = bk_cap_;

			buckets_	= new_buckets;
			bk_cap_		= new_bk_cap;

			bucket_alloc_.deallocate(rm_buckets, rm_cap);

			return true;
		};

		void deallocate_bucket(void)
		{
			bucket_alloc_.deallocate(buckets_, bk_cap_);
			bk_cap_  = 0;
			buckets_ = _bucket_ptr();
		}

		void move_elems(_bucket_ptr &dst_buckets, const size_t dst_cap, _bucket_ptr &src_buckets, const size_t src_cap)
		{
			_bucket_ptr src_bkp;
			_bucket_ptr dst_bkp;
			for(size_t i = 0; i < src_cap; ++ i){
				src_bkp = src_buckets + i;
				_elem_type_ptr elemp = src_bkp->elem;
				_elem_type_ptr elemp_next = _elem_type_ptr();
				size_t n = 0;
				while(elemp != _elem_type_ptr() && n < src_bkp->size)
				{
					++ n;
					if(!elemp->used) break;
					elemp_next = elemp->bknext;
					elemp->bknext = _elem_type_ptr();

					hk_type k = hh_(elemp->val, dst_cap);
					dst_bkp = (dst_buckets + k);

					elemp->bknext = dst_bkp->elem;
					dst_bkp->elem = elemp;
						
					++ (dst_bkp->size);

					elemp = elemp_next;
				}
			}
		}

		_elem_type_ptr get_elem(void)
		{
			_elem_type_ptr pret = _elem_type_ptr();
			if(elem_pool_ == _elem_type_ptr()){
				if(!allocate_elem()) return pret;
			}
			pret = elem_pool_;
			elem_pool_ = elem_pool_->bknext;
			pret->bknext = _elem_type_ptr();
			pret->used = true;
			return pret;
		};

		void free_elem(_elem_type_ptr &elemp)
		{
			elemp->reset();
			elemp->bknext = elem_pool_;
			elem_pool_    = elemp;
		};

		bool allocate_elem(void)
		{
			_char_ptr mem = mem_alloc_.allocate(sizeof(elem_mem) + ALLOC_ELEM_COUNT * sizeof(elem_type));
			if(mem == _char_ptr()) return false;
			_elem_mem_ptr emem = (_elem_mem_ptr)(mem + 0);
			elem_mem_alloc_.construct(emem, elem_mem());
			emem->next = elem_mem_;
			elem_mem_ = emem;

			_elem_type_ptr elemp = (_elem_type_ptr)(mem + sizeof(elem_mem));
			for(size_t i = 0; i < ALLOC_ELEM_COUNT; ++ i)
			{
				_elem_type_ptr elem = elemp + i;
				elem_type_alloc_.construct(elem, elem_type());
				elem->bknext = elem_pool_;
				elem_pool_ = elem;
			}
			return true;
		};

		void deallocate_elem(void)
		{
			_elem_mem_ptr next;
			_char_ptr mem;
			while(elem_mem_ != _elem_mem_ptr())
			{
				next = elem_mem_->next;
				elem_mem_alloc_.destroy(elem_mem_);

				mem = (_char_ptr)(elem_mem_ + 0);

				_elem_type_ptr elemp = (_elem_type_ptr)(mem + sizeof(elem_mem));
				for(size_t i = 0; i < ALLOC_ELEM_COUNT; ++ i)
				{
					_elem_type_ptr elem = elemp + i;
					elem_type_alloc_.destroy(elem);
				}

				mem_alloc_.deallocate(mem, sizeof(elem_mem) + ALLOC_ELEM_COUNT * sizeof(elem_type));

				elem_mem_ = next;
			}
		};

		_elem_type_ptr find_i(const T &v)
		{
			if(bk_cap_ == 0) return _elem_type_ptr();

			_elem_type_ptr elem_ret = _elem_type_ptr();

			hk_type k = hh_(v, bk_cap_);
			_bucket_ptr bkp = buckets_ + k;
			_elem_type_ptr elemp = bkp->elem;
			for(size_t i = 0; i < bkp->size; ++ i)
			{
				if(elemp == _elem_type_ptr()) break;
				if(!elemp->used) break;
				if(elemp->val == v){
					elem_ret = elemp;
					break;
				}
				elemp = elemp->bknext;
			}
			return elem_ret;
		}

		const _elem_type_ptr find_i(const T &v) const 
		{
			if(bk_cap_ == 0) return _elem_type_ptr();

			const _elem_type_ptr elem_ret = _elem_type_ptr();

			hk_type k = hh_(v, bk_cap_);
			const _bucket_ptr bkp = buckets_ + k;
			const _elem_type_ptr elemp = bkp->elem;
			for(size_t i = 0; i < bkp->size; ++ i)
			{
				if(elemp == _elem_type_ptr()) break;
				if(!elemp->used) break;
				if(elemp->val == v){
					elem_ret = elemp;
					break;
				}
				elemp = elemp->bknext;
			}
			return elem_ret;
		}

		_elem_type_ptr insert_i(const T &v)
		{
			if(bk_cap_ == 0) return _elem_type_ptr();

			hk_type k = hh_(v, bk_cap_);
			
			_bucket_ptr bkp = buckets_ + k;

			if(!trait_.multi_elem())
			{
				bool bfind = false;
				_elem_type_ptr elemp = bkp->elem;
				for(size_t i = 0; i < bkp->size; ++ i)
				{
					if(elemp == _elem_type_ptr()) break;
					if(!elemp->used) break;
					if(elemp->val == v)
					{
						bfind = true;
						break;
					}
					elemp = elemp->bknext;
				}
				if(bfind) return _elem_type_ptr();
			}

			_elem_type_ptr elem_ret = get_elem();
			if(elem_ret == _elem_type_ptr()) return elem_ret;

			elem_ret->val = v;
			elem_ret->bknext = bkp->elem;
			bkp->elem = elem_ret;
			++ (bkp->size);

			elem_list_.push_back(elem_ret);
			++ elem_count_;

			if(trait_.inc_cap() && elem_count_ > bk_cap_)
			{
				allocate_bucket();
			}
			return elem_ret;
		};

		bool erase_i(const T &v)
		{
			if(bk_cap_ == 0) return false;

			hk_type k = hh_(v, bk_cap_);
			_bucket_ptr bkp = buckets_ + k;
			_elem_type_ptr elemp = bkp->elem;
			_elem_type_ptr elem_pre = _elem_type_ptr();
			bool ret = false;
			size_t idx = 0;

			do{
				_elem_type_ptr elem_ret = _elem_type_ptr();
				for(; idx < bkp->size; ++ idx)
				{
					if(elemp == _elem_type_ptr()) break;
					if(!elemp->used) break;
					if(elemp->val == v){
						elem_ret = elemp;
						break;
					}
					elem_pre = elemp;
					elemp = elemp->bknext;
				}
				if(elem_ret == _elem_type_ptr()) break;
				ret = true;
				elemp = elem_ret->bknext;
				if(elem_pre == _elem_type_ptr())
				{
					bkp->elem = elem_ret->bknext;
				}else{
					elem_pre->bknext = elem_ret->bknext;
				}
				if(bkp->size > 0) {
					--(bkp->size);
				}

				elem_list_.remove(elem_ret);
				-- elem_count_;

				free_elem(elem_ret);

			}while(elemp != _elem_type_ptr() && trait_.multi_elem());
			
			return ret;
		};

	private:
		Trait			trait_;
		H				hh_;

		_char_alloc		mem_alloc_;
		_bucket_alloc	bucket_alloc_;
		_elem_type_alloc elem_type_alloc_;
		_elem_mem_alloc	 elem_mem_alloc_;

		_bucket_ptr		buckets_;
		size_t			bk_cap_;;

		_elem_list		elem_list_;
		size_t			elem_count_;

		_elem_mem_ptr	elem_mem_;
		_elem_type_ptr	elem_pool_;
	};
}
}


#endif

