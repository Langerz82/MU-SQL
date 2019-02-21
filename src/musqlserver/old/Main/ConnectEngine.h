#ifndef _MU_CONNECTENGINE_H
#define _MU_CONNECTENGINE_H

#include "StdAfx.h"
#include "IOCP.h"

#include <map>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>


std::map<int, STR_CS_USER*> gUsers;


void UserInit();
short UserAdd(char* SocketKey, char* IP);
short UserDelete(int index);
bool UserSet(int index, unsigned int Port, int type, char* Name);
void UserRun();

extern STR_CS_USER* getCSUser(int index) { return gUsers.find(index)->second; }
extern void insertUser(STR_CS_USER* Obj) { gUsers.insert(std::pair<int, STR_CS_USER*>(Obj->Index, Obj)); }
extern void eraseUser(int index) { gUsers.erase(index); }

#endif



