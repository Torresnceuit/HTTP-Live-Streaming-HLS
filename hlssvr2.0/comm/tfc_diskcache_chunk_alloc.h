#ifndef _TFC_DISKCACHE_CHUNK_ALLOC_H_
#define _TFC_DISKCACHE_CHUNK_ALLOC_H_

#include <string>

//预分配磁盘文件的数据存储分片类

namespace tfc{namespace diskcache{
	
#pragma pack(1)
	
	typedef int BC_DISK_HANDLER;
	const BC_DISK_HANDLER INVALID_BC_DISK_HANDLER = -1;

#ifndef NULL
	#define NULL 0
#endif
	
	typedef struct tagTDiskChunkNode
	{
		BC_DISK_HANDLER next_;			//CHUNK分片后指针
		unsigned long long offset_;     //数据区在文件中的偏移
	}TDiskChunkNode;
	
	typedef struct tagTDiskChunk
	{
		int chunk_total_;           //CHUNK总节点数
		int chunk_size_;            //CHUNK数据片尺寸
		int free_total_;            //空闲CHUNK总数
		BC_DISK_HANDLER free_list_;    //空闲CHUNK链表头指针
		TDiskChunkNode chunk_nodes_[1]; //CHUNK数组
	}TDiskChunk;
	
#pragma pack()
	
	
	class CDiskChunkAllocator 
	{
	public:
		enum CHUNK_ALLOCATOR_ERROR
		{
			CHUNK_ALLOCATOR_ERROR_BASE = -1,    
				CHUNK_ALLOCATOR_ERROR_INVALID_PARAM = CHUNK_ALLOCATOR_ERROR_BASE -1,    //非法参数
				CHUNK_ALLOCATOR_ERROR_FREE_CHUNK_LACK = CHUNK_ALLOCATOR_ERROR_BASE -2,    //空闲内存块不足
				CHUNK_ALLOCATOR_ERROR_DATA_VERIFY_FAIL = CHUNK_ALLOCATOR_ERROR_BASE -3,    //内存数据检查失败
				CHUNK_ALLOCATOR_ERROR_READ_DISK = CHUNK_ALLOCATOR_ERROR_BASE -4,		//读磁盘失败
				CHUNK_ALLOCATOR_ERROR_WRITE_DISK = CHUNK_ALLOCATOR_ERROR_BASE -5,	//写磁盘失败
				CHUNK_ALLOCATOR_ERROR_OPEN_FILE = CHUNK_ALLOCATOR_ERROR_BASE -6			//打开文件失败
		};
		
	public:
		CDiskChunkAllocator():data_fd_(0){};
		~CDiskChunkAllocator(){ if (data_fd_ != 0) close(data_fd_);	};

		//初始化CHUNK 管理内存块和数据文件，调用者确保data_fd可读写，磁盘空间足够
		int open(char *mem, const std::string& file_path, bool init, int n_chunks, int chunk_size);
		
		//从空闲链表中分配CHUNK.  
		BC_DISK_HANDLER malloc(int chunk_num=1);
		
		//将CHUNK插入到空闲链表中.
		void free(BC_DISK_HANDLER head_chunk_hdr);
		
		//将CHUNK中的数据分片进行组合
		int merge(BC_DISK_HANDLER chunk_node_hdr, int chunk_len,
			void* data_buf, int* data_len);
        
		//将数据分片存在到各CHUNK中.
		int split(BC_DISK_HANDLER head_hdr, const void* data_buf, int data_len);        
		
		//将偏移量转换成真实的内存地址
		TDiskChunkNode *handler2ptr(BC_DISK_HANDLER handler);
		
		//将内存地址转换成偏移量
		BC_DISK_HANDLER ptr2handler(TDiskChunkNode* ptr);
		
		//计算需要多少CHUNK进行数据存储. 
		int get_chunk_num(int data_len);    
		
		//返回使用的CHUNK数
		int get_used_chunk_num() { return (chunk_->chunk_total_ - chunk_->free_total_); };
		
		//返回空闲的CHUNK数
		int get_free_chunk_num() { return chunk_->free_total_; };

		//返回所有的chunk数
		int get_chunk_total() { return chunk_->chunk_total_; }
		
		//计算CHUNK 的管理内存块尺寸
		static int get_chunk_node_size()
		{
			return (sizeof(TDiskChunkNode)); 
		}
		static int get_pool_size(int n_chunks)
		{
			int chunk_total_size = n_chunks * get_chunk_node_size();
			int head_size = (sizeof(TDiskChunk) - sizeof(TDiskChunkNode));
			int pool_size = head_size + chunk_total_size;
			return pool_size;        
		}

		void get_chunk_useinfo(unsigned& uiChunkTotal, unsigned& uiChunkFree, unsigned& uiChunkSize)
		{
		    uiChunkTotal = chunk_->chunk_total_;
		    uiChunkFree  = chunk_->free_total_;
		    uiChunkSize  = chunk_->chunk_size_;
		}

	protected:    
		//将CHUNK插入到空闲链表中.
		void free_list_insert(TDiskChunkNode *node);
		
		//从空闲链表中分配CHUNK.  
		TDiskChunkNode *free_list_remove();
		
		//初始化CHUNK 内存块中的数据结构
		void init_pool_data(int n_chunks, unsigned long long chunk_size);
		
		//检查 CHUNK 内存块中的数据结构
		int verify_pool_data(int n_chunks, int chunk_size);
		
		char *pool_;        //CHUNK 管理内存块起始地址
		char *pool_tail_;   //CHUNK 管理内存块结束地址
		int data_fd_;
		TDiskChunk *chunk_; //管理内存块中的 TChunk 结构的指针
	};
	
}}
#endif
