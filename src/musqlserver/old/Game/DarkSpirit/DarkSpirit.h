////////////////////////////////////////////////////////////////////////////////
// DarkSpirit.h
#ifndef DARKSPIRIT_H
#define DARKSPIRIT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "User/CUserData.h"
#include "MagicInf.h"
#include "ItemObject.h"
#include "ObjBaseAttack.h"


#define RAVEN_ATTACK_DISTANCE		7


struct PMSG_SEND_PET_ITEMINFO
{
	PBMSG_HEAD2 h;	// C1:[A9:AC]
	BYTE PetType;	// 3
	BYTE InvenType;	// 4
	BYTE nPos;	// 5
	BYTE Level;	// 6
	int Exp;	// 8
	BYTE Dur;
};

// updated in 1.01.04
class CDarkSpirit : public CObjBaseAttack
{

public:

	int m_AttackDamageMin;	// 4
	int m_AttackDamageMax;	// 8
	int m_AttackSpeed;	// C
	int m_SuccessAttackRate;	// 10
	double m_CriticalDamage;
	double m_ExcellentDamage;
	int	m_DoubleDamage;
	int	m_IgnoreEnemy;
	int m_iMasterIndex;	// 14
	int m_iTargetIndex;	// 18
	DWORD m_dwLastAttackTime;	// 1C
	enum ePetItem_Mode
	{
		PetItem_Mode_Normal = 0x0,
		PetItem_Mode_Attack_Random = 0x1,
		PetItem_Mode_Attack_WithMaster = 0x2,
		PetItem_Mode_Attack_Target = 0x3,

	} m_ActionMode;	// 20
	CItemObject * m_pPetItem;	// 24
	CRITICAL_SECTION m_SpiritCriti;

public:

  CDarkSpirit();
  virtual ~CDarkSpirit();

  void Init();
  void Run();
  void ModeNormal();
  void ModeAttackRandom();
  void ModeAttackWithMaster();
  void ModeAttakTarget();
  void SetTarget(int aTargetIndex);
  void ReSetTarget(int aTargetIndex);
  void Set(CGameObject &lpObj, CItemObject* pPetItem);
  void SetMode(ePetItem_Mode mode, int iTargetindex);
  int Attack(CGameObject &lpObj, CGameObject lpTargetObj, CMagicInf* lpMagic, int criticaldamage, int iActionType);
  int GetAttackDamage(CGameObject &lpObj, int targetDefense, int criticaldamage);
  int MissCheck(CGameObject &lpObj, CGameObject lpTargetObj, int skill,  int skillSuccess, int& bAllMiss);
  int MissCheckPvP(CGameObject &lpObj, CGameObject lpTargetObj, int skill,  int skillSuccess, int& bAllMiss);
  void ChangeCommand(int command, int targetindex);
  void RangeAttack(CGameObject &lpObj, int aTargetIndex);
  void SendAttackMsg(CGameObject &lpObj, int aTargetIndex, int criticaldamage, int iActionType);
  int GetShieldDamage(CGameObject &lpObj, CGameObject lpTargetObj, int iAttackDamage);
 
  static void __cdecl CDarkSpirit::SendLevelmsg(CGameObject &lpObj, int nPos, int PetType, int InvenType);


};

extern CDarkSpirit * gDarkSpirit;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

