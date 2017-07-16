#ifndef _MSG_PROCESS_H_
#define _MSG_PROCESS_H_

#include "msg_process_interface.h"
#include "fifo_loop_buffer.h"
#include "mx_mutex.h"
class CMsgProcess 
: public IMsgProcessor
{
public:
    CMsgProcess();
    ~CMsgProcess(){};
    bool ProcessMsg(STCMsg* msg);
    bool ProcessMsg(CFIFOLoopBuffer::tagBufferNode* pNodeInput);

    bool SetBufferObj(CFIFOLoopBuffer* pInputBuf);
    
private:
	CMxMutex _mutex;
    
private:
    CFIFOLoopBuffer*	m_pInputBuffer;
    CFIFOLoopBuffer*	m_pOutputBuffer;
};



#endif
