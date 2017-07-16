#ifndef RB_TREE_HPP_201203191651
#define RB_TREE_HPP_201203191651

#include <map>
#include <utility>

namespace sparrow
{
namespace container
{
	template<typename K, 
		typename V, 
		typename LOP = std::less<K>, 
		typename A =  std::allocator< std::pair<K, V> >
	>
	class rb_tree
	{
		enum node_color{
			RED,
			BLACK
		};
	public:
		typedef K		key_type;
		typedef V		value_type;
		typedef std::pair<key_type, value_type>  pair_type;
		typedef LOP		less_op;
		typedef A		allocator_type;

		struct node;

		typedef node	node_type;
		typedef node*	node_pointer;
		typedef typename allocator_type::rebind<node>::other inside_allocator_type;
		typedef typename inside_allocator_type::pointer	node_pointer_type;

		struct node{
			key_type	key;
			value_type	val;
			char		color;
			node_pointer_type	parent, cl, cr;

			node(void) : color(RED),
			parent((node*)NULL), cl((node*)NULL), cr((node*)NULL){}
		};

		typedef rb_tree<K, V, LOP, A> this_type;

		typedef std::pair<node_pointer_type, bool>	result_type;

	public:
		rb_tree(void) : root_((node*)NULL){};

		result_type insert(const pair_type &item){
			node_pointer_type pn;
			if(root_ == (node_pointer)NULL){
				pn = alloc_.allocate(1);
				if(pn == (node_pointer)NULL) return result_type(pn, false);
				alloc_.construct(pn, node_type());
				root_ = pn;
				pn->color = BLACK;
				return result_type(pn, true);
			}

			node_pointer_type pi = root_;
			while(pi != (node_pointer)NULL){
				if(less_op(item.first, pi->key)){
					if(pi->cl == (node_pointer)NULL){
						pn = alloc_.allocate(1);
						if(pn == (node_pointer)NULL) return result_type(pn, false);
						alloc_.construct(pn, node_type());
						pi->cl = pn;
						pn->parent = pi;
						break;
					}
					pi = pi->cl;
				}else if(lessop(pi->key, item.first)){
					if(pi->cr == (node_pointer)NULL){
						pn = alloc_.allocate(1);
						if(pn == (node_pointer)NULL) return result_type(pn, false);
						alloc_.construct(pn, node_type());
						pi->cr = pn;
						pn->parent = pi;
						break;
					}
					pi = pi->cr;
				}
			}
			pi = pn;
			while(pi->parent->color == RED){
				if(pi->parent-parent->cl = pi->parent){
					if(pi->parent->parent->cr->color == RED){
						pi->parent->paran->cr->color = BLACK;
						pi->parent->color = BLACK;
						pi->parent->parent->color = RED;
						pi = pi->parent->parent;
					}else{
						if(pi->parent->cr == pi){
							pi = pi->parent;
							left_revolve(pi);
							pi = pi->cl;
						}else{
							pi = pi->parent->parent;
							right_revolve(pi);
							pi->color     = BLACK;
							pi->cr->color = RED;
							pi = pi->cl;
						}
					}
				}
			}

			return result_type(pn, true);
		};
		
	private:
		void left_revolve(node_pointer_type& p){
			node_pointer_type c_left = p->cl;
			if(c_left == (node_pointer)NULL) return;
			p->cl = c_left->cr;
			p->cl->parent = p;

			c_left->cr = p;
			c_left->parent = p->parent;
			c_left->cr->parent = c_left;
			
			if(c_left->parent == (node_pointer*)NULL){
				root_ = c_left;
			}else{
				if(c_left->parent->cl == p){
					c_left->parent->cl = c_left;
				}else{
					c_left->parent->cr = c_left;
				}
			}
			p = c_left;
		};
		void right_revolve(node_pointer_type& p){
			node_pointer_type c_right = p->cr;
			if(c_right == (node_pointer*)NULL) return;
			p->cr = c_right->cl;
			p->cr->parent = p;

			c_right->cl	= p;
			c_right->parent = p->parent;
			c_right->cl->parent = c_right;

			if(c_right->parent == (node_pointer*)NULL){
				root_ = c_right;
			}else{
				if(c_right->parent->cl == p){
					c_right->parent->cl = c_right;
				}else{
					c_right->parent->cr = c_right;
				}
			}
			p = c_right;
		};


	private:
		node_pointer_type	root_;

		std::size_t			size_;
		
		inside_allocator_type	alloc_;
	};
}
}






#endif

