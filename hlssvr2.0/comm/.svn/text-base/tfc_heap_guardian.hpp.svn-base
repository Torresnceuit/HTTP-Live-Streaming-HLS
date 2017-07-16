
#ifndef _TFC_PTR_HPP_
#define _TFC_PTR_HPP_

#include <assert.h>
#include "tfc_ex.hpp"

//////////////////////////////////////////////////////////////////////////
//	template class: intelligent pointer;
//	protect class which is allocated on heap;
//	with reference count and auto release due to owner count
//	array flag added
//////////////////////////////////////////////////////////////////////////

namespace tfc{


template <typename T> class ptr
{
	public:
		typedef T true_type;	//	nested type, for template method
		
		ptr();
		ptr(T* t, bool array_flag = false);
		~ptr();
		ptr(const ptr<T>& right);
		ptr<T>& operator=(const ptr<T>& right);
		bool operator==(const ptr<T>& right) const;
		
		T& operator *();
		T* operator->();
		T* operator()();
		template<class T1> ptr<T1> DyCast();	//	T::~T virtual destructor asserted

		bool IsNil() const;
		void Release();
		
		T* _p_t;
		unsigned * _p_ref_count;
		bool* _p_array_flag;
};

class ptr_ex:public bt_ex{public:ptr_ex(const std::string& s):bt_ex(s){}};

//////////////////////////////////////////////////////////////////////////
//	implementations
//////////////////////////////////////////////////////////////////////////

template<typename T> ptr<T>::ptr()
: _p_t(NULL), _p_ref_count (NULL), _p_array_flag(NULL)
{}

template<typename T> ptr<T>::ptr(T* t, bool array_flag)
:_p_t(NULL), _p_ref_count(NULL), _p_array_flag(NULL)
{
	_p_ref_count = new unsigned(1);
	_p_array_flag = new bool(array_flag);
	_p_t = t;
	//	assert(t);
}

template<typename T> ptr<T>::ptr(const ptr<T>& right)
:_p_t(NULL), _p_ref_count(NULL), _p_array_flag(NULL)
{
	if (right._p_ref_count != NULL)
	{
		assert(*(right._p_ref_count) > 0);
		_p_ref_count = right._p_ref_count;
		_p_array_flag = right._p_array_flag;
		_p_t = right._p_t;
		(*_p_ref_count)++;
	}
	else
		assert(right._p_array_flag == NULL && right._p_t == NULL);
}

template<typename T>
ptr<T>& ptr<T>::operator=(const ptr<T>& right)
{
	//	first, if equal now, return directly
	if (_p_t != right._p_t)
	{
		Release();
		_p_ref_count = right._p_ref_count;
		_p_array_flag = right._p_array_flag;
		_p_t = right._p_t;
		(*_p_ref_count)++;
	}
	else
		assert(_p_array_flag == right._p_array_flag && _p_ref_count == right._p_ref_count);
	return *this;
}

template<typename T> ptr<T>::~ptr(){Release();}

template<typename T> bool ptr<T>::operator==
(const ptr<T>& right) const {return (_p_t == right._p_t);}

//////////////////////////////////////////////////////////////////////////

template<typename T> T& ptr<T>::operator *()
{
	if (!_p_ref_count) throw ptr_ex("null pointer member referred");
	return *_p_t;
}

template<typename T> T* ptr<T>::operator->()
{
	if (!_p_ref_count) throw ptr_ex("null pointer member referred");
	return _p_t;
}

template<typename T> T* ptr<T>::operator()()
{
	if (!_p_ref_count) throw ptr_ex("null pointer member referred");
	return _p_t;
}

template<typename T> bool ptr<T>::IsNil()const{return !_p_ref_count;}

//////////////////////////////////////////////////////////////////////////

template<typename T> void ptr<T>::Release()
{
	if (_p_ref_count)
	{
		if (*_p_ref_count == 1)
		{
			delete _p_ref_count;
			_p_ref_count = NULL;
			
			if (*_p_array_flag && _p_t)
				delete [] _p_t;
			else if (_p_t)
				delete _p_t;
			
			_p_t = NULL;
			delete _p_array_flag;
			_p_array_flag = NULL;
		}
		else
		{
			(*_p_ref_count)--;
			_p_ref_count = NULL;
			_p_t = NULL;
		}
	}
}

template<typename T> template<class T2> ptr<T2> ptr<T>::DyCast()
{
	if (!_p_ref_count)
		throw ptr_ex("dynamic cast null pointer");
	
	T2* t2 = dynamic_cast<T2*>(_p_t);
	if (!t2)
		throw ptr_ex("dynamic cast failed");
	
	ptr<T2> tRet;
	tRet._p_ref_count = _p_ref_count;
	tRet._p_array_flag = _p_array_flag;
	tRet._p_t = t2;
	(*_p_ref_count)++;
	return tRet;
}

};	//	namespace tfc;

//////////////////////////////////////////////////////////////////////////
#endif//_TFC_PTR_HPP_
///:~
