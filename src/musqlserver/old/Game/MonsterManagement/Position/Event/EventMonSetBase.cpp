////////////////////////////////////////////////////////////////////////////////
// EventMonSetBase.cpp
#include "StdAfx.h"
#include "EventMonSetBase.h"
#include "User/CUserData.h"
#include "MapClass.h"
#include "GameMain.h"
#include "MapServerManager.h"

CEventMonSetBase gEventMSetBase;

CEventMonSetBase::CEventMonSetBase()
{
}

CEventMonSetBase::~CEventMonSetBase()
{
}

void CEventMonSetBase::LoadScript(char *szFile)
{
	this->DeleteAllMonsters();

	this->m_mapMonsterData.clear();
	this->m_mapEventSpotData.clear();

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(szFile);

	if (res.status != pugi::status_ok)
	{
		sLog->outError("%s file load fail (%s)", szFile, res.description());
		return;
	}

	pugi::xml_node mainXML = file.child("MonsterSpawnEvent");
	int iTotalCount = 0;

	for (pugi::xml_node spot = mainXML.child("Spot"); spot; spot = spot.next_sibling())
	{
		bool bEnable = spot.attribute("Enable").as_bool();

		if (bEnable == false)
		{
			continue;
		}

		int iStartDay = spot.attribute("StartDay").as_int();
		int iEndDay = spot.attribute("EndDay").as_int();
		int iStartHour = spot.attribute("StartHour").as_int();
		int iEndHour = spot.attribute("EndHour").as_int();

		for (pugi::xml_node spawn = spot.child("Spawn"); spawn; spawn = spawn.next_sibling())
		{
			WORD wType = spawn.attribute("Index").as_int();
			BYTE btMapNumber = spawn.attribute("MapNumber").as_int();
			BYTE btDis = spawn.attribute("Distance").as_int();
			BYTE btX = spawn.attribute("StartX").as_int();
			BYTE btY = spawn.attribute("StartY").as_int();
			BYTE btW = spawn.attribute("EndX").as_int();
			BYTE btH = spawn.attribute("EndY").as_int();
			BYTE btDir = spawn.attribute("Dir").as_int();
			BYTE btDoSpawn = FALSE;

			int iSpawnCount = spawn.attribute("Count").as_int();

			for (int i = 0; i < iSpawnCount; i++)
			{
				boost::shared_ptr<EVENT_MONSTER_POSITION> lpEventPos(new EVENT_MONSTER_POSITION);

				lpEventPos->m_StartDay = iStartDay;
				lpEventPos->m_EndDay = iEndDay;
				lpEventPos->m_StartHour = iStartHour;
				lpEventPos->m_EndHour = iEndHour;
				lpEventPos->m_Type = wType;
				lpEventPos->m_MapNumber = btMapNumber;
				lpEventPos->m_Dis = btMapNumber;
				lpEventPos->m_X = btX;
				lpEventPos->m_Y = btY;
				lpEventPos->m_W = btW;
				lpEventPos->m_H = btH;
				lpEventPos->m_Dir = btDir;
				lpEventPos->m_DoSpawn = btDoSpawn;

				this->m_mapEventSpotData.insert(std::pair<int, boost::shared_ptr<EVENT_MONSTER_POSITION>>(iTotalCount, lpEventPos));
				iTotalCount++;
			}
		}
	}

	//sLog->outBasic("[Event Spawn] Load %d spots", this->m_mapEventSpotData.size());
}

void CEventMonSetBase::Run()
{
	SYSTEMTIME time;
	GetLocalTime(&time);

	for (std::map<int, boost::shared_ptr<EVENT_MONSTER_POSITION>>::iterator It = this->m_mapEventSpotData.begin(); It != this->m_mapEventSpotData.end(); It++)
	{
		if (time.wDay >= It->second->m_StartDay && time.wDay <= It->second->m_EndDay && time.wHour >= It->second->m_StartHour && time.wHour < It->second->m_EndHour)
		{
			if (It->second->m_DoSpawn == FALSE)
			{
				It->second->m_DoSpawn = TRUE;
				this->SpawnMonster(It->second);
			}
		}

		else
		{
			if (It->second->m_DoSpawn == TRUE)
			{
				It->second->m_DoSpawn = FALSE;
			}
		}
	}
}

bool CEventMonSetBase::IsEventMonster(CGameObject &Obj)
{
	std::map<int, boost::shared_ptr<EVENT_MONSTER_DATA>>::iterator Iter = this->m_mapMonsterData.find(Obj.m_Index);

	if (Iter != this->m_mapMonsterData.end())
	{
		return true;
	}

	return false;
}

int CEventMonSetBase::GetDistance(CGameObject &Obj)
{
	std::map<int, boost::shared_ptr<EVENT_MONSTER_DATA>>::iterator Iter = this->m_mapMonsterData.find(Obj.m_Index);

	if (Iter == this->m_mapMonsterData.end())
	{
		return 0;
	}

	return Iter->second->lpSpotData->m_Dis;
}

