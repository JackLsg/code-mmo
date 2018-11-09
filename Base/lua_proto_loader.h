#ifndef _lua_proto_loader_h_
#define _lua_proto_loader_h_

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "lua_message.h"

//type base
#define T_NIL 0xFFFFFFFF
#define T_MIN 0
#define T_NUMBER_MIN 0
#define T_BOOLEAN 0
#define T_BYTE 1
#define T_SHORT 2
#define T_USHORT 3
#define T_INT 4
#define T_UINT 5
#define T_INT64 6
#define T_UINT64 7
#define T_NUMBER_MAX 8
#define T_STRING 8
#define T_STRUCT 9

//member type
#define T_SINGLE 0
#define T_VECTOR 1
#define T_MAP 2

#define STR_LEN 64

struct lua_proto_slot
{
	size_t member_type_;
	char   member_name_[STR_LEN];
	size_t type1_;
	size_t type2_;
	struct lua_proto* proto1_;
	struct lua_proto* proto2_;
	struct lua_proto_slot* next_;
};

struct lua_proto
{
	size_t sysid_;
	size_t cmd_;
	char name_[STR_LEN];
	struct lua_proto_slot* slot_;
	size_t slot_list_sz_;
};

//convert type int to type string
size_t parse_string_2_base_type(const char* str, size_t sz);
//function ptr for string check
typedef int(*STR_CHECK_FUNC)(const char* start, const char* end, size_t* p_read_ptr, const char* str, size_t str_sz);

//search string
size_t search_string_t(const char* start, const char* end, const char* str, size_t str_sz, STR_CHECK_FUNC check_func);
//check string is valid
int check_valid_string(const char* start, const char* end, size_t* p_read_ptr, const char* str, size_t str_sz);
//check string is finded
int check_find_str_t(const char* start, const char* end, size_t* p_read_ptr, const char* str, size_t str_sz);
//find the first valid character where the character is not in {' ', '\t', '\r', '\n'}
size_t search_valid_string(const char* start, const char* end);
//find the first valid character where the character is in {' ', '\t', '\r', '\}
size_t search_invalid_string(const char* start, const char* end);
//find the first position of string
size_t search_string(const char* start, const char* end, const char* str, size_t str_sz);
//r-trim
size_t string_rtrim(char* str, size_t str_sz);

//read a struct
struct lua_proto* read_one_struct(const char* str, size_t str_sz, size_t* pread_ptr);

//load proto file
void load_proto_file(const char* str, size_t str_sz);
//get struct lua_proto
struct lua_proto* get_lua_proto(size_t sysid, size_t cmd);
struct lua_proto* get_lua_proto_ex(const char* proto_name);

#endif