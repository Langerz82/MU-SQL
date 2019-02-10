// Fprotocol.h: interface for the Fprotocol class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FPROTOCOL_H__A35D7AEF_4EC5_4387_9DAA_293F0080F4E5__INCLUDED_)
#define AFX_FPROTOCOL_H__A35D7AEF_4EC5_4387_9DAA_293F0080F4E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "ProtocolStructs.h"

using namespace std;

struct FHP_FRIENDLIST_REQ;
struct FHP_FRIEND_STATE_C;
struct FHP_FRIEND_ADD_REQ;
struct FHP_WAITFRIEND_ADD_REQ;
struct FHP_FRIEND_ADD_REQ;
struct FHP_FRIEND_MEMO_SEND;
struct FHP_FRIEND_MEMO_LIST_REQ;
struct FHP_FRIEND_MEMO_RECV_REQ;
struct FHP_FRIEND_MEMO_DEL_REQ;
struct FRIEND_MASTER;
struct MEMO_HEADER;
struct MEMO_SEND_HEADER;
struct MEMO_READ;
struct FRIEND_MASTER;

class CFriendSystem
{
public:

	CFriendSystem();
	~CFriendSystem();

	BOOL FriendDBConnect();

	void FriendListRequest(LPGameObject &lpObj, FHP_FRIENDLIST_REQ* lpMsg);
	void FriendStateClientRecv(LPGameObject &lpObj, FHP_FRIEND_STATE_C* lpMsg);
	void FriendAddRequest(LPGameObject &lpObj, FHP_FRIEND_ADD_REQ* lpMsg);
	void WaitFriendAddRequest(LPGameObject &lpObj, FHP_WAITFRIEND_ADD_REQ* lpMsg);
	void FriendDelRequest(LPGameObject &lpObj, FHP_FRIEND_ADD_REQ* lpMsg);

	void FriendMemoSend(LPGameObject &lpObj, FHP_FRIEND_MEMO_SEND* lpMsg);
	void FriendMemoListReq(LPGameObject &lpObj, FHP_FRIEND_MEMO_LIST_REQ *lpMsg);
	void FriendMemoReadReq(LPGameObject &lpObj, FHP_FRIEND_MEMO_RECV_REQ* lpMsg);
	void FriendMemoDelReq(LPGameObject &lpObj, FHP_FRIEND_MEMO_DEL_REQ* lpMsg);

	int ExDBGetIndexByCode(int iServerCode);

	BOOL UserGuidDBCreate(char *szName);
	BOOL GetDBGuidAndMemoTotal(char *szName, int& guid, int& memo_total);
	BOOL GetDBFriendList(char *szMaster);

	BOOL FriendExists(char *szMaster, char *szFriend);
	int GetFriendGuid(char *szMaster);
	BYTE GetFriendState(char *szMaster);
	int GetFriendServer(char *szMaster);
	int GetFriendNumber(char *szMaster);

	void FriendListSend(LPGameObject &lpObj, char *szMaster);

	BOOL GetDBWaitFriend(char *szMaster, OUT char *szWaitFriend);
	BOOL FriendWaitSend(LPGameObject &lpObj, char *szMaster);

	void SendState(LPGameObject &lpObj, char *szMaster, int iNumber, char *szName, BYTE btState);
	void SendStateToAllFriends(LPGameObject &lpObj, char *szMaster);

	BOOL AddFriend(char *szMaster, char *szFriend);
	BOOL DelFriend(char *szMaster, char *szFriend);
	int AddDBWaitFriend(char *szMaster, char *szFriend);
	int DelDBWaitFriend(char *szMaster, char *szFriend);

	int WaitFriendAdd(char *szMaster, char *szFriend);
	BOOL AddDBFriend(char *szMaster, char *szFriend);
	BOOL DelDBFriend(char *szMaster, char *szFriend);

	void FriendClose(LPGameObject &lpObj, LPBYTE lpMsg);

	BOOL CreateFriendMaster(char *szMaster);
	BOOL CreateFriendMaster(char *szMaster, int iNumber, int iServer);
	BOOL DeleteFriendMaster(char *szMaster);

	FRIEND_MASTER* GetFriendMaster(char *szMaster);

	void MemoHeaderSend(LPGameObject &lpObj, WORD wNumber, MEMO_HEADER * lpMemoHead );
	void MemoListSend(LPGameObject &lpObj, WORD wNumber, char *szName);

	int DBWriteMail(MEMO_SEND_HEADER * lpMemoSendHdr, LPBYTE Photo, BYTE btPhotoSize, char *sMemo, int memo_size);
	int DBReadMail(int iMemoId, int iGuid, MEMO_READ * lpMemoRead, LPDWORD lpdwMemoSize, LPBYTE lpPhoto, LPBYTE lpPhotoSize);
	int DBDelMail(char *szName, int iMemoId);

	CRITICAL_SECTION m_csMapFriendMaster;

private:

	std::map<string, FRIEND_MASTER, strCmp> m_MapFriendManager;
	int m_iMaxFriends;

};

#endif // !defined(AFX_FPROTOCOL_H__A35D7AEF_4EC5_4387_9DAA_293F0080F4E5__INCLUDED_)


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

