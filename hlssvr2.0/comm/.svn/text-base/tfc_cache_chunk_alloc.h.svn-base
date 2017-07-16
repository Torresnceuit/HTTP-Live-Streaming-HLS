
#ifndef _CHUNK_ALLOC_HPP_
#define _CHUNK_ALLOC_HPP_

//预分配内存区的数据存储分片类

//#include "comm.hpp"

namespace tfc{namespace cache{
	
#pragma pack(1)
	
	typedef int BC_MEM_HANDLER;
	const BC_MEM_HANDLER INVALID_BC_MEM_HANDLER = -1;
#ifndef NULL
#define NULL 0
#endif
	
	typedef struct tagTChunkNode
	{
		BC_MEM_HANDLER next_;     //CHUNK分片后指针
		char data_[1];              //数据区
	}TChunkNode;
	
	typedef struct tagTChunk
	{
		int chunk_total_;           //CHUNK总节点数
		int chunk_size_;            //CHUNK数据片尺寸
		int free_total_;            //空闲CHUNK总数
		BC_MEM_HANDLER free_list_;    //空闲CHUNK链表头指针
		TChunkNode chunk_nodes_[1]; //CHUNK数组
	}TChunk;
	
#pragma pack()
	
	
	class CChunkAllocator 
	{
	public:
		enum CHUNK_ALLOCATOR_ERROR
		{
			CHUNK_ALLOCATOR_ERROR_BASE = -1,    
				CHUNK_ALLOCATOR_ERROR_INVALID_PARAM = CHUNK_ALLOCATOR_ERROR_BASE -1,    //非法参数
				CHUNK_ALLOCATOR_ERROR_FREE_CHUNK_LACK = CHUNK_ALLOCATOR_ERROR_BASE -2,    //空闲内存块不足
				CHUNK_ALLOCATOR_ERROR_DATA_VERIFY_FAIL = CHUNK_ALLOCATOR_ERROR_BASE -3,    //内存数据检查失败
		};
		
	public:
		//初始化CHUNK 内存块    
		int open(char *pool,bool init, int n_chunks, int chunk_size);
		
		//从空闲链表中分配CHUNK.  
		BC_MEM_HANDLER malloc (int chunk_num=1);
		
		//将CHUNK插入到空闲链表中.
		void free(BC_MEM_HANDLER head_chunk_hdr);
		
		//将CHUNK中的数据分片进行组合
		int merge(BC_MEM_HANDLER chunk_node_hdr, int chunk_len,
			void* data_buf, int* data_len);
        
		//将数据分片存在到各CHUNK中.
		void split(BC_MEM_HANDLER head_hdr, const void* data_buf, int data_len);        
		
		//将偏移量转换成真实的内存地址
		TChunkNode *handler2ptr(BC_MEM_HANDLER handler);
		
		//将内存地址转换成偏移量
		BC_MEM_HANDLER ptr2handler(TChunkNode* ptr);
		
		//计算需要多少CHUNK进行数据存储. 
		int get_chunk_num(int data_len);    
		
		//返回chunk总数
		int get_chunk_total() { return chunk_->chunk_total_; }
		
		//返回使用的CHUNK数
		int get_used_chunk_num() { return (chunk_->chunk_total_ - chunk_->free_total_); };
		
		//返回空闲的CHUNK数
		int get_free_chunk_num() { return chunk_->free_total_; };
		
		//计算CHUNK 的内存块尺寸
		static int get_chunk_size(int n_chunks, int chunk_size)
		{
			return (sizeof(TChunkNode) - sizeof(char[1]) + chunk_size); 
		}
		static int get_pool_size(int n_chunks, int chunk_size)
		{
			int chunk_total_size = n_chunks * get_chunk_size(n_chunks, chunk_size);
			int head_size = (sizeof(TChunk) - sizeof(TChunkNode));
			int pool_size = head_size + chunk_total_size;
			return pool_size;        
		}
		
		//   void print_stat();
		
	protected:    
		//将CHUNK插入到空闲链表中.
		void free_list_insert(TChunkNode *node);
		
		//从空闲链表中分配CHUNK.  
		TChunkNode *free_list_remove();
		
		//初始化CHUNK 内存块中的数据结构
		void init_pool_data(int n_chunks, int chunk_size);
		
		//检查 CHUNK 内存块中的数据结构
		int verify_pool_data(int n_chunks, int chunk_size);
		
		char *pool_;        //CHUNK 内存块起始地址
		char *pool_tail_;   //CHUNK 内存块结束地址
		TChunk *chunk_; //内存块中的 TChunk 结构的指针
	};
	
}}
#endif
