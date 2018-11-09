#include "memory_allocator.h"
#include <assert.h>

memory_block_pool MP64;
memory_block_pool MP128;
memory_block_pool MP256;
memory_block_pool MP512;
bool pool_inited_ = false;

bool memory_block_pool_init(struct memory_block_pool* p, size_t unit_sz) {
	p->block_list_all_ = 0;
	p->block_list_all_sz_ = 0;
	p->block_list_free_ = 0;
	p->block_list_free_sz_ = 0;
	p->block_unit_size_ = unit_sz;
	p->data_list_ = 0;
	p->data_list_sz_ = 0;
	POOL_LOCK_INIT(p);
	return true;
}

bool memory_block_pool_init_all() {
	if (pool_inited_ == true) return false;
	memory_block_pool_init(&MP64, 64);
	memory_block_pool_init(&MP128, 128);
	memory_block_pool_init(&MP256, 256);
	memory_block_pool_init(&MP512, 512);
	pool_inited_ = true;
	return true;
}

char* memory_block_pool_malloc_huge(size_t req_sz, size_t* alloc_sz, const char* file, int line) {
	void* data = malloc(sizeof(struct memory_block_header) + req_sz);
	struct memory_block_header* h = (struct memory_block_header*)data;
	h->alloc_file_ = file;
	h->alloc_line_ = line;
	h->free_ = false;
	h->from_pool_ = false;
	h->alloc_sz_ = *alloc_sz = req_sz;
	return (char*)data + sizeof(struct memory_block_header);
}

bool memory_block_pool_header_init(struct memory_block_header* h) {
	h->alloc_file_ = NULL;
	h->alloc_line_ = 0;
	h->free_ = true;
	h->from_pool_ = true;
	h->alloc_sz_ = 0;
	return true;
}

bool memory_block_pool_expand(struct memory_block_pool* mp) {
	size_t block_real_sz = sizeof(struct memory_block_header) + mp->block_unit_size_;
	if (mp->data_list_sz_ == 0) {
		size_t expand_sz = POOL_DEFAULT_SIZE;

		mp->data_list_ = (void**)malloc(sizeof(void*) * 1);
		mp->data_list_sz_ = 1;
		mp->block_list_all_ = (void**)malloc(sizeof(void*) * expand_sz);
		mp->block_list_all_sz_ = expand_sz;
		mp->block_list_free_ = (void**)malloc(sizeof(void*) * expand_sz);
		mp->block_list_free_sz_ = expand_sz;

		void* new_block_list = malloc(block_real_sz * expand_sz);
		mp->data_list_[0] = new_block_list;
		for (size_t i = 0; i < expand_sz; i++) {
			struct memory_block_header* h = (struct memory_block_header*)((char*)new_block_list + block_real_sz * i);
			memory_block_pool_header_init(h);
			mp->block_list_all_[i] = mp->block_list_free_[i] = ((char*)h + sizeof(*h));
		}
	}
	else {
		size_t expand_sz = mp->block_list_all_sz_;
		mp->data_list_ = (void**)realloc(mp->data_list_, sizeof(void*) * (++mp->data_list_sz_));
		mp->block_list_all_ = (void**)realloc(mp->block_list_all_, sizeof(void*) * (mp->block_list_all_sz_ + expand_sz));
		mp->block_list_free_ = (void**)realloc(mp->block_list_free_, sizeof(void*) * (mp->block_list_free_sz_ + expand_sz));

		void* new_block_list = malloc(block_real_sz * expand_sz);
		mp->data_list_[mp->data_list_sz_ - 1] = new_block_list;
		for (size_t i = 0; i < expand_sz; i++) {
			struct memory_block_header* h = (struct memory_block_header*)((char*)new_block_list + block_real_sz * i);
			memory_block_pool_header_init(h);
			mp->block_list_all_[mp->block_list_all_sz_ + i] = ((char*)h + sizeof(*h));
			mp->block_list_free_[mp->block_list_free_sz_ + i] = ((char*)h + sizeof(*h));
		}

		mp->block_list_all_sz_ += expand_sz;
		mp->block_list_free_sz_ += expand_sz;
	}
	return true;
}

char* memory_block_pool_malloc(size_t req_sz, size_t* alloc_sz, const char* file, int line) {
	memory_block_pool* mp = NULL;
	if (req_sz <= 64) {
		mp = &MP64;
	}
	else if (req_sz <= 128) {
		mp = &MP128;
	}
	else if (req_sz <= 256) {
		mp = &MP256;
	}
	else if (req_sz <= 512) {
		mp = &MP512;
	}

	if (mp) {
		void* alloc_block = 0;
		POOL_LOCK(mp);
		if (mp->block_list_free_sz_ == 0) {
			memory_block_pool_expand(mp);
		}
		alloc_block = mp->block_list_free_[--mp->block_list_free_sz_];
		struct memory_block_header* h = (struct memory_block_header*)((char*)alloc_block - sizeof(struct memory_block_header));
		h->alloc_file_ = file;
		h->alloc_line_ = line;
		h->alloc_sz_ = *alloc_sz = mp->block_unit_size_;
		h->free_ = false;
		POOL_UNLOCK(mp);
		return (char*)alloc_block;
	}
	else {
		return memory_block_pool_malloc_huge(req_sz, alloc_sz, file, line);
	}
}

bool memory_block_pool_free(void* block)
{
	struct memory_block_header* h = (struct memory_block_header*)((char*)block - sizeof(struct memory_block_header));
	if (h->from_pool_) {
		memory_block_pool* mp = NULL;
		if (h->alloc_sz_ <= 64) {
			mp = &MP64;
		}
		else if (h->alloc_sz_ <= 128) {
			mp = &MP128;
		}
		else if (h->alloc_sz_ <= 256) {
			mp = &MP256;
		}
		else if (h->alloc_sz_ <= 512) {
			mp = &MP512;
		}
		else {
			assert(0);
			return false;
		}
		assert(h->free_ == false);
		h->free_ = true;
		assert(mp->block_list_free_sz_ < mp->block_list_all_sz_);
		POOL_LOCK(mp);
		mp->block_list_free_[mp->block_list_free_sz_++] = block;
		POOL_UNLOCK(mp);
		return true;
	}
	else {
		free(h);
		return true;
	}
	return false;
}
