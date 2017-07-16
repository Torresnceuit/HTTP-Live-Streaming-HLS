
#ifndef _TIMER_ASSIST_H
#define _TIMER_ASSIST_H

#include "timer_base.h"

class CTimerService;

class CTimerAssist 
: public CTimerBase
{
public:
    CTimerService& m_oTimerService;

    CTimerAssist(CTimerService& oTimerService);

    ~CTimerAssist();
    
protected:
    bool DelTimer(int  uiId);

    bool AddTimer(int  uiId, bool bAutoDel,int  uiInterval = 0);

    bool ResetTimer(int  uiId);

    bool SetTimer(int  uiId, int  uiInterval = 0);

    bool DelMyTimer();
};

#endif 
