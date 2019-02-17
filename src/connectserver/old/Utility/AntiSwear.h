////////////////////////////////////////////////////////////////////////////////
// AntiSwear.h
#ifndef _MU_ANTISWEAR_H_
#define _MU_ANTISWEAR_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include <vector>
#include <string>


class CAntiSwear
{
public:
	CAntiSwear(void);
	virtual ~CAntiSwear(void);

	bool LoadFile(const char* File);
	bool CompareText(const char* Text);

	std::vector<std::string> m_SwearWords;
};

extern CAntiSwear SwearFilter;

#endif

