
#ifndef _TFC_EX_HPP_
#define _TFC_EX_HPP_

#include <execinfo.h>
#include <stdexcept>
#include <stdlib.h>
//////////////////////////////////////////////////////////////////////////

namespace tfc
{

	//	class declare: abstract class, add backtrace information
	class bt_ex : public std::exception
	{
	public:
		bt_ex(const std::string& s, bool trace = true) : _err_str(s)
		{
			if (trace)
			{
				void * array[64];
				int nSize = backtrace(array, 64);
				char ** symbols = backtrace_symbols(array, nSize);
				
				for (int i = 0; i < nSize; i++)
				{
					_bt_str += symbols[i];
					_bt_str += "\n";
				}
                free(symbols);
			}
		}
		virtual ~bt_ex() throw() {}
		virtual const char* what() const throw() {return _err_str.c_str();}
		const char* where() const throw() {return _bt_str.c_str();}
		
	protected:
		std::string _bt_str;
		std::string _err_str;
	};
};

//////////////////////////////////////////////////////////////////////////
#endif//_TFC_EX_HPP_
///:~
