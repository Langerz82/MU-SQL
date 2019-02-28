////////////////////////////////////////////////////////////////////////////////
// ViewportGuild.h
// ------------------------------
// Decompiled by Deathway
// Date : 2007-05-09
// ------------------------------
#ifndef VIEWPORTGUILD_H
#define VIEWPORTGUILD_H

#include "User/CUserData.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CViewportGuild
{

public:

	CViewportGuild();
	virtual ~CViewportGuild();

	void Init();
	BOOL Add(int number, int TargetObj);

private:

	short m_GuildNumber[MAX_VIEWPORT];
	short m_GuildCount;
};

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

