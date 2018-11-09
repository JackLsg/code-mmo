//#include "lua_message.h"
//#include "memory_allocator.h"
//
//#define DEFAULT_SIZE 64
//
//struct lua_message* message_malloc()
//{
//	size_t sz;
//	lua_message* new_msg = (lua_message*)memory_block_pool_malloc(sizeof(lua_message), &sz, __FILE__, __LINE__);
//	message_init(new_msg);
//	return new_msg;
//}
//
//bool message_init(struct lua_message* m) {
//	m->data_ = 0;
//	m->sz_ = 0;
//	m->write_ptr_ = 0;
//	m->read_ptr_ = 0;
//	return true;
//}
//
//bool message_expand(struct lua_message* m) {
//	if (m->data_ == 0) {
//		m->sz_ = DEFAULT_SIZE;
//		m->data_ = memory_block_pool_malloc(m->sz_, &m->sz_, __FILE__, __LINE__);
//	}
//	else {
//		size_t old_sz = m->sz_;
//		m->sz_ <<= 1;
//		char* new_data = memory_block_pool_malloc(m->sz_, &m->sz_, __FILE__, __LINE__);
//		memcpy(new_data, m->data_, old_sz);
//		memory_block_pool_free(m->data_);
//		m->data_ = new_data;
//	}
//	return true;
//}
//
//bool message_free(struct lua_message*& m) {
//	if (m) {
//		if (m->data_) {
//			memory_block_pool_free(m->data_);
//			m->sz_ = 0;
//			m->data_ = 0;
//			m->sz_ = 0;
//			m->write_ptr_ = 0;
//			m->read_ptr_ = 0;
//		}
//		memory_block_pool_free(m);
//		m = 0;
//	}
//	return true;
//}
//
//bool message_write_boolean(struct lua_message* m, char* v) {
//	while (m->sz_ - m->write_ptr_ < sizeof(*v))
//		message_expand(m);
//	*(char*)(m->data_ + m->write_ptr_) = *v;
//	m->write_ptr_ += sizeof(*v);
//	return true;
//}
//
//bool message_read_boolean(struct lua_message* m, char* v) {
//	if (m->sz_ - m->read_ptr_ < sizeof(*v))
//		return false;
//	*v = *(char*)(m->data_ + m->read_ptr_);
//	m->read_ptr_ += sizeof(*v);
//	return true;
//}
//
//bool message_write_short(struct lua_message* m, short* v) {
//	while (m->sz_ - m->write_ptr_ < sizeof(*v))
//		message_expand(m);
//	*(short*)(m->data_ + m->write_ptr_) = *v;
//	m->write_ptr_ += sizeof(*v);
//	return true;
//}
//
//bool message_read_short(struct lua_message* m, short* v) {
//	if (m->sz_ - m->read_ptr_ < sizeof(*v))
//		return false;
//	*v = *(short*)(m->data_ + m->read_ptr_);
//	m->read_ptr_ += sizeof(*v);
//	return true;
//}
//
//bool message_write_ushort(struct lua_message* m, unsigned short* v) {
//	while (m->sz_ - m->write_ptr_ < sizeof(*v))
//		message_expand(m);
//	*(unsigned short*)(m->data_ + m->write_ptr_) = *v;
//	m->write_ptr_ += sizeof(*v);
//	return true;
//}
//
//bool message_read_ushort(struct lua_message* m, unsigned short* v) {
//	if (m->sz_ - m->read_ptr_ < sizeof(*v))
//		return false;
//	*v = *(unsigned short*)(m->data_ + m->read_ptr_);
//	m->read_ptr_ += sizeof(*v);
//	return true;
//}
//
//bool message_write_int(struct lua_message* m, int* v) {
//	while (m->sz_ - m->write_ptr_ < sizeof(*v))
//		message_expand(m);
//	*(int*)(m->data_ + m->write_ptr_) = *v;
//	m->write_ptr_ += sizeof(*v);
//	return true;
//}
//
//bool message_read_int(struct lua_message* m, int* v) {
//	if (m->sz_ - m->read_ptr_ < sizeof(*v))
//		return false;
//	*v = *(int*)(m->data_ + m->read_ptr_);
//	m->read_ptr_ += sizeof(*v);
//	return true;
//}
//
//bool message_write_uint(struct lua_message* m, unsigned int* v) {
//	while (m->sz_ - m->write_ptr_ < sizeof(*v))
//		message_expand(m);
//	*(unsigned int*)(m->data_ + m->write_ptr_) = *v;
//	m->write_ptr_ += sizeof(*v);
//	return true;
//}
//
//bool message_read_uint(struct lua_message* m, unsigned int* v) {
//	if (m->sz_ - m->read_ptr_ < sizeof(*v))
//		return false;
//	*v = *(unsigned int*)(m->data_ + m->read_ptr_);
//	m->read_ptr_ += sizeof(*v);
//	return true;
//}
//
//bool message_write_int64(struct lua_message* m, long long* v) {
//	while (m->sz_ - m->write_ptr_ < sizeof(*v))
//		message_expand(m);
//	*(long long*)(m->data_ + m->write_ptr_) = *v;
//	m->write_ptr_ += sizeof(*v);
//	return true;
//}
//
//bool message_read_int64(struct lua_message* m, long long* v) {
//	if (m->sz_ - m->read_ptr_ < sizeof(*v))
//		return false;
//	*v = *(long long*)(m->data_ + m->read_ptr_);
//	m->read_ptr_ += sizeof(*v);
//	return true;
//}
//
//bool message_write_uint64(struct lua_message* m, unsigned long long* v) {
//	while (m->sz_ - m->write_ptr_ < sizeof(*v))
//		message_expand(m);
//	*(unsigned long long*)(m->data_ + m->write_ptr_) = *v;
//	m->write_ptr_ += sizeof(*v);
//	return true;
//}
//
//bool message_read_uint64(struct lua_message* m, unsigned long long* v) {
//	if (m->sz_ - m->read_ptr_ < sizeof(*v))
//		return false;
//	*v = *(unsigned long long*)(m->data_ + m->read_ptr_);
//	m->read_ptr_ += sizeof(*v);
//	return true;
//}
//
//bool message_write_string(struct lua_message* m, const char* v, unsigned short sz) {
//	while (m->sz_ - m->write_ptr_ < sizeof(sz) + sz)
//		message_expand(m);
//	*(unsigned short*)(m->data_ + m->write_ptr_) = sz;
//	m->write_ptr_ += sizeof(sz);
//	memcpy(m->data_ + m->write_ptr_, v, sz);
//	m->write_ptr_ += sz;
//	return true;
//}
//
//bool message_read_string(struct lua_message* m, char* v, unsigned short sz) {
//	if (sz == 0)
//		return false;
//	if (m->sz_ - m->read_ptr_ < sizeof(sz))
//		return false;
//	unsigned short str_sz, read_sz;
//	str_sz = *(unsigned short*)(m->data_ + m->read_ptr_);
//	read_sz = str_sz;
//	m->read_ptr_ += sizeof(sz);
//	if (m->sz_ - m->read_ptr_ < read_sz)
//		return false;
//	if (sz < read_sz)
//		read_sz = sz;
//	memcpy(v, m->data_ + m->read_ptr_, read_sz);
//	m->read_ptr_ += str_sz;
//	return true;
//}