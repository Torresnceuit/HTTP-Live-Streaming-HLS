
#include <math.h>
#include <time.h>

#include "tfc_diskcache_hash_map.h"

using namespace tfc::diskcache;

CHashMap::CHashMap()
{
	pool_ = NULL;
	pool_tail_ = NULL;
	hash_map_ = NULL;
	hash_node_ = NULL;
	right_rotate_ = 0;
}

// Clear things up.
CHashMap::~CHashMap()
{
	//NOTHING TO DO
}

int CHashMap::open(char* pool, const std::string& file_path, bool init, int node_total, int bucket_size, int n_chunks, int chunk_size)
{
	int ret = 0;
	
	int hash_map_pool_size = get_pool_size(node_total, bucket_size);
	int head_size = sizeof(THashMap) - sizeof(BC_DISK_HANDLER[1]);
	int bucket_total_size = bucket_size * sizeof(BC_DISK_HANDLER);
	
	pool_ = pool;
	pool_tail_ = pool_ + hash_map_pool_size;
	hash_map_ = (THashMap*)pool_;
	hash_node_ = (THashNode*)(pool_ + head_size + bucket_total_size);
	
	if (init)
	{
		init_pool_data(node_total, bucket_size);
	}
	else
	{
		if ((ret = verify_pool_data(node_total, bucket_size)) != 0) 
		{
			return ret;
		}
	}
	if ((ret = allocator_.open(pool_tail_, file_path, init, n_chunks, chunk_size)) != 0) 
	{
		return ret;
	}
	//bucket_size_应该是pow(2, x)
	right_rotate_ = 32 -(int)( log((double)(hash_map_->bucket_size_ + 0.01)) / log(2.0));
	//DEBUG_P(LOG_TRACE, " CHashMap::open right_rotate %d\n", right_rotate_);
	
	return 0;	
}
void  CHashMap::get_chunk_useinfo(unsigned& uiChunkTotal, unsigned& uiChunkFree, unsigned& uiChunkSize)
{
    return allocator_.get_chunk_useinfo(uiChunkTotal, uiChunkFree, uiChunkSize);
}

void CHashMap::init_pool_data(int node_total, int bucket_size)
{
	hash_map_->node_total_ = node_total;
	hash_map_->bucket_size_ = bucket_size;
	hash_map_->used_node_num_ = 0;
	hash_map_->used_bucket_num_ = 0;

       hash_map_->add_head_ = INVALID_BC_DISK_HANDLER;
	hash_map_->add_tail_ = INVALID_BC_DISK_HANDLER;
	hash_map_->free_list_ = INVALID_BC_DISK_HANDLER;
	
	int i;
	for(i = 0; i < bucket_size; i++)
	{
		hash_map_->bucket[i] = INVALID_BC_DISK_HANDLER;
	}
	
	//将所有节点插入到空闲链表中
	THashNode* hash_node;
	int offset;
	for(i = 0; i < node_total; i++)
	{
		offset = i * (sizeof(THashNode));
		hash_node = (THashNode*)((char*)hash_node_ + offset);
		init_node(hash_node);
		free_list_insert(hash_node);
	}
	
	return;
}

int CHashMap::verify_pool_data(int node_total, int bucket_size)
{
	if (node_total != hash_map_->node_total_)
	{
		//ERROR_RETURN(HASH_MAP_ERROR_BASE, "pool data verify fail");
		return HASH_MAP_ERROR_BASE;
	}
	if (bucket_size != hash_map_->bucket_size_)
	{
		//ERROR_RETURN(HASH_MAP_ERROR_BASE, "pool data verify fail");
		return HASH_MAP_ERROR_BASE;
	}
	
	int used_bucket_count = 0;
	for (int i = 0; i < hash_map_->bucket_size_; i++)
	{
		if (hash_map_->bucket[i] != INVALID_BC_DISK_HANDLER)
		{
			used_bucket_count ++;
		}
	}
	if (used_bucket_count != hash_map_->used_bucket_num_)
	{
		//ERROR_RETURN(HASH_MAP_ERROR_BASE, "pool data verify fail");
		return HASH_MAP_ERROR_BASE;
	}
	
	int free_node_count = 0;
	THashNode* free_node = handler2ptr(hash_map_->free_list_);
	while(free_node)
	{
		free_node_count++;
		free_node = handler2ptr(free_node->node_next_);
	}
	
	if ((hash_map_->used_node_num_ + free_node_count) != hash_map_->node_total_) 
	{
		//ERROR_RETURN(HASH_MAP_ERROR_BASE, "pool data verify fail");
		return HASH_MAP_ERROR_BASE;
	}
	
	return 0;
}

