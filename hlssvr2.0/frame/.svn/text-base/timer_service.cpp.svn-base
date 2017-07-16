#include "timer_service.h"

CTimerService::CTimerService()
{
    //LOG(TCLOG_DETAIL, "CTimerService::CTimerService");
}

CTimerService::~CTimerService()
{
    //LOG(TCLOG_DETAIL, "CTimerService::~CTimerService");
}

bool CTimerService::DelTimer(const STimerId& oId)
{
    TTimerMap::iterator it = m_mTimerObject.find(oId);
    if(it != m_mTimerObject.end())
    {
        it->second.bDeleted = true;
        return true;
    }
    else
        return false; 
}

bool CTimerService::AddTimer(const STimerId& oId, bool bAutoDel, int  uiInterval)
{
    STCTimerCtrl oTimerCtrl;

    oTimerCtrl.pTimerBase = oId.Key_Object;
    oTimerCtrl.uiInterval = uiInterval;
    oTimerCtrl.uiStartTime = MyTickCount();
    oTimerCtrl.bDeleted = false;
    oTimerCtrl.bIsAutoDel = bAutoDel;

    m_mTimerObject[oId] = oTimerCtrl;

    return true;
}

bool CTimerService::ResetTimer(const STimerId& oId)
{
    TTimerMap::iterator it = m_mTimerObject.find(oId);
    if(it != m_mTimerObject.end())
    {
        it->second.uiStartTime = MyTickCount();
        it->second.bDeleted = false;

        return true;
    }
    else
        return false;
}

bool CTimerService::SetTimer(const STimerId& oId, int  uiInterval)
{
    TTimerMap::iterator it = m_mTimerObject.find(oId);
    if(it != m_mTimerObject.end())
    {
        it->second.uiStartTime = MyTickCount();
        it->second.uiInterval = uiInterval;
        it->second.bDeleted = false;

        return true;
    }
    else
        return false;
}

bool CTimerService::DelMyTimer(const STimerId& oId)
{
    TTimerMap::iterator it = m_mTimerObject.begin();
    while(it != m_mTimerObject.end())
    {
        STCTimerCtrl * pTimer = &it->second;

        if(pTimer->pTimerBase!=oId.Key_Object)
        {
            ++it;
        }
        else
        {
            m_mTimerObject.erase(it++);
        }
    }

    return true;
}

void CTimerService::DoTimerProcess()
{
    // 删除timer
    TTimerMap::iterator it = m_mTimerObject.begin();
    while(it != m_mTimerObject.end())
    {
        if(it->second.bDeleted)
        {
            m_mTimerObject.erase(it++);
        }
        else
        {
            ++it;
        }
    }

    // 执行到期的timer
    unsigned int uiNow =  MyTickCount();
    for(it = m_mTimerObject.begin(); it != m_mTimerObject.end(); it++)
    {
        //STCTimerCtrl * pTimer = &it->second;
        if(it->second.bDeleted)
            continue;

        if(it->second.uiInterval == 0 )
            continue;

        //表示定时器开始时间 + 间隔时间已经小于当前的时间了，也即定时器超时
        if(uiNow >= it->second.uiStartTime + it->second.uiInterval)
        {
            it->second.pTimerBase->OnTimer(it->first.Key_TimerId);
            if( it->second.bIsAutoDel)
                it->second.bDeleted = true ;
            else
                it->second.uiStartTime = uiNow;
        }
    }
}


