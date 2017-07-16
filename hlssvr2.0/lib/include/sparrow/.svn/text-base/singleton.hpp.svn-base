#ifndef SINGLETON_HPP_201107151805
#define SINGLETON_HPP_201107151805

#include <deque>
#include "thread_synch.hpp"

namespace sparrow{

	class singleton_clearup
	{
	public:
		singleton_clearup(void){};
		virtual ~singleton_clearup(void){};
	};

	class singleton_mgr
	{
		singleton_mgr(void){};
		~singleton_mgr(void){};
	public:
		static void init(void);

		static void push_singleton(singleton_clearup *sgt);
		static void remove_singleton(singleton_clearup *sgt);

	public:
		static void finit(void);

	private:
		typedef std::deque<singleton_clearup*>		_deq_clearup;

	private:
		void remove(singleton_clearup *sgt);
		void clearup(void);

	private:
		_deq_clearup	deq_clearup_;

	private:
		static singleton_mgr	*instance_;
		static mutex		lock_;
	};


	template<typename T>
	class singleton : public singleton_clearup
	{
		typedef singleton<T> this_type;
	private:
		singleton(void){};
	public:
		virtual ~singleton(void){};

		static T* instance(void){
			this_type*& ptr = instance_i();
			return &(ptr->obj_);
		};
		static void clearup(void){
			this_type*& ptr = instance_i();
			singleton_mgr::remove_singleton(ptr);
			delete ptr;
			ptr = NULL;
		};

	private:
		static this_type*& instance_i(void){
			static this_type *ptr = NULL;
			static mutex lock;

			if(NULL == ptr){
				mutex::scoped_lock guard(lock);
				if(NULL == ptr){
					singleton_mgr::init();
					ptr = new this_type();
					singleton_mgr::push_singleton(ptr);
				}
			}
			return ptr;
		};

	private:
		T		obj_;
	};

}

#endif

