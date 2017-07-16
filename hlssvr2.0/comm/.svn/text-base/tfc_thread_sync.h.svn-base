
#ifndef _THREAD_SYNCH_H_
#define _THREAD_SYNCH_H_

#include "tfc_comm.h" 

class CMutex 
{
public:
    enum MUTEX_ERROR
    {
        MUTEX_ERROR_BASE = -1,    
    };

public:
	CMutex(int kind);
    CMutex();
	~CMutex();
	int lock();
    int trylock();
	int unlock();

    pthread_mutex_t* mutex(); 

protected:
	pthread_mutex_t mutex_;
};

template <class LLLOCK>
class Guard
{
public:
	Guard ( LLLOCK &m , bool block = true ): lock_( m )
	{
		result_ = block ? lock_.lock() : lock_.trylock();
	}

	~Guard (void)
	{
		lock_.unlock();
	}

	bool locked( )
	{
		return (result_ == 0);
	}

private:
	LLLOCK &lock_;

	// Tracks if acquired the lock or failed.
	int result_;
};

typedef Guard< CMutex > MutexGuard;


class CCond 
{
public:
    enum COND_ERROR
    {
        COND_ERROR_BASE = -1,   
        COND_ERROR_TIMEOUT = COND_ERROR_BASE -1,   //操作超时 
        COND_ERROR_BREAK = COND_ERROR_BASE -2,   //操作被中断
    };

public:
    CCond(CMutex& mutex);
    ~CCond();

    int remove();
    int wait(const struct timespec* abstime=NULL);
    int signal() ;
    int broadcast();

    CMutex& mutex();
    pthread_cond_t* cond();
    
protected:
    pthread_cond_t cond_;
    CMutex& mutex_;
    bool removed_;
    
};

#ifndef SEM_R
#define SEM_R (0400)
#endif
#ifndef SEM_A
#define SEM_A (0200)
#endif

#define DEFAULT_SVSEM_MODE (SEM_R | SEM_A | SEM_R>>3 | SEM_R>>6)

class CSysVSem
{
public:
    enum SYS_V_SEM_ERROR
    {
        SYS_V_SEM_ERROR_BASE = -1,   
        SYS_V_SEM_ERROR_TIMEOUT = SYS_V_SEM_ERROR_BASE -1,   //操作超时 
        SYS_V_SEM_ERROR_EXIST = SYS_V_SEM_ERROR_BASE -2,   //资源已经存在
        SYS_V_SEM_ERROR_NOT_EXIST = SYS_V_SEM_ERROR_BASE -3,   //资源不存在
    };

public:
	static CSysVSem* open(key_t key, size_t size, int& errNo, char* errMsg);
	static CSysVSem* create_only(key_t key, size_t size, int& errNo, char* errMsg);

	~CSysVSem();

	//	semctl + IPC_STAT
	int is_raw(bool& isRaw);
	//	semctl + SETALL
	int init(unsigned short init_value = 0);
	//	semctl + SETVAL
	int set_value(unsigned short init_value = 0, int index = 0);
	//	semctl + GETVAL
	int	 get_value(unsigned short & value, int index = 0);
	//	semctl + IPC_RMID
	int destroy();

	int		id() const{return id_ ;}
	key_t  key() const{return key_;}
	size_t size()const{return size_;}

	//	operation
	int post(int index = 0);
	int wait(int index = 0);
	int trywait(int index = 0);
	int timewait(int index, int& sec, int& nanosec);

protected:
	CSysVSem(key_t key, unsigned n, int id);

	struct CSysVSemBuf
	{
    	CSysVSemBuf(unsigned short sem_num, short sem_op, short sem_flg)
        {
            buf_.sem_num = sem_num;
            buf_.sem_op = sem_op;
            buf_.sem_flg = sem_flg;
        }
		struct sembuf buf_;
	};

	//	semctl + IPC_STAT
	int last_operate_time(time_t& lot);
	int create_time(time_t& ct);
	
	const key_t key_;
	const unsigned size_;
	const int id_;
};


#endif  //_THREAD_SYNCH_H_

