#include "mx_thread.h"
#include <sys/time.h>
#include <sys/select.h>
#include <cassert>

void TC::sleep(unsigned int millisec)
{
	if(0 == millisec)
		return;

	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = millisec;
	select(0,0,0,0,&tv);  
}

void TC::usleep_r(unsigned int usec)
{
	//usleep的线程安全版本
	struct timespec rqtp;
	struct timespec rem;
	memset(&rem,0,sizeof(rem));
	rqtp.tv_sec = usec/1000000;
	rqtp.tv_nsec = usec%1000000*1000;
	nanosleep(&rqtp, &rem);
	return;
}

inline MX_HANDLE CreateThread(void *(*__start_routine) (void *),  void* pParam, unsigned int uiStackSize)
{
#ifdef _WIN32
	DWORD dwID = 0;
	HANDLE h = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)__start_routine, pParam, 0, &dwID);
	if(0 == h)
	{
		return 0;
	}
	return h;
#else
	pthread_t th = 0;
	if(uiStackSize == 0)
	{
		pthread_create(&th, 0, __start_routine, pParam);
	}
	else
	{
		//int  pthread_attr_init ( pthread_attr_t *attr )
		//   attr->detachstate = PTHREAD_CREATE_JOINABLE; /* 线程与其他线程关联*/
		//   attr->priority    = PRIORITY_NORMAL;        /* 线程的优先级   */
		//   attr->stacksize   = PTHREAD_STACK_MIN;      /* 线程所占堆栈的 */
		//   attr->guardsize   = DEFAULT_STACKGUARD;     /* 警戒堆栈的大小 */ 
		//   attr->stackaddr   = 0;                      /* 堆栈的大小     */ 
		//attr->policy     = SCHED_RR;                /* 线程调度类型   */ 
		//attr->inherit    = PTHREAD_EXPLICIT_SCHED;  /* 线程继承类型   */ 

		pthread_attr_t attr;
		pthread_attr_init(&attr);
		size_t oldSize = 0;

		if(pthread_attr_setstacksize (&attr, uiStackSize) != 0)
		{
			return 0;
		}
		pthread_attr_getstacksize(&attr, &oldSize);

		int iError = pthread_create(&th, &attr, __start_routine, pParam);
		if(iError != 0)
		{
			return 0;
		}
	}
	return (int)th;
#endif

}

CThread::CThread(const std::string& threadName)
:m_threadName(threadName)
,m_oStatus(STATUS_INIT)
{
}

CThread::~CThread()
{
}

int CThread::Run()
{    

	m_oStatus = STATUS_RUNNING;
	int ret = pthread_create(&m_oHandle, NULL, CThread::StartRoutine, this);
	if (0 != ret)
	{
		m_oStatus = STATUS_INIT;
	}

	return ret;
}


int CThread::Stop(unsigned int milliseconds)
{
    if (STATUS_RUNNING != m_oStatus)
        return 0;

    m_oStatus = STATUS_WAITING_STOP;

	if (CEvent::E_OK != m_oStopEvent.Wait(milliseconds))
    {
        //LOG(MODEL_THREAD).warn("stop() wait stopEvent timeout! thread name: %s", m_threadName.c_str());
    }
    else
    {
        #ifdef _WIN32
            WaitForSingleObject(m_oHandle, INFINITE);
            CloseHandle(m_oHandle);
            m_oHandle = NULL;
        #else
            pthread_join(m_oHandle, NULL);
        #endif

    }

    m_oStatus = STATUS_STOPPED;

    return 0;
}

void CThread::NotifyStop()
{
    m_oStopEvent.SetEvent();
    OnThreadStop();
}

void* CThread::StartRoutine(void* param)
{
    assert(NULL != param);
    CThread* pThread = (CThread*)param;

    try
    {
        pThread->Process();
    }
    catch (...)
    {
        //    	LOG(MODEL_THREAD).error("StartRoutine() error! catch some exception, threadName = %s", pThread->m_threadName.c_str());
        assert(false);
    }

    pThread->NotifyStop();

    return NULL;
}
