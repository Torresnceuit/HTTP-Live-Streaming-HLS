#ifndef EXT_TYPE_HPP_201207271827
#define EXT_TYPE_HPP_201207271827

namespace sparrow
{
	template<typename T, size_t N>
	class ext_type
	{
	public:
		ext_type(void){
			new(mem_) T();
		};
		~ext_type(void){
			((T*)mem_)->~T();
		};

		static bool check(void){
			if(sizeof(T) > N) return false;
			return true;
		};

		operator const T&(void) const
		{
			return *((T*)mem_);	
		};
		operator T&(void){
			return *((T*)mem_);
		};

	private:
		char  mem_[N];
	};
}

#endif
