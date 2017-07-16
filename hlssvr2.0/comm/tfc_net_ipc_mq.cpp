
#include "tfc_net_ipc_mq.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

using namespace tfc::net;
using namespace tfc::ipc;
using namespace std;

//////////////////////////////////////////////////////////////////////////

CShmMQ::CShmMQ(){}
CShmMQ::~CShmMQ(){}

int CShmMQ::init(int shm_key, unsigned shm_size)
{
	assert(shm_size > C_HEAD_SIZE);
	try
	{
		try
		{
			_shm = CShm::create_only(shm_key, shm_size);
			memset(_shm->memory(), 0, C_HEAD_SIZE);	//	set head and tail
		}
		catch (ipc_ex& ex)
		{
			_shm = CShm::open(shm_key, shm_size);
		}
	}
	catch (...)
	{
		return -1;
	}

	_head = (unsigned*)_shm->memory();
	_tail = _head+1;
	_block = (char*) (_tail+1);
	_block_size = shm_size - C_HEAD_SIZE;
	return 0;
}

int CShmMQ::enqueue(const void* data, unsigned data_len, unsigned flow)
{
	unsigned head = *_head;
	unsigned tail = *_tail;	
	unsigned free_len = head>tail? head-tail: head+_block_size-tail;
	unsigned tail_len = _block_size - tail;

	
	char sHead[C_HEAD_SIZE] = {0};
	unsigned total_len = data_len+C_HEAD_SIZE;

	//	first, if no enough space?
	if (free_len <= total_len)
		return -1;

	memcpy(sHead, &total_len, sizeof(unsigned));
	memcpy(sHead+sizeof(unsigned), &flow, sizeof(unsigned));

	//	second, if tail space > 8+len
	//	copy 8 byte, copy data
	if (tail_len >= total_len)
	{
		memcpy(_block+tail, sHead, C_HEAD_SIZE);
		memcpy(_block+tail+ C_HEAD_SIZE, data, data_len);
		*_tail += data_len + C_HEAD_SIZE;
	}

	//	third, if tail space > 8 && < 8+len
	else if (tail_len >= C_HEAD_SIZE && tail_len < C_HEAD_SIZE+data_len)
	{
		//	copy 8 byte
		memcpy(_block+tail, sHead, C_HEAD_SIZE);

		//	copy tail-8
		unsigned first_len = tail_len - C_HEAD_SIZE;
		memcpy(_block+tail+ C_HEAD_SIZE, data, first_len);

		//	copy left
		unsigned second_len = data_len - first_len;
		memcpy(_block, ((char*)data) + first_len, second_len);

        //modify by felixxie 2010 3.3
		int itmp = *_tail + data_len + C_HEAD_SIZE - _block_size;
		*_tail = itmp;

		//*_tail += data_len + C_HEAD_SIZE;
		//*_tail -= _block_size;
	}

	//	fourth, if tail space < 8
	else
	{
		//	copy tail byte
		memcpy(_block+tail, sHead, tail_len);

		//	copy 8-tail byte
		unsigned second_len = C_HEAD_SIZE - tail_len;
		memcpy(_block, sHead + tail_len, second_len);

		//	copy data
		memcpy(_block + second_len, data, data_len);
		
		// 此处原来没有修改尾指针，已修改 tomxiao 2006.12.10
		*_tail = second_len + data_len;
	}

    //fix me: 如果mq在enqueue和dequeue的时候都不加锁，那么有可能这里free_len的判断是不准确的，
    //因为在enqueue的同时，可能有另外的进程在dequeue，而且是dequeue最后一个包，此时enqueue的判断
    //不会是第一个包，但是当enqueue完成的时候，实际上应该是第一个包。
    //要解决这个问题，一定要有一个原子变量来辅助这个判断，但是这样又会增加维护此变量的代价
    if(free_len == _block_size)	//第一个包
        return 1;
    else						//非第一个包
        return 0;
}

