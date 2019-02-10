////////////////////////////////////////////////////////////////////////////////
// EventMonSetBase.h
#ifndef EVENTMONSETBASE_H
#define EVENTMONSETBASE_H

struct EVENT_MONSTER_POSITION
{
	WORD m_Type;	// 2
	BYTE m_MapNumber;	// 4
	BYTE m_Dis;	// 5
	BYTE m_X;	// 6
	BYTE m_Y;	// 7
	BYTE m_Dir;	// 8
	BYTE m_W;	// 9
	BYTE m_H;	// A
	BYTE m_StartDay;
	BYTE m_EndDay;
	BYTE m_StartHour;
	BYTE m_EndHour;
	BYTE m_DoSpawn;
};

struct EVENT_MONSTER_DATA
{
	int iMonsterIndex;
	boost::shared_ptr<EVENT_MONSTER_POSITION> lpSpotData;
};

typedef

#pragma once
class CEventMonSetBase
{
public:
	CEventMonSetBase();
	virtual ~CEventMonSetBase();

	void LoadScript(char *szFile);

	void Run();

	void SpawnMonster(boost::shared_ptr<EVENT_MONSTER_POSITION> lpSpotData);
	void RegenMonster(int iIndex);
	void DeleteAllMonsters();

	bool IsEventMonster(int iIndex);

	int GetDistance(int iIndex);

private:

	int SetPosMonster(LPGameObject &lpObj, int nMapNumber, int nBeginX, int nBeginY, int nEndX, int nEndY, int nDir);
	int GetBoxPosition(int mapnumber, int ax, int ay, int aw, int ah, short &mx, short &my);
	
	std::map<int, boost::shared_ptr<EVENT_MONSTER_POSITION>> m_mapEventSpotData;
	std::map<int, boost::shared_ptr<EVENT_MONSTER_DATA>> m_mapMonsterData;
};

extern CEventMonSetBase gEventMSetBase;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

