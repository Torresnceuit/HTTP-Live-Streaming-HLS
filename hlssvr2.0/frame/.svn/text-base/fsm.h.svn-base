 #include <map>

template <typename TClass, typename TStatus, typename TEvent, typename TParam1, typename TParam2>
class CTCFsm  
{
public:
    typedef bool (TClass::*FUNC)(TParam1 oParam1,TParam2 oParam2);
    typedef bool (TClass::*AFUNC)(TParam1 oParam1,TParam2 oParam2);    

    typedef TStatus EStatus;
    typedef TEvent  EEvent;

private:    
    struct  STCFsmKey
    {
        EStatus    eFromStatus;
        EEvent     eEvent;
        
        STCFsmKey():eFromStatus(0),EEvent(0){};
        
        STCFsmKey(EStatus  eFromStatus_p,EEvent  eEvent_p)
        {
            eFromStatus = eFromStatus_p;
            eEvent = eEvent_p;
        };
        
        bool operator < (const STCFsmKey& rsh) const
        {
            if(eFromStatus != rsh.eFromStatus)
                return eFromStatus < rsh.eFromStatus;
            return eEvent < rsh.eEvent;
        }
    }; 
    
    struct  STCFsmAction
    {
        EStatus      eToStatus;
        FUNC         func;
        AFUNC        aFunc;
        
        STCFsmAction():eToStatus(EStatus(TStatus(0))),func(0),aFunc(0){};
        
        STCFsmAction(EStatus eToStatus_p, FUNC func_p,AFUNC aFunc_p)
        {
            eToStatus = eToStatus_p;
            func = func_p;
            aFunc = aFunc_p;
        };
    }; 
    
public:
    typedef std::map<STCFsmKey,STCFsmAction> TMapStutas;
private:
    TMapStutas      m_mStatusMap;
    EStatus         m_eCurStatus;
    EStatus         m_eInitStatus;
    TClass&         m_oOwner;
    bool            m_bIsStatusChange;
public:
    CTCFsm(TClass & oClass,EStatus eStartStatus)
        :m_oOwner(oClass)
    {
        m_eInitStatus = m_eCurStatus = eStartStatus;
        m_bIsStatusChange = false;
    };
    
    virtual ~CTCFsm()
    {
    };

    void Reset()
    {  
        m_eCurStatus = m_eInitStatus;
        m_bIsStatusChange = false;
    }

    void ChangeStatus(EStatus eStatus)
    {
        if (eStatus == m_eCurStatus)
        {
            return;
        }
        else
        {
            m_eCurStatus = eStatus;
            m_bIsStatusChange = true;
        }
    }

    void SetCurStatus(EStatus eStatus)
    {
       m_eCurStatus = eStatus;
    }

    EStatus GetCurStatus() const
    {
        return m_eCurStatus;
    }
    
    CTCFsm& Add(EStatus  eFromStatus,
                EEvent   eEvent,
                EStatus  eToStatus,
                FUNC func = NULL, 
                AFUNC aFunc = NULL)
    {
        STCFsmKey       fsmKey(eFromStatus,eEvent);
        STCFsmAction    fsmAction(eToStatus,func,aFunc);
        m_mStatusMap[fsmKey]=fsmAction;
        return *this;
    }

    bool DoFsm(const EEvent uiEvent, const TParam1  Param1 = 0,const TParam2 Param2 = 0,EStatus *eNextStatus = 0)
    {
        
        typename TMapStutas::iterator it = m_mStatusMap.find(STCFsmKey(m_eCurStatus,uiEvent));
        if(it != m_mStatusMap.end())
        {
            STCFsmAction& oFsmAction = it->second;
//             LOG(TCLOG_FSM, typeid(m_oOwner).name() << ":" << (tc_uint)this 
//                             <<  " State From:" <<  m_eCurStatus <<" Event:" << uiEvent);
            if(oFsmAction.func)
            {
                FUNC func = oFsmAction.func;
                (m_oOwner.*func)(Param1,Param2);
            }

			//以下支持多分枝的状态转变
            if (m_bIsStatusChange)
            {
                m_bIsStatusChange = false;
                return DoFsm(uiEvent, Param1, Param2, eNextStatus);
            }
            
            m_eCurStatus = oFsmAction.eToStatus;
/*            LOG(TCLOG_FSM,typeid(m_oOwner).name() << " State To:" <<  m_eCurStatus);*/

            if(oFsmAction.aFunc)
            {
		        AFUNC aFunc =oFsmAction.aFunc;
               (m_oOwner.*aFunc)(Param1,Param2);
            }
			
	        if(eNextStatus)
                *eNextStatus = m_eCurStatus;
		 
            return true;
        }
        else
        {
//             LOG(TCLOG_FSM, typeid(m_oOwner).name() << ":" << (tc_uint)this 
//                             <<  " Unhandle From:" <<  m_eCurStatus <<" Event:" << uiEvent);
            
        }
        
        return false;
    };
};

