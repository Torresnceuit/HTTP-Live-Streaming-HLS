
#ifndef _TFC_STR_HPP_
#define _TFC_STR_HPP_

#include <string>
#include <sstream>
#include <sys/time.h>
#include <stdlib.h>

namespace tfc{

//////////////////////////////////////////////////////////////////////////

template<typename T> std::string to_str(const T& t)
{
	std::ostringstream s;
	s << t;
	return s.str();
}

template<typename T> T from_str(const std::string& s)
{
	std::istringstream is(s);
	T t;
	is >> t;
	return t;
}

inline void sran()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	srand(tv.tv_usec);
}

inline unsigned int ran(unsigned range)
{
	unsigned r = rand() | (rand() << 16);
	r = r%range;
	//r = (unsigned) ( (0.0+range) * (r+0.0) / (ULONG_MAX+1.0));
	//cerr << "ran " << r << endl;
	return r;
}

//
//template<typename T> std::string operator+(const std::string& left, T& right)
//{
//	return left + to_str(right);
//}
//
//template<typename T> std::string operator+(T& left, const std::string& right)
//{
//	return to_str(left) + right;
//}

//////////////////////////////////////////////////////////////////////////
}	//	namespace tfc
#endif//_TFC_STR_HPP_
///:~
