#ifndef _TIMER_BASE_H
#define _TIMER_BASE_H

class CTimerBase; 

struct STimerId 
{
    
    CTimerBase*     Key_Object;
    
    unsigned int    Key_TimerId;    
    STimerId():Key_Object(0),Key_TimerId(0){};    
    STimerId(unsigned int uiId, CTimerBase*  pTimerBase)
    {
        Key_Object = pTimerBase;
        Key_TimerId = uiId;
    };
    
    bool operator < (const STimerId& rsh) const
    {
        if(Key_Object == rsh.Key_Object)
            return Key_TimerId < rsh.Key_TimerId;
        return Key_Object < rsh.Key_Object;
    }
};

class CTimerBase 
{
public:
    virtual void OnTimer(unsigned int uiId);
	virtual ~CTimerBase();
};


#endif 
