
#include "tfc_net_mem_pool.h"
#include <cassert>
#include <iostream>

using namespace std;
using namespace tfc::net;

static const unsigned C_EXTEND_SET = 128;
static const unsigned C_SLOT_1 = 1<<20;	 // 1M
static const unsigned C_SLOT_2 = 1<<26;	 // 64M
static const unsigned C_ALLOC_CAPCITY= 2*C_SLOT_2;	// 128M 


//////////////////////////////////////////////////////////////////////////

void* CMemPool::allocate(size_t size, size_t& result_size)
{
	//	first, get block size
	size_t block_size = fit_block_size(size);

	//	second, find the size pool
	mml::iterator it_list = _free.find(block_size);

	//	third, if size empty, extend for a list and a stub
	if (it_list == _free.end())
	{
		// changed by Joy.Wu 2006.10.23
		if (extend_new_size(block_size)!=0)
		{
			cerr << __FILE__ << __LINE__ << "alloc_size" << _allocated_size << endl;
			return NULL;
		}

		it_list = _free.find(block_size);
		if (it_list == _free.end())
		{
			cerr << __FILE__ << __LINE__ << "alloc_size" << _allocated_size << endl;
			return NULL;
		}
	}
	
	//	fourth, get from pool	
	list<void*>* p_list = it_list->second;
	list<void*>::iterator it_block = p_list->begin();
	if (it_block == p_list->end())
	{
		mms::iterator it_set = _stub.find(block_size);
		
		// changed by Joy.Wu 2006.10.23
		//if (!extend(block_size, p_list, it_set->second))
		if (extend(block_size, p_list, it_set->second) != 0)
		{
			cerr << __FILE__ << __LINE__ << "alloc_size" << _allocated_size << endl;
			return NULL;
		}

		it_block = p_list->begin();
		if (it_block == p_list->end())
		{
			cerr << __FILE__ << __LINE__ << "alloc_size" << _allocated_size << endl;
			return NULL;
		}
	}
	
	void* ret = *it_block;
	p_list->pop_front();
	result_size = block_size;

	//	return
	return ret;
}


int CMemPool::recycle(void* mem, size_t mem_size)
{
	// changed by Joy.Wu
	// 释放内存得时候，不需要调用fit_block_size
	//	first, check block size
	//size_t block_size = fit_block_size(mem_size);
	//if (mem_size != block_size)
	//	return -1;
	
	//	second, sure the mem in stub
	mms::iterator it_set = _stub.find(mem_size);
	if (it_set == _stub.end())
		return -1;

	set<void*>* p_set = it_set->second;
	set<void*>::iterator it_mem = p_set->find(mem);
	if (it_mem == p_set->end())
		return -1;

	//	third, add into free list
	mml::iterator it_list = _free.find(mem_size);
	if (it_list == _free.end())
		return -1;
	
	list<void*>* p_list = it_list->second;
	p_list->push_front(mem);
	
	release(mem_size, p_list, p_set);
	//	return
	return 0;
}


int CMemPool::release_size(size_t mem_size)
{
	// 水标位改为分配容积
	_water_mark = C_ALLOC_CAPCITY;

	unsigned purge_size = 1 << 31;
	unsigned last_alloc_size = _allocated_size;
	
	while(((last_alloc_size - _allocated_size) < mem_size)
		  && (purge_size>=C_SLOT_1))
	{
		purge_size = purge_size >> 1;
		mms::iterator it_set = _stub.find(purge_size);
		if (it_set == _stub.end())
			continue;
		
		set<void*>* p_set = it_set->second;

		mml::iterator it_list = _free.find(purge_size);
		if (it_list == _free.end())
			continue;

		list<void*>* p_list = it_list->second;
	
		release(purge_size, p_list, p_set);
	}

	// 水标位内存池的最大水标位

	_water_mark = C_MAX_WATER_MARK;
	if ((last_alloc_size - _allocated_size) < mem_size)
	{
		return -1;
	}

	//	return
	return 0;

}

inline size_t CMemPool::fit_extend_set(unsigned size)
{
	if ((size >= C_SLOT_1)&&(size < C_SLOT_2))
	{
		// >=C_SLOT_1 && <C_SLOT_2, 时候总分配容量为C_ALLOC_CAPCITY
		return C_ALLOC_CAPCITY/size;
	}

	if (size >= C_SLOT_2)
	{
		// > C_SLOT_2,分配一份
		return 1;
	}
	return C_EXTEND_SET;
}

int CMemPool::extend(size_t size, std::list<void*>* l, std::set<void*>* s)
{

    unsigned extend_set = fit_extend_set(size);	
	
	if (_allocated_size + size * extend_set >= C_MAX_POOL_SIZE)
	{
		if (release_size(_allocated_size + size * extend_set - C_MAX_POOL_SIZE ) != 0)
		{
			return -1;
		}
	}

	//	extend it
	for(unsigned i = 0; i < extend_set; i++)
	{
		void* p = new char[size];
		l->push_front(p);
		s->insert(p);
	}
	
	_allocated_size += size * extend_set;

	//	fourth, return
	return 0;
}

int CMemPool::extend_new_size(size_t size)
{
	list<void*>* l = new list<void*>;
	set<void*>* s = new set<void*>;
	
	_free.insert(mml::value_type(size, l));
	_stub.insert(mms::value_type(size, s));

	//	fourth, return
	return extend(size, l, s);
}

int CMemPool::release(size_t size,  list<void*>* l, set<void*>* s)
{
	//	release it
	unsigned todo_count = release_size(size, l->size(), s->size());
	if (todo_count == 0)
		return 0;
	
	assert(_allocated_size >= size * todo_count);

	//	assert(todo_count < l->size());
	for(unsigned i = 0; i < todo_count; i++)
	{
		void* p = l->front();
		char* cp = (char*) p;
		delete [] cp;
		l->pop_front();
	}

	_allocated_size -= size * todo_count;

	//	fourth, return
	return 0;
}

//
//	rule one, free 1/2, release 1/4
//	rule two, free 1024. release 512
//	rule three, free < 1M
//

unsigned CMemPool::release_size(size_t block_size, unsigned free_count, unsigned stub_count)
{
	if (free_count * block_size < (1<<20))
		return 0;

	if (block_size > C_SLOT_2)
	{
		// >C_SLOT_2
		// rule one free >1 ,release 1/2
		// rule two free =1, release 1
		return (free_count>1)?(free_count/2):free_count;
	}
	
	// 空闲空间大于水标位，开始释放
	if (free_count >= stub_count/2 || free_count*block_size >= _water_mark)
		return (free_count/2);

	return 0;
}

//////////////////////////////////////////////////////////////////////////
///:~
