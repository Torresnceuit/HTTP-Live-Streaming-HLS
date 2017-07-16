/************************************************************
  Description:
        TFC应用基本类. 用于管理TFC最基础的模块, 这些模块一般
    采用instance模式, 全局唯一实例. 
        目前主要为 日志模块(最基础的模块), TfcError模块.
        每一个使用TFC的程序, 在main()中需要包含一个TFCObject
    实例, 并调用其init()方法. 如:
    int main(int argc, char* argv[])
    {
        TFCObject tfc_obj;
        if (tfc_obj.init() != 0) 
        {
            return -1;
        }
        //......
        //程序主程序...
        return 0;
    }
 ***********************************************************/
#ifndef _TFC_OBJECT_H_
#define _TFC_OBJECT_H_

#include "tfc_comm.h"
#include "tfc_debug_log.h"
#include "tfc_error.h"


class TFCObject
{
public:
    enum TFC_OBJECT_ERROR
    {
        TFC_OBJECT_ERROR_BASE = -1,   
        TFC_OBJECT_ERROR_LOG_INIT_FAIL = TFC_OBJECT_ERROR_BASE -1,   //初始化日志模块失败
        TFC_OBJECT_ERROR_ERR_INIT_FAIL = TFC_OBJECT_ERROR_BASE -2,   //初始化TsdError模块失败
    };
public:

    TFCObject() 
    {
    }
    virtual ~TFCObject()
    {
        fini();
    }
    virtual int init()     
    {
        if (DEBUG_INIT() != 0)
        {
            return TFC_OBJECT_ERROR_LOG_INIT_FAIL;
        }
        if (TFC_ERROR_INIT() != 0)
        {
            return TFC_OBJECT_ERROR_ERR_INIT_FAIL;
        }
        return 0;
    }
    virtual void fini()     
    {
        TFC_ERROR_FINI();
        DEBUG_FINI();
    }
};


#endif //_TFC_OBJECT_H_
