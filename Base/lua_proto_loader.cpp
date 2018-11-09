#include "lua_proto_loader.h"

//type string desc
#define STR_BOOLEAN "bool"
#define STR_BYTE "byte"
#define STR_SHORT "short"
#define STR_USHORT "ushort"
#define STR_INT "int32"
#define STR_UINT "uint32"
#define STR_INT64 "int64"
#define STR_UINT64 "uint64"
#define STR_STRING "string"


#define STR_COMMENT "//"
#define STR_COMMENT_BLOCK "/*"
#define STR_COMMENT_BLOCK_END "*/"
#define STR_STRUCT_END "}"
#define STR_VECTOR "vector"
#define STR_MAP_TAG "map"
#define STR_VECTOR_TYPE_START "<"
#define STR_MAP_TYPE_START "<"
#define STR_VECTOR_TYPE_END ">"
#define STR_MAP_TYPE_END ">"
#define STR_MAP_TYPE_SPLIT ","
#define STR_STRUCT_START "message"
#define STR_MEMBER_START "{"
#define STR_PTOTOID_START ":"
#define STR_MEMBER_END ";"
#define STR_PROTO_START "//@"
#define STR_SYSID "sysid"
#define STR_CMD "cmd"
#define STR_EQ "="
#define STR_STRUCT "message"
#define STR_REQUIRED "required"
#define STR_REPEATED "repeated"

#define MAX_ID 512
#define MAX_STRUCT_ID 5120
#define MAX_HASH_SIZE 102400

struct lua_proto* proto_list_[MAX_ID][MAX_ID] = { 0 };
struct lua_proto* struct_list_[MAX_STRUCT_ID] = { 0 };
size_t max_sysid_ = 0;
size_t max_cmd_ = 0;
size_t max_structid_ = 0;

struct cmd_hash_node
{
	char name[STR_LEN];
	size_t sysid_;
	size_t cmd_;
	cmd_hash_node* next_;
}* cmd_hash_[MAX_HASH_SIZE] = {0};

size_t parse_string_2_base_type(const char* str, size_t sz) {
	if (memcmp(str, STR_BOOLEAN, strlen(STR_BOOLEAN)) == 0) {
		return T_BOOLEAN;
	}
	else if (memcmp(str, STR_BYTE, strlen(STR_BYTE)) == 0) {
		return T_BYTE;
	}
	else if (memcmp(str, STR_SHORT, strlen(STR_SHORT)) == 0) {
		return T_SHORT;
	}
	else if (memcmp(str, STR_USHORT, strlen(STR_USHORT)) == 0) {
		return T_USHORT;
	}
	else if (memcmp(str, STR_INT64, strlen(STR_INT64)) == 0) {
		return T_INT64;
	}
	else if (memcmp(str, STR_UINT64, strlen(STR_UINT64)) == 0) {
		return T_UINT64;
	}
	else if (memcmp(str, STR_INT, strlen(STR_INT)) == 0) {
		return T_INT;
	}
	else if (memcmp(str, STR_UINT, strlen(STR_UINT)) == 0) {
		return T_UINT;
	}
	else if (memcmp(str, STR_STRING, strlen(STR_STRING)) == 0) {
		return T_STRING;
	}
	else {
		if (strlen(str) == 0) {
			assert(0);
		}
		else {
			return T_STRUCT;
		}
	}
	return T_NIL;;
}

size_t search_string_t(const char* start, const char* end, const char* str, size_t str_sz, STR_CHECK_FUNC check_func) {
	size_t read_ptr = 0;
	int find = 0;
	int is_comment = 0;
	int is_comment_ex = 0;
	while (start + read_ptr <= end) {
		if (is_comment || is_comment_ex) {
			while (start + read_ptr <= end) {
				if (is_comment && start[read_ptr] == '\n' || start[read_ptr] == '\r') {
					read_ptr++;
					is_comment = 0;
					break;
				}
				else if (is_comment_ex && start + read_ptr + 1 <= end && memcmp(start + read_ptr, STR_COMMENT_BLOCK_END, strlen(STR_COMMENT_BLOCK_END)) == 0) {
					read_ptr += 2;
					is_comment = 0;
					is_comment_ex = 0;
					break;
				}
				else {
					read_ptr++;
				}
			}
		}
		else {
			if ((start + read_ptr + 2 <= end) && ( 
				(memcmp(start + read_ptr, STR_COMMENT, strlen(STR_COMMENT)) == 0 && memcmp(start + read_ptr, STR_PROTO_START, strlen(STR_PROTO_START)) != 0)
					|| (memcmp(start + read_ptr, STR_COMMENT_BLOCK, strlen(STR_COMMENT_BLOCK)) == 0))
				) {
				is_comment = 1;
				if (memcmp(start + read_ptr, STR_COMMENT_BLOCK, strlen(STR_COMMENT_BLOCK)) == 0) {
					is_comment_ex = 1;
				}
				read_ptr += 2;
			}
			else {
				if (check_func(start, end, &read_ptr, str, str_sz)) {
					find = 1;
					break;
				}
				read_ptr++;
			}
		}
	}
	return find == 1 ? read_ptr : 0;
}

