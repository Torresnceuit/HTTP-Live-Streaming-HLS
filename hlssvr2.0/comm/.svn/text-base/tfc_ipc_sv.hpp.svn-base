
#ifndef _TFC_IPC_SV_HPP_
#define _TFC_IPC_SV_HPP_

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include <string>

#include "tfc_ex.hpp"
#include "tfc_heap_guardian.hpp"

namespace tfc{ namespace ipc{
	
//////////////////////////////////////////////////////////////////////////

static const int C_PROJECT_ID_TFC = 9527;
static const int C_DEFAULT_OPEN_FLAG = 0600;

class ipc_ex : public bt_ex { public : ipc_ex (const std::string& s) : bt_ex(s) {} };

//////////////////////////////////////////////////////////////////////////

class CShm
{
public:
	static ptr<CShm> open(key_t key, size_t size);
	static ptr<CShm> create_only(key_t key, size_t size);

	int		id() const	{return _id ;}
	key_t  key() const	{return _key;}
	size_t size()const	{return _size;}
	char* memory()		{return _mem;}

	char* attach();
	void  detach();

protected:
	friend class ptr<CShm>;
	CShm(key_t key, size_t size, int id);
	~CShm();

	const key_t _key;
	const size_t _size;
	const int _id;
	char* _mem;
};

//////////////////////////////////////////////////////////////////////////

class CSem
{
public:
	static ptr<CSem> open(key_t key, size_t size);
	static ptr<CSem> create_only(key_t key, size_t size);

	//	semctl + IPC_STAT
	bool if_raw();
	//	semctl + SETALL
	void init(unsigned short init_value = 1);
	//	semctl + SETVAL
	void set_value(unsigned index = 0, unsigned short init_value = 1);
	//	semctl + GETVAL
	int	 get_value(unsigned index = 0);
	//	semctl + IPC_RMID
	void destroy();

	int		id() const{return _id ;}
	key_t  key() const{return _key;}
	size_t size()const{return _size;}

	//	operation
	void post(unsigned index = 0);
	void wait(unsigned index = 0);
	bool trywait(unsigned index = 0);
	bool timewait(unsigned index, unsigned& sec, unsigned& nanosec);

protected:
	friend class ptr<CSem>;
	CSem(key_t key, unsigned n, int id);
	~CSem();

	struct CSemBuf
	{
		CSemBuf(unsigned short sem_num, short sem_op, short sem_flg)
		{
			_buf.sem_num = sem_num;
			_buf.sem_op = sem_op;
			_buf.sem_flg = sem_flg;
		}
		struct sembuf _buf;
	};

	//	semctl + IPC_STAT
	time_t last_operate_time();
	time_t create_time();
	
	const key_t _key;
	const unsigned _size;
	const int _id;
};

//////////////////////////////////////////////////////////////////////////
}}	//	namespace tfc::ipc
#endif//_TFC_IPC_SV_HPP_
///:~
