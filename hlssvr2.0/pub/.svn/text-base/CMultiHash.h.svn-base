#ifndef _CMULTIHASH_H
#define _CMULTIHASH_H
#include <string.h>
#include <list>
#include <math.h>
#include <sys/shm.h>
#include <assert.h>

using namespace::std;

#define	MAX_HASHTIME 100

template <typename TNodeType>
class CMultiHash
{
public:
    typedef bool (TNodeType::*GETFUNC)(const TNodeType &lsh, const TNodeType &rsh);
    typedef bool (TNodeType::*INSFUNC)(const TNodeType &lsh);
    typedef bool (TNodeType::*VALFUNC)(const TNodeType &lsh);

public:
    typedef std::list<TNodeType*> TNodeList;

    CMultiHash(TNodeType &oHashNode, GETFUNC get_func, INSFUNC ins_func, VALFUNC val_func)
        :m_oOwner(oHashNode)
    {
        m_pHash         = NULL;
        m_uiHashTime    = 0;
        m_uiHashLen     = 0;
        m_uiNodeSize    = sizeof(TNodeType);
        memset(&m_uiHashBase, 0, sizeof(m_uiHashBase));
        m_oOwner        = oHashNode;

        m_getFunc = get_func;
        m_insFunc = ins_func;
        m_valFunc = val_func;
    }

    ~CMultiHash()
    {
        m_pHash         = NULL;
        m_uiHashTime    = 0;
        m_uiHashLen     = 0;
        m_uiNodeSize    = 0;
        memset(&m_uiHashBase, 0, sizeof(m_uiHashBase));

        m_getFunc = NULL;
        m_insFunc = NULL;
        m_valFunc = NULL;
    }

    int init(unsigned int uiKey, unsigned int uiHashLen, unsigned int uiHashTime, bool bCreate)
    {
        m_uiHashLen  = uiHashLen;
        m_uiHashTime = uiHashTime;
        memset(&m_uiHashBase, 0, sizeof(m_uiHashBase));

        unsigned int uiSqrt = 0;
        unsigned int i = 0;
        unsigned int j = 0;
        unsigned int k = 0;
        int flag = 0;

        uiSqrt = (unsigned int)sqrt(m_uiHashLen);

        for(i=m_uiHashLen, j=0; j<m_uiHashTime; i--)
        {
            flag = 1;

            for(k=2; k<=uiSqrt; k++)
            {
                if(i%k == 0)
                {
                    flag = 0;
                    break;
                }
            }

            if(flag == 1)
            {
                m_uiHashBase[j] = i;
                j++;
            }
        }

        int nFlag = 0666;
        m_pHash = (void *)GetShm(uiKey, m_uiNodeSize*m_uiHashLen*m_uiHashTime, nFlag);

        if(m_pHash == NULL)
        {
            if(bCreate)
                nFlag = nFlag|IPC_CREAT;
            else
                return -7;

            m_pHash = (void *)GetShm(uiKey, m_uiNodeSize*m_uiHashLen*m_uiHashTime, nFlag);

            if(m_pHash == NULL)
                return -8;
            else
                memset((char *)m_pHash, 0, m_uiNodeSize*m_uiHashLen*m_uiHashTime);
        }

        return 0;
    }

    int clear()
    {
        if (m_pHash)
        {
            memset((char *)m_pHash, 0, m_uiNodeSize*m_uiHashLen*m_uiHashTime);
        }
        else
        {
            return -1;
        }

        return 0;
    }

    bool check(unsigned int uihashKey, const TNodeType &rsh, unsigned int uiXHashTime, unsigned int uiYHashLen)
    {
        unsigned int hash = 0;
        TNodeType *pHashNode = NULL;

        hash = uihashKey%m_uiHashBase[uiXHashTime];
        pHashNode = (TNodeType *)((char *)m_pHash + m_uiNodeSize*(uiXHashTime*m_uiHashLen+hash));

        if ((pHashNode != NULL) && ((m_oOwner.*m_getFunc)(*pHashNode, rsh)))
        {
            return (TNodeType *)pHashNode;
        }                        

        return NULL;
    }

    TNodeType *get(unsigned int uihashKey, const TNodeType &rsh)
    {
        unsigned int hash = 0;
        TNodeType *pHashNode = NULL;

        for(unsigned int i=0; i<m_uiHashTime; i++)
        {
            hash = uihashKey%m_uiHashBase[i];
            pHashNode = (TNodeType *)((char *)m_pHash + m_uiNodeSize*(i*m_uiHashLen+hash));

            if ((pHashNode != NULL) && ((m_oOwner.*m_getFunc)(*pHashNode, rsh)))
            {
                return (TNodeType *)pHashNode;
            }                        
        }

        return NULL;
    }

    TNodeType *get(unsigned int uihashKey, const TNodeType &rsh, unsigned int uiXHashTime, unsigned int uiYHashLen)
    {
        unsigned int hash = 0;
        TNodeType *pHashNode = NULL;

        hash = uihashKey%m_uiHashBase[uiXHashTime];
        pHashNode = (TNodeType *)((char *)m_pHash + m_uiNodeSize*(uiXHashTime*m_uiHashLen+hash));

        if ((pHashNode != NULL) && ((m_oOwner.*m_getFunc)(*pHashNode, rsh)))
        {
            return (TNodeType *)pHashNode;
        }                        

        return NULL;
    }

