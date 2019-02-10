#ifndef _MU_CONNECTENGINE_H
#define _MU_CONNECTENGINE_H

#include "StdAfx.h"
#include "IOCP.h"

void UserInit();
short UserAdd(SOCKET s, char* IP);
short UserDelete(int index);
bool UserSet(int index, unsigned int Port, int type, char* Name);
void UserRun();

extern std::vector<STR_CS_USER*> gConnUsers;

#endif