THashNode* CHashMap::find_node(TMBHashKey &key)
{
	int bucket_id = get_bucket_id(key);
	BC_DISK_HANDLER node_hdr = hash_map_->bucket[bucket_id];
	while(node_hdr != INVALID_BC_DISK_HANDLER)
	{
		THashNode* node = handler2ptr(node_hdr);
		if (node->key_ == key) 
		{
			//将该节点插入到附加链表头部
			//node->add_info_1_ = time(NULL);
			insert_add_list_head(node);
			return node;
		}
		node_hdr = node->node_next_;
	}
	//ERROR_RETURN_NULL(HASH_MAP_ERROR_NODE_NOT_EXIST, "node not exist");
	return NULL;
}


THashNode* CHashMap::insert_node(TMBHashKey &key, void* new_data, int new_len)
{
	THashNode* node = free_list_remove();
	if (node == NULL) 
	{
		return NULL;				
	}
	
	int new_chunk_num = allocator_.get_chunk_num(new_len);
	BC_DISK_HANDLER head_hdr = allocator_.malloc(new_chunk_num);
	if(head_hdr == INVALID_BC_DISK_HANDLER)
	{
		free_list_insert(node);
		return NULL;
	}

	if( allocator_.split(head_hdr, new_data, new_len) < 0 )
       {
             allocator_.free(head_hdr);
             free_list_insert(node);
             return NULL;
	}

	use_node(node, key, new_len, head_hdr);

	//将该节点插入到附加链表头部
	node->add_info_1_ = time(NULL);
	insert_add_list_head(node);
	return node;
}

THashNode* CHashMap::insert_node_metadata(TMBHashKey &key, int new_len, int expiretime)
{
	THashNode* node = free_list_remove();
	if (node == NULL) 
	{
		return NULL;				
	}
	
	int new_chunk_num = allocator_.get_chunk_num(new_len);
	BC_DISK_HANDLER head_hdr = allocator_.malloc(new_chunk_num);
	if(head_hdr == INVALID_BC_DISK_HANDLER)
	{
		free_list_insert(node);
		return NULL;
	}

	/*恢复共享内存中的元数据不用恢复存储数据
	if( allocator_.split(head_hdr, new_data, new_len) < 0 )
       {
             allocator_.free(head_hdr);
             free_list_insert(node);
             return NULL;
	}
	*/

	use_node(node, key, new_len, head_hdr);

	//将该节点插入到附加链表头部
	node->add_info_1_ = time(NULL);
	node->add_info_2_ = expiretime;
	insert_add_list_head(node);
	return node;
}


THashNode* CHashMap::update_node(THashNode* node, void* new_data, int new_len, 
								 char* old_data, int* old_len)
{
	if(old_data != NULL && old_len != NULL)
	{
		//返回旧数据
		if(allocator_.merge(node->chunk_head_, node->chunk_len_,  old_data, old_len) != 0)
		{
			return NULL;
		}
	}
	else if(old_len != NULL)
	{
		*old_len = node->chunk_len_;
	}
	
	int old_chunk_num = allocator_.get_chunk_num(node->chunk_len_);
	int new_chunk_num = allocator_.get_chunk_num(new_len);
	
	if (old_chunk_num != new_chunk_num)
	{
		//需要重新分配CHUNK. 先FREE再MALLOC.		
		if (new_chunk_num > old_chunk_num)
		{
			if (allocator_.get_free_chunk_num() < (new_chunk_num - old_chunk_num))
			{
				//剩余CHUNK数不足
				//ERROR_RETURN_NULL(CChunkAllocator::CHUNK_ALLOCATOR_ERROR_FREE_CHUNK_LACK, "free chunk lack");
				return NULL;
			}
		}
		
		allocator_.free(node->chunk_head_);
		
		BC_DISK_HANDLER head_hdr = allocator_.malloc(new_chunk_num);   //CHUNK数足够, 不会失败
		if( allocator_.split(head_hdr, new_data, new_len) < 0)
              {
                    allocator_.free(head_hdr);
                    free_list_insert(node);
                    return NULL;
              }
		
		node->chunk_len_ = new_len;
		node->chunk_head_ = head_hdr;
	}
	else
	{
		if ( allocator_.split(node->chunk_head_, new_data, new_len) < 0)
		{
                    allocator_.free(node->chunk_head_);
                    free_list_insert(node);		
                    return NULL;
		}
        
		node->chunk_len_ = new_len;
	}
	
	//将该节点插入到附加链表头部
	node->add_info_1_ = time(NULL);
	insert_add_list_head(node);
	return node;
}