    int get(unsigned int uihashKey, const TNodeType &rsh, TNodeList& oList)
    {
        unsigned int hash = 0;
        TNodeType *pHashNode = NULL;

        for(unsigned int i=0; i<m_uiHashTime; i++)
        {
            hash = uihashKey%m_uiHashBase[i];
            pHashNode = (TNodeType *)((char *)m_pHash + m_uiNodeSize*(i*m_uiHashLen+hash));

            if ((pHashNode != NULL) && ((m_oOwner.*m_getFunc)(*pHashNode, rsh)))
            {
                oList.push_back(pHashNode);
            } 
            else
            {
                continue;
            }
        }

        return 0;
    }

    int val(TNodeList &oList)
    {
        TNodeType *pHashNode = NULL;

        for(unsigned int i=0; i<m_uiHashTime; i++)
        {
            for(unsigned int j=0; j<m_uiHashLen; j++)
            {
                pHashNode = (TNodeType *)((char *)m_pHash + m_uiNodeSize*(i*m_uiHashLen+j));

                if ((pHashNode != NULL) && ((m_oOwner.*m_valFunc)(*pHashNode)))
                {
                    oList.push_back(pHashNode);
                } 
                else
                {
                    continue;
                }
            }
        }
        return 0;
    }

    unsigned int valSize() const
    {
        unsigned int uiNum = 0;
        TNodeType *pHashNode = NULL;

        for(unsigned int i=0; i<m_uiHashTime; i++)
        {
            for(unsigned int j=0; j<m_uiHashLen; j++)
            {
                pHashNode = (TNodeType *)((char *)m_pHash + m_uiNodeSize*(i*m_uiHashLen+j));

                if ((pHashNode != NULL) && ((m_oOwner.*m_valFunc)(*pHashNode)))
                {
                    uiNum++;
                } 
            }
        }

        return uiNum;
    }

    int del(unsigned int uihashKey, const TNodeType &rsh)
    {
        unsigned int hash = 0;
        TNodeType *pHashNode = NULL;

        for(unsigned int i=0; i<m_uiHashTime; i++)
        {
            hash = uihashKey%m_uiHashBase[i];
            pHashNode = (TNodeType *)((char *)m_pHash + m_uiNodeSize*(i*m_uiHashLen+hash));

            if ((pHashNode != NULL) && ((m_oOwner.*m_getFunc)(*pHashNode, rsh)))
            {
                memset((char *)pHashNode, 0, m_uiNodeSize);
            } 
        }

        return 0;
    }

    int del(unsigned int uihashKey, const TNodeType &rsh, unsigned int uiXHashTime, unsigned int uiYHashLen)
    {
        unsigned int hash = 0;
        TNodeType *pHashNode = NULL;

        hash = uihashKey%m_uiHashBase[uiXHashTime];
        pHashNode = (TNodeType *)((char *)m_pHash + m_uiNodeSize*(uiXHashTime*m_uiHashLen+hash));

        if ((pHashNode != NULL) && ((m_oOwner.*m_getFunc)(*pHashNode, rsh)))
        {
            memset((char *)pHashNode, 0, m_uiNodeSize);
        } 

        return 0;
    }

    TNodeType *ins(unsigned int uihashKey, const TNodeType &oNode)
    {
        unsigned int hash = 0;
        TNodeType *pHashNode = NULL;

        for(unsigned int i=0; i<m_uiHashTime; i++)
        {
            hash = uihashKey%m_uiHashBase[i];
            pHashNode = (TNodeType *)((char *)m_pHash + m_uiNodeSize*(i*m_uiHashLen+hash));

            if((pHashNode != NULL) && ((m_oOwner.*m_insFunc)(*pHashNode)))
            {
                memcpy((char *)pHashNode, (char *)&oNode, sizeof(TNodeType));
                return (TNodeType *)pHashNode;
            }
        }

        return NULL;
    }

    TNodeType *ins(unsigned int uihashKey, const TNodeType &oNode, unsigned int &uiXHashTime, unsigned int &uiYHashLen)
    {
        unsigned int hash = 0;
        TNodeType *pHashNode = NULL;

        unsigned int i=0;
        for(i=0; i<m_uiHashTime; i++)
        {
            hash = uihashKey%m_uiHashBase[i];
            pHashNode = (TNodeType *)((char *)m_pHash + m_uiNodeSize*(i*m_uiHashLen+hash));

            if((pHashNode != NULL) && ((m_oOwner.*m_insFunc)(*pHashNode)))
            {
                memcpy((char *)pHashNode, (char *)&oNode, sizeof(TNodeType));    
                uiXHashTime = i;
                uiYHashLen  = hash;
                return (TNodeType *)pHashNode;
            }
        }

        return NULL;
    }

private:
    void *m_pHash;
    unsigned int m_uiHashTime;
    unsigned int m_uiHashLen;
    unsigned int m_uiNodeSize;
    unsigned int m_uiHashBase[MAX_HASHTIME];
    TNodeType &m_oOwner;

    GETFUNC      m_getFunc;
    INSFUNC      m_insFunc;
    VALFUNC      m_valFunc;

    void *GetShm(int key, unsigned int size, int flags)
    {
        int shmId;
        int ret;

        if ((shmId = shmget(key, size, flags)) == -1)
            return NULL;

        if ((ret = (int)shmat(shmId, NULL, 0)) == -1)
            return NULL;

        return (void *)ret;
    }
};

#endif

