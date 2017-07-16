#ifndef ATTR_API_H
#define ATTR_API_H

#include "oi_shm.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define	ATTR_SHM_ID		24555
#define MAX_ATTR_NODE		1000


    //
    //带字符串报警特性相关的宏定义
    //
#define DEP_ATTTIBUTE_BASE 600
#define DEP_SHM_ID              24552   //告警共享内存
#define DEP_SHM_LOCK_ID         34552
#define MAX_DEP_SHMEM_LEN  2*1024*1024
#define MAX_DEP_VALUE_LEN  (MAX_DEP_SHMEM_LEN - sizeof(int))
#define ATTRID_ALARM(id, fmt, args...) ({ \
		static char __sBuf[1024];\
		int __iRet = 0;\
		__iRet = snprintf(__sBuf, sizeof(__sBuf), fmt, ##args);\
		if(__iRet < 0) __iRet = sizeof(__sBuf) - 1;\
		Attr_API_Alarm(id, __iRet, __sBuf);\
		})
#define ATTR_ALARM(fmt, args...) ATTRID_ALARM(52574, fmt, ##args)

    typedef struct
    {
        int iUse;
        int iAttrID;
        int iCurValue;
    } AttrNode;

    typedef struct
    {
        AttrNode astNode[MAX_ATTR_NODE];
    } AttrList;

    /*
     *上报统计值
     * @param [in] attr ID
     * @param [in] iValue 值
     * @return 0成功
     */


    // 上报值 attr 为属性ID 
    int Attr_API(int attr, int iValue);

    int Attr_API_EX(int attr, int iValue, int *piCurValue);
    int Attr_API_Set(int attr, int iValue);
    int Attr_API_Get(int attr, int *piValue);
    int Attr_API_Alarm(int attr_id, size_t len, char *pvalue);

    int AttrProfile(int iId, int iAttr1, int iAttr2);
#ifdef __cplusplus
}
#endif


#endif