THashNode* CHashMap::update_node_metadata(THashNode* node, int new_len, int expiretime)
{
	int old_chunk_num = allocator_.get_chunk_num(node->chunk_len_);
	int new_chunk_num = allocator_.get_chunk_num(new_len);
	
	if (old_chunk_num != new_chunk_num)
	{
		//需要重新分配CHUNK. 先FREE再MALLOC.		
		if (new_chunk_num > old_chunk_num)
		{
			if (allocator_.get_free_chunk_num() < (new_chunk_num - old_chunk_num))
			{
				//剩余CHUNK数不足
				//ERROR_RETURN_NULL(CChunkAllocator::CHUNK_ALLOCATOR_ERROR_FREE_CHUNK_LACK, "free chunk lack");
				return NULL;
			}
		}
		
		allocator_.free(node->chunk_head_);
		
		BC_DISK_HANDLER head_hdr = allocator_.malloc(new_chunk_num);   //CHUNK数足够, 不会失败

		/*恢复共享内存中的元数据不用恢复存储数据		
		if( allocator_.split(head_hdr, new_data, new_len) < 0)
              {
                    allocator_.free(head_hdr);
                    free_list_insert(node);
                    return NULL;
              }
        */
		
		node->chunk_len_ = new_len;
		node->chunk_head_ = head_hdr;
	}
	else
	{
		/*恢复共享内存中的元数据不用恢复存储数据		
		if ( allocator_.split(node->chunk_head_, new_data, new_len) < 0)
		{
                    allocator_.free(node->chunk_head_);
                    free_list_insert(node);		
                    return NULL;
		}
		*/
        
		node->chunk_len_ = new_len;
	}
	
	//将该节点插入到附加链表头部
	node->add_info_1_ = time(NULL);
	node->add_info_1_ = expiretime;
	insert_add_list_head(node);
	return node;
}

THashNode* CHashMap::replace_node(TMBHashKey &key, void* new_data, int new_len, char* old_data, int* old_len)
{
	THashNode* node = find_node(key);
	if(node != NULL)
	{
		return update_node(node, new_data, new_len, old_data, old_len);
	}
	
	return insert_node(key, new_data, new_len);
}

int CHashMap::delete_node(THashNode* node, char* data, int* data_len)
{
	//旧节点存在
	if(data != NULL && data_len != NULL)
	{
		//返回旧数据
		if(allocator_.merge(node->chunk_head_, node->chunk_len_, data, data_len) != 0)
		{
			return -1;
		}
	}
	else if(data_len != NULL)
	{
		*data_len = node->chunk_len_;
	}
	
	delete_from_add_list(node);
	
	free_node(node);
	free_list_insert(node);
	return 0;
}

void CHashMap::insert_node_list(THashNode* node)
{
	//插入到节点链表头
	int bucket_id = get_bucket_id(node->key_);
	BC_DISK_HANDLER node_hdr = ptr2handler(node);
	
	node->node_next_ = hash_map_->bucket[bucket_id];
	node->node_prev_ = INVALID_BC_DISK_HANDLER;
	hash_map_->bucket[bucket_id] = node_hdr;
	THashNode* next_node = handler2ptr(node->node_next_);
	if(next_node != NULL)
	{
		next_node->node_prev_ = node_hdr;
	}
	
	//stat
	hash_map_->used_node_num_ ++;	
}

void CHashMap::delete_from_node_list(THashNode* node)
{
	BC_DISK_HANDLER next_node_hdr = node->node_next_;
	BC_DISK_HANDLER prev_node_hdr = node->node_prev_;
	
	if(prev_node_hdr != INVALID_BC_DISK_HANDLER)
	{
		THashNode* prev_node = handler2ptr(prev_node_hdr);
		prev_node->node_next_ = node->node_next_;
	}
	if(next_node_hdr != INVALID_BC_DISK_HANDLER)
	{
		THashNode* next_node = handler2ptr(next_node_hdr);
		next_node->node_prev_ = node->node_prev_;
	}
	
	BC_DISK_HANDLER node_hdr = ptr2handler(node);
	
	int bucket_id = get_bucket_id(node->key_);
	if (node_hdr == hash_map_->bucket[bucket_id]) 
	{
		//当前节点为链表头节点
		hash_map_->bucket[bucket_id] = next_node_hdr;
		
	}
	
	//将前后链表指针清零
	node->node_next_ = INVALID_BC_DISK_HANDLER;
	node->node_prev_ = INVALID_BC_DISK_HANDLER;	
	
	//stat
	hash_map_->used_node_num_ --;	
}


void CHashMap::free_node(THashNode *node)
{
	//从链表中删除
	delete_from_node_list(node);
	
	//释放 chunk
	allocator_.free(node->chunk_head_);
	
	//stat
	int bucket_list_len = get_bucket_list_len(get_bucket_id(node->key_));
	if (bucket_list_len == 0)
	{
		//the bucket change to unused
		hash_map_->used_bucket_num_ --;
	}	
	
	//reset member
	init_node(node);
}

