#include "mx_event.h"

#include <time.h>
pthread_cond_t oCond = PTHREAD_COND_INITIALIZER;

inline CLock::CLock()
{
#ifdef WIN32
#else
	memset(&m_lock, 0, sizeof(m_lock));
	pthread_mutex_init(&m_lock, 0);
#endif
}
inline CLock::~CLock()
{
#ifdef WIN32
#else
	pthread_mutex_destroy(&m_lock);
#endif
}
inline void CLock::Lock()
{
	//DOLOGEX(logTypeDebug, "[debug] %X, lock", &m_lock);

#ifdef WIN32
	m_lock.Lock();
#else
	pthread_mutex_lock(&m_lock);
#endif
}
inline	void CLock::UnLock()
{

#ifdef WIN32
	m_lock.Unlock();
#else
	pthread_mutex_unlock(&m_lock);
#endif
}
inline	bool CLock::TryLock()
{
#ifdef _WIN32
	BOOL b = false;//::TryEnterCriticalSection(&m_lock.m_sec);
	if(b)
		return true;
#else
	int i = pthread_mutex_trylock(&m_lock);
	if(EBUSY != i) 
		return true;
#endif
	return false;
}

CEvent::CEvent(bool bSignaled, bool bAutoReset, bool bBroadcast)
:m_bSingal(false)
,m_bAutoReset(bAutoReset)
,m_bBroadcast(bBroadcast)
{
	pthread_cond_init(&m_oCond, NULL);// = PTHREAD_COND_INITIALIZER;
	pthread_mutex_init(&m_oMutex, NULL); // = PTHREAD_MUTEX_INITIALIZER;

	if(bSignaled)
	{
		SetEvent();
	}
}

CEvent::~CEvent()
{
	pthread_cond_destroy(&m_oCond);
	pthread_mutex_destroy(&m_oMutex);
}

bool CEvent::SetEvent()
{
	pthread_mutex_lock(&m_oMutex);
	m_bSingal = true;
	if (m_bBroadcast)
	{
		pthread_cond_broadcast(&m_oCond);
	}
	else
	{
		pthread_cond_signal(&m_oCond);
	}
	pthread_mutex_unlock(&m_oMutex);

	return true;
}

bool CEvent::ResetEvent()
{
	pthread_mutex_lock(&m_oMutex);
	m_bSingal = false;
	pthread_mutex_unlock(&m_oMutex);

	return true;
}

int CEvent::Wait(unsigned int milliseconds)
{
	struct timespec ts;
	ts.tv_sec = time(NULL) + milliseconds / 1000;
	ts.tv_nsec = (milliseconds % 1000) * 1000000;

	int ret = E_OK;
	pthread_mutex_lock(&m_oMutex);
	while (!m_bSingal)
	{
		ret = pthread_cond_timedwait(&m_oCond, &m_oMutex, &ts);
		if (ETIMEDOUT == ret)
		{
			ret = E_TIMEOUT;
			break;
		}
	}

	if (m_bAutoReset && E_OK == ret)
	{
		m_bSingal = false;
	}
	pthread_mutex_unlock(&m_oMutex);

	return ret;
}
