#ifndef INTERPROCESS_SYNCH_HPP_201204171507
#define INTERPROCESS_SYNCH_HPP_201204171507

#include "locks.hpp"
#include "noncopyable.hpp"
#include <stdint.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

namespace sparrow
{
	class ipc_null_mutex
	{
	public:
		typedef sparrow::lock<ipc_null_mutex>		scoped_lock;
		typedef trylock<ipc_null_mutex>				scoped_trylock;
		
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

	class ipc_mutex : public noncopyable
	{
	public:
		typedef sparrow::lock<ipc_mutex>		scoped_lock;
		typedef trylock<ipc_mutex>				scoped_trylock;

	private:
		unsigned char	m_t_;
		pid_t			pid_;

	public:
		ipc_mutex(void) : m_t_(0), pid_(0){
		};
		~ipc_mutex(void){
		};

		int lock(void){
			while(try_lock() != 0){};
			return 0;
		};

		int try_lock(void);

		int unlock(void){
			if(m_t_ != 1) return -1;
			m_t_ = 0;
			return -1;
		};
	};

	class ipc_rw_mutex : public noncopyable
	{
		enum{
			PID_COUNT = 211,
			PID_COUNT_1 = 179
		};

		struct pid_i
		{
			pid_t pid;
			bool  is_r;

			pid_i(void) : 
			pid(0), is_r(false)
			{};
		};
	public:
		typedef sparrow::read_lock<ipc_rw_mutex> scoped_readlock;
		typedef read_trylock<ipc_rw_mutex>		 scoped_try_readlock;
		typedef sparrow::write_lock<ipc_rw_mutex> scoped_writelock;
		typedef write_trylock<ipc_rw_mutex>		  scoped_try_writelock;

	public:
		ipc_rw_mutex(void);

		int read_lock(void);
		int try_read_lock(void);
		int read_unlock(void);

		int write_lock(void);
		int try_write_lock(void);
		int write_unlock(void);

	private:
		bool set_rpid(const pid_t pid);
		bool unset_rpid(const pid_t pid);
		bool del_rpid(const pid_t pid);

	private:
		pid_i	rpids_[PID_COUNT];
		pid_t	wpid_;
		int		stat_;
	};

	/***********************************************************************
	进程间通讯条件变量
	只能两个进程间使用
	只能在主线程中使用
	/************************************************************************/
	class ipc_condition : public noncopyable
	{
	public:
		ipc_condition(void) : 
		wait_pid_(0)
		{
		};

		/************************************************************************/
		/* tout 超时时间(ms) =0 立即返回  >0 超时毫秒 <0 不超时                                                                    */
		/************************************************************************/
		int wait(const int32_t tout = 0);

		void notify(void);

	private:

		static void set_sig(void);
		static void sigusr1_hdl(int n){};

	private:
		pid_t		wait_pid_;
	};
		
}


#endif
