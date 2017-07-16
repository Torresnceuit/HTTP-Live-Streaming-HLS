#include "timer_service.h"
#include "timer_assist.h"

CTimerAssist::CTimerAssist(CTimerService& oTimerService)
:m_oTimerService(oTimerService)
{
}

CTimerAssist::~CTimerAssist()
{
    DelMyTimer();
}

bool CTimerAssist::AddTimer(int uiId, bool bAutoDel, int uiInterval)
{
    return m_oTimerService.AddTimer(STimerId(uiId,this), bAutoDel, uiInterval);
}

bool CTimerAssist::DelTimer(int  uiId)
{
    return m_oTimerService.DelTimer(STimerId(uiId,this));
}


bool CTimerAssist::ResetTimer(int  uiId)
{
    return m_oTimerService.ResetTimer(STimerId(uiId,this));
}

bool CTimerAssist::SetTimer(int  uiId,int  uiInterval)
{
    return m_oTimerService.SetTimer(STimerId(uiId,this),uiInterval);
}

bool CTimerAssist::DelMyTimer()
{
    
    return m_oTimerService.DelMyTimer(STimerId(0,this));
}

