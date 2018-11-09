#ifndef _memory_allocator_h_
#define _memory_allocator_h_

#include <stdlib.h>
#include <string.h>

#ifdef __WINDOWS__
#include "windows.h"
typedef LONG pool_lock;
#define POOL_LOCK(p) while (InterlockedExchange(&p->lock_, 1)){}
#define POOL_UNLOCK(p) InterlockedExchange(&p->lock_, 0)
#else 
typedef int pool_lock;
#define POOL_LOCK(p) while (__sync_lock_test_and_set(&p->lock_, 1)){}
#define POOL_UNLOCK(p) __sync_lock_release(&p->lock_)
#endif
#define POOL_LOCK_INIT(p) p->lock_ = 0

#define ALLOCID_LUA_MESSAGE 0
#define ALLOCID_LUA_MESSAGE_DATA 1

#define POOL_DEFAULT_SIZE 8

struct memory_alloctor
{
	const char* name_;
	char* data_;
	size_t count_;
	size_t used_;
	size_t free_;
};

struct memory_block_header
{
	const char* alloc_file_;
	int alloc_line_;
	bool free_;
	bool from_pool_;
	size_t alloc_sz_;
};

struct memory_block_pool
{
	void** data_list_;
	size_t data_list_sz_;
	void** block_list_all_;
	size_t block_list_all_sz_;
	void** block_list_free_;
	size_t block_list_free_sz_;
	size_t block_unit_size_;
	pool_lock lock_;
};

extern bool memory_block_pool_init(struct memory_block_pool* p, size_t unit_sz);
extern bool memory_block_pool_init_all();
extern char* memory_block_pool_malloc_huge(size_t req_sz, size_t* alloc_sz, const char* file, int line);
extern bool memory_block_pool_header_init(struct memory_block_header* h);
extern bool memory_block_pool_expand(struct memory_block_pool* mp);
extern char* memory_block_pool_malloc(size_t req_sz, size_t* alloc_sz, const char* file, int line);
extern bool memory_block_pool_free(void* block);

#endif