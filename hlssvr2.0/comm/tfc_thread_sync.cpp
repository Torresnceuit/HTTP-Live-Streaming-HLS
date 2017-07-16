#include "tfc_thread_sync.h"
#include "tfc_object.h"

CMutex::CMutex(int kind)
{
	pthread_mutexattr_t  attr;
	pthread_mutexattr_init(&attr);
	//attr.__mutexkind = kind;

	pthread_mutex_init(&mutex_, &attr);
}

CMutex::CMutex()
{
	pthread_mutexattr_t  attr;
	pthread_mutexattr_init(&attr);
	//attr.__mutexkind = PTHREAD_MUTEX_TIMED_NP;

	pthread_mutex_init(&mutex_, &attr);
}

CMutex::~CMutex()
{
    pthread_mutex_destroy(&mutex_);
}

int CMutex::lock()
{
    if (pthread_mutex_lock(&mutex_) != 0)
    {
        return MUTEX_ERROR_BASE;        
    }
    return 0;
}

int CMutex::trylock()
{
    if (pthread_mutex_trylock(&mutex_) != 0)
    {
        return MUTEX_ERROR_BASE;        
    }
    return 0;
}

int CMutex::unlock()
{
    if (pthread_mutex_unlock(&mutex_) != 0)
    {
        return MUTEX_ERROR_BASE;        
    }

    return 0;
}

pthread_mutex_t* CMutex::mutex()
{
    return &mutex_;
}


CCond::CCond(CMutex& mutex) : mutex_(mutex), removed_(false)
{
	pthread_cond_init( &cond_ , NULL);
}

CCond::~CCond()
{
    remove();
}
    
int CCond::wait(const struct timespec* abstime)
{
    int ret = 0;
    if (abstime == NULL)
    {
        ret = pthread_cond_wait(&cond_, mutex_.mutex());
        if (ret == EINTR)
        {
            return COND_ERROR_BREAK;
        }
        if (ret != 0)
        {
            return COND_ERROR_BASE;
        }
    }
    else
    {
        int ret = pthread_cond_timedwait(&cond_, mutex_.mutex(), abstime);
        if (ret == ETIMEDOUT || ret == EAGAIN)
        {
            return COND_ERROR_TIMEOUT;        
        }
        else if (ret == EINTR)
        {
            return COND_ERROR_BREAK ;
        }
        else if (ret != 0)
        {
            return COND_ERROR_BASE;        
        }
    }
    return 0;
}

int CCond::signal()
{ 
    if (pthread_cond_signal(&cond_) != 0)
    {
        return COND_ERROR_BASE;        
    }
    return 0;
}

int CCond::broadcast()
{
    if (pthread_cond_broadcast(&cond_) != 0)
    {
        return COND_ERROR_BASE;        
    }
    return 0;
}

int CCond::remove()
{
    int result = 0;
    
    if (this->removed_ == false)
    {
        this->removed_ = true;
    
        while ((result = pthread_cond_destroy(&cond_)) != 0  && errno == EBUSY)
        {
            broadcast ();
            sleep(0);
        }
    }
    if(result != 0 )
    {
        return COND_ERROR_BASE;        
    }
    return 0;
}

pthread_cond_t* CCond::cond()
{
    return &cond_;
}

CMutex& CCond::mutex()
{
    return mutex_;
}


CSysVSem::CSysVSem(key_t key, unsigned size, int id): key_(key), size_(size), id_(id)
{
}

CSysVSem::~CSysVSem()
{
}

CSysVSem* CSysVSem::open(key_t key, size_t size, int& errNo, char* errMsg)
{
	int id = semget(key, size, DEFAULT_SVSEM_MODE);
	if (id == -1)
	{
		if (errno == ENOENT)
		{
            errNo = SYS_V_SEM_ERROR_EXIST;
            strncpy(errMsg, "sys V sem is exist", 255);
		}
		else
		{
            errNo = SYS_V_SEM_ERROR_BASE;
            snprintf(errMsg, 255, "semget fail[%d:%s]",errno, strerror(errno));
		}
        return NULL;
    }

	CSysVSem* sem = new CSysVSem(key, size, id);
	if (sem == NULL)
	{
        errNo = SYS_V_SEM_ERROR_BASE;
        strncpy(errMsg, "new CSysVSem fail", 255);
	}
	return sem;
}

CSysVSem* CSysVSem::create_only(key_t key, size_t size, int& errNo, char* errMsg)
{
	int id = semget(key, size, DEFAULT_SVSEM_MODE | IPC_CREAT | IPC_EXCL);
	if (id == -1)
	{
		if (errno == EEXIST)
		{
            errNo = SYS_V_SEM_ERROR_EXIST;
            strncpy(errMsg, "sys V sem is exist", 255);
		}
		else
		{
            errNo = SYS_V_SEM_ERROR_BASE;
            snprintf(errMsg, 255, "semget fail[%d:%s]",errno, strerror(errno));
		}
        return NULL;
    }

	CSysVSem* sem = new CSysVSem(key, size, id);
	if (sem == NULL)
	{
        errNo = SYS_V_SEM_ERROR_BASE;
        strncpy(errMsg, "new CSysVSem fail", 255);
	}
	return sem;
}


