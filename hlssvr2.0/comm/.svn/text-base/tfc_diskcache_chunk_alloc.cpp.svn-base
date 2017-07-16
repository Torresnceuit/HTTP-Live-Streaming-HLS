#include <cstring>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>

using namespace std;
#include "tfc_diskcache_chunk_alloc.h"

// 使用pread，pwrite的64位版本
#ifndef __USE_LARGEFILE64
	#define __USE_LARGEFILE64
#endif

using namespace tfc::diskcache;

int CDiskChunkAllocator::open(char *pool, const std::string& file_path, bool init, int n_chunks, int chunk_size)
{
    int ret = 0;
    int pool_size = get_pool_size(n_chunks);

    pool_ = pool;
    pool_tail_ = pool + pool_size;
    chunk_ = (TDiskChunk *)pool_;

	if(init)
    {
        //初始化管理内存块
        init_pool_data(n_chunks, chunk_size);
    }
    else
    {
        //检查管理内存块
        if ((ret = verify_pool_data(n_chunks, chunk_size)) != 0)
        {
            return ret;
        }
    }
    
    if (data_fd_ > 0)
	{
		close(data_fd_); 
	}
    //打开数据文件
    data_fd_ = ::open(file_path.c_str(), O_CREAT | O_RDWR| O_LARGEFILE, 0666);
	if (data_fd_ < 0)
	{
		return CHUNK_ALLOCATOR_ERROR_OPEN_FILE;
	}

    return 0;
} 

BC_DISK_HANDLER CDiskChunkAllocator::malloc (int chunk_num)
{
    if (chunk_->free_total_ < chunk_num) 
    {
        return INVALID_BC_DISK_HANDLER;
    }

    TDiskChunkNode* node = NULL;
    TDiskChunkNode* head = NULL;
    for(int i = 0; i < chunk_num; i++)
    {
        node = free_list_remove();
        BC_DISK_HANDLER head_hdr = ptr2handler(head);
        node->next_ = head_hdr;
        head = node;
    }

    chunk_->free_total_ -= chunk_num;
    return ptr2handler(head);
}

void CDiskChunkAllocator::free(BC_DISK_HANDLER head_chunk_hdr)
{
    BC_DISK_HANDLER chunk_hdr = head_chunk_hdr;
    while(chunk_hdr != INVALID_BC_DISK_HANDLER)
    {
        TDiskChunkNode* node = handler2ptr(chunk_hdr);
        BC_DISK_HANDLER next_chunk_hdr = node->next_;
        
        free_list_insert(node);
        chunk_->free_total_++;
        
        chunk_hdr = next_chunk_hdr;
    }
}

void CDiskChunkAllocator::init_pool_data(int n_chunks, unsigned long long chunk_size)
{
    chunk_->free_list_ = INVALID_BC_DISK_HANDLER;
    
    TDiskChunkNode* chunk_node = NULL;
    for(int i = 0; i < n_chunks; i++)
    {
        int offset = i * get_chunk_node_size();
        chunk_node = (TDiskChunkNode*)((char*)(chunk_->chunk_nodes_) + offset);
        chunk_node->next_ = INVALID_BC_DISK_HANDLER;
		chunk_node->offset_ = i * chunk_size;
        free_list_insert(chunk_node);
    }
    
    chunk_->chunk_total_ = n_chunks;
    chunk_->chunk_size_ = chunk_size;
    chunk_->free_total_ = n_chunks;
}

int CDiskChunkAllocator::verify_pool_data(int n_chunks, int chunk_size)
{
    //检查管理内存块的数据的有效性
    if((chunk_->chunk_size_ != chunk_size) || 
        (chunk_->chunk_total_ != n_chunks))
    {
        //ERROR_RETURN_2(CHUNK_ALLOCATOR_ERROR_DATA_VERIFY_FAIL, "data verify fail[total:%d, size:%d]", chunk_->chunk_total_, chunk_->chunk_size_);
		return CHUNK_ALLOCATOR_ERROR_DATA_VERIFY_FAIL;
    }
    
    //检查空闲链
    TDiskChunkNode* free_node = handler2ptr(chunk_->free_list_);
    int free_total = 0;
    while (free_node != NULL) 
    {
        free_total ++;
        free_node = handler2ptr(free_node->next_);
    }
    
    if (free_total != chunk_->free_total_)
    {
        //ERROR_RETURN_2(CHUNK_ALLOCATOR_ERROR_DATA_VERIFY_FAIL, "data verify fail[free_total:%d, free_list:%d]", chunk_->free_total_, free_total);
        return CHUNK_ALLOCATOR_ERROR_DATA_VERIFY_FAIL;
    }

    return 0;
}

inline TDiskChunkNode* CDiskChunkAllocator::handler2ptr(BC_DISK_HANDLER handler)
{
    if (handler == INVALID_BC_DISK_HANDLER)
    {
        return NULL;
    }
    
    return (TDiskChunkNode*)(pool_ + handler);
}

