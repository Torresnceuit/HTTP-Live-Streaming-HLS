#ifndef _TIMER_SERVICE_H
#define _TIMER_SERVICE_H

#include "timer_base.h"
#include <sys/time.h>
#include <time.h>
#include <map>
inline unsigned int MyTickCount()
{
#ifdef _WIN32
	return GetTickCount();
#else
	struct timeval curTime;
	gettimeofday(&curTime, NULL);
	return curTime.tv_sec*1000 + curTime.tv_usec/1000;
#endif
}

inline void MySleep(unsigned int millSec)
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

class CTimerService 
{
public:
    struct STCTimerCtrl 
    {
        CTimerBase* pTimerBase;
        unsigned int     uiStartTime;
        unsigned int     uiInterval;
        bool        bIsAutoDel;
        bool        bDeleted;
    };

public:
    CTimerService();
    ~CTimerService();

    bool DelTimer(const STimerId& oId);

    bool AddTimer(const STimerId& oId, bool bAutoDel, int  uiInterval = 0);

    bool ResetTimer(const STimerId& oId);

    bool SetTimer(const STimerId& oId, int  uiInterval = 0);

    bool DelMyTimer(const STimerId& oId);

    void DoTimerProcess();

    unsigned int GetTimerNum()
    {
        return m_mTimerObject.size();
    }
private:
    typedef std::map <STimerId , STCTimerCtrl> TTimerMap;
    TTimerMap   m_mTimerObject;
};

#endif 
