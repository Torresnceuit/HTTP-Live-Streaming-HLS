#ifndef _MSG_PROCESS_INTERFACE_H
#define _MSG_PROCESS_INTERFACE_H

const unsigned int g_uiDefauleMsgSize = 4*1024;
struct STCMsgHeader
{

};

struct STCMsg
{
    STCMsgHeader    oHeader;
    unsigned int    uiMsgLen;     //ָʾ����pBody�ĳ���
    char*  pBody;                 //ʵ�ʵ���Ϣ������
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