int check_valid_string(const char* start, const char* end, size_t* p_read_ptr, const char* str, size_t str_sz) {
	if (start[*p_read_ptr] != ' ' && start[*p_read_ptr] != '\t' && start[*p_read_ptr] != '\n'
		&& start[*p_read_ptr] != '\r') {
		return 1;
	}
	return 0;
}

int check_find_str_t(const char* start, const char* end, size_t* p_read_ptr, const char* str, size_t str_sz) {
	if (start + *p_read_ptr + str_sz <= end
		&& memcmp(start + *p_read_ptr, str, str_sz) == 0) {
		return 1;
	}
	return 0;
}

size_t search_valid_string(const char* start, const char* end) {
	return search_string_t(start, end, 0, 0, check_valid_string);
}

size_t search_invalid_string(const char* start, const char* end) {
	size_t read_ptr = 0;
	while (start + read_ptr <= end) {
		if (start[read_ptr] == ' ' || start[read_ptr] == '\t' || start[read_ptr] == '\n'
			|| start[read_ptr] == '\r' || start[read_ptr] == ';') {
			break;
		}
		read_ptr++;
	}
	return read_ptr;
}

size_t search_string(const char* start, const char* end, const char* str, size_t str_sz) {
	return search_string_t(start, end, str, str_sz, check_find_str_t);
}

//r-trim
size_t string_rtrim(char* str, size_t str_sz) {
	if (str_sz == 0)
		return str_sz;
	size_t ret_sz = str_sz;
	for (size_t i = str_sz - 1; i != 0; i--) {
		if (str[i] != ' ' && str[i] != '\t') {
			break;
		}
		str[i] = '\0';
		ret_sz--;
	}
	return ret_sz;
}

struct lua_proto* read_one_struct(const char* str, size_t str_sz, size_t* pread_ptr) {
	struct lua_proto* p = 0;
	char proto_id_str[STR_LEN] = { 0 };
	//calculate end position of string
	const char* end = str + str_sz;

	//search sysid and cmd
	size_t proto_start_pos = search_string(str, end, STR_PROTO_START, strlen(STR_PROTO_START));
	size_t read_ptr = proto_start_pos;
	read_ptr += strlen(STR_PROTO_START);
	char str_sysid[STR_LEN] = { 0 };
	char str_cmd[STR_LEN] = {0};

	//read sysid
	read_ptr += search_string(str + read_ptr, end, STR_SYSID, strlen(STR_SYSID));
	read_ptr += strlen(STR_SYSID);
	read_ptr += search_string(str + read_ptr, end, STR_EQ, strlen(STR_EQ));
	read_ptr += strlen(STR_EQ);
	read_ptr += search_valid_string(str + read_ptr, end);
	size_t str_sysid_sz = search_invalid_string(str + read_ptr, end);
	assert(str_sysid_sz < STR_LEN);
	memcpy(str_sysid, str + read_ptr, str_sysid_sz);
	read_ptr += str_sysid_sz;

	//read cmd
	read_ptr += search_string(str + read_ptr, end, STR_CMD, strlen(STR_CMD));
	read_ptr += strlen(STR_CMD);
	read_ptr += search_string(str + read_ptr, end, STR_EQ, strlen(STR_EQ));
	read_ptr += strlen(STR_EQ);
	read_ptr += search_valid_string(str + read_ptr, end);
	size_t str_cmd_sz = search_invalid_string(str + read_ptr, end);
	assert(str_cmd_sz < STR_LEN);
	memcpy(str_cmd, str + read_ptr, str_cmd_sz);
	read_ptr += str_cmd_sz;

	//convert sysid and cmd to int
	int sysid = atoi(str_sysid);
	int cmd = atoi(str_cmd);

