#ifndef _MSG_PROCESS_INTERFACE_H
#define _MSG_PROCESS_INTERFACE_H

const unsigned int g_uiDefauleMsgSize = 4*1024;
struct STCMsgHeader
{

};

struct STCMsg
{
    STCMsgHeader    oHeader;
    unsigned int    uiMsgLen;     //指示的是pBody的长度
    char*  pBody;                 //实际的消息体码流
    STCMsg()
    {
        uiMsgLen = 0;
        pBody    = 0;
    }
};

class IMsgProcessor
{
public:
    //IMsgProcessor(){};
    virtual ~IMsgProcessor(){};
    virtual bool ProcessMsg(STCMsg* msg) = 0;
};


#endif
