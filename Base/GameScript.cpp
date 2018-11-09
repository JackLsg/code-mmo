#include "GameScript.h"
#include "Scene/Script/Script_DataPacket.h"
#include "Scene/Script/Script_User.h"
#include "Scene/Script/script_luapacket.h"

bool SceneScript::LuaLog()
{
	LogTop();
	return false;
}


static const char* scenename = "G_Scene";
static const char* serviceId = "G_ServiceId";

void * lua_gethostenv(lua_State* L)
{
	lua_getglobal(L, scenename);
	int inx = lua_gettop(L);
	const void* p = lua_topointer(L, inx);
	lua_pop(L, 1);

	return (void*)p;
}

tint32 lua_getServiceId(lua_State* L)
{
	lua_getglobal(L, serviceId);
	int inx = lua_gettop(L);
	tint32 p = (tint32)lua_tointeger(L, inx);
	lua_pop(L, 1);
	return p;
}

void SceneScript::SetHostEnv(void* data)
{
	RegistGlobalTable(data, scenename);
}

void SceneScript::SetServiceId(tint32 sid)
{
	RegistGlobalInt(sid, serviceId);
}

void SceneScript::RegistGlobalTable(void* data, const char* name)
{
	if (!data || !name)
	{
		return ;
	}
	lua_pushlightuserdata(m_pLua, data);
	lua_setglobal(m_pLua, name);
	return ;
}

void SceneScript::RegistGlobalInt(tint64 data, const char* name)
{
	if (!name)
	{
		return;
	}
	lua_pushinteger(m_pLua, data);
	lua_setglobal(m_pLua, name);
	return;
}

//定义宏用来方便使用设置键值  
#define LUA_REGFUNC(L, func) lua_register(L, #func, func)

#define  LUA_REGTABLEFUNC(L, func) \
	lua_pushliteral(L, #func ); \
    lua_pushcfunction(L, func); \
	lua_settable(m_pLua, -3)

#define LUA_REGTABLEENUM(L, name, val) \
  lua_pushliteral(L, name); \
  lua_pushnumber(L, val); \
  lua_settable(L, -3)

using namespace DataPack;

bool SceneScript::RegistLocalLibs()
{
	__ENTER_PROTECT

	AssertEx(m_pLua != null_ptr, "");

	//luapacket
	load_lib_luapacket(m_pLua);

	// DataPack begin
	lua_newtable(m_pLua);

	LUA_REGTABLEFUNC(m_pLua, AllocDataPack);
	LUA_REGTABLEFUNC(m_pLua, getAvaliableLength);
	LUA_REGTABLEFUNC(m_pLua, getPosition);
	LUA_REGTABLEFUNC(m_pLua, getLength);
	LUA_REGTABLEFUNC(m_pLua, setPosition);

	LUA_REGTABLEFUNC(m_pLua, readByte);
	LUA_REGTABLEFUNC(m_pLua, writeByte);
	LUA_REGTABLEFUNC(m_pLua, readShort);
	LUA_REGTABLEFUNC(m_pLua, writeShort);
	LUA_REGTABLEFUNC(m_pLua, readInt);
	LUA_REGTABLEFUNC(m_pLua, writeInt);
	LUA_REGTABLEFUNC(m_pLua, readInt64);
	LUA_REGTABLEFUNC(m_pLua, writeInt64);
	LUA_REGTABLEFUNC(m_pLua, readString);
	LUA_REGTABLEFUNC(m_pLua, writeString);

	lua_setglobal(m_pLua, "DataPack");
	// DataPack end

	LUA_REGFUNC(m_pLua, C_SendLuaPacket);
	LUA_REGFUNC(m_pLua, C_CreateLuaInvoker);
	LUA_REGFUNC(m_pLua, C_StartLuaInvoker);


	LUA_REGFUNC(m_pLua, C_GetTime);
	LUA_REGFUNC(m_pLua, C_GetTimeMS);
	LUA_REGFUNC(m_pLua, C_Test);
	LUA_REGFUNC(m_pLua, C_Encode);
	LUA_REGFUNC(m_pLua, C_Decode);
	LUA_REGFUNC(m_pLua, C_Encode_Only);
	LUA_REGFUNC(m_pLua, C_Decode_Only);

	return true;
}