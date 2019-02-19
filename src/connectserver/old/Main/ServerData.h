//////////////////////////////////////////////////////////////////////
// ServerData.h
#pragma once

class CServerData
{
public:
	CServerData(void);
	virtual ~CServerData(void);

	void LoadServerFile(LPSTR lpszFile);
	void LoadNewsFile(LPSTR lpszFile);
	void Run();

	SERVER_INFO m_Servers[100];
	NEWS_INFO m_News[10];

	char szTitle[40];
};

extern CServerData m_ServerData;




////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

