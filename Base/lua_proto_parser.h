#ifndef _lua_proto_parser_h_
#define _lua_proto_parser_h_

#include "LibLua.h"
#include "DataPacket.h"

//get value-number by name
int proto_get_integer(lua_State *L, int idx, const char* name, long long* v);
//get valuestring by name
int proto_get_string(lua_State *L, int idx, const char* name, const char** v);
//write number by type
int proto_write_number(DataPacket* m, long long v, size_t type);
//encode type T_SINGLE
int proto_encode_single(DataPacket* m, struct lua_proto_slot* pslot, lua_State *L, int idx);
//write type T_SINGLE
int proto_write_single(DataPacket* m, size_t type, struct lua_proto* p, lua_State *L, int idx);
//encode type array
int proto_encode_vector(DataPacket* m, struct lua_proto_slot* pslot, lua_State *L, int idx);
//get real table size
size_t proto_get_table_size(lua_State *L, int idx);
//encode type T_MAP
int proto_encode_map(DataPacket* m, struct lua_proto_slot* pslot, lua_State *L, int idx);
//encode table
int proto_encode_table(DataPacket* m, struct lua_proto* p, lua_State *L, int idx);

//read number
int proto_read_number(DataPacket* m, size_t type, long long* pv);
//read type single
int proto_read_single(DataPacket* m, size_t type, struct lua_proto* p, lua_State *L);
//decode type T_GINGLE
int proto_decode_single(DataPacket* m, struct lua_proto_slot* pslot, lua_State *L);
//decode type T_VECTOR
int proto_decode_vector(DataPacket* m, struct lua_proto_slot* pslot, lua_State *L);
//decode type T_MAP
int proto_decode_map(DataPacket* m, struct lua_proto_slot* pslot, lua_State *L);
//decode table
int proto_decode_table(DataPacket* m, struct lua_proto* p, lua_State *L);

#endif