int CSysVSem::is_raw(bool& isRaw)
{
	time_t t;
	if (last_operate_time(t)!=0)
	{
	    return (int)TFC_ERRNO;
	}
	isRaw = (t == 0);
	return 0;
}

int CSysVSem::destroy()
{
	int ret = semctl(id_, 0, IPC_RMID);
	if (ret == -1)
	{
	    ERROR_RETURN_SYS_ERROR(SYS_V_SEM_ERROR_BASE, "fail on remove sem");
	}
	return 0;
}

int CSysVSem::init(unsigned short init_value)
{
	unsigned short* init_array = new unsigned short[size_];
	for(unsigned i = 0; i < size_; i++)
		init_array[i] = init_value;

	int ret = semctl(id_, 0, SETALL, init_array);
	delete [] init_array;
    if (ret)
    {
        ERROR_RETURN_SYS_ERROR(SYS_V_SEM_ERROR_BASE, "fail on init sem");
    }
    return 0;
}

int CSysVSem::set_value(unsigned short init_value, int index)
{
	int ret = semctl(id_, index, SETVAL, init_value);
	if (ret == -1)
	{
	    ERROR_RETURN_SYS_ERROR(SYS_V_SEM_ERROR_BASE, "fail on set sem");
	}
	return 0;
}

int CSysVSem::get_value(unsigned short& value, int index)
{
	int ret = semctl(id_, index, GETVAL);
	if (ret == -1)
	{
	    ERROR_RETURN_SYS_ERROR(SYS_V_SEM_ERROR_BASE, "fail on get sem");
	}
	
	return 0;
}

int CSysVSem::post(int index)
{
    CSysVSemBuf buf(index, 1, SEM_UNDO);
	int ret = semop(id_, &(buf.buf_), 1);
		
    if (ret == -1)
    {
        ERROR_RETURN_SYS_ERROR(SYS_V_SEM_ERROR_BASE, "fail on semop post");
    }

    return 0;
}

int CSysVSem::wait(int index )
{
	static const unsigned C_LOOP_LIMIT = 5;
	for(unsigned i = 0; i < C_LOOP_LIMIT; i++)
	{
        CSysVSemBuf buf(index, -1, SEM_UNDO);
		int ret = semop(id_, &(buf.buf_), 1);
		if (ret == -1)
		{
			if (errno != EINTR)
			{
                ERROR_RETURN_SYS_ERROR(SYS_V_SEM_ERROR_BASE, "fail on semop wait");
			}
		}
		else
		{
			return 0;
		}
	}
    ERROR_RETURN_SYS_ERROR(SYS_V_SEM_ERROR_BASE, "fail on semop wait");
}

int CSysVSem::trywait(int index )
{
    CSysVSemBuf buf(index, -1, SEM_UNDO | IPC_NOWAIT);
	int ret = semop(id_, &(buf.buf_), 1);
	if (ret == -1)
	{
		if (errno != EAGAIN)
		{
            ERROR_RETURN(SYS_V_SEM_ERROR_TIMEOUT, "semop trywait should be again");
        }
		else
		{
            ERROR_RETURN_SYS_ERROR(SYS_V_SEM_ERROR_BASE, "fail on semop trywait");
		}
	}
	return 0;
}

int CSysVSem::timewait(int index, int& sec, int& nanosec)
{//!!!!!!!!!!!!!!!!!!!semtimedop问题，以后打开
/*
	struct timespec ts;
	ts.tv_sec = sec;
	ts.tv_nsec = nanosec;
	
	static const unsigned C_LOOP_LIMIT = 5;
	for(unsigned i = 0; i < C_LOOP_LIMIT; i++)
	{
        CSysVSemBuf buf(index, -1, SEM_UNDO);
		int ret = semtimedop(id_, &(buf.buf_), 1, &ts);
		if (ret == -1)
		{
			if (errno == EAGAIN)
			{
                ERROR_RETURN(SYS_V_SEM_ERROR_TIMEOUT, "semop timewait timeout");
			}
			else if (errno == EINTR)
			{
				continue;
			}
			else
			{
                ERROR_RETURN_SYS_ERROR(SYS_V_SEM_ERROR_BASE, "fail on semop timewait");
			}
		}
		return 0;
	}*/
    ERROR_RETURN_SYS_ERROR(SYS_V_SEM_ERROR_BASE, "fail on semop timewait");
}

int CSysVSem::last_operate_time(time_t & lot)
{
	struct semid_ds buf;
	int ret = semctl(id_, 0, IPC_STAT, &buf);
	if (ret < 0)
	{
	    ERROR_RETURN_SYS_ERROR(SYS_V_SEM_ERROR_BASE, "semctl fail");
	}
	return buf.sem_otime;
}

int CSysVSem::create_time(time_t & ct)
{
	struct semid_ds buf;
	int ret = semctl(id_, 0, IPC_STAT, &buf);
	if (ret < 0)
	{
	    ERROR_RETURN_SYS_ERROR(SYS_V_SEM_ERROR_BASE, "semctl fail");
	}
	return buf.sem_ctime;
}




