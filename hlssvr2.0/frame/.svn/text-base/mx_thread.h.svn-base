#ifndef _THREAD_H
#define _THREAD_H

#include <string>
#include "mx_event.h"

#ifndef PTHREAD_STACK_MIN
#define PTHREAD_STACK_MIN	1024 * 1024
#endif

namespace TC
{
	void sleep(unsigned int millisec);
	void usleep_r(unsigned int usec);
}

#ifdef _WIN32
    typedef HANDLE MX_HANDLE;
#else
    typedef pthread_t MX_HANDLE;
#endif

MX_HANDLE CreateThread(void *(*__start_routine) (void *),  void* pParam, unsigned int uiStackSize = PTHREAD_STACK_MIN);//, int iPrio = PRIORITY_NORMAL);

class CThread : public CNonCopyable
{
public:

    enum TThreadStatus 
    {    
        STATUS_INIT = 0,     
        STATUS_RUNNING,  
        STATUS_WAITING_STOP,
        STATUS_STOPPED
    };

    CThread(const std::string& threadName = "");
    virtual ~CThread();

    const std::string& threadName() const { return m_threadName;}

    inline bool IsThreadWaitingStop() const { return (STATUS_WAITING_STOP == m_oStatus)||(STATUS_STOPPED == m_oStatus) ; }
    bool IsThreadStopped() const { return STATUS_STOPPED == m_oStatus; }
    TThreadStatus GetThreadStatus() const { return m_oStatus; }

    int Run();
    int Stop(unsigned int milliseconds = 500);

protected:
    virtual bool Process() = 0; 
    virtual void OnThreadStop(){};

private:
    std::string     m_threadName;
    MX_HANDLE       m_oHandle;
    volatile TThreadStatus   m_oStatus;    

    CEvent			m_oStopEvent;

private:
    void NotifyStop();

private:
    friend void* StartRoutine(void* param);
    static void* StartRoutine(void* param);
};


#endif
