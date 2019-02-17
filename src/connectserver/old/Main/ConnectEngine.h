#ifndef _MU_CONNECTENGINE_H
#define _MU_CONNECTENGINE_H

#include "StdAfx.h"
#include "IOCP.h"

#include <map>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>


extern std::map<int, STR_CS_USER*> gCSUsers;
// Functions forward declared.
STR_CS_USER* getUser(int index);
void insertUser(STR_CS_USER* Obj);
void eraseUser(int index);


void UserInit();
STR_CS_USER* UserAdd(char* SocketKey, char* IP);
short UserDelete(int index);
bool UserSet(int index, unsigned int Port, int type, char* Name);
void UserRun();

extern STR_CS_USER* getCSUser(int index) { return gCSUsers.find(index)->second; }
extern void insertUser(STR_CS_USER* Obj) { gCSUsers.insert(std::pair<int, STR_CS_USER*>(Obj->Index, Obj)); }
extern void eraseUser(int index) { gCSUsers.erase(index); }

#endif



