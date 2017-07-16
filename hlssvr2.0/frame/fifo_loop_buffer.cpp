//////////////////////////////////////////////////////////////////////
// FIFOLoopMsgBuffer.cpp: implementation of the CFIFOLoopMsgBuffer class.
// �Ƚ��ȳ�ѭ��buffer,���ü���һ���߳�д��,һ���̶߳�ȡ
//
//    >>>>>>>>>>>>>>>------------read pointer
//    ^             v
//    ^<<<<<<<<<<<<<v------------write Pointer
//    read pointer������ǰԽλwrite pointer
//    write pointer������ǰԽλread pointer
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
	//�γ�һ����,pwrite pointer��pRead pointerǰ��һ��
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

//�ƶ�����һ�����ýڵ�
bool CFIFOLoopBuffer::SeekToFirstValidReadNode(CFIFOLoopBuffer::BufferNode** pNode)
{
	bool b=false;	
	while(1)
	{
		CFIFOLoopBuffer::BufferNode* pTempNode=0;
		if(GetCurrentReadNode(&pTempNode) && pTempNode)
		{//��ȡ��ǰ���ڵ�ɹ�
			//��ǰ��ȡ�ڵ����
			if(pTempNode->bValid)
			{
				*pNode=pTempNode;
				b=true;
				break;
			}

			//��ǰ��ȡ�ڵ����һ���ڵ����ݿ���
			if(pTempNode->pNext->bValid)
			{
				*pNode=pTempNode->pNext;
				b=true;
				break;
			}

			//��ǰ���ڵ�͵�ǰ���ڵ����һ���ڵ����ݶ�������
			if(!MoveToNextReadNode())
			{
				break;
			}						
		}
		else
		{//��ȡ��ǰ���ڵ�ʧ��
			break;
		}
	}	
	return b;
}

//��ʾ��β�������,��Ч�õ�ʱ�������������������.
//�Ȼ�ȡ�ڵ�,ֱ��д��ڵ�,Ȼ��MoveNext
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
				//�����ڴ�
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
		{//�޷���ȡ�ڵ��ڵ���������
			//b=false;
		}
	}
	return b;
}

//��ȡ��ǰ�ɶ��ڵ�
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

//��ȡ��ǰд�ڵ�
bool	CFIFOLoopBuffer::GetCurrentWriteNode(CFIFOLoopBuffer::BufferNode** pNode)
{
    bool b=false;
    if(pNode && m_pWriteStart)
    {
        if(m_pWriteStart->bValid)
        {//��ǰ�ڵ���������
            //�����ƶ�����һ����д�ڵ�
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

//�ƶ�����һ���ɶ��ڵ�.�����һ���ڵ�Ϊ��ǰд�ڵ���ʧ��
bool CFIFOLoopBuffer::MoveToNextReadNode()
{
	bool b=false;
	//��ǰ���ڵ�͵�ǰд�ڵ��м����ټ��һ���ڵ�	
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
	//��ǰ���ڵ�͵�ǰд�ڵ��м����ټ��һ���ڵ�
	if(!m_pWriteStart->bValid)
	{//��ǰ�ڵ㻹ûʹ��,���˷�
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

//��ʾ��β�������,��Ч�õ�ʱ�������������������.
//�Ȼ�ȡ�ڵ�,ֱ��д��ڵ�,Ȼ��MoveNext
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
				pNode->bValid=false;//��������ڵ���

				//��ǰ�ڵ㲻������
				MoveToNextReadNode();
				b=true;
			}
		}
		else
		{
			//��ǰ�ڵ㲻�ɶ�ȡ
			//b=false;
			MoveToNextReadNode();
		}
	}
	return b;
}
