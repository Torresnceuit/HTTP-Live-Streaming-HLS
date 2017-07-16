#include "msg_process.h"
#include <cassert>
#include <string.h>


CMsgProcess::CMsgProcess()
    :m_pInputBuffer(NULL)
    ,m_pOutputBuffer(NULL)
{

}

bool CMsgProcess::ProcessMsg(STCMsg* msg)
{  
    MxMutexGuard g(_mutex);

    assert(msg != NULL);
    CFIFOLoopBuffer::tagBufferNode* pNode=0; 
    if(m_pInputBuffer)
    {
        m_pInputBuffer->GetCurrentWriteNode(&pNode);
    }
    if(pNode)
    {
        ///////////////
        //write data
        //if(pNode->nBufferLength < sizeof(STCMsg) + msg->uiMsgLen)
        if(pNode->nBufferLength < (int)g_uiDefauleMsgSize)
        {
            if(pNode->pData)
            {
                delete []pNode->pData;
                pNode->pData=0;
                pNode->nBufferLength=0;
            }					
            
			//pNode->pData   =  new unsigned char[sizeof(STCMsg) + msg->uiMsgLen + 4];
			//pNode->pData   =  new unsigned char[g_uiDefauleMsgSize];
            //pNode->nBufferLength = sizeof(STCMsg) + msg->uiMsgLen + 4;
			pNode->nBufferLength = g_uiDefauleMsgSize;
        }
     
        //此处采取了数据复制的形式，后续可以考虑优化，只传递指针
        //memcpy(pNode->pData, msg, sizeof(STCMsg));
        //memcpy(pNode->pData + sizeof(STCMsg), msg->pBody, msg->uiMsgLen);
        pNode->nDataLength = sizeof(STCMsg) + msg->uiMsgLen;
        pNode->bValid = true;
        
        //////////////
        //move to next node
        m_pInputBuffer->MoveToNextWriteNode();
    }
    else
    {        
        //m_slog.warn("ProcessPackage() Loop buffer full");
    }
    
    return true;
}

bool CMsgProcess::ProcessMsg(CFIFOLoopBuffer::tagBufferNode* pNodeInput)
{  
    MxMutexGuard g(_mutex);
    
    CFIFOLoopBuffer::tagBufferNode* pNode=0; 
    if(m_pInputBuffer)
    {
        m_pInputBuffer->GetCurrentWriteNode(&pNode);
    }
    if(pNode)
    {
        ///////////////
        //write data
        if(pNode->nBufferLength < pNodeInput->nBufferLength)
        {
            if(pNode->pData)
            {
                delete []pNode->pData;
                pNode->pData=0;
                pNode->nBufferLength=0;
            }					
            pNode->pData   =  new unsigned char[pNodeInput->nBufferLength];
            pNode->nBufferLength = pNodeInput->nBufferLength;
        }
        
        //此处采取了数据复制的形式，后续可以考虑优化，只传递指针
        memcpy(pNode->pData, pNodeInput->pData, pNodeInput->nBufferLength);
        pNode->nDataLength = pNodeInput->nDataLength;
        pNode->bValid = true;
        
        //////////////
        //move to next node
        m_pInputBuffer->MoveToNextWriteNode();
    }
    else
    {        
        //m_slog.warn("ProcessPackage() Loop buffer full");
    }
    
    return true;
}

bool CMsgProcess::SetBufferObj(CFIFOLoopBuffer* pInputBuf)
{
    m_pInputBuffer = pInputBuf;   
    return true;
}

