#include "tfc_error.h"
#include "tfc_debug_log.h"
//#include "thread_pool.h"

TFCError* TFCError::instance_ = 0;
#ifdef WIN32
DWORD TFCError::err_key_ = 0xffffffff;
#else
pthread_key_t TFCError::err_key_ = 0xffffffff;
#endif

TFCError::TFCError()
{
}

TFCError* TFCError::instance()
{
    if (instance_ == NULL)
    {
        instance_ = new  TFCError;
    }
    return instance_;
}

int TFCError::init ()
{
    if (instance() == NULL)
    {
        //DEBUG_P(LOG_FATAL, "[%d] new TFC Error  fail:%d,%s\n", (int)ThreadPool::thr_id(), errno, strerror(errno));
        return -1;
    }
#ifdef WIN32
    err_key_ = TlsAlloc ();
#else
     pthread_key_create(&err_key_, free_err);
#endif
    return init_t();    //主线程也调用一次
}

void TFCError::fini ()
{
    if(instance_ != NULL)
    {
        fini_t();       //主线程也调用一次
        delete instance_;
        instance_ = NULL;
    }
}

int TFCError::init_t()
{
    TFC_ERROR_INFO* err_info = new TFC_ERROR_INFO;
    if (err_info == NULL)
    {
        //DEBUG_P(LOG_FATAL, "[%d] new ERROR INFO fail:%d,%s\n", (int)ThreadPool::thr_id(), errno, strerror(errno));
        return -1;
    }
    memset(err_info, 0, sizeof(TFC_ERROR_INFO));
#ifdef WIN32
    TlsSetValue(err_key_, err_info);
#else
    pthread_setspecific(err_key_, err_info);
#endif
   // DEBUG_P(LOG_TRACE, "[%d] init_t tfc_error\n", (int)ThreadPool::thr_id());

    return 0;
}

void TFCError::fini_t()
{
    TFC_ERROR_INFO* err_info = NULL;

#ifdef WIN32
    err_info = (TFC_ERROR_INFO*)TlsGetValue(err_key_);
    TlsSetValue(err_key_, NULL);
#else
    err_info = (TFC_ERROR_INFO*)pthread_getspecific(err_key_);
    pthread_setspecific(err_key_, NULL);
#endif
    free_err(err_info);
}

void TFCError::free_err(void* ptr)
{
    TFC_ERROR_INFO* err_info = (TFC_ERROR_INFO*)ptr;
    if (err_info != NULL)
    {
        //DEBUG_P(LOG_TRACE, "[%d] fini_t tfc_error\n", (int)ThreadPool::thr_id());
        delete err_info;
    }
}

TFCError::operator int (void) const
{
    TFC_ERROR_INFO* err_info = NULL;

#ifdef WIN32
    err_info = (TFC_ERROR_INFO*)TlsGetValue(err_key_);
#else
    err_info = (TFC_ERROR_INFO*)pthread_getspecific(err_key_);
#endif
    return err_info->err_no_;
}

int TFCError::operator= (int x)
{
    TFC_ERROR_INFO* err_info = NULL;

#ifdef WIN32
    err_info = (TFC_ERROR_INFO*)TlsGetValue(err_key_);
#else
    err_info = (TFC_ERROR_INFO*)pthread_getspecific(err_key_);
#endif
    err_info->err_no_ = x;
    return x;
}

TFCError::operator char* (void)
{
    TFC_ERROR_INFO* err_info = NULL;

#ifdef WIN32
    err_info = (TFC_ERROR_INFO*)TlsGetValue(err_key_);
#else
    err_info = (TFC_ERROR_INFO*)pthread_getspecific(err_key_);
#endif
    return err_info->err_msg_;
}


void TFCError::clear_error()
{
    TFC_ERROR_INFO* err_info = NULL;

#ifdef WIN32
    err_info = (TFC_ERROR_INFO*)TlsGetValue(err_key_);
#else
    err_info = (TFC_ERROR_INFO*)pthread_getspecific(err_key_);
#endif
    err_info->err_no_ = 0;
    err_info->err_msg_[0] = 0;
}

void TFCError::set_error(int iErrno, const char* sErrMsg, ...)
{          
    TFC_ERROR_INFO* err_info = NULL;

#ifdef WIN32
    err_info = (TFC_ERROR_INFO*)TlsGetValue(err_key_);
#else
    err_info = (TFC_ERROR_INFO*)pthread_getspecific(err_key_);
#endif

    va_list ap;
    va_start(ap, sErrMsg);
    vsnprintf(err_info->err_msg_, TFC_MAX_ERROR_MSG_LEN, sErrMsg, ap);
    va_end(ap);
    err_info->err_msg_[TFC_MAX_ERROR_MSG_LEN -1] = 0;

    err_info->err_no_ = iErrno;
    
    //DEBUG_P(LOG_TRACE, "[%d] set tfc_error:%d,%s\n", (int)ThreadPool::thr_id(), err_info->err_no_, err_info->err_msg_);
}

void TFCError::set_error_sys(int iErrno, const char* sErrMsg)
{          
    TFC_ERROR_INFO* err_info = NULL;

#ifdef WIN32
    err_info = (TFC_ERROR_INFO*)TlsGetValue(err_key_);
#else
    err_info = (TFC_ERROR_INFO*)pthread_getspecific(err_key_);
#endif

    snprintf(err_info->err_msg_, TFC_MAX_ERROR_MSG_LEN, "%s.[%d:%s]", sErrMsg, errno, strerror(errno));
    err_info->err_msg_[TFC_MAX_ERROR_MSG_LEN -1] = 0;

    err_info->err_no_ = iErrno;
    //DEBUG_P(LOG_TRACE, "[%d] set tfc_error:%d,%s\n", (int)ThreadPool::thr_id(), err_info->err_no_, err_info->err_msg_);
}




