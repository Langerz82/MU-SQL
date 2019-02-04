////////////////////////////////////////////////////////////////////////////////
// LuaClass.h
#ifndef LUACLASS_H
#define LUACLASS_H

#pragma once
class CLuaClass
{
public:
	CLuaClass(void);
	virtual ~CLuaClass(void);

	bool DoFile(const char* szFileName);
	lua_State * GetLua();
	bool Create();
	bool Release();

private:

	lua_State * m_pLuaState;
};

extern bool g_Generic_Call(lua_State *L, const char* func, const char* sig, ...);

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

