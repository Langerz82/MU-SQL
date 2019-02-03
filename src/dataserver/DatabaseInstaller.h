////////////////////////////////////////////////////////////////////////////////
// DatabaseInstaller.h
#pragma once
class CDatabaseInstaller
{
public:
	CDatabaseInstaller(void);
	virtual ~CDatabaseInstaller(void);

	int GetDBVersion();

	void InstallUpdates();

	bool Install7041();
	bool Install7042();

	void Init();
	void End();

	CQuery * Database;
	
	bool m_IsConnected;
};



////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

