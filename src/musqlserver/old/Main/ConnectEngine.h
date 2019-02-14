#ifndef _MU_CONNECTENGINE_H
#define _MU_CONNECTENGINE_H

#include "StdAfx.h"
#include "IOCP.h"

#include <map>


std::map<int, STR_CS_USER*> gUsers;
// Functions forward declared.
STR_CS_USER* getUser(int index);
void insertUser(STR_CS_USER* Obj);
void eraseUser(STR_CS_USER* Obj);


void UserInit();
short UserAdd(SOCKET s, char* IP);
short UserDelete(int index);
bool UserSet(int index, unsigned int Port, int type, char* Name);
void UserRun();

extern STR_CS_USER* getUser(int index) { return gUsers.find(index)->second; }
extern void insertUser(STR_CS_USER* Obj) { gUsers.insert(std::pair<int, STR_CS_USER*>(Obj->Index, Obj)); }
extern void eraseUser(int index) { gUsers.erase(index); }

#endif



