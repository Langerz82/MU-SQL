////////////////////////////////////////////////////////////////////////////////
// AntiSpeed.cpp
#include "AntiSpeed.h"
#include "ObjUseSkill.h"
#include "GameProtocol.h"
#include "GOFunctions.h"


CAttackMelee::CAttackMelee(CGameObject &Obj, CGameObject &TargetObj)
{
	m_Obj = &Obj;
	m_TargetObj = &TargetObj;
}

void CAttackMelee::Process()
{
	gObjAttack(*m_Obj, *m_TargetObj, NULL, 0, 0, 0, 0, 0, 0);
}

CAttackMagic::CAttackMagic( CGameObject &Obj,  BYTE* pmsg, int len)
{
	m_Obj = &Obj;
	m_Msg = new BYTE[len];
	std::memcpy(m_Msg, pmsg, len);
}

CAttackMagic::~CAttackMagic()
{
	delete [] m_Msg;
};

void CAttackMagic::Process()
{
	GSProtocol.CGMagicAttack(this->m_Msg, m_Obj);
	//gObjUseSkill.UseSkill(m_Obj->m_Index, m_TargetObj.m_Index, m_Magic);
}

CAttackRange::CAttackRange( CGameObject &Obj, BYTE* pmsg, int len, int type )
{
	m_Obj = &Obj;
	m_Msg = new BYTE[len];
	std::memcpy(m_Msg, pmsg, len);
	m_Type = type;
}

CAttackRange::~CAttackRange()
{
	delete [] m_Msg;
}

void CAttackRange::Process()
{
	if(m_Type == 0)
		GSProtocol.CGBeattackRecv(m_Msg, m_Obj, FALSE);
	else
		GSProtocol.CGDurationMagicRecv(m_Msg, m_Obj);
}


CAttackQueue::CAttackQueue(CGameObject* lpObj)
{
	//InitializeCriticalSection(&this->m_CritQueue);
	this->m_Obj = lpObj;
}

void CAttackQueue::Push( BYTE* msg, int len, int type )
{
	try
	{
		m_Queue.push(new CAttackMsg(this->Obj, msg, len, type));
	}

	catch ( char * szMsg )
	{
		sLog->outError(szMsg);
	}
}

void CAttackQueue::ProcessQueue()
{
	//CGameObject m_Obj = getGameObject(this->Obj.m_Index);

	//EnterCriticalSection(&this->m_CritQueue);
	int TickCount = GetTickCount();
	float dt = (float)(TickCount - m_Obj->m_LastAttackTime);

	float HitPerSec;
	// By hand
	m_HandDelay = (0.0142333198777464 * m_Obj->m_AttackSpeed) + (2.03207312150395);
	// By sword
	m_SwordDelay = (0.0109365655658977 * m_Obj->m_AttackSpeed) + (1.0853155620929);
	// Magic
	m_MagicDelay = (0.0142333198777464 * m_Obj->m_AttackSpeed) + (2.03207312150395);

	CheckSize();
	if(m_Obj->Class == CLASS_WIZARD)	// DW
		HitPerSec = m_MagicDelay;
	else{
		if(m_Obj->pntInventory[0]->IsItem() || m_Obj->pntInventory[1]->IsItem())
			HitPerSec = m_SwordDelay;
		else
			HitPerSec = m_HandDelay;
	}

	float AttackDelay = 1000.0f/HitPerSec;
	if(dt > AttackDelay)
	{
		m_LastFrameAttack = false;

		int count = (int)(dt/(1000.0f/AttackDelay));
		//CheckSize();
		for(int i = 0; i < count; i++)
		{
			if(m_Queue.size())
			{
				
				CAttackMsg* pAtt = m_Queue.front();
				m_Queue.pop();

				if(pAtt)
				{
					m_LastFrameAttack = true;
					m_Obj->m_LastAttackTime = TickCount;
					pAtt->Process();
					delete pAtt; // nie wiem juz co robic ^. Kurde to jest przypadek tak specyficzny. Przecie¿ pop() idzie niemal w tym samym monencie co front(). Musia³oby w tym samym momencie w obu w¹tkach wywo³aæ front().... Sytuacja 1 na milion, nie wiem czemu tak sie dzieje...
				}
				else{
					throw std::exception("[ANTI_HACK] Attack queue: pAtt == 0");
				}
				
			}
			else{
				break;
			}
		}
	}
	else if(!m_LastFrameAttack)
	{
		if(m_Queue.size())
		{

			CAttackMsg* pAtt = m_Queue.front();
			m_Queue.pop();

			if(pAtt)
			{
				m_LastFrameAttack = true;
				m_Obj->m_LastAttackTime = TickCount;
				pAtt->Process();
				delete pAtt;
			}
			else{
				throw std::exception("[ANTI_HACK] Attack queue: pAtt == 0");
			}

		}
	}
	//LeaveCriticalSection(&this->m_CritQueue);
}

