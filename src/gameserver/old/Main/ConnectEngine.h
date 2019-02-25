#ifndef _MU_CONNECTENGINE_H
#define _MU_CONNECTENGINE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "IOCP.h"
#include "Game/User/CUserData.h"

extern std::map<int, STR_CS_USER*> gCSUsers;

void UserInit();
STR_CS_USER* UserAdd(const char* socketKey, char* IP);
short UserDelete(int index);
bool UserSet(int index, unsigned int Port, int type, char* Name);
void UserRun();

inline STR_CS_USER* getCSUser(int index) { return gCSUsers.find(index)->second; }
inline int insertCSUser(STR_CS_USER* Obj) 
{ 
	gCSUsers.insert(std::pair<int, STR_CS_USER*>(12000 - Obj->Index, Obj));
	return 12000 - Obj->Index;
}

inline void eraseCSUser(int index) { gCSUsers.erase(index); }

#endif