	//search proto real start position
	size_t struct_head_search_pos = search_string(str + read_ptr, end, STR_STRUCT_START, strlen(STR_STRUCT_START));
	if (struct_head_search_pos == 0) {
		return p;
	}
	read_ptr += struct_head_search_pos;
	//create struct lua_proto
	p = (struct lua_proto*)malloc(sizeof(struct lua_proto));
	memset(p, 0, sizeof(struct lua_proto));
	//skip keyword "struct"
	read_ptr += strlen(STR_STRUCT_START);
	//check rule
	assert(str[read_ptr] == ' ' || str[read_ptr] == '\t');
	//read proto name
	read_ptr += search_valid_string(str + read_ptr, end);
	size_t proto_name_sz = search_invalid_string(str + read_ptr, end);
	assert(proto_name_sz < STR_LEN);
	memcpy(p->name_, str + read_ptr, proto_name_sz);
	read_ptr += proto_name_sz;
	//set protoid
	p->sysid_ = sysid;
	p->cmd_ = cmd;
	//check keyword "{"
	read_ptr += search_string(str + read_ptr, end, STR_MEMBER_START, strlen(STR_MEMBER_START));
	read_ptr += strlen(STR_MEMBER_START);

	//parse member
	struct
	{
		char member_type_str[STR_LEN];
		char member_name[STR_LEN];
	} member_str_list[512] = { 0 };
	size_t member_cnt = 0;

	lua_proto_slot* cur_slot = 0;
	while (read_ptr < str_sz) {
		read_ptr += search_valid_string(str + read_ptr, end);
		size_t read_sz = 0;
		lua_proto_slot* pslot = 0;

		int is_array = 0;
		if (strncmp(str + read_ptr, STR_REPEATED, strlen(STR_REPEATED)) == 0) {
			is_array = 1;
		}
		else {
			if (strncmp(str + read_ptr, STR_REQUIRED, strlen(STR_REQUIRED)) != 0){
				assert(0);
			}
		}

		read_ptr += strlen(STR_REQUIRED);
		read_ptr += search_valid_string(str + read_ptr, end);
		//member type
		char str_member_type[STR_LEN] = { 0 };
		size_t str_member_type_sz = search_invalid_string(str + read_ptr, end);
		assert(str_member_type_sz < STR_LEN);
		memcpy(str_member_type, str + read_ptr, str_member_type_sz);
		read_ptr += str_member_type_sz;
		//member name
		char str_member_name[STR_LEN] = { 0 };
		read_ptr += search_valid_string(str + read_ptr, end);
		size_t str_member_name_sz = search_invalid_string(str + read_ptr, end);
		assert(str_member_name_sz < STR_LEN);
		memcpy(str_member_name, str + read_ptr, str_member_name_sz);
		read_ptr += str_member_name_sz;
		//end str of member
		read_ptr += search_string(str + read_ptr, end, STR_MEMBER_END, strlen(STR_MEMBER_START));
		read_ptr += strlen(STR_MEMBER_END);
		pslot = (struct lua_proto_slot*)malloc(sizeof(lua_proto_slot));
		memset(pslot, 0, sizeof(*pslot));
		pslot->member_type_ = is_array ? T_VECTOR : T_SINGLE;
		memcpy(pslot->member_name_, str_member_name, STR_LEN);
		pslot->type1_ = parse_string_2_base_type(str_member_type, str_member_type_sz);
		if (pslot->type1_ == T_STRUCT) {
			int find = 0;
			//search from struct_list_
			for (size_t i = 0; i < max_structid_; i++) {
				if (struct_list_[i] && memcmp(struct_list_[i]->name_, str_member_type, str_member_type_sz) == 0) {
					find = 1;
					pslot->proto1_ = struct_list_[i];
					break;
				}
			}
			//search from proto_list if not find
			if (!find) {
				for (size_t i = 0; i < max_sysid_; i++) {
					for (size_t j = 0; j < max_cmd_; j++) {
						if (proto_list_[i][j] && memcmp(proto_list_[i][j]->name_, str_member_type, str_member_type_sz) == 0) {
							find = 1;
							pslot->proto1_ = proto_list_[i][j];
							break;
						}
					}
				}
			}
			assert(find);
		}

		assert(pslot);

		if (cur_slot) {
			cur_slot->next_ = pslot;
			cur_slot = pslot;
		}
		else {
			p->slot_ = cur_slot = pslot;
		}

		//check keyword "};"
		read_ptr += search_valid_string(str + read_ptr, end);

		//incr count of member
		member_cnt++;

		//check end of struct
		if (read_ptr + strlen(STR_STRUCT_END) <= str_sz && memcmp(str + read_ptr, STR_STRUCT_END, strlen(STR_STRUCT_END)) == 0) {
			read_ptr += strlen(STR_STRUCT_END);
			break;
		}
	}

