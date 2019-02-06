////////////////////////////////////////////////////////////////////////////////
// MuLua.h
// ------------------------------
// Decompiled by Hybrid
// 1.01.00
// ------------------------------

#ifndef MULUA_H
#define MULUA_H

#include "StdAfx.h"

#define LUA_REGULARSKILL_SCRIPT_PATH "\\Scripts\\Skills\\RegularSkillCalc.lua"

#pragma once

class MULua
{
public:
	MULua(bool UseSync = false);
	~MULua();

private:
	bool Create();

public:
	void Release();

	bool DoFile(const char* szFileName);
	bool DoFile(char* szBuff, size_t buffSize);
	bool DoFile(lua_State* L, const char* szFileName);

	bool DoString(std::string kString);

	lua_State* GetLua();

	void Register(void* pLua);
	void RegisterData(const luaL_Reg * Table, char * TableName, char * DataName, size_t Size);

	bool Generic_Call(const char* func, const char* sig, ...);

	void CreateWinConsole(HINSTANCE hInstance);
	void DestroyWinConsole();

private:
	lua_State*			m_luaState;
	CRITICAL_SECTION	m_luaCritical;
	bool				m_bUseSync;
};

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

