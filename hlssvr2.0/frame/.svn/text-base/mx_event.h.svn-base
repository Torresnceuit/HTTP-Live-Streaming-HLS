#ifndef _MX_EVENT_H
#define _MX_EVENT_H

#include <string.h>
#include <pthread.h>
#include <sys/timeb.h>
#include <stdio.h>
#include <errno.h>
#include "noncopyable.h"

#ifndef WAIT_SLEEP_TIME
#define WAIT_SLEEP_TIME			200  //为了等待信号，而默认的休眠时间 耗秒
#endif

#ifndef INFINITE
#define INFINITE				0xFFFFFFFF
#endif

#define EBUSY 16
class CLock
{
private:
#ifdef WIN32
	CComAutoCriticalSection m_lock;
#else
	pthread_mutex_t m_lock; 
#endif

public:
	CLock();
	~CLock();
	void Lock();
	void UnLock();
	void Unlock(){return UnLock();}
	bool TryLock();
};

class CElapseTime
{
public:
	struct timeb m_begin;
	struct timeb m_end;
public:
	CElapseTime(const struct timeb* pBegin = 0)
	{
		if(pBegin == 0)
		{
			ftime(&m_begin);
		}
		else
		{
			m_begin = *pBegin;
		}
	}
	double GetElapse(bool bClear = false)
	{
		ftime(&m_end);
		double sec = difftime(m_end.time, m_begin.time);
		sec = sec + double(m_end.millitm - m_begin.millitm) / 1000;
		if(bClear)
		{
			m_begin = m_end;
		}
		return sec;
	}
	double GetElapse(timeb& tCur, timeb& tOld)
	{
		double sec = difftime(tCur.time, tOld.time);
		sec = sec + double(tCur.millitm - tOld.millitm) / 1000;
		return sec;
	}
};

inline void MxSleep(unsigned int millSec)
{
#ifdef _WIN32
	Sleep(millSec);
#else
	int sec = millSec /1000;
	int ms = millSec % 1000;
	struct timespec req;
	req.tv_sec = sec;
	req.tv_nsec = ms * 1000;
	nanosleep(&req, 0);
#endif
}

class CEvent:public CNonCopyable
{
public:
	enum
	{
		E_OK = 0,
		E_TIMEOUT = ETIMEDOUT,
	};

	CEvent(bool bSignaled = false, bool bAutoReset = true, bool bBroadcast = false);
	~CEvent();

	bool SetEvent();
	bool ResetEvent();
	int Wait(unsigned milliseconds);

private:
	pthread_cond_t  m_oCond;
	pthread_mutex_t m_oMutex;

	bool m_bSingal;
	bool m_bAutoReset;
	bool m_bBroadcast;
};



#endif
