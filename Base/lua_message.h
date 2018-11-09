#ifndef _lua_message_h_
#define _lua_message_h_

#include <stdlib.h>
#include <string.h>

struct lua_message
{
	char* data_;
	size_t sz_;
	size_t write_ptr_;
	size_t read_ptr_;
};

//new lua_message
struct lua_message* message_malloc();
//init message, not alloc memory
int message_init(struct lua_message* m);
//expand message(*2)
int message_expand(struct lua_message* m);
//free memory
int message_free(struct lua_message*& m);
//write boolean
int message_write_boolean(struct lua_message* m, char* v);
//read boolean
int message_read_boolean(struct lua_message* m, char* v);
//write byte
int message_write_byte(struct lua_message* m, char* v);
//read byte
int message_read_byte(struct lua_message* m, char* v);
//write short
int message_write_short(struct lua_message* m, short* v);
//read short
int message_read_short(struct lua_message* m, short* v);
//write unsigned short
int message_write_ushort(struct lua_message* m, unsigned short* v);
//read unsigned short
int message_read_ushort(struct lua_message* m, unsigned short* v);
//write int32
int message_write_int(struct lua_message* m, int* v);
//read int32
int message_read_int(struct lua_message* m, int* v);
//write unsigned int32
int message_write_uint(struct lua_message* m, unsigned int* v);
//read unsigned int32
int message_read_uint(struct lua_message* m, unsigned int* v);
//write int64
int message_write_int64(struct lua_message* m, long long* v);
//read int64
int message_read_int64(struct lua_message* m, long long* v);
//wtite unsigned int64
int message_write_uint64(struct lua_message* m, unsigned long long* v);
//read unsigned int64
int message_read_uint64(struct lua_message* m, unsigned long long* v);
//write string, string size(unsigned short) + strlen(string)
int message_write_string(struct lua_message* m, const char* v, unsigned short sz);
//read string, string size(unsigned short) + strlen(string)
int message_read_string(struct lua_message* m, char* v, unsigned short sz);

#endif