int CShmMQ::dequeue(void* buf, unsigned buf_size, unsigned& data_len, unsigned& flow)
{
	unsigned head = *_head;
	unsigned tail = *_tail;
	if (head == tail)
	{
		data_len = 0;
		return 0;
	}
	unsigned used_len = tail>head ? tail-head : tail+_block_size-head;
	char sHead[C_HEAD_SIZE];
	
	//	if head + 8 > block_size
	if (head+C_HEAD_SIZE > _block_size)
	{
		unsigned first_size = _block_size - head;
		unsigned second_size = C_HEAD_SIZE - first_size;
		memcpy(sHead, _block + head, first_size);
		memcpy(sHead + first_size, _block, second_size);
		head = second_size;
	}
	else
	{
		memcpy(sHead, _block + head, C_HEAD_SIZE);
		head += C_HEAD_SIZE;
	}
	
	//	get meta data
	unsigned total_len  = *(unsigned*) (sHead);
	flow = *(unsigned*) (sHead+sizeof(unsigned));
	assert(total_len <= used_len);
	
	data_len = total_len-C_HEAD_SIZE;
	if (data_len > buf_size)
		return -1;

	if (head+data_len > _block_size)	//	
	{
		unsigned first_size = _block_size - head;
		unsigned second_size = data_len - first_size;
		memcpy(buf, _block + head, first_size);
		memcpy(((char*)buf) + first_size, _block, second_size);
		*_head = second_size;
	}
	else
	{
		memcpy(buf, _block + head, data_len);
		*_head = head+data_len;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////

CSemLockMQ::CSemLockMQ(CShmMQ& mq) : _mq(mq), _sem_index(0){}
CSemLockMQ::~CSemLockMQ(){}

static const int C_UNINIT_VALUE = (1<<16) - 1;

int CSemLockMQ::init(int sem_key, unsigned sem_size, unsigned sem_index)
{
	try
	{
		try
		{
			_sem = CSem::create_only(sem_key, sem_size);
			if (_sem->if_raw())
			{
				_sem->init(1);
				//_sem->set_value(sem_index, 1);
			}
		}
		catch (ipc_ex& ex)
		{
			_sem = CSem::open(sem_key, sem_size);
			if (_sem->get_value(sem_index) == C_UNINIT_VALUE)
				_sem->set_value(sem_index, 1);
		}
	}
	catch (...)
	{
		return -1;
	}
	_sem_index = sem_index;
	return 0;
}

int CSemLockMQ::enqueue(const void* data, unsigned data_len, unsigned flow)
{
	//_sem->wait(_sem_index);
	int ret = _mq.enqueue(data, data_len, flow);
	//_sem->post(_sem_index);
	return ret;
}

int CSemLockMQ::dequeue(void* buf, unsigned buf_size, unsigned& data_len, unsigned& flow)
{
	//_sem->wait(_sem_index);
	int ret = _mq.dequeue(buf, buf_size, data_len, flow);
	//_sem->post(_sem_index);
	return ret;
}

//////////////////////////////////////////////////////////////////////////

CFifoSyncMQ::CFifoSyncMQ(CSemLockMQ& mq) : _mq(mq), _fd(-1), _count(0)
{
}
CFifoSyncMQ::~CFifoSyncMQ(){}

int CFifoSyncMQ::init(const std::string& fifo_path, unsigned wait_sec, unsigned wait_usec)
{
    m_oAdaptiveNotifyCtrl.init(time(NULL), 3);

	_wait_sec = wait_sec;
	_wait_usec = wait_usec;

    int mode = 0666 | O_NONBLOCK | O_NDELAY;
    
	errno = 0;
    if ((mkfifo(fifo_path.c_str(), mode)) < 0)
    {
		if (errno != EEXIST)
			return -1;
    }

	if (_fd != -1)
	{
		close(_fd);
		_fd = -1;
	}

    if ((_fd = open(fifo_path.c_str(), O_RDWR)) < 0)
    {
		return -1;
    }
	if (_fd > 1024)
	{
		close(_fd);
		return -1;
	}
    
	int val = fcntl(_fd, F_GETFL, 0);
	
	if (val == -1)
		return errno ? -errno : val;
	
	if (val & O_NONBLOCK)
		return 0;
	
	int ret = fcntl(_fd, F_SETFL, val | O_NONBLOCK | O_NDELAY);
	return (ret < 0) ? (errno ? -errno : ret) : 0;
    return 0;
}

int CFifoSyncMQ::enqueue(const time_t uiCurTime, const void* data, unsigned data_len, unsigned flow)
{
    _count++;
    int ret = _mq.enqueue(data, data_len, flow);
    if(ret < 0)
        return ret;

    m_oAdaptiveNotifyCtrl.AddLoad(uiCurTime, 1);
    unsigned int uiFactor = m_oAdaptiveNotifyCtrl.GetNotifyFactor();
	
    if (0 == uiFactor)
    {
        uiFactor = 5;
    }
    else if (uiFactor > 100)
    {
        uiFactor = 100;
    }
    
    if (0 == _count%uiFactor)
    {
        //if (0 == ret)
        {
            errno = 0;
            ret = write(_fd, "\0", 1);
        }       
    }
    
    
    //	if (ret < 0 && errno != EAGAIN)
    //		return -1;
    return 0;
}

int CFifoSyncMQ::dequeue(void* buf, unsigned buf_size, unsigned& data_len, unsigned& flow)
{
	//	first, try to get data from Q
	int ret = _mq.dequeue(buf, buf_size, data_len, flow);
	if (ret || data_len)
		return ret;

	//	second, if no data, wait on fifo
	ret = select_fifo();
	if (ret == 0)
	{
		data_len = 0;
		return ret;
	}
	else if (ret < 0)
	{
		return -1;
	}

	//	third, if fifo activated, read the signals
	static const unsigned buf_len = 1<<10;
	char buffer[buf_len];
	ret = read(_fd, buffer, buf_len);
	if (ret < 0 && errno != EAGAIN)
		return -1;
	
	//	fourth, get data
	return _mq.dequeue(buf, buf_size, data_len, flow);
}

int CFifoSyncMQ::try_dequeue(void* buf, unsigned buf_size, unsigned& data_len, unsigned& flow)
{
    return _mq.dequeue(buf, buf_size, data_len, flow);
}

int CFifoSyncMQ::select_fifo()
{
	errno = 0;
    fd_set readfd;
    FD_ZERO(&readfd);
    FD_SET(_fd, &readfd);
    struct timeval tv;
    tv.tv_sec = _wait_sec;
    tv.tv_usec = _wait_usec;
    
    int ret = select(_fd+1, &readfd, NULL, NULL, &tv);
    if(ret > 0)
    {
        if(FD_ISSET(_fd, &readfd))
			return ret;
		else
			return -1;
    }
	else if (ret == 0)
	{
		return 0;
	}
	else
	{
		// select函数可能被USR信号中断，此时不应该close。 tomxiao 2006.10.26. 
		if (errno != EINTR)
		{
			close(_fd);
		}
		return -1;
	}
}

void CFifoSyncMQ::clear_flag() {
    static char buffer[1];
    read(_fd, buffer, 1);
}
//////////////////////////////////////////////////////////////////////////
///:~
