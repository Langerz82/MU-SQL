////////////////////////////////////////////////////////////////////////////////
// NewsSystem.h
#pragma once

struct NEWS_DATA
{
	int	GUID;
	std::string News[3];
};

class CNewsSystem
{
public:
	CNewsSystem(void);
	~CNewsSystem(void);

	bool LoadFile(LPSTR lpFile);
	void Run();

	bool m_bEnable;
	std::vector<NEWS_DATA> m_NewsData;
	int	m_iMaxGUID;
	int m_iMinuteDelay;
	ULONGLONG m_i64Tick;
};

extern CNewsSystem g_NewsSystem;

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

