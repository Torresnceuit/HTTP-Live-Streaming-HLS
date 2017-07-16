#ifndef LIST_HPP_201203091822
#define LIST_HPP_201203091822

#include <memory>
#include "offset_ptr.hpp"

namespace sparrow
{
namespace container
{
	template<typename T, typename A = std::allocator<T> >
	class list
	{
		enum{
			MIN_ALLOC_NODES	= 32
		};
	public:
		struct node;

		typedef node	 node_type;
		typedef A		 allocator_type;
		typedef typename allocator_type::template rebind<node_type>::other inside_allocator_type;
		typedef T        value_type;
		typedef T*       pointer;
		typedef T&       reference;
		typedef const T& const_reference;

		typedef typename inside_allocator_type::pointer node_pointer;
		
		struct node
		{
			T	val;
			node_pointer pre;
			node_pointer next;

			node(void) : 
			val(),
			pre(node_pointer()),
			next(pre)
			{
			};

			void reset(void){
				val.~T();
				new(&val)T();
				pre	= node_pointer();
				next = pre;
			};
		};
		
		typedef std::size_t size_type;

		typedef list<T, A> this_type;

	private:
		struct freenodes;

		typedef typename allocator_type::template rebind<freenodes>::other freenodes_alloc_type;
		typedef typename freenodes_alloc_type::pointer freenodes_pointer;

		struct freenodes{
			node_pointer	nodes;
			size_t			count;
			freenodes_pointer	next;

			freenodes(void) : 
			nodes(node_pointer()),
			count(0),
			next(freenodes_pointer())
			{
			};
		};

		struct node_pool{
			freenodes_pointer	fns;
			node_pointer		head;

			node_pool(void) :
			fns(freenodes_pointer()),
			head(node_pointer())
			{
			};
		};
		
	public:
		class iterator
		{
		public:
			iterator(void){};
			explicit iterator(const node_pointer &pnode) : pnode_(pnode){};
			iterator(const iterator &rhs) : pnode_(rhs.pnode_){};

			iterator& operator=(const iterator &rhs){
				if(this == &rhs) return *this;
				pnode_	= rhs.pnode_;
				return *this;
			};

			bool operator==(const iterator &rhs){
				return (pnode_ == rhs.pnode_);
			};
			bool operator!=(const iterator &rhs){
				return (!(*this == rhs));
			};

			reference operator*(void){
				return pnode_->val;
			};
			
			pointer operator->(void){
				return &(pnode_->val);
			};

			iterator& operator++(void){
				if(pnode_ == (void*)NULL) return *this;
				pnode_ = pnode_->next;
				return *this;
			};
			iterator operator++(int){
				iterator tmp(*this);
				++*this;
				return tmp;
			};
			iterator& operator--(void){
				if(pnode_ == (void*)NULL) return *this;
				pnode_ = pnode_->pre;
				return *this;
			};
			iterator operator--(int){
				iterator tmp(*this);
				--*this;
				return tmp;
			};

		private:
			node_pointer	pnode_;

			template<typename U, typename B> friend class list;
		};

	public:
		list(void) : 
		header_(node_pointer()),
		tail_(node_pointer()),
		size_(0)
		{};
		list(const allocator_type &alloc) : 
		alloc_(alloc),
		fnds_alloc_(alloc),
		header_(node_pointer()),
		tail_(node_pointer()),
		size_(0)
		{
		};

		list(const this_type &rhs) : 
		alloc_(rhs.alloc_), 
		fnds_alloc_(rhs.fnds_alloc_),
		header_(rhs.header_), tail_(rhs.tail_),
		size_(rhs.size_)
		{
		};

		~list(void){
			destroy();
		};

		this_type& operator=(const this_type &rhs){
			if(this == &rhs) return *this;
			clear();

			for(typename this_type::iterator iter = rhs.begin(); iter != rhs.end(); ++ iter){
				push_back(*iter);
			}
			return *this;
		};

		iterator begin(void) const {
			return iterator(header_);
		};
		iterator end(void) const {
			return iterator(node_pointer());
		};

		iterator rbegin(void) const{
			return iterator(tail_);
		};
		iterator rend(void) const{
			return end();
		};

