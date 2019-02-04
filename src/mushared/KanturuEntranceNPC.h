// KanturuEntranceNPC.h: interface for the CKanturuEntranceNPC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KANTURUENTRANCENPC_H__A062469A_13C1_449C_A227_A97ECE1B50C2__INCLUDED_)
#define AFX_KANTURUENTRANCENPC_H__A062469A_13C1_449C_A227_A97ECE1B50C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CKanturuEntranceNPC
{

public:

  CKanturuEntranceNPC();
  virtual ~CKanturuEntranceNPC();

  void NotifyEntranceInfo(int iIndex);
  void NotifyResultEnterKanturuBossMap(int iIndex);

private:

};

extern CKanturuEntranceNPC g_KanturuEntranceNPC;

#endif // !defined(AFX_KANTURUENTRANCENPC_H__A062469A_13C1_449C_A227_A97ECE1B50C2__INCLUDED_)


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

