#ifndef DETAIL_HPP_201203081558
#define DETAIL_HPP_201203081558

namespace sparrow
{
	template<typename T> struct type2type{};
	
	template<int tag> struct int2type{};

	template<typename T>
	class construct_proxy
	{
	public:
		typedef construct_proxy<T> this_type;
	
	public:
		construct_proxy(T *p) : ptr_(p){};
		construct_proxy(const this_type &rhs) : ptr_(rhs.ptr_){};

		this_type& operator=(const this_type &rhs){
			if(this == &rhs) return *this;
			ptr_	= rhs.ptr_;
			return *this;
		};

		T* operator()(void){
			new ((void*)ptr_) T();
			return ptr_;
		};

		template<typename Arg>
		T* operator()(Arg &arg){
			new ((void*)ptr_) T(arg);
			return ptr_;
		};

		template<typename Arg1, typename Arg2>
		T* operator()(Arg1 &arg1, Arg2 &arg2){
			new ((void*)ptr_) T(arg1, arg2);
			return ptr_;
		};

		template<typename Arg1, typename Arg2, typename Arg3>
		T* operator()(Arg1 &arg1, Arg2 &arg2, Arg3 &arg3){
			new ((void*)ptr_) T(arg1, arg2, arg3);
			return ptr_;
		};


	private:
		T		*ptr_;
	};
}









#endif

