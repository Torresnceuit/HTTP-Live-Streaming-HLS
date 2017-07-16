#ifndef _MX_MUTEX_H
#define _MX_MUTEX_H

#include <assert.h>
#ifdef WIN32
#include <windows.h>


class  CMxMutex 
{ 
	CRITICAL_SECTION cs;
	bool             initialized;
public:
	CMxMutex() 
	{
		InitializeCriticalSection(&cs);
		initialized = true;
	}
	~CMxMutex() 
	{
		DeleteCriticalSection(&cs);
		initialized = false;
	}
	bool isInitialized() 
	{ 
		return initialized;
	}
	void lock()
	{
		if (initialized) 
		{ 
			EnterCriticalSection(&cs);
		}
	}
	void unlock() 
	{
		if (initialized) 
		{ 
			LeaveCriticalSection(&cs);
		}
	}
};

#else
#include <pthread.h>

class CMxMutex 
{ 
	pthread_mutex_t cs;
	bool            initialized;
public:
	CMxMutex() 
	{
		int rc = pthread_mutex_init(&cs, NULL);
		assert(rc == 0);
		initialized = true;
	}
	~CMxMutex() 
	{
		int rc = pthread_mutex_destroy(&cs);
		assert(rc == 0);
		initialized = false;
	}
	bool isInitialized() 
	{ 
		return initialized;
	}
	void lock() 
	{
		if (initialized) 
		{ 
			int rc = pthread_mutex_lock(&cs);
			assert(rc == 0);
		}
	}
	void unlock() 
	{
		if (initialized) 
		{ 
			int rc = pthread_mutex_unlock(&cs);
			assert(rc == 0);
		}
	}
};

#endif

template <class LLLOCK>
class MxGuard
{
public:
	MxGuard ( LLLOCK &m ): lock_( m )
	{
		lock_.lock();
	}

	~MxGuard (void)
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

typedef MxGuard< CMxMutex > MxMutexGuard;

#endif
