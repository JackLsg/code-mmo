#include "lua_message.h"

#define DEFAULT_SIZE 128

struct lua_message* message_malloc()
{
	lua_message* new_msg = (lua_message*)malloc(sizeof(lua_message));
	message_init(new_msg);
	return new_msg;
}

int message_init(struct lua_message* m) {
	m->data_ = 0;
	m->sz_ = 0;
	m->write_ptr_ = 0;
	m->read_ptr_ = 0;
	return 1;
}

int message_expand(struct lua_message* m) {
	if (m->data_ == 0) {
		m->sz_ = DEFAULT_SIZE;
		m->data_ = (char*)malloc(m->sz_);
	}
	else {
		m->sz_ <<= 1;
		m->data_ = (char*)realloc(m->data_, m->sz_);
	}
	return 1;
}

int message_free(struct lua_message*& m) {
	if (m) {
		if (m->data_) {
			free(m->data_);
			m->sz_ = 0;
			m->data_ = 0;
			m->sz_ = 0;
			m->write_ptr_ = 0;
			m->read_ptr_ = 0;
		}
		free(m);
		m = 0;
		return 1;
	}
	return 0;
}

int message_write_boolean(struct lua_message* m, char* v) {
	while (m->sz_ - m->write_ptr_ < sizeof(*v))
		message_expand(m);
	*(char*)(m->data_ + m->write_ptr_) = *v;
	m->write_ptr_ += sizeof(*v);
	return 1;
}

int message_read_boolean(struct lua_message* m, char* v) {
	if (m->sz_ - m->read_ptr_ < sizeof(*v))
		return 0;
	*v = *(char*)(m->data_ + m->read_ptr_);
	m->read_ptr_ += sizeof(*v);
	return 1;
}

int message_write_byte(struct lua_message* m, char* v) {
	while (m->sz_ - m->write_ptr_ < sizeof(*v))
		message_expand(m);
	*(char*)(m->data_ + m->write_ptr_) = *v;
	m->write_ptr_ += sizeof(*v);
	return 1;
}

int message_read_byte(struct lua_message* m, char* v) {
	if (m->sz_ - m->read_ptr_ < sizeof(*v))
		return 0;
	*v = *(char*)(m->data_ + m->read_ptr_);
	m->read_ptr_ += sizeof(*v);
	return 1;
}

int message_write_short(struct lua_message* m, short* v) {
	while (m->sz_ - m->write_ptr_ < sizeof(*v))
		message_expand(m);
	*(short*)(m->data_ + m->write_ptr_) = *v;
	m->write_ptr_ += sizeof(*v);
	return 1;
}

int message_read_short(struct lua_message* m, short* v) {
	if (m->sz_ - m->read_ptr_ < sizeof(*v))
		return 0;
	*v = *(short*)(m->data_ + m->read_ptr_);
	m->read_ptr_ += sizeof(*v);
	return 1;
}

int message_write_ushort(struct lua_message* m, unsigned short* v) {
	while (m->sz_ - m->write_ptr_ < sizeof(*v))
		message_expand(m);
	*(unsigned short*)(m->data_ + m->write_ptr_) = *v;
	m->write_ptr_ += sizeof(*v);
	return 1;
}

int message_read_ushort(struct lua_message* m, unsigned short* v) {
	if (m->sz_ - m->read_ptr_ < sizeof(*v))
		return 0;
	*v = *(unsigned short*)(m->data_ + m->read_ptr_);
	m->read_ptr_ += sizeof(*v);
	return 1;
}

int message_write_int(struct lua_message* m, int* v) {
	while (m->sz_ - m->write_ptr_ < sizeof(*v))
		message_expand(m);
	*(int*)(m->data_ + m->write_ptr_) = *v;
	m->write_ptr_ += sizeof(*v);
	return 1;
}

int message_read_int(struct lua_message* m, int* v) {
	if (m->sz_ - m->read_ptr_ < sizeof(*v))
		return 0;
	*v = *(int*)(m->data_ + m->read_ptr_);
	m->read_ptr_ += sizeof(*v);
	return 1;
}

int message_write_uint(struct lua_message* m, unsigned int* v) {
	while (m->sz_ - m->write_ptr_ < sizeof(*v))
		message_expand(m);
	*(unsigned int*)(m->data_ + m->write_ptr_) = *v;
	m->write_ptr_ += sizeof(*v);
	return 1;
}

int message_read_uint(struct lua_message* m, unsigned int* v) {
	if (m->sz_ - m->read_ptr_ < sizeof(*v))
		return 0;
	*v = *(unsigned int*)(m->data_ + m->read_ptr_);
	m->read_ptr_ += sizeof(*v);
	return 1;
}

int message_write_int64(struct lua_message* m, long long* v) {
	while (m->sz_ - m->write_ptr_ < sizeof(*v))
		message_expand(m);
	*(long long*)(m->data_ + m->write_ptr_) = *v;
	m->write_ptr_ += sizeof(*v);
	return 1;
}

int message_read_int64(struct lua_message* m, long long* v) {
	if (m->sz_ - m->read_ptr_ < sizeof(*v))
		return 0;
	*v = *(long long*)(m->data_ + m->read_ptr_);
	m->read_ptr_ += sizeof(*v);
	return 1;
}

int message_write_uint64(struct lua_message* m, unsigned long long* v) {
	while (m->sz_ - m->write_ptr_ < sizeof(*v))
		message_expand(m);
	*(unsigned long long*)(m->data_ + m->write_ptr_) = *v;
	m->write_ptr_ += sizeof(*v);
	return 1;
}

int message_read_uint64(struct lua_message* m, unsigned long long* v) {
	if (m->sz_ - m->read_ptr_ < sizeof(*v))
		return 0;
	*v = *(unsigned long long*)(m->data_ + m->read_ptr_);
	m->read_ptr_ += sizeof(*v);
	return 1;
}

int message_write_string(struct lua_message* m, const char* v, unsigned short sz) {
	while (m->sz_ - m->write_ptr_ < sizeof(sz) + sz)
		message_expand(m);
	*(unsigned short*)(m->data_ + m->write_ptr_) = sz;
	m->write_ptr_ += sizeof(sz);
	memcpy(m->data_ + m->write_ptr_, v, sz);
	m->write_ptr_ += sz;
	return 1;
}

int message_read_string(struct lua_message* m, char* v, unsigned short sz) {
	if (sz == 0)
		return 0;
	if (m->sz_ - m->read_ptr_ < sizeof(sz))
		return 0;
	unsigned short str_sz, read_sz;
	str_sz = *(unsigned short*)(m->data_ + m->read_ptr_);
	read_sz = str_sz;
	m->read_ptr_ += sizeof(sz);
	if (m->sz_ - m->read_ptr_ < read_sz)
		return 0;
	if (sz < read_sz)
		read_sz = sz;
	memcpy(v, m->data_ + m->read_ptr_, read_sz);
	m->read_ptr_ += str_sz;
	return 1;
}