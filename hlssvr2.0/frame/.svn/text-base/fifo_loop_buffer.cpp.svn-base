//////////////////////////////////////////////////////////////////////
// FIFOLoopMsgBuffer.cpp: implementation of the CFIFOLoopMsgBuffer class.
// 先进先出循环buffer,不用加锁一个线程写入,一个线程读取
//
//    >>>>>>>>>>>>>>>------------read pointer
//    ^             v
//    ^<<<<<<<<<<<<<v------------write Pointer
//    read pointer不能向前越位write pointer
//    write pointer不能向前越位read pointer
//    
//////////////////////////////////////////////////////////////////////
#include "fifo_loop_buffer.h"
#include <memory.h>
#include <stdio.h>

CFIFOLoopBuffer::CFIFOLoopBuffer(int nMaxNodes)
{
	m_pReadStart=0;
	m_pWriteStart=0;
	m_nMaxNodes=nMaxNodes;
	
#ifdef _DEBUG
	if(nMaxNodes<=4)
	{
		//Xprintf("[*DEBUG*]Error:at least 4 nodes.\r\n");
	}	
#endif//_DEBUG

	////////////
	//init loop buffer
	BufferNode* pTail=0;
	for(int i=0;i<m_nMaxNodes;i++)
	{
		BufferNode* p=new BufferNode;
		if(!p)//out of memory
		{
			m_pReadStart=0;
			break;
		}

		memset(p,0,sizeof(BufferNode));
		if(!m_pReadStart)
		{
			m_pReadStart=p;
			pTail=m_pReadStart;
		}
		else
		{
			pTail->pNext=p;
			pTail=pTail->pNext;
		}
	}
	
	/////////////////
	//形成一个环,pwrite pointer在pRead pointer前面一个
	if(m_pReadStart)
	{
		m_pWriteStart=m_pReadStart->pNext->pNext;
		pTail->pNext=m_pReadStart;
	}
}

CFIFOLoopBuffer::~CFIFOLoopBuffer()
{
	CleanNodes();
}

void CFIFOLoopBuffer::CleanNodes()
{
	//release all objects
	m_pWriteStart=0;
	if(m_pReadStart)
	{	
		//BufferNode* p=m_pReadStart->pNext;
		//while(p)
		//{
		//	BufferNode* pPre=p;
		//	p=p->pNext;
		//	if(p==m_pReadStart)
		//		p->pNext=0;
		//
		//	if(pPre->pData)
		//	{
		//		delete []pPre->pData;
		//		pPre->pData=0;
		//	}			
		//	delete pPre;
		//}

		//Xprintf("Release %d Nodes..",m_nMaxNodes);

		for(int i=0;i<m_nMaxNodes;i++)
		{
			BufferNode* pPre=m_pReadStart;
			m_pReadStart=m_pReadStart->pNext;
			if(pPre)
			{
				if(pPre->pData)
				{
					delete []pPre->pData;					
				}
				delete pPre;
			}
			
			if(i%2000==0)
			{
				//Xprintf(".");
			}
		}		
		//Xprintf("\r\n");
	}
	m_pReadStart=0;	
}

//移动到第一个有用节点
bool CFIFOLoopBuffer::SeekToFirstValidReadNode(CFIFOLoopBuffer::BufferNode** pNode)
{
	bool b=false;	
	while(1)
	{
		CFIFOLoopBuffer::BufferNode* pTempNode=0;
		if(GetCurrentReadNode(&pTempNode) && pTempNode)
		{//获取当前读节点成功
			//当前读取节点可用
			if(pTempNode->bValid)
			{
				*pNode=pTempNode;
				b=true;
				break;
			}

			//当前读取节点的下一个节点数据可用
			if(pTempNode->pNext->bValid)
			{
				*pNode=pTempNode->pNext;
				b=true;
				break;
			}

			//当前读节点和当前读节点的下一个节点数据都不可用
			if(!MoveToNextReadNode())
			{
				break;
			}						
		}
		else
		{//获取当前读节点失败
			break;
		}
	}	
	return b;
}

