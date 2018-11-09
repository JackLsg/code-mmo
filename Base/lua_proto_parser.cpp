#include "lua_proto_parser.h"
#include "lua_proto_loader.h"
#include "DataPacket.h"

int proto_write_number(DataPacket* m, long long v, size_t type) {
	int err = 0;
	switch (type) {
	case T_BOOLEAN:
	{
		char v_ = (char)v;
		*m << v_;
	}
	break;
	case T_BYTE:
	{
		tint8 v_ = (tint8)v;
		*m << v_;
	}
	break;
	case T_SHORT:
	{
		short v_ = (short)v;
		*m << v_;
	}
	break;
	case T_USHORT:
	{
		unsigned short v_ = (unsigned short)v;
		*m << v_;
	}
	break;
	case T_INT:
	{
		int v_ = (int)v;
		*m << v_;
	}
	break;
	case T_UINT:
	{
		unsigned int v_ = (unsigned int)v;
		*m << v_;
	}
	break;
	case T_INT64:
	{
		long long v_ = (long long)v;
		*m << v_;
	}
	break;
	case T_UINT64:
	{
		unsigned long long v_ = (unsigned long long)v;
		*m << v_;
	}
	break;
	default:
	{
		err = 1;
		assert(0);
	}
	break;
	}
	return !err;
}

int proto_encode_single(DataPacket* m, struct lua_proto_slot* pslot, lua_State *L, int idx) {
	assert(pslot->member_type_ == T_SINGLE);
	int err = 0;
	if (pslot->type1_ >= T_NUMBER_MIN && pslot->type1_ < T_NUMBER_MAX) {
		long long v = 0;
		lua_pushstring(L, pslot->member_name_);
		if (lua_rawget(L, -2) == LUA_TNUMBER) {
			v = lua_tointeger(L, -1);
		}
		else {
			err = 1;
		}
		lua_pop(L, 1);
		if (!err) {
			proto_write_number(m, v, pslot->type1_);
		}
	}
	else if (pslot->type1_ == T_STRING) {
		const char* v = 0;
		lua_pushstring(L, pslot->member_name_);
		if (lua_rawget(L, -2) == LUA_TSTRING) {
			v = lua_tostring(L, -1);
		}
		else {
			err = 1;
		}
		lua_pop(L, 1);
		if (!err) {
			m->writeString(v);
		}
	}
	else if (pslot->type1_ == T_STRUCT) {
		struct lua_proto* p = pslot->proto1_;
		if (p) {
			int find = 0;
			lua_pushnil(L);
			while (lua_next(L, idx - 1)) {
				if (!lua_isstring(L, -2)) {
					lua_pop(L, 1);
					continue;
				}
				const char* k = lua_tostring(L, -2);
				if (strcmp(k, pslot->member_name_) != 0) {
					lua_pop(L, 1);
					continue;
				}
				if (!lua_istable(L, -1)) {
					lua_pop(L, 2);
					err = 1;
					break;
				}
				err = !proto_encode_table(m, p, L, -1);
				find = 1;
				lua_pop(L, 2);
				break;
			}

			if (!find || err) {
				err = 1;
			}
		}
		else {
			err = 1;
		}
	}
	else {
		err = 1;
	}
	return !err;
}

int proto_write_single(DataPacket* m, size_t type, struct lua_proto* p, lua_State *L, int idx) {
	int err = 0;
	if (type >= T_NUMBER_MIN && type < T_NUMBER_MAX) {
		long long v = (long long)lua_tointeger(L, idx);
		if (!proto_write_number(m, v, type)) {
			err = 1;
		}
	}
	else if (type == T_STRING) {
		const char* v = lua_tostring(L, idx);
		m->writeString(v);
	}
	else if (type == T_STRUCT) {
		if (p) {
			if (!proto_encode_table(m, p, L, idx)) {
				err = 1;
			}
		}
		else {
			err = 1;
		}
	}
	else {
		err = 1;
	}
	return !err;
}

