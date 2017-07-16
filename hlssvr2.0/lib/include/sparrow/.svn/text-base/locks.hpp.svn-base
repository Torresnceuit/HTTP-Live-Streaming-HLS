#ifndef LOCKS_HPP_201204171507
#define LOCKS_HPP_201204171507

#include "noncopyable.hpp"

namespace sparrow
{
	template<typename T>
	class lock : public noncopyable
	{
	public:
		explicit lock(T &l) : l_(&l){
			l_->lock();
		};
		~lock(void){
			l_->unlock();
		};

	private:
		T	*l_;
	};

	template<typename T>
	class trylock : public noncopyable
	{
	public:
		explicit trylock(T &l) : l_(&l), isowner_(false){
			try_lock();
		};
		~trylock(void){
			if(isowner_) l_->unlock();
		};

		bool try_lock(void){
			if(isowner_) return true;
			if(0 == l_->try_lock()){
				isowner_ = true;
			}
			return isowner_;
		};

		bool isowner(void) const {return isowner_;};

	private:
		T		*l_;
		bool	isowner_;
	};

	template<typename T>
	class read_lock : public noncopyable
	{
	public:
		explicit read_lock(T &l) : l_(&l){
			l_->read_lock();
		};
		~read_lock(void){
			l_->read_unlock();
		};

	private:
		T	*l_;
	};

	template<typename T>
	class read_trylock : public noncopyable
	{
	public:
		explicit read_trylock(T &l) : l_(&l), isowner_(false){
			try_lock();
		};
		~read_trylock(void){
			if(isowner_) l_->read_unlock();
		};

		bool try_lock(void){
			if(isowner_) return true;
			if(0 == l_->try_read_lock()){
				isowner_ = true;
			}
			return isowner_;
		};

		bool isowner(void) const {return isowner_;};

	private:
		T		*l_;
		bool	isowner_;
	};
	
	template<typename T>
	class write_lock : public noncopyable
	{
	public:
		explicit write_lock(T &l) : l_(&l){
			l_->write_lock();
		};
		~write_lock(void){
			l_->write_unlock();
		};

	private:
		T	*l_;
	};

	template<typename T>
	class write_trylock : public noncopyable
	{
	public:
		explicit write_trylock(T &l) : l_(&l), isowner_(false){
			try_lock();
		};
		~write_trylock(void){
			if(isowner_) l_->write_unlock();
		};

		bool try_lock(void){
			if(isowner_) return true;
			if(0 == l_->try_write_lock()){
				isowner_ = true;
			}
			return isowner_;
		};

		bool isowner(void) const {return isowner_;};

	private:
		T		*l_;
		bool	isowner_;
	};
}

#endif
