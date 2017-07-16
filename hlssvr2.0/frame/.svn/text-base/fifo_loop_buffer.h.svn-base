// FIFOLoopBuffer.h: interface for the CFIFOLoopBuffer class.

//////////////////////////////////////////////////////////////////////
#ifndef _FIFOLOOPBUFFER_H_
#define _FIFOLOOPBUFFER_H_

//适合数据FIFO队列使用
//用链表实现首尾相连循环队列
//先进先出
//一个线程写入数据,一个线程读取数据,不用加锁
class CFIFOLoopBuffer
{	
 public:
	
#pragma  pack(1)
     typedef struct tagBufferNode
     {
         volatile bool			bValid;
         unsigned char*			pData;
         long					nBufferLength;
         long					nDataLength;//data length<=nBuffer length		
         struct tagBufferNode*	pNext;
     }BufferNode;
#pragma pack()
	
 public:
     CFIFOLoopBuffer(int nMaxNodes);
     virtual ~CFIFOLoopBuffer();
	
     ///////////////
     //write thread
     //获取当前写节点
     bool	GetCurrentWriteNode(CFIFOLoopBuffer::BufferNode** pNode);
     //移动到下一个可写节点,如果无可写节点则失败.如果有可写节点则返回之前将节点则为无效
     bool	MoveToNextWriteNode();

     //演示如何插入数据,高效用的时候不用这个函数插入数据.
     //先获取节点,直接写入节点,然后MoveNext
     bool	InsertNodeData(const unsigned char* pData,const long nLen);
     ///////////////

     //bool	InsertNodeData(long msg,long wParam,long lParam);
	
     //////////////
     //read thread
     //移动到当前第一个有效读节点
     bool SeekToFirstValidReadNode(CFIFOLoopBuffer::BufferNode** pNode);
     //获取当前可读节点
     inline bool GetCurrentReadNode(CFIFOLoopBuffer::BufferNode** pNode);
     //移动到下一个可读节点.如果下一个节点为当前写节点则失败
     bool MoveToNextReadNode();

     //演示如何获取数据,高效用的时候不用这个函数获取数据
     //先获取节点,直接读取节点,然后MoveNext
     bool	GetNodeData(unsigned char* pData,long* nLen);

     //bool	GetNodeData(long* nMsg,long* wParam,long* lParam);
     //////////////
      void CleanNodes();    
 private:

     BufferNode*	m_pReadStart;
     BufferNode*    m_pWriteStart;
     int			m_nMaxNodes;
};

#endif
