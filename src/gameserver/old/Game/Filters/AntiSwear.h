////////////////////////////////////////////////////////////////////////////////
// AntiSwear.h
#ifndef _MU_ANTISWEAR_H
#define _MU_ANTISWEAR_H

#include <vector>
#include <string>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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