void CEventMonSetBase::SpawnMonster(boost::shared_ptr<EVENT_MONSTER_POSITION> lpSpotData)
{
	if (lpSpotData == nullptr)
	{
		return;
	}

	if (g_MapServerManager.CheckMapCanMove(lpSpotData->m_MapNumber) == FALSE)
	{
		return;
	}

	int result = gObjAddMonster(lpSpotData->m_MapNumber);

	if (result == -1)
	{
		sLog->outBasic("error : %s %d", __FILE__, __LINE__);
		return;
	}

	boost::shared_ptr<EVENT_MONSTER_DATA> lpMonsterData(new EVENT_MONSTER_DATA);

	lpMonsterData->iMonsterIndex = result;
	lpMonsterData->lpSpotData = lpSpotData;

	if (this->SetPosMonster(result, lpSpotData->m_MapNumber, lpSpotData->m_X, lpSpotData->m_Y, lpSpotData->m_W, lpSpotData->m_H, lpSpotData->m_Dir) == FALSE)
	{
		gObjDel(result);
		sLog->outBasic("error : %s %d", __FILE__, __LINE__);
		return;
	}

	gObjSetMonster(result, lpSpotData->m_Type);
	this->m_mapMonsterData.insert(std::pair<int, boost::shared_ptr<EVENT_MONSTER_DATA>>(result, lpMonsterData));

	//sLog->outBasic("[Event Spawn] Set Monster (%d) (%d) (%d)(%d)(%d)", result, lpSpotData->m_Type, gGameObjects[result]->MapNumber, gGameObjects[result]->X, gGameObjects[result]->Y);
}

void CEventMonSetBase::RegenMonster(CGameObject &Obj)
{
	if (this->IsEventMonster(Obj.m_Index) == false)
	{
		return;
	}

	std::map<int, boost::shared_ptr<EVENT_MONSTER_DATA>>::iterator Iter = this->m_mapMonsterData.find(Obj.m_Index);
	CGameObject lpObj = Obj;

	if (Iter->second->lpSpotData->m_DoSpawn == FALSE)
	{
		sLog->outBasic("[Event Spawn] End of Regen Time -> Delete Monster (%d)(%d) (%d)(%d)(%d)",
			iIndex, lpObj.Class, lpObj.MapNumber, lpObj.X, lpObj.Y);

		gObjDel(Obj.m_Index);
		this->m_mapMonsterData.erase(Iter);
		return;
	}

	if (this->GetBoxPosition(Iter->second->lpSpotData->m_MapNumber, Iter->second->lpSpotData->m_X,
		Iter->second->lpSpotData->m_Y, Iter->second->lpSpotData->m_W, Iter->second->lpSpotData->m_H,
		lpObj.X, lpObj.Y) == FALSE)
	{
		sLog->outBasic("[Event Spawn] Failed to find empty space on map -> Delete Monster (%d)(%d) (%d)(%d)(%d)",
			iIndex, lpObj.Class, lpObj.MapNumber, lpObj.X, lpObj.Y);

		gObjDel(Obj.m_Index);
		this->m_mapMonsterData.erase(Iter);
		return;
	}

	lpObj.MTX = lpObj.X;
	lpObj.MTY = lpObj.Y;
	lpObj.TX = lpObj.X;
	lpObj.TY = lpObj.Y;
	lpObj.StartX = lpObj.X;
	lpObj.StartY = lpObj.Y;

	//sLog->outBasic("[Event Spawn] Regen Monster (%d)(%d) (%d)(%d)(%d)",iIndex, lpObj.Class, lpObj.MapNumber, lpObj.X, lpObj.Y);
}

void CEventMonSetBase::DeleteAllMonsters()
{
	for (std::map<int, boost::shared_ptr<EVENT_MONSTER_DATA>>::iterator It = this->m_mapMonsterData.begin(); It != this->m_mapMonsterData.end(); It++)
	{
		//sLog->outBasic("[Event Spawn] Delete Monster (%d)(%d) (%d)(%d)(%d)", It->first,	gGameObjects[It->first]->Class, gGameObjects[It->first]->MapNumber, gGameObjects[It->first]->X, gGameObjects[It->first]->Y);

		gObjDel(It->first);
		this->m_mapMonsterData.erase(It);
	}

}

int CEventMonSetBase::SetPosMonster(CGameObject &lpObj, int nMapNumber, int nBeginX, int nBeginY, int nEndX, int nEndY, int nDir)
{
	if (!ObjectMaxRange(aIndex))
	{
		sLog->outBasic("error : %s %d", __FILE__, __LINE__);
		return false;
	}

	

	lpObj.m_PosNum = -1;
	lpObj.MapNumber = nMapNumber;
	this->GetBoxPosition(nMapNumber, nBeginX, nBeginY, nEndX, nEndY, lpObj.X, lpObj.Y);
	lpObj.TX = lpObj.X;
	lpObj.TY = lpObj.Y;
	lpObj.m_OldX = lpObj.X;
	lpObj.m_OldY = lpObj.Y;

	if (nDir == -1)
		lpObj.Dir = rand() % 8;
	else
		lpObj.Dir = nDir;

	lpObj.StartX = lpObj.X;
	lpObj.StartY = lpObj.Y;
	lpObj.DieRegen = 0;
	lpObj.MaxRegenTime = 0;

	return true;
}

BOOL CEventMonSetBase::GetBoxPosition(int mapnumber, int ax, int ay, int aw, int ah, short & mx, short & my)
{
	int count = 100;
	int w;
	int h;
	int tx;
	int ty;
	BYTE attr;

	while (count-- != 0)
	{
		w = aw - ax;
		h = ah - ay;

		__try
		{
			tx = ax + (rand() % w);
			ty = ay + (rand() % h);
		}
		__except (w = 1, h = 1, 1)
		{

		}

		attr = MapC[mapnumber].GetAttr(tx, ty);

		if (((attr & 1) != 1) && ((attr & 4) != 4) && ((attr & 8) != 8))
		{
			mx = tx;
			my = ty;
			return TRUE;
		}
	}

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

