/*
TFC_ERROR模块, 采用TSD(Thread-specific Data)技术, 
以实现 自定义的 errNo, errMsg 在多线程环境中的安全访问.
*/


#ifndef _TFC_ERROR_H_
#define _TFC_ERROR_H_

#include "tfc_comm.h"

#pragma pack(1)

typedef struct tagERROR_INFO
{
    int err_no_;
    char err_msg_[TFC_MAX_ERROR_MSG_LEN];
}TFC_ERROR_INFO;

#pragma pack()

class TFCError
{
protected:
    
    TFCError ();
public:
    static int init ();    //程序启动时调用, 用于创建key, 全局只调用一次
    static void fini ();    //程序退出时调用, 用于删除key, 全局只调用一次
    static int init_t();    //线程启动时调用, 用于创建线程专属内存块,各线程均需调用
    static void fini_t();   //线程退出时调用, 用于删除线程专属内存块,各线程均需调用
    static TFCError *instance ();

    operator int (void) const;
    int operator= (int);
    operator char* (void);

    static void clear_error();
    static void set_error(int iErrno, const char* sErrMsg, ...);
    static void set_error_sys(int iErrno, const char* sErrMsg);

private:
    static TFCError *instance_;
#ifdef WIN32
    static DWORD err_key_;
#else
    static pthread_key_t err_key_;
#endif

    static void free_err(void * ptr);
};


#define TFC_ERRNO (*(TFCError::instance ()))
#define TFC_ERRMSG (*(TFCError::instance ())) 

#define TFC_ERROR_INIT  TFCError::init
#define TFC_ERROR_FINI  TFCError::fini

#define TFC_ERROR_INIT_T  TFCError::init_t
#define TFC_ERROR_FINI_T  TFCError::fini_t

#define CLEAR_ERROR  TFCError::clear_error
#define SET_ERROR  TFCError::set_error
#define SET_ERROR_SYS  TFCError::set_error_sys


//设置错误值, 并函数返回
#define ERROR_RETURN_SYS_ERROR(errNo, errMsg) \
            TFCError::set_error_sys(errNo, errMsg); \
            return errNo;

#define ERRNO_RETURN(errNo); \
    		TFC_ERRNO = (int)errNo; \
    		return errNo;

#define ERROR_RETURN(errNo, errMsg); \
    		TFCError::set_error(errNo, errMsg); \
    		return errNo;

#define ERROR_RETURN_1(errNo, errMsg, param1) \
            TFCError::set_error(errNo, errMsg, param1); \
            return errNo;

#define ERROR_RETURN_2(errNo, errMsg, param1, param2) \
            TFCError::set_error(errNo, errMsg, param1, param2); \
            return errNo;

#define ERROR_RETURN_3(errNo, errMsg, param1, param2, param3) \
            TFCError::set_error(errNo, errMsg, param1, param2, param3); \
            return errNo;

#define ERROR_RETURN_NULL(errNo, errMsg); \
    		TFCError::set_error(errNo, errMsg); \
    		return NULL;

#define ERROR_RETURN_NULL_1(errNo, errMsg, param1) \
            TFCError::set_error(errNo, errMsg, param1); \
            return NULL;

#define ERROR_RETURN_NULL_2(errNo, errMsg, param1, param2) \
            TFCError::set_error(errNo, errMsg, param1, param2); \
            return NULL;

#define ERROR_RETURN_NULL_3(errNo, errMsg, param1, param2, param3) \
            TFCError::set_error(errNo, errMsg, param1, param2, param3); \
            return NULL;


#endif //_TFC_ERROR_H_


