#pragma once
#include "Base.h"
#include "LibLua.h"
#include <set>
#include "DataPacket.h"

void * lua_gethostenv(lua_State* L);
tint32 lua_getServiceId(lua_State* L);
class SceneScript:public LuaBase
{
public:
	virtual bool RegistLocalLibs();
	void RegistGlobalTable(void* data, const char* name);
	void RegistGlobalInt(tint64 data, const char* name);
	void SetHostEnv(void* data);
	void SetServiceId(tint32 sid);
	bool LuaLog();
	//void SetEnv();
	//DataPacket& GetDataPacket()
	//{

	//}
private:
	typedef std::set<tint32> ScriptIDSet;
	ScriptIDSet m_ErrorScriptIDSet;		//¡¾stl¡¿
};