		size_type size(void) const{ return size_;};

		iterator insert(const iterator &iter, const T &val){
			node_pointer pn = get_free_node();
			if(pn == (void*)NULL) return end();
			pn->val = val;

			++ size_;

			//¿Õ
			if(header_ == (void*)NULL){
				header_		= pn;
				tail_		= header_;
				header_->pre = node_pointer();
				tail_->next  = node_pointer();
				
				return iterator(pn);
			}
			
			node_pointer cur = iter.pnode_;
			if(cur == (void*)NULL){//²åÈëÎ²²¿
				pn->next = tail_->next;
				pn->pre  = tail_;
				tail_->next = pn;
				tail_	 = pn;
				return iterator(pn);
			}

			node_pointer pre = cur->pre;
			pn->pre		= pre;
			pn->next	= cur;
			cur->pre	= pn;

			if(pre != (void*)NULL){
				pre->next = pn;
			}else{
				header_ = pn;
			}

			return iterator(pn);
		};

		iterator erase(const iterator &iter){
			if(end() == iter) return end();
			node_pointer cur = iter.pnode_;

			node_pointer pre = cur->pre;
			node_pointer next = cur->next;
			
			if(pre != (void*)NULL){
				pre->next = next;
			}else{
				header_ = next;
			}
			if(next != (void*)NULL){
				next->pre = pre;
			}else{
				tail_ = pre;
			}

			put_free_node(cur);

			--size_;

			return iterator(next);
		};

		void push_front(const T &val){
			insert(iterator(header_), val);
		};
		T& front(void){
			return header_->val;
		};
		void pop_front(void){
			if(begin() == end()) return;

			erase(iterator(header_));
		};

		void push_back(const T &val){
			insert(end(), val);
		};
		T& back(void){
			return tail_->val;
		};
		void pop_back(void){
			if(iterator(tail_) == end()) return;
			erase(iterator(tail_));
		};

		void clear(){
			while(size_ > 0){
				pop_back();
			}
		};

		void reset(void){
			destroy();
		};

	private:
		void destroy(void){
			clear();
			
			while(node_pool_.fns != (void*)NULL){
				freenodes_pointer	tmp = node_pool_.fns;
				node_pool_.fns	= node_pool_.fns->next;

				alloc_.deallocate(tmp->nodes, tmp->count);

				fnds_alloc_.destroy(tmp);
				fnds_alloc_.deallocate(tmp, 1);
			}
			node_pool_.head = node_pointer();
		};
		node_pointer get_free_node(void){
			node_pointer pret = node_pointer();
			if(node_pool_.head == (void*)NULL){
				size_t count = MIN_ALLOC_NODES;
				if(size_ >= count) count = size_;
				freenodes_pointer fndsp = fnds_alloc_.allocate(1);
				if(fndsp == (void*)NULL) return pret;
				fnds_alloc_.construct(fndsp, freenodes());

				node_pointer ndsp = alloc_.allocate(count);
				if(ndsp == (void*)NULL) {
					fnds_alloc_.destroy(fndsp);
					fnds_alloc_.deallocate(fndsp, 1);
					return pret;
				}

				fndsp->nodes	= ndsp;
				fndsp->count	= count;
				fndsp->next		= node_pool_.fns;
				node_pool_.fns	= fndsp;

				for(size_t i = 0; i < count; ++ i){
					node_pointer tmp = ndsp + i;
					alloc_.construct(tmp, node());
					tmp->next = node_pool_.head;
					node_pool_.head = tmp;

				}
			}

			if(node_pool_.head != (void*)NULL){
				pret = node_pool_.head;
				node_pool_.head = node_pool_.head->next;
				pret->reset();
			}
			return pret;
		};
		void put_free_node(node_pointer ndp){
			ndp->reset();
			ndp->next = node_pool_.head;
			node_pool_.head = ndp;
		};
		
	private:
		inside_allocator_type		alloc_;
		freenodes_alloc_type		fnds_alloc_;

		node_pointer		header_, tail_;
		size_type			size_;

		node_pool			node_pool_;
	};

}
}


#endif




