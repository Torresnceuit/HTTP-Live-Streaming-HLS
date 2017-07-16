
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "tfc_str.hpp"
#include "tfc_ipc_sv.hpp"

namespace tfc { namespace ipc{

//////////////////////////////////////////////////////////////////////////

CShm::CShm(key_t key, size_t size, int id)
: _key(key), _size(size), _id(id), _mem(NULL)
{}

CShm::~CShm(){detach();}

ptr<CShm> CShm::open(key_t key, size_t size)
{
    int id = shmget(key, size, C_DEFAULT_OPEN_FLAG);
	if (id < 0)
		throw ipc_ex(strerror(errno));
	
	ptr<CShm> shm = new CShm(key, size, id);
	shm->_mem = shm->attach();
	return shm;
}

ptr<CShm> CShm::create_only(key_t key, size_t size)
{
    int id = shmget(key, size, C_DEFAULT_OPEN_FLAG | IPC_CREAT | IPC_EXCL);
	if (id < 0)
		throw ipc_ex(strerror(errno));
	
	ptr<CShm> shm = new CShm(key, size, id);
	shm->_mem = shm->attach();
	return shm;
}

char* CShm::attach()
{
	char* p = (char*) shmat(_id, NULL, 0);
	if (int(p) == -1)
		throw ipc_ex(strerror(errno));
	
	_mem = p;
	return p;
}

void CShm::detach()
{
	if (_mem == NULL)
		return;
	
	int ret = ::shmdt(_mem);
	if (ret < 0)
		throw ipc_ex(strerror(errno));
}

//////////////////////////////////////////////////////////////////////////

CSem::CSem(key_t key, unsigned size, int id): _key(key), _size(size), _id(id){}

CSem::~CSem(){}

ptr<CSem> CSem::open(key_t key, size_t size)
{
	int id = semget(key, size, C_DEFAULT_OPEN_FLAG);
	if (id < 0)
		throw ipc_ex(strerror(errno));

	ptr<CSem> sem = new CSem(key, size, id);
	return sem;
}

ptr<CSem> CSem::create_only(key_t key, size_t size)
{
	int id = semget(key, size, C_DEFAULT_OPEN_FLAG | IPC_CREAT | IPC_EXCL);
	if (id < 0)
		throw ipc_ex(strerror(errno));
	
	ptr<CSem> sem = new CSem(key, size, id);
	return sem;
}

bool CSem::if_raw()
{
	time_t t = last_operate_time();
	return t == 0;
}

void CSem::destroy()
{
	int ret = semctl(_id, 0, IPC_RMID);
	if (ret)
		throw ipc_ex("fail on remove semaphore: " + to_str(_id) + strerror(errno));
}

void CSem::init(unsigned short init_value /* = 1 */)
{
	unsigned short* init_array = new unsigned short[_size];
	for(unsigned i = 0; i < _size; i++)
		init_array[i] = init_value;

	int ret = semctl(_id, 0, SETALL, init_array);
	delete [] init_array;
	if (ret)
		throw ipc_ex("fail on semctl SETALL: " + to_str(_id) + strerror(errno));
}

void CSem::set_value(unsigned index /* = 0 */, unsigned short init_value /* = 1 */)
{
	int ret = semctl(_id, index, SETVAL, init_value);
	if (ret)
		throw ipc_ex("fail on semctl SETVAL: " + to_str(_id) + strerror(errno));
}

int CSem::get_value(unsigned index /* = 0 */)
{
	int ret = semctl(_id, index, GETVAL);
	if (ret < 0)
		throw ipc_ex("fail on semctl GETVAL: " + to_str(_id) + strerror(errno));
	else
		return ret;
}

void CSem::post(unsigned index /* = 0 */)
{
	int ret = semop(_id, &(CSemBuf(index, 1, SEM_UNDO)._buf), 1);
	if (ret)
		throw ipc_ex("fail on semop post: " + to_str(_id) + strerror(errno));
}

void CSem::wait(unsigned index /* = 0 */)
{
	static const unsigned C_LOOP_LIMIT = 5;
	for(unsigned i = 0; i < C_LOOP_LIMIT; i++)
	{
		errno = 0;
		int ret = semop(_id, &(CSemBuf(index, -1, SEM_UNDO)._buf), 1);
		if (ret)
		{
			if (errno != EINTR)
				throw ipc_ex("fail on semop wait: " + to_str(_id) + strerror(errno));
		}
		else
		{
			return ;
		}
	}
	throw ipc_ex("fail on semop wait: " + to_str(_id) + strerror(errno));
}

bool CSem::trywait(unsigned index /* = 0 */)
{
	errno = 0;
	int ret = semop(_id, &(CSemBuf(index, -1, SEM_UNDO | IPC_NOWAIT)._buf), 1);
	if (ret)
	{
		if (errno != EAGAIN)
			throw ipc_ex("fail on semop wait: " + to_str(_id) + strerror(errno));
		else
			return false;
	}
	else
	{
		return true;
	}
}

bool CSem::timewait(unsigned index, unsigned& sec, unsigned& nanosec)
{
	struct timespec ts;
	ts.tv_sec = sec;
	ts.tv_nsec = nanosec;
	
	static const unsigned C_LOOP_LIMIT = 5;
	for(unsigned i = 0; i < C_LOOP_LIMIT; i++)
	{
		errno = 0;
		int ret = semtimedop(_id, &(CSemBuf(index, -1, SEM_UNDO)._buf), 1, &ts);
		if (ret)
		{
			if (errno == EAGAIN)
			{
				return false;
			}
			else if (errno == EINTR)
			{
				continue;
			}
			else
			{
				throw ipc_ex("fail on semop wait: " + to_str(_id) + strerror(errno));
			}
		}
		else
		{
			return true;
		}
	}
	return false;
}

time_t CSem::last_operate_time()
{
	struct semid_ds buf;
	int ret = semctl(_id, 0, IPC_STAT, &buf);
	if (ret < 0)
		throw ipc_ex("fail on semctl GETVAL: " + to_str(_id) + strerror(errno));
	return buf.sem_otime;
}

time_t CSem::create_time()
{
	struct semid_ds buf;
	int ret = semctl(_id, 0, IPC_STAT, &buf);
	if (ret < 0)
		throw ipc_ex("fail on semctl GETVAL: " + to_str(_id) + strerror(errno));
	return buf.sem_ctime;
}

//////////////////////////////////////////////////////////////////////////
}}	//	namespace tfc::ipc
///:~
