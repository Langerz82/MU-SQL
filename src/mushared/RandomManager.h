// RandomManager.h: interface for the CRandomManager class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

struct RANDOM_MANAGER_INFO
{
	int value;
	int MinRate;
	int MaxRate;
};

class CRandomManager
{
public:
	CRandomManager();
	virtual ~CRandomManager();
	void Clear();
	bool GetRandomElement(int* value);
	bool GetRandomElement(WORD* value);
	bool GetRandomElement(BYTE* value);
	void AddElement(int value, int rate);
private:
	int m_MaxRate;
	std::vector<RANDOM_MANAGER_INFO> m_RandomManagerInfo;
};


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

