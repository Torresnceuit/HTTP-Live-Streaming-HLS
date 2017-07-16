#ifndef AYN_TYPE_OF_HPP_201111171809
#define AYN_TYPE_OF_HPP_201111171809

#include "type_lists.hpp"

namespace sparrow
{
	template<typename tlist> 
	class any_type_of
	{
		enum{
			MAX_T_SIZE	= max_size<tlist>::result
		};
	public:
		typedef any_type_of<tlist>	this_type;
	public:
		any_type_of(void) : id_(-1){memset(data_, 0, MAX_T_SIZE);};
		any_type_of(const this_type &rhs) : id_(0) {memcpy(data_, rhs.data_, MAX_T_SIZE);}

		this_type& operator=(const this_type &rhs){
			if(this == &rhs) return *this;
			id_		= rhs.id_;
			memcpy(data_, rhs.data_, MAX_T_SIZE);
			return *this;
		};

		template<typename T>
		T& val(void){
			typedef has_type<tlist, T> _hastype;
			typedef typename _hastype::result _dsttype;
			int id = _hastype::value;
			if(id_ != id){
				id_ = id;
				new (data_) _dsttype();
			}
			return *((_dsttype*)data_);
		};

		template<typename T>
		const T& val(void) const{
			typedef has_type<tlist, T> _hastype;
			typedef typename _hastype::result _dsttype;
			int id = _hastype::value;
			if(id_ != id){
				id_ = id;
				new (data_) _dsttype();
			}
			return *((_dsttype*)data_);
		};


	private:
		mutable unsigned char		 data_[MAX_T_SIZE];
		mutable int					 id_;
	};
}

#endif