bool CAttackQueue::ThreadActive = true;
VOID CAttackQueue::AttackQueueProc(std::vector<CGameObject*> gObj)
{
	while(ThreadActive)
	{
		//for(int i = OBJ_STARTUSERINDEX; i < OBJMAX; i++)
		for each(CGameObject* Obj in gObj)
		{
			if(Obj->m_PlayerData->m_AttackQueue == NULL)
			{
				continue;
			}

			if(Obj->m_PlayerData->ConnectUser->Connected >= PLAYER_CONNECTED)
			{
				if(Obj->m_PlayerData->ConnectUser->Connected >= PLAYER_PLAYING)
				{
					Obj->m_PlayerData->m_AttackQueue->ProcessQueue();
				}
				else{
					Obj->m_PlayerData->m_AttackQueue->Clear();
				}
			}
		}
		Sleep(10);
	}
}

CAttackQueue::~CAttackQueue()
{
	//EnterCriticalSection(&this->m_CritQueue);
	int cnt = m_Queue.size();
	for(int i = 0; i < cnt; i++)
	{
		CAttackMsg* pAtt = m_Queue.front();
		m_Queue.pop();

		if(pAtt)
		{
			delete pAtt;
		}
	}
	//LeaveCriticalSection(&this->m_CritQueue);
	//DeleteCriticalSection(&this->m_CritQueue);
}

void CAttackQueue::CheckSize()
{
	int count = m_Queue.size();
	if(count > ATTACK_TOLERANCE)
	{
		int toDelete = count-ATTACK_TOLERANCE;
		for(int i = 0; i < toDelete; i++)
		{
			CAttackMsg* pAtt = m_Queue.front();
			m_Queue.pop();

			if(pAtt)
			{
				delete pAtt;
			}
		}
	}
}

void CAttackQueue::Clear()
{
	//EnterCriticalSection(&this->m_CritQueue);
	int cnt = m_Queue.size();
	for(int i = 0; i < cnt; i++)
	{
		CAttackMsg* pAtt = m_Queue.front();
		m_Queue.pop();

		if(pAtt)
		{
			delete pAtt;
		}
	}
	//LeaveCriticalSection(&this->m_CritQueue);
}

CAttackMsg::CAttackMsg(CGameObject &Obj, BYTE* pmsg, int len, int type )
{
	if(pmsg == NULL)
	{
		throw "Message is empty!";
	}

	if(len < 4)
	{
		throw "Too short len!";
	}

	m_Msg = new BYTE[len];
	m_Len = len;
	std::memcpy(m_Msg, pmsg, len);
	m_Type = type;
}

CAttackMsg::~CAttackMsg()
{
	delete [] m_Msg;
}

void CAttackMsg::Process()
{
	switch(m_Type)
	{
	case ATTACK_MELEE:
		GSProtocol.CGAttack((PMSG_ATTACK*)m_Msg, m_Obj);
		break;
	case ATTACK_MAGIC:
		GSProtocol.CGMagicAttack(m_Msg, m_Obj);
		break;
	case ATTACK_RANGE_OLD:
		GSProtocol.CGBeattackRecv(m_Msg, m_Obj, FALSE);
		break;
	case ATTACK_RANGE_NEW:
		GSProtocol.CGDurationMagicRecv(m_Msg, m_Obj);
		break;
	case ATTACK_RAGE_FIGHTER:
		//
		break;
	}
}


CAntiSpeed* g_AntiSpeed = new CAntiSpeed;

void CAntiSpeed::ProcessQueue()
{

}

bool CAntiSpeed::GlobalThreadActive = true;

VOID CAntiSpeed::GlobalQueueProc()
{
	while(GlobalThreadActive)
	{
		int iQueueSize = g_AntiSpeed->m_Queue.size();
		if(iQueueSize)
		{
			for(int i = 0; i < iQueueSize; i++)
			{
				CAttackHandler* pAtt = g_AntiSpeed->m_Queue.front();
				g_AntiSpeed->m_Queue.pop();

				if(pAtt)
				{
					if(pAtt->m_pMsg)
					{
					}
				}
			}
		}
		Sleep(10);
	}
}


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