int proto_encode_vector(DataPacket* m, struct lua_proto_slot* pslot, lua_State *L, int idx) {
	assert(pslot->member_type_ == T_VECTOR);
	int err = 0;
	lua_pushstring(L, pslot->member_name_);
	if (lua_rawget(L, -2) == LUA_TTABLE) {
		int arr_sz = (unsigned int)lua_rawlen(L, -1);
		*m << arr_sz;
		for (int arr_idx = 1; arr_idx <= arr_sz; arr_idx++) {
			lua_rawgeti(L, -1, arr_idx);
			if (!proto_write_single(m, pslot->type1_, pslot->proto1_, L, -1)) {
				err = 1;
				lua_pop(L, 1);
				break;
			}
			lua_pop(L, 1);
		}
	}
	else {
		err = 1;
	}
	lua_pop(L, 1);
	return !err;
}

size_t proto_get_table_size(lua_State *L, int idx) {
	size_t sz = 0;
	lua_pushnil(L);
	while (lua_next(L, idx - 1)) {
		sz++;
		lua_pop(L, 1);
	}
	return sz;
}

int proto_encode_map(DataPacket* m, struct lua_proto_slot* pslot, lua_State *L, int idx) {
	assert(pslot->member_type_ == T_MAP);
	int err = 0;
	lua_pushnil(L);
	while (lua_next(L, idx - 1)) {
		if (!lua_isstring(L, -2)) {
			lua_pop(L, 1);
			continue;
		}
		const char* k = lua_tostring(L, -2);
		if (strcmp(k, pslot->member_name_) != 0) {
			lua_pop(L, 1);
			continue;
		}
		if (!lua_istable(L, -1)) {
			lua_pop(L, 2);
			err = 1;
			break;
		}

		int map_sz = (tuint32)proto_get_table_size(L, -1);
		*m << map_sz;

		lua_pushnil(L);
		while (lua_next(L, -2)) {
			if (!proto_write_single(m, pslot->type1_, pslot->proto1_, L, -2)) {
				err = 1;
				lua_pop(L, 2);
				break;
			}
			if (!proto_write_single(m, pslot->type2_, pslot->proto2_, L, -1)) {
				err = 1;
				lua_pop(L, 2);
				break;
			}
			lua_pop(L, 1);
		}

		lua_pop(L, 2);
		break;
	}
	return !err;
}

int proto_encode_table(DataPacket* m, struct lua_proto* p, lua_State *L, int idx) {
	struct lua_proto_slot* pslot = p->slot_;
	int err = 0;

	while (pslot) {
		if (pslot->member_type_ == T_SINGLE) {
			err = !proto_encode_single(m, pslot, L, idx);
			if (err) {
				break;
			}
		}
		else if (pslot->member_type_ == T_VECTOR) {
			err = !proto_encode_vector(m, pslot, L, idx);
			if (err) {
				break;
			}
		}
		else if (pslot->member_type_ == T_MAP) {
			err = !proto_encode_map(m, pslot, L, idx);
			if (err) {
				break;
			}
		}
		else {
			err = 1;
			break;
		}
		pslot = pslot->next_;
	}
	return !err;
}

int proto_read_number(DataPacket* m, size_t type, long long* pv) {
	int err = 0;
	switch (type)
	{
	case T_BOOLEAN:
	{
		*pv = (long long)m->readAtom<char>();
	}
	break;
	case T_BYTE:
	{
		*pv = (long long)m->readAtom<tuint8>();
	}
	break;
	case T_SHORT:
	{
		*pv = (long long)m->readAtom<tint16>();
	}
	break;
	case T_USHORT:
	{
		*pv = (long long)m->readAtom<tuint16>();
	}
	break;
	case T_INT:
	{
		*pv = (long long)m->readAtom<tint32>();
	}
	break;
	case T_UINT:
	{
		*pv = (long long)m->readAtom<tuint32>();
	}
	break;
	case T_INT64:
	{
		*pv = (long long)m->readAtom<tint64>();
	}
	break;
	case T_UINT64:
	{
		*pv = (long long)m->readAtom<tuint64>();
	}
	break;
	default:
		err = 1;
		break;
	}
	return !err;
}