//演示如何插入数据,高效用的时候不用这个函数插入数据.
//先获取节点,直接写入节点,然后MoveNext
bool	CFIFOLoopBuffer::InsertNodeData(const unsigned char* pData,const long nLen)
{
	bool b=false;
	if(pData && nLen>0)
	{
		CFIFOLoopBuffer::BufferNode* pNode=0;
		if(GetCurrentWriteNode(&pNode) && pNode && !pNode->bValid)
		{
			//check buffer
			if(pNode->nBufferLength<nLen)
			{
				/////////
				//remalloc
				if(pNode->pData)
				{
					delete []pNode->pData;
					pNode->pData=0;
					pNode->nBufferLength=0;
				}
				//增加内存
				pNode->pData=new unsigned char[nLen+4];
				if(pNode->pData)
				{
					pNode->nBufferLength=nLen+4;
				}
			}

			//copy data			
			if(pNode->pData)
			{
				memcpy(pNode->pData,pData,nLen);
				pNode->nDataLength=nLen;
				pNode->bValid=true;
				b=true;
				MoveToNextWriteNode();
			}
		}
		else
		{//无法获取节点或节点已有数据
			//b=false;
		}
	}
	return b;
}

//获取当前可读节点
inline bool CFIFOLoopBuffer::GetCurrentReadNode(CFIFOLoopBuffer::BufferNode** pNode)
{
	bool b=false;	
	if(pNode && m_pReadStart)
	{
		*pNode=m_pReadStart;
		b=true;				
	}
	return b;
}

//获取当前写节点
bool	CFIFOLoopBuffer::GetCurrentWriteNode(CFIFOLoopBuffer::BufferNode** pNode)
{
    bool b=false;
    if(pNode && m_pWriteStart)
    {
        if(m_pWriteStart->bValid)
        {//当前节点已有数据
            //尝试移动到下一个可写节点
            bool bNewNode=MoveToNextWriteNode();
            if(bNewNode)
            {
                b=!m_pWriteStart->bValid;
                if(b)
                {
                    *pNode=m_pWriteStart;
                }
            }
        }
        else
        {
            *pNode=m_pWriteStart;
            b=true;
        }		
    }
    return b;
}

//移动到下一个可读节点.如果下一个节点为当前写节点则失败
bool CFIFOLoopBuffer::MoveToNextReadNode()
{
	bool b=false;
	//当前读节点和当前写节点中间至少间隔一个节点	
	if(m_pReadStart && m_pWriteStart &&
		m_pReadStart->pNext->pNext!=m_pWriteStart)
	{
		m_pReadStart=m_pReadStart->pNext;
		b=true;		
	}
	return b;
}

bool	CFIFOLoopBuffer::MoveToNextWriteNode()
{
	bool b=false;
	//当前读节点和当前写节点中间至少间隔一个节点
	if(!m_pWriteStart->bValid)
	{//当前节点还没使用,别浪费
		b=true;
		return b;
	}

	if(m_pWriteStart && m_pReadStart)
	{
		if(m_pWriteStart->pNext->pNext==m_pReadStart)
		{
			b=false;
		}
		else
		{
			m_pWriteStart->pNext->bValid=false;
			m_pWriteStart=m_pWriteStart->pNext;
			b=true;
		}
	}
	return b;
}

//演示如何插入数据,高效用的时候不用这个函数插入数据.
//先获取节点,直接写入节点,然后MoveNext
bool	CFIFOLoopBuffer::GetNodeData(unsigned char* pData,long* nLen)
{
	bool b=false;
	if(pData && nLen && *nLen>0)
	{
		CFIFOLoopBuffer::BufferNode* pNode=0;
		if(GetCurrentReadNode(&pNode) && pNode && pNode->bValid)
		{
			if(pNode->pData && pNode->nDataLength<=*nLen)
			{
				memcpy(pData,pNode->pData,pNode->nDataLength);
				*nLen=pNode->nDataLength;
				//....do something
				pNode->bValid=false;//用完这个节点了

				//当前节点不想用了
				MoveToNextReadNode();
				b=true;
			}
		}
		else
		{
			//当前节点不可读取
			//b=false;
			MoveToNextReadNode();
		}
	}
	return b;
}
