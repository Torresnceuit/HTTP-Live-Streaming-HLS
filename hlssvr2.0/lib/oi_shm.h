/******************************************************************************
 *Copyright (C), 1996-2002, Sunrise Tech. Co., Ltd.
 *File name:      oi_shm.h
 *Author:         emandyliu
 *Version:        1.0
 *Date:           2008.09.25
 *Description:   共享内存操作函数
 *Others:
 *Function List:
 *History: 
            1.  2008.09.25  初始化  emandyliu
			     增加函数 GetShmOnlyMat ， 只shmmat共享内存
			    
 *******************************************************************************/

#ifndef 	OI_SHM_H_
#define  	OI_SHM_H_

#include <sys/ipc.h>
#include <sys/shm.h>


#ifdef  __cplusplus
extern "C"
{
#endif

#ifdef linux
    union semun
    {
        int val;                /* value for SETVAL */
        struct semid_ds *buf;   /* buffer for IPC_STAT, IPC_SET */
        unsigned short int *array;  /* array for GETALL, SETALL */
        struct seminfo *__buf;  /* buffer for IPC_INFO */
    };
#endif


#if 1
#define RWLock_Init(key,iLockNums,iChldID)  OI_RWLock_Init(key,iLockNums,iChldID)
#define RWLock_GetValue(key) 				OI_RWLock_GetValue(key)
#define RWLock_UnLock() 					OI_RWLock_UnLock()
#define RWLock_RDLock() 					OI_RWLock_RDLock()
#define RWLock_WDLock() 					OI_RWLock_WDLock()
#define Lib_Semp_Init(pstLockInfo,iSemKey)  OI_Lib_Semp_Init(pstLockInfo,iSemKey)
#define Lib_Semp_Lock(pstLockInfo) 			OI_Lib_Semp_Lock(pstLockInfo)
#define Lib_Semp_Unlock(pstLockInfo) 		OI_Lib_Semp_Unlock(pstLockInfo)
#define GetShm(iKey,iSize,iFlag) 			OI_GetShm(iKey,iSize,iFlag)
#define GetShmOnlyMat(iKey,iSize,iFlag) 	OI_GetShmOnlyMat(iKey,iSize,iFlag)
#define GetShm2(pstShm,iKey,iSize,iFlag)	OI_GetShm2(pstShm,iKey,iSize,iFlag)
#define GetShm3(pstShm,iKey,iSize,iFlag) 	OI_GetShm3(pstShm,iKey,iSize,iFlag)
#endif

    int OI_RWLock_Init(key_t key, int iLockNums, int iChldID);
    int OI_RWLock_GetValue(key_t key);
    int OI_RWLock_UnLock(void);
    int OI_RWLock_RDLock(void);
    int OI_RWLock_WDLock(void);

    typedef struct
    {
        int iSemID;
        char sErrMsg[255];
    } OI_SEMLOCKINFO;


    /* 创建信号量,只创建一个, 默认使用权限 0666
     * @param [in] pstLockInfo 要用户自己申请指向的空间 
     * @param [in] iShmKey 信号量的Key
     * @return NULL 表示失败 
     */
    int OI_Lib_Semp_Init(OI_SEMLOCKINFO * pstLockInfo, int iSemKey);

    /* 创建信号量,只创建一个, 默认使用权限 0666
     * @param [in] pstLockInfo 要用户自己申请指向的空间 
     * @param [in] iShmKey 信号量的Key
     * @return NULL 表示失败 
     */
    int OI_Lib_Semp_Lock(OI_SEMLOCKINFO *);

    int OI_Lib_Semp_Unlock(OI_SEMLOCKINFO *);

    /* 创建共享内存
     * @param [in] iKey  
     * @param [in] iSize
     * @param [in] iFlag  0644 | IPC_CREATE 
     * @return NULL 表示失败 
     */
    char *OI_GetShm(int iKey, int iSize, int iFlag);


    /* shmmat 共享内存
     * @param [in] iKey  
     * @param [in|out] piSize
     * @param [in] iFlag  0644 | IPC_CREATE 
     * @return NULL 表示失败 
     */
    char *OI_GetShmOnlyMat(int iKey, int *piSize, int iFlag);


    /* shmmat 共享内存, 如果没有,并且设置IPC_CREATE就创建，并且初始化为0
     * @param [out]  pstShm
     * @param [in] iKey  
     * @param [in] iSize
     * @param [in] iFlag  0644 | IPC_CREATE 
     * @return  <0 表示失败
     */
    int OI_GetShm2(void **pstShm, int iKey, int iSize, int iFlag);


    /* shmmat 共享内存  如果没有,并且设置IPC_CREATE就创建，但不初始化
     * @param [out]  pstShm
     * @param [in] iKey  
     * @param [in] iSize
     * @param [in] iFlag  0644 | IPC_CREATE 
     * @return  <0 表示失败 0表示成功，1表示是新建的共享内存
     */
    int OI_GetShm3(void **pstShm, int iKey, int iSize, int iFlag);


    char *OI_GetShm4(int iKey, int iSize, int iFlag, int *piShmID);


#ifdef  __cplusplus
}
#endif


#endif