int proto_read_single(DataPacket* m, size_t type, struct lua_proto* p, lua_State *L) {
	int err = 0;
	if (type >= T_NUMBER_MIN && type < T_NUMBER_MAX) {
		long long v = 0;
		err = !proto_read_number(m, type, &v);
		if (!err) {
			lua_pushnumber(L, (lua_Number)v);
		}
	}
	else if (type == T_STRING) {
		const char* str = NULL;
		*m >> str;
		if (!err) {
			lua_pushstring(L, str);
		}
	}
	else if (type == T_STRUCT) {
		if (p) {
			err = !proto_decode_table(m, p, L);
		}
		else {
			err = 1;
		}
	}
	else {
		err = 1;
	}
	return !err;
}

int proto_decode_single(DataPacket* m, struct lua_proto_slot* pslot, lua_State *L) {
	assert(pslot->member_type_ == T_SINGLE);
	int err = 0;
	lua_pushstring(L, pslot->member_name_);
	err = !proto_read_single(m, pslot->type1_, pslot->proto1_, L);
	if (!err) {
		lua_rawset(L, -3);
	}
	else {
		lua_pop(L, 1);
	}
	return !err;
}

int proto_decode_vector(DataPacket* m, struct lua_proto_slot* pslot, lua_State *L) {
	assert(pslot->member_type_ == T_VECTOR);
	int err = 0;
	lua_pushstring(L, pslot->member_name_);

	int arr_sz = 0;
	*m >> arr_sz;

	if (err) {
		lua_pop(L, 1);
		return !err;
	}
	else {
		lua_createtable(L, (int)arr_sz, 0);
	}

	for (unsigned short i = 0; i < arr_sz; i++) {
		lua_pushnumber(L, (lua_Number)(i + 1));
		err = !proto_read_single(m, pslot->type1_, pslot->proto1_, L);
		if (!err) {
			lua_rawset(L, -3);
		}
		else {
			lua_pop(L, 1);
			break;
		}
	}

	if (!err) {
		lua_rawset(L, -3);
	}
	else {
		lua_pop(L, 2);
	}

	return !err;
}

int proto_decode_map(DataPacket* m, struct lua_proto_slot* pslot, lua_State *L) {
	assert(pslot->member_type_ == T_MAP);
	int err = 0;
	lua_pushstring(L, pslot->member_name_);

	int map_sz = 0;
	*m >> map_sz;

	if (err) {
		lua_pop(L, 1);
		return !err;
	}
	else {
		lua_createtable(L, 0, (int)map_sz);
	}

	for (unsigned short i = 0; i < map_sz; i++) {
		err = !proto_read_single(m, pslot->type1_, pslot->proto1_, L);
		if (err) {
			break;
		}
		err = !proto_read_single(m, pslot->type2_, pslot->proto2_, L);
		if (err) {
			lua_pop(L, 1);
			break;
		}
		lua_rawset(L, -3);
	}

	if (!err) {
		lua_rawset(L, -3);
	}
	else {
		lua_pop(L, 2);
	}

	return !err;
}

int proto_decode_table(DataPacket* m, struct lua_proto* p, lua_State *L) {
	struct lua_proto_slot* pslot = p->slot_;
	int err = 0;

	int slot_sz = 0;
	struct lua_proto_slot* pslot_ = pslot;
	while (pslot_) {
		++slot_sz;
		pslot_ = pslot_->next_;
	}
	lua_createtable(L, 0, slot_sz);

	while (pslot) {
		if (pslot->member_type_ == T_SINGLE) {
			err = !proto_decode_single(m, pslot, L);
			if (err) {
				break;
			}
		}
		else if (pslot->member_type_ == T_VECTOR) {
			err = !proto_decode_vector(m, pslot, L);
			if (err) {
				break;
			}
		}
		else if (pslot->member_type_ == T_MAP) {
			err = !proto_decode_map(m, pslot, L);
			if (err) {
				break;
			}
		}
		else {
			err = 1;
			break;
		}
		pslot = pslot->next_;
	}

	if (err) {
		lua_pop(L, 1);
	}

	return !err;
}