	*pread_ptr = read_ptr;
	return p;
}

void close_proto_mod() 
{
	//free proto_list_
	for (size_t i = 0; i <= max_sysid_; i++) {
		for (size_t j = 0; j <= max_cmd_; j++) {
			if (proto_list_[i][j]) {
				free(proto_list_[i][j]);
				proto_list_[i][j] = 0;
			}
		}
	}
	max_sysid_ = 0;
	max_cmd_ = 0;

	//free struct_list_
	for (size_t i = 0; i < max_structid_; i++) {
		if (struct_list_[i]) {
			free(struct_list_[i]);
			struct_list_[i] = 0;
		}
	}
	max_structid_ = 0;

	//free hash node
	for (size_t i = 0; i < MAX_HASH_SIZE; i++) {
		if (cmd_hash_[i]) {
			struct cmd_hash_node* node = cmd_hash_[i];
			while (node) {
				struct cmd_hash_node* del_node = node;
				node = node->next_;
				free(del_node);
			}
			cmd_hash_[i] = 0;
		}
	}
}

size_t hash(const char* k)
{
	register size_t h = 0;
	register size_t g;
	while (*k) {
		h = (h << 4) + *k++;
		g = h & 0xf0000000L;
		if (g) h ^= g >> 24;
		h &= ~g;
	}
	return h % MAX_HASH_SIZE;
}

static int flag = 0;
void load_proto_file(const char* str, size_t str_sz) {
	if (flag > 0)
		return;

	close_proto_mod();

	size_t read_ptr = 0;
	while (read_ptr < str_sz) {
		size_t read_sz = 0;
		struct lua_proto* p = read_one_struct(str + read_ptr, str_sz - read_ptr, &read_sz);
		if (!p) {
			break;
		}
		read_ptr += read_sz;

		assert(p->sysid_ >= 0 && p->sysid_ < MAX_ID);
		assert(p->cmd_ >= 0 && p->cmd_ < MAX_ID);

		if (p->sysid_ == 0 && p->cmd_ == 0) {
			assert(max_structid_ + 1 <= MAX_STRUCT_ID);
			struct_list_[max_structid_++] = p;
		}
		else {
			assert(!proto_list_[p->sysid_][p->cmd_]);
			proto_list_[p->sysid_][p->cmd_] = p;
			if (max_sysid_ < p->sysid_)
				max_sysid_ = p->sysid_;
			if (max_cmd_ < p->cmd_)
				max_cmd_ = p->cmd_;
			//build cmd hash
			size_t h = hash(p->name_);
			struct cmd_hash_node* new_node = (struct cmd_hash_node*)malloc(sizeof(cmd_hash_node));
			new_node->sysid_ = p->sysid_;
			new_node->cmd_ = p->cmd_;
			new_node->next_ = 0;
			if (!cmd_hash_[h]) {
				cmd_hash_[h] = new_node;
			}
			else {
				struct cmd_hash_node* node = cmd_hash_[h];
				while (node) {
					if (node->next_) {
						node = node->next_;
					}
					else {
						node->next_ = new_node;
						break;
					}
				}
			}
		}
	}
	flag++;
}

struct lua_proto* get_lua_proto(size_t sysid, size_t cmd) {
	if (sysid <= max_sysid_ && cmd <= max_cmd_)
		return proto_list_[sysid][cmd];
	return 0;
}

struct lua_proto* get_lua_proto_ex(const char* proto_name) {
	size_t h = hash(proto_name);
	struct cmd_hash_node* ret_node = 0;
	struct cmd_hash_node* node = cmd_hash_[h];

	if (!node->next_) {
		ret_node = node;
	}
	else {
		struct cmd_hash_node* cmp_node = node;
		while (cmp_node) {
			if (strcmp(cmp_node->name, proto_name) == 0) {
				ret_node = node;
				break;
			}
			else {
				cmp_node = cmp_node->next_;
			}
		}
	}

	if (ret_node) {
		if (ret_node->sysid_ <= max_sysid_ && ret_node->cmd_ <= max_cmd_)
			return proto_list_[ret_node->sysid_][ret_node->cmd_];
	}

	return 0;
}


