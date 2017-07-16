
#ifndef _TFC_NET_IPC_MQ_H_
#define _TFC_NET_IPC_MQ_H_

#include "tfc_ipc_sv.hpp"	//	use tfc_ipc_sv temporarily, in a month

//////////////////////////////////////////////////////////////////////////

//
//	
//

namespace tfc{namespace net
{
	static const int E_DEQUEUE_BUF_NOT_ENOUGH = -13001;
	class CShmMQ
	{
	public:
		typedef struct tagMQStat
		{
			unsigned _used_len;
			unsigned _free_len;
			unsigned _total_len;
			unsigned _shm_key;
			unsigned _shm_id;
			unsigned _shm_size;
		}TMQStat;

	public:
		CShmMQ();
		~CShmMQ();
		int init(int shm_key, unsigned shm_size);
		int enqueue(const void* data, unsigned data_len, unsigned flow);
		int dequeue(void* buf, unsigned buf_size, unsigned& data_len, unsigned& flow);
		
		void get_stat(TMQStat& mq_stat)
		{
			unsigned head = *_head;
			unsigned tail = *_tail;
			
			mq_stat._used_len = (tail>=head) ? tail-head : tail+_block_size-head;
			mq_stat._free_len = head>tail? head-tail: head+_block_size-tail;
			mq_stat._total_len = _block_size;
			mq_stat._shm_key = _shm->key();
			mq_stat._shm_id = _shm->id();
			mq_stat._shm_size = _shm->size();
		}

	private:
		tfc::ptr<tfc::ipc::CShm> _shm;

		unsigned* _head;
		unsigned* _tail;
		char* _block;
		unsigned _block_size;

		static const unsigned C_HEAD_SIZE = 8;
	};

	class CSemLockMQ
	{
	public:
		typedef struct tagSemLockMQStat
		{
			CShmMQ::TMQStat _mq_stat;
			unsigned _sem_key;
			unsigned _sem_id;
			unsigned _sem_index;
			unsigned _sem_size;
		}TSemLockMQStat;

	public:
		CSemLockMQ(CShmMQ& mq);
		~CSemLockMQ();
		
		int init(int sem_key, unsigned sem_size, unsigned sem_index);
		int enqueue(const void* data, unsigned data_len, unsigned flow);
		int dequeue(void* buf, unsigned buf_size, unsigned& data_len, unsigned& flow);
		void get_stat(TSemLockMQStat& mq_stat)
		{
			_mq.get_stat(mq_stat._mq_stat);
			mq_stat._sem_key = _sem->key();
			mq_stat._sem_id = _sem->id();
			mq_stat._sem_size = _sem->size();
			mq_stat._sem_index = _sem_index;
		}
		
	private:
		CShmMQ& _mq;
		tfc::ptr<tfc::ipc::CSem> _sem;
		unsigned _sem_index;
	};
	

    class CAdaptiveNotifyCtrl
    {
    public:
        CAdaptiveNotifyCtrl()
        {
            m_uiCheckTimeSpan = 0;
            m_uiMsgCount = 0;
            m_uiLastMsgCount = 0;
            m_uiLastCheckTime = 0;
			m_uiFactor = 1;
        }

        int init(const time_t uiCurTime, const unsigned int uiCheckTimeSpan)
        {
            m_uiCheckTimeSpan = uiCheckTimeSpan;

            m_uiMsgCount = 0;
            m_uiLastCheckTime = uiCurTime;
			m_uiFactor = 1;

            return 0;
        }

        int AddLoad(const time_t uiCurTime, const unsigned int uiMsgCount)
        {
            if (uiCurTime < m_uiLastCheckTime + (int)m_uiCheckTimeSpan)
            {
                m_uiMsgCount = m_uiMsgCount + uiMsgCount;
            }
            else
            {
                m_uiLastCheckTime = uiCurTime;
                m_uiLastMsgCount = m_uiMsgCount;
                m_uiMsgCount = uiMsgCount;

				m_uiFactor = GenNotifyFactor(m_uiLastMsgCount);
            }  
            return 0;
        }

        int GetNotifyFactor()
        {
            return m_uiFactor;
        }


    private:
        //生成通知因子
        int GenNotifyFactor(const unsigned int uiMsgCount)
        {
            if (uiMsgCount <= 5000)
            {
                return 1;
            }
            else if (uiMsgCount <= 10000)
            {
                return 3;
            }
            else if (uiMsgCount <= 20000)
            {
                return 6;
            }
            else if (uiMsgCount <= 30000)
            {
                return 9;
            }
            else if (uiMsgCount <= 40000)
            {
                return 15;
            }
            else if (uiMsgCount <= 50000)
            {
                return 24;
            }
            else if (uiMsgCount <= 60000)
            {
                return 39;
            }
            else if (uiMsgCount <= 70000)
            {
                return 63;
            }
            else if (uiMsgCount <= 80000)
            {
                return 73;
            }
            else if (uiMsgCount <= 90000)
            {
                return 83;
            }
            else if (uiMsgCount <= 100000)
            {
                return 93;
            }

            return 100;
                
        }
    public:
        unsigned int  m_uiCheckTimeSpan;            //检测时间间隔,单位秒
        unsigned int  m_uiMsgCount;                 //消息数目
        unsigned int  m_uiLastMsgCount;             //上一周期总消息个数
        time_t        m_uiLastCheckTime;            //上次检查时间,时间秒

        unsigned int  m_uiFactor;                   //通知因子
    };

	class CFifoSyncMQ
	{
	public:
		typedef struct tagFifoSyncMQStat
		{
			CSemLockMQ::TSemLockMQStat _semlockmq_stat;
			unsigned _wait_sec;
			unsigned _wait_usec;
		}TFifoSyncMQStat;

	public:
		CFifoSyncMQ(CSemLockMQ& mq);
		~CFifoSyncMQ();
		
		int init(const std::string& fifo_path, unsigned wait_sec, unsigned wait_usec);
		int enqueue(const time_t uiCurTime, const void* data, unsigned data_len, unsigned flow);
		int dequeue(void* buf, unsigned buf_size, unsigned& data_len, unsigned& flow);
        int try_dequeue(void* buf, unsigned buf_size, unsigned& data_len, unsigned& flow);

        //当被epoll或者select等激活读的时候需要调用此函数
        void clear_flag();

		void get_stat(TFifoSyncMQStat& mq_stat)
		{
			_mq.get_stat(mq_stat._semlockmq_stat);
			mq_stat._wait_sec = _wait_sec;
			mq_stat._wait_usec = _wait_usec;
		}
		int fd(){ return _fd; };
		
	public:
		int select_fifo();

		CSemLockMQ& _mq;
		int _fd;
		unsigned _wait_sec;
		unsigned _wait_usec;

        unsigned _count;

        CAdaptiveNotifyCtrl m_oAdaptiveNotifyCtrl;
	};
}}

//////////////////////////////////////////////////////////////////////////
#endif//_TFC_NET_IPC_MQ_H_
///:~
