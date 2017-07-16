#ifndef NONCOPYABLE_HPP_201208061138
#define NONCOPYABLE_HPP_201208061138

namespace sparrow
{
	class noncopyable
	{
	protected:
		noncopyable() {}
		~noncopyable() {}
	private:  // emphasize the following members are private
		noncopyable( const noncopyable& );
		const noncopyable& operator=( const noncopyable& );
	};
}



#endif

