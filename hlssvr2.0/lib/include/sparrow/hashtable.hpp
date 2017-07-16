#ifndef HASHTABLE_HPP_201204241642
#define HASHTABLE_HPP_201204241642

#include "hashbase.hpp"
#include <utility>


namespace sparrow
{
namespace container
{
	template<typename K,
		typename T,
		typename Trait = hh_trait_inccap,
		typename H = hash<K>,
		typename A = std::allocator<T>
	>
	class hashtable
	{
	public:
		class elem_type{
		public:
			elem_type(void) : 
			first(), second()
			{};

			elem_type(const K &k, const T &v) :
			first(k), second(v)
			{};
			elem_type(const elem_type &rhs) : 
			first(rhs.first), second(rhs.second)
			{};

			elem_type& operator=(const elem_type &rhs)
			{
				if(this == &rhs) return *this;
				first		= rhs.first;
				second		= rhs.second;
				return *this;
			};

			bool operator==(const elem_type &rhs){
				if(first == rhs.first) return true;
				return false;
			};

		public:
			K	first;
			T	second;
		};

		class hash_elem
		{
			enum{
				INVALID_RANGE	= 0xffffffff
			};
		public:
			hk_type operator()(const elem_type &elem, const hk_type m = INVALID_RANGE) const
			{
				return hh_(elem.first, m);
			};

		private:
			H	hh_;
		};

		
		typedef hashbase<elem_type, Trait, hash_elem, A> _hhbase_type;

		typedef hashtable<K, T, Trait, H, A> _this_type;

	public:
		class iterator
		{
			typedef typename _hhbase_type::iterator _iter_type;

			iterator(const _iter_type &iter) : 
			iter_(iter)
			{};
		public:
			iterator(void){};

			iterator(const iterator &rhs) : 
			iter_(rhs.iter_)
			{};
			
			elem_type* operator->(void)
			{
				return &(*iter_);
			};
			
			iterator& operator=(const iterator &rhs)
			{
				if(this == &rhs) return *this;
				iter_ = rhs.iter_;
				return *this;
			};

			bool operator==(const iterator &rhs)
			{
				return (iter_ == rhs.iter_);
			};
			bool operator!=(const iterator &rhs){
				return (iter_ != rhs.iter_);
			};

			iterator& operator++(void)
			{
				++iter_;
				return *this;
			};

			iterator operator++(int)
			{
				iterator ret(iter_);
				++iter_;
				return ret;
			};
			
		private:
			_iter_type	iter_;

			template<typename Kt,
				typename Tt,
				typename Triatt,
				typename Ht,
				typename At
			>
			friend class hashtable;
		};

	public:
		hashtable(void){};
		
		hashtable(const size_t cap) : 
		hhbase_(cap)
		{};

		hashtable(const A &alloc) :
		hhbase_(alloc)
		{};

		hashtable(const size_t cap, const A &alloc) : 
		hhbase_(cap, alloc)
		{};

		hashtable(const _this_type &rhs) : 
		hhbase_(rhs.hhbase_)
		{};

		_this_type& operator=(const _this_type &rhs)
		{
			if(this == &rhs) return *this;
			hhbase_ = rhs.hhbase_;
			return *this;
		};

		iterator begin(void)
		{
			return iterator(hhbase_.begin());
		};
		iterator begin(void) const
		{
			return iterator(hhbase_.begin());
		};
		iterator end(void)
		{
			return iterator(hhbase_.end());
		};
		iterator end(void) const{
			return iterator(hhbase_.end());
		};
		
		iterator find(const K &k){
			elem_type elem;
			elem.first = k;
			return iterator(hhbase_.find(elem));
		};
		void find(const K &k, iterator &iter)
		{
			elem_type elem;
			elem.first = k;
			hhbase_.find(elem, iter.iter_);
		};
		iterator find(const K &k) const{
			elem_type elem;
			elem.first = k;
			return iterator(hhbase_.find(elem));
		};
		void find(const K &k, iterator &iter) const
		{
			elem_type elem;
			elem.first = k;
			hhbase_.find(elem, iter.iter_);
		};
		
		iterator insert(const std::pair<K, T> &pr)
		{
			elem_type elem;
			elem.first = pr.first;
			elem.second = pr.second;
			return iterator(hhbase_.insert(elem));
		};	
		iterator insert(const K &k, const T &v)
		{
			elem_type elem(k, v);
			return iterator(hhbase_.insert(elem));
		};

		bool erase(const K &k){
			elem_type elem;
			elem.first = k;
			return hhbase_.erase(elem);
		};
		iterator erase(const iterator &iter)
		{
			return iterator(hhbase_.erase(iter.iter_));
		};

		T& operator[](const K &k)
		{
			elem_type elem;
			elem.first = k;

			return hhbase_[elem].second;
		};

		size_t size(void) const {return hhbase_.size();};

		void clear(void)
		{
			hhbase_.clear();
		};

	private:
		_hhbase_type	hhbase_;

	};
}
}

#endif



