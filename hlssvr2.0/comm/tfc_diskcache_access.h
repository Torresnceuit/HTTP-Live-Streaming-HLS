
#ifndef _TFC_DISKCACHE_ACCESS_H_
#define _TFC_DISKCACHE_ACCESS_H_

#include "tfc_diskcache_hash_map.h"
#include "tfc_binlog.hpp"

//////////////////////////////////////////////////////////////////////////

namespace tfc{namespace diskcache
{
	
	class CacheAccess
	{
	public:
		CacheAccess()
		{
			_lastdumptime = time(NULL);
			_cache_dump_min= -1;
			_cache_dump_point = 0;
			strncpy(_cache_dump_file,"./cache.dump",255);
			_cacheinit = true;
		}
		~CacheAccess(){}
		
		int open(char* mem, unsigned mem_size, const std::string& cache_file, bool inited
			, int node_total, int bucket_size, int n_chunks, int chunk_size);
		
		int set(const char* key, const char* data, unsigned data_len, ENodeFlag flag=NODE_FLAG_DIRTY);
		int set(const char* key, const char* data, unsigned data_len, int expiretime, ENodeFlag flag=NODE_FLAG_DIRTY);
		int del(const char* key);
		int get(const char* key, char* buf, unsigned buf_size
			, unsigned& data_len, bool& dirty_flag, int& time_stamp);
		int get(const char* key, char* buf, unsigned buf_size
			, unsigned& data_len, bool& dirty_flag, int& time_stamp, int& expiretime);

		int get_key(const char* key, unsigned& data_len, bool& dirty_flag, int& time_stamp);
		int mark_clean(const char* key);
		
		int get_lru_pre(const char* key, char* pre_key
			, char* buf, unsigned buf_size
			, unsigned& data_len, bool& dirty_flag, int& time_stamp);
		int oldest(char* buf, unsigned buf_size
			, char* key, unsigned& data_len, bool& dirty_flag, int& time_stamp);
		int oldest_key(char* key, unsigned& data_len, bool& dirty_flag, int& time_stamp);
		int oldest_key(char* key, unsigned& data_len, bool& dirty_flag, int& time_stamp, int& expiretime);

		int mark_clean(int modbase, int mobres);
		int del_node(int modbase, int mobres);


		int core_dump_mem(char *buff, int maxsize,int modbase, int mobres);
		int core_recover_mem(char *buff, int buffsize);
		int core_dump(char *szcorefile);
		int core_recover(char *szcorefile);
		int StartUp();
		int CoreInit(int coredump_min, unsigned coredump_point,
				char *coredump_file,char * sBinLogBaseName, 
				long lMaxBinLogSize, int iMaxBinLogNum);

		int time_check();

		// check if need dump
		bool is_need_dump();
		// init _last_dump_time
		void init_lastdumptime();

		// for mirror dump
		int mirror_dump(char *mirror_file);
		int mirror_recover(char *mirror_file);
		int StartUp_mirror();
		// ret 0: 进行了dump
		//     1: 没有进行dump
		int time_check_mirror();

		void get_memory(char **mem, unsigned& mem_size)
		{
			*mem = _mem;
			mem_size = _mem_size;
		};
		// 获得是否新建共享内存的标志
		bool is_new_init(){ return _cacheinit; };
		void get_node_num(unsigned &hash_node_used, unsigned &hash_node_total,
							 unsigned &chunk_node_used, unsigned &chunk_node_total);
		float get_used_percent();
        void  get_chunk_useinfo(unsigned& uiChunkTotal, unsigned& uiChunkFree, unsigned& uiChunkSize);
		
		/////////////////////////////////
		enum
		{
			op_set = 0,
			op_mark_clean,
			op_del
		};
		
	class dirty_iterator
	{
	public:
		dirty_iterator(const dirty_iterator& right)
			: _cache(right._cache)
			, _cur_node(right._cur_node)
		{}
		dirty_iterator& operator ++(int)
		{
			if (_cur_node == NULL)
				return *this;	//	error tolerance
			
			for(_cur_node = _cache->get_add_list_next(_cur_node)
				; _cur_node != NULL && _cur_node->flag_ == NODE_FLAG_UNCHG
				; _cur_node = _cache->get_add_list_next(_cur_node))
				;
			
			return *this;
		}
		bool operator ==(const dirty_iterator& right)
		{return _cur_node == right._cur_node;}
		bool operator !=(const dirty_iterator& right)
		{return _cur_node != right._cur_node;}
		int get(char* key, char* buf, unsigned buf_size, unsigned& data_len)
		{
			if (_cur_node == NULL)
				return -1;

			memcpy(key,_cur_node->key_._.md5_,TMBHashKey::C_DATA_LEN);
			data_len = buf_size;
			return  _cache->merge_node_data(_cur_node, buf, (int*)&data_len);
		}
	
	protected:
		dirty_iterator(CHashMap* cache, THashNode* cur_node)
			: _cache(cache)
			, _cur_node(cur_node)
		{}
		CHashMap * const _cache;
		THashNode* _cur_node;

	private:
		dirty_iterator& operator ++();
		void* operator->();
		int operator* ();
		friend class CacheAccess;
	};

	dirty_iterator begin(){return dirty_iterator(&_cache, _cache.get_add_list_head());}
	dirty_iterator end(){return dirty_iterator(&_cache, NULL);}
	
		CHashMap _cache;
		
		time_t _lastdumptime;
		int _cache_dump_min;
		unsigned _cache_dump_point;
		char _cache_dump_file[256];

		CBinLog _binlog;

		bool _cacheinit;
		char *_mem;
		unsigned _mem_size;
	};
	
	//////////////////////////////////////////////////////////////////////////
	
	class CacheAccessUin
	{
	public:
		CacheAccessUin(CacheAccess& da) : _da(da){}
		~CacheAccessUin(){}
		
		int set(unsigned uin, const char* data, unsigned data_len,ENodeFlag flag=NODE_FLAG_DIRTY);
		int del(unsigned uin);
		int get(unsigned uin, char* buf, unsigned buf_size
			, unsigned& data_len, bool& dirty_flag, int& time_stamp);
		int get_key(unsigned uin, unsigned& data_len, bool& dirty_flag, int& time_stamp);
		int mark_clean(unsigned uin);

		int get_lru_pre(unsigned key, unsigned& pre_key
			, char* buf, unsigned buf_size
			, unsigned& data_len, bool& dirty_flag, int& time_stamp);
		int oldest(char* buf, unsigned buf_size
			, unsigned& uin, unsigned& data_len, bool& dirty_flag, int& time_stamp);
		int oldest_key(unsigned& uin, unsigned& data_len, bool& dirty_flag, int& time_stamp);

		int mark_clean(int modbase, int mobres);
		int del_node(int modbase, int mobres);

		//diskcache的以下函数不要使用
		int core_dump_mem(char *buff, int maxsize,int modbase, int mobres);
		int core_recover_mem(char *buff, int buffsize);
		int core_dump(char *szcorefile);
		int core_recover(char *szcorefile);
		int CoreInit(int coredump_min, unsigned coredump_point, char *coredump_file,
			char * sBinLogBaseName, long lMaxBinLogSize, int iMaxBinLogNum);

		int StartUp();

		int time_check();
		void get_node_num(unsigned &hash_node_used, unsigned &hash_node_total,
							 unsigned &chunk_node_used, unsigned &chunk_node_total);
		float get_used_percent();		
	
		CacheAccess& _da;
	};
}}

//////////////////////////////////////////////////////////////////////////
#endif//_TFC_DISKCACHE_ACCESS_H_
///:~
