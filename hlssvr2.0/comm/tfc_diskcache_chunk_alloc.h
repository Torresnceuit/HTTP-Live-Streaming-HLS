#ifndef _TFC_DISKCACHE_CHUNK_ALLOC_H_
#define _TFC_DISKCACHE_CHUNK_ALLOC_H_

#include <string>

//Ԥ��������ļ������ݴ洢��Ƭ��

namespace tfc{namespace diskcache{
	
#pragma pack(1)
	
	typedef int BC_DISK_HANDLER;
	const BC_DISK_HANDLER INVALID_BC_DISK_HANDLER = -1;

#ifndef NULL
	#define NULL 0
#endif
	
	typedef struct tagTDiskChunkNode
	{
		BC_DISK_HANDLER next_;			//CHUNK��Ƭ��ָ��
		unsigned long long offset_;     //���������ļ��е�ƫ��
	}TDiskChunkNode;
	
	typedef struct tagTDiskChunk
	{
		int chunk_total_;           //CHUNK�ܽڵ���
		int chunk_size_;            //CHUNK����Ƭ�ߴ�
		int free_total_;            //����CHUNK����
		BC_DISK_HANDLER free_list_;    //����CHUNK����ͷָ��
		TDiskChunkNode chunk_nodes_[1]; //CHUNK����
	}TDiskChunk;
	
#pragma pack()
	
	
	class CDiskChunkAllocator 
	{
	public:
		enum CHUNK_ALLOCATOR_ERROR
		{
			CHUNK_ALLOCATOR_ERROR_BASE = -1,    
				CHUNK_ALLOCATOR_ERROR_INVALID_PARAM = CHUNK_ALLOCATOR_ERROR_BASE -1,    //�Ƿ�����
				CHUNK_ALLOCATOR_ERROR_FREE_CHUNK_LACK = CHUNK_ALLOCATOR_ERROR_BASE -2,    //�����ڴ�鲻��
				CHUNK_ALLOCATOR_ERROR_DATA_VERIFY_FAIL = CHUNK_ALLOCATOR_ERROR_BASE -3,    //�ڴ����ݼ��ʧ��
				CHUNK_ALLOCATOR_ERROR_READ_DISK = CHUNK_ALLOCATOR_ERROR_BASE -4,		//������ʧ��
				CHUNK_ALLOCATOR_ERROR_WRITE_DISK = CHUNK_ALLOCATOR_ERROR_BASE -5,	//д����ʧ��
				CHUNK_ALLOCATOR_ERROR_OPEN_FILE = CHUNK_ALLOCATOR_ERROR_BASE -6			//���ļ�ʧ��
		};
		
	public:
		CDiskChunkAllocator():data_fd_(0){};
		~CDiskChunkAllocator(){ if (data_fd_ != 0) close(data_fd_);	};

		//��ʼ��CHUNK �����ڴ��������ļ���������ȷ��data_fd�ɶ�д�����̿ռ��㹻
		int open(char *mem, const std::string& file_path, bool init, int n_chunks, int chunk_size);
		
		//�ӿ��������з���CHUNK.  
		BC_DISK_HANDLER malloc(int chunk_num=1);
		
		//��CHUNK���뵽����������.
		void free(BC_DISK_HANDLER head_chunk_hdr);
		
		//��CHUNK�е����ݷ�Ƭ�������
		int merge(BC_DISK_HANDLER chunk_node_hdr, int chunk_len,
			void* data_buf, int* data_len);
        
		//�����ݷ�Ƭ���ڵ���CHUNK��.
		int split(BC_DISK_HANDLER head_hdr, const void* data_buf, int data_len);        
		
		//��ƫ����ת������ʵ���ڴ��ַ
		TDiskChunkNode *handler2ptr(BC_DISK_HANDLER handler);
		
		//���ڴ��ַת����ƫ����
		BC_DISK_HANDLER ptr2handler(TDiskChunkNode* ptr);
		
		//������Ҫ����CHUNK�������ݴ洢. 
		int get_chunk_num(int data_len);    
		
		//����ʹ�õ�CHUNK��
		int get_used_chunk_num() { return (chunk_->chunk_total_ - chunk_->free_total_); };
		
		//���ؿ��е�CHUNK��
		int get_free_chunk_num() { return chunk_->free_total_; };

		//�������е�chunk��
		int get_chunk_total() { return chunk_->chunk_total_; }
		
		//����CHUNK �Ĺ����ڴ��ߴ�
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
		//��CHUNK���뵽����������.
		void free_list_insert(TDiskChunkNode *node);
		
		//�ӿ��������з���CHUNK.  
		TDiskChunkNode *free_list_remove();
		
		//��ʼ��CHUNK �ڴ���е����ݽṹ
		void init_pool_data(int n_chunks, unsigned long long chunk_size);
		
		//��� CHUNK �ڴ���е����ݽṹ
		int verify_pool_data(int n_chunks, int chunk_size);
		
		char *pool_;        //CHUNK �����ڴ����ʼ��ַ
		char *pool_tail_;   //CHUNK �����ڴ�������ַ
		int data_fd_;
		TDiskChunk *chunk_; //�����ڴ���е� TChunk �ṹ��ָ��
	};
	
}}
#endif
