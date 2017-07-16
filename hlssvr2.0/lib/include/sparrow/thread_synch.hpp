#ifndef THREAD_SYNCH_HPP_201202241101
#define THREAD_SYNCH_HPP_201202241101

#include <pthread.h>
#include "noncopyable.hpp"
#include "locks.hpp"

namespace sparrow
{
	class null_mutex : public noncopyable
	{
	public:
		typedef sparrow::lock<null_mutex>		scoped_lock;
		typedef trylock<null_mutex>				scoped_trylock;
		
		int lock(void){
			return 0;
		};

		int try_lock(void){
			return 0;
		};

		int unlock(void){
			return 0;
		};
	};

	class mutex : public noncopyable
	{
	public:
		typedef sparrow::lock<mutex>		scoped_lock;
		typedef trylock<mutex>				scoped_trylock;

	private:
		pthread_mutex_t		m_t_;

	public:
		mutex(void){
			pthread_mutex_init(&m_t_, NULL);
		};
		~mutex(void){
			pthread_mutex_destroy(&m_t_);
		};

		int lock(void){
			return pthread_mutex_lock(&m_t_);
		};

		int try_lock(void){
			return pthread_mutex_trylock(&m_t_);
		};

		int unlock(void){
			return pthread_mutex_unlock(&m_t_);
		};
	};

	class spin_mutex : public noncopyable
	{
	public:
		typedef sparrow::lock<spin_mutex>		scoped_lock;
		typedef trylock<spin_mutex>				scoped_trylock;

	private:
		pthread_spinlock_t					spin_t_;

	public:
		spin_mutex(void){
			pthread_spin_init(&spin_t_, 0);
		};
		~spin_mutex(void){
			pthread_spin_destroy(&spin_t_);
		};

		int lock(void){
			return pthread_spin_lock(&spin_t_);
		};
		
		int try_lock(void){
			return pthread_spin_trylock(&spin_t_);
		};

		int unlock(void){
			return pthread_spin_unlock(&spin_t_);
		};
	};
}










#endif