inline BC_DISK_HANDLER CDiskChunkAllocator::ptr2handler(TDiskChunkNode* ptr)
{
    char *tmp_ptr = (char *)ptr;
    if((tmp_ptr < pool_) || (tmp_ptr >= pool_tail_))
    {
        return INVALID_BC_DISK_HANDLER;
    }
    return (BC_DISK_HANDLER)(tmp_ptr - pool_);
}

inline void CDiskChunkAllocator::free_list_insert(TDiskChunkNode *node)
{
    //插入到空闲链表头
	BC_DISK_HANDLER node_hdr = ptr2handler(node);
	if (node_hdr != INVALID_BC_DISK_HANDLER)
	{
		node->next_ = chunk_->free_list_;
    	chunk_->free_list_ = node_hdr;
	}
}
    
inline TDiskChunkNode* CDiskChunkAllocator::free_list_remove()
{
    if(chunk_->free_list_ == INVALID_BC_DISK_HANDLER)
    {
        //没有空闲CHUNK
        return NULL;
    }
    //从空闲链表头分配CHUNK
    TDiskChunkNode* head_node = handler2ptr(chunk_->free_list_);
    chunk_->free_list_ = head_node->next_;

    head_node->next_ = INVALID_BC_DISK_HANDLER;
    return head_node;
}
    

int CDiskChunkAllocator::get_chunk_num(int data_len)
{
    int num = data_len / chunk_->chunk_size_;
    if ((data_len % chunk_->chunk_size_) != 0) 
    {
        num++;
    }
    return num;
}

int CDiskChunkAllocator::merge(BC_DISK_HANDLER chunk_node_hdr, int chunk_len,
                               void* data_buf, int* data_len)
{
    if(*data_len < chunk_len)
    {
        //输入的缓存区过短
        //ERROR_RETURN_2(CHUNK_ALLOCATOR_ERROR_INVALID_PARAM, "input date_len too short[%d < %d]", *data_len, chunk_len);
		*data_len = chunk_len;
		return CHUNK_ALLOCATOR_ERROR_INVALID_PARAM;
    }
    
    int remain_len = chunk_len;
    char* write_pos = (char *)data_buf;
    int read_ret;
    while(chunk_node_hdr != INVALID_BC_DISK_HANDLER)
    {
        TDiskChunkNode* chunk_node = handler2ptr(chunk_node_hdr);
        
        if (remain_len < chunk_->chunk_size_) 
        {
			// 读硬盘操作
			// memcpy(write_pos, chunk_node->data_, remain_len);
			read_ret = pread64(data_fd_, write_pos, remain_len, chunk_node->offset_);
			if (read_ret != remain_len)
			{
				return CHUNK_ALLOCATOR_ERROR_READ_DISK;
			}
            break;
        }
        
		// 读硬盘操作
        // memcpy(write_pos, chunk_node->data_, chunk_->chunk_size_);
		read_ret = pread64(data_fd_, write_pos, chunk_->chunk_size_, chunk_node->offset_);
		if (read_ret != chunk_->chunk_size_)
		{
			return CHUNK_ALLOCATOR_ERROR_READ_DISK;
		}
        write_pos += chunk_->chunk_size_;
        remain_len -= chunk_->chunk_size_;
        
        chunk_node_hdr = chunk_node->next_;    //to next
    }
    
    *data_len = chunk_len;
    
    return 0;
}

int CDiskChunkAllocator::split(BC_DISK_HANDLER head_hdr, const void* data_buf, int data_len)
{    
    TDiskChunkNode* chunk_node = handler2ptr(head_hdr);
    
    char* read_pos = (char*)data_buf;
    int remain_len = data_len;
	int write_ret;
    while (chunk_node != NULL) 
    {
        if (remain_len < chunk_->chunk_size_) 
        {
			// 写硬盘操作
            //memcpy(chunk_node->data_, read_pos, remain_len);
			write_ret = pwrite64(data_fd_, read_pos, remain_len, chunk_node->offset_);
			if (write_ret != remain_len)
			{
				return CHUNK_ALLOCATOR_ERROR_WRITE_DISK;
			}
            break;
        }
        // 写硬盘操作
        //memcpy(chunk_node->data_, read_pos, chunk_->chunk_size_);
		write_ret = pwrite64(data_fd_, read_pos, chunk_->chunk_size_, chunk_node->offset_);
		if (write_ret != chunk_->chunk_size_)
		{
			return CHUNK_ALLOCATOR_ERROR_WRITE_DISK;
		}
        read_pos += chunk_->chunk_size_;
        remain_len -= chunk_->chunk_size_;
        
        //to next
        chunk_node = handler2ptr(chunk_node->next_);
    }
    return 0;
}