void CHashMap::use_node(THashNode *node, TMBHashKey &key, int chunk_len, 
						BC_DISK_HANDLER chunk_head)
{
	//set member
	node->key_ = key;
	node->chunk_len_ = chunk_len;
	node->chunk_head_ = chunk_head;
	node->add_info_1_ = 0;
	node->add_info_2_ = 0;
	
	
	int bucket_list_len = get_bucket_list_len(get_bucket_id(node->key_));
	if (bucket_list_len == 0)
	{
		//the bucket change from unused
		hash_map_->used_bucket_num_ ++;
	}
	
	insert_node_list(node);
	return;
}

int CHashMap::get_bucket_list_len(int bucket_id)
{
	int num = 0;
	
	BC_DISK_HANDLER node_hdr;
	node_hdr = hash_map_->bucket[bucket_id];
	
	while (node_hdr != INVALID_BC_DISK_HANDLER)
	{
		num ++;		
		THashNode* node = handler2ptr(node_hdr);
		node_hdr = node->node_next_;
	}
	
	return num;
}
void CHashMap::insert_add_list_head(THashNode* node)
{
	delete_from_add_list(node);
	BC_DISK_HANDLER node_hdr = ptr2handler(node);
	
	//insert node into head of add list
	node->add_next_ = hash_map_->add_head_;
	hash_map_->add_head_ = node_hdr;
	
	if (hash_map_->add_tail_ == INVALID_BC_DISK_HANDLER)
	{
		hash_map_->add_tail_ = node_hdr;		
	}
	
	node->add_prev_ = INVALID_BC_DISK_HANDLER;
	THashNode* next_node = handler2ptr(node->add_next_);
	if(next_node != NULL)
	{
		next_node->add_prev_ = node_hdr;
	}
	
}

void CHashMap::insert_add_list_tail(THashNode* node)
{
	delete_from_add_list(node);
	//reform add list, insert to head
	BC_DISK_HANDLER node_hdr = ptr2handler(node);
	
	node->add_prev_ = hash_map_->add_tail_;
	hash_map_->add_tail_ = node_hdr;
	
	if (hash_map_->add_head_ == INVALID_BC_DISK_HANDLER)
	{
		hash_map_->add_head_ = node_hdr;		
	}	
	
	node->add_next_ = INVALID_BC_DISK_HANDLER;
	THashNode* prev_node = handler2ptr(node->add_prev_);
	if(prev_node != NULL)
	{
		prev_node->add_next_ = node_hdr;
	}	   
}

void CHashMap::delete_from_add_list(THashNode* node)
{
	//link the prev add node and the next add node
	BC_DISK_HANDLER node_hdr = ptr2handler(node);
	BC_DISK_HANDLER next_add_hdr = node->add_next_;
	BC_DISK_HANDLER prev_add_hdr = node->add_prev_;
	
	if ((next_add_hdr == INVALID_BC_DISK_HANDLER) &&
		(prev_add_hdr == INVALID_BC_DISK_HANDLER) &&
		(hash_map_->add_head_ != node_hdr) &&
		(hash_map_->add_tail_ != node_hdr)) 
	{
		//不在链表中
		return ;
	}
	
	if(prev_add_hdr != INVALID_BC_DISK_HANDLER)
	{
		THashNode* prev_add = handler2ptr(prev_add_hdr);
		prev_add->add_next_ = node->add_next_;
	}
	if(next_add_hdr != INVALID_BC_DISK_HANDLER)
	{
		THashNode* next_add = handler2ptr(next_add_hdr);
		next_add->add_prev_ = node->add_prev_;
	}
	
	
	if (hash_map_->add_head_ == node_hdr)
	{
		hash_map_->add_head_ =  next_add_hdr;		
	}
	if (hash_map_->add_tail_ == node_hdr) 
	{
		hash_map_->add_tail_ =  prev_add_hdr;
	}
	
	//将前后链表指针清零
	node->add_prev_ = INVALID_BC_DISK_HANDLER;
	node->add_next_ = INVALID_BC_DISK_HANDLER;
	
}

THashNode* CHashMap::get_add_list_head()
{
	return handler2ptr(hash_map_->add_head_);
}

THashNode* CHashMap::get_add_list_tail()
{
	return handler2ptr(hash_map_->add_tail_);
}

THashNode* CHashMap::get_add_list_prev(THashNode* node)
{
	return handler2ptr(node->add_prev_);
}


THashNode* CHashMap::get_add_list_next(THashNode* node)
{
	return handler2ptr(node->add_next_);
}

void CHashMap::set_expiretime(THashNode* node, int expiretime)
{
	node->add_info_2_ = expiretime;
	return;
}

int CHashMap::get_expiretime(THashNode* node)
{
	return node->add_info_2_;
}


