#pragma once

/* todo:
 * db
 */

// import
#include "prodef.h"
#include "TLog.h"
#include "user.h"

// defines
#define USERCHATBLOCK_LIST_LIMIT	50
#define USERCHATBLOCK_DB_CONNECTED	FALSE

enum UserChatBlockActionType {
	chatblockActionInit = 1,
	chatblockActionAdd = 2,
	chatblockActionDelete = 3,
};

// data
struct dataUserChatBlockListSet {
	dataUserChatBlockListSet() {}
	~dataUserChatBlockListSet() {}
	void initList() {
		if (m_List.capacity() > 0) {
			m_List.clear();
		}
	}
	bool addName(const char* Name) {
		if (m_List.size() >= USERCHATBLOCK_LIST_LIMIT) {
			return false;
		}
		if (Name == NULL || !strcmp("", Name)) {
			return false;
		}
		if (!isFreeName(Name)) {
			return false;
		}
		std::string tmpName(Name);
		m_List.push_back(tmpName);
		return true;
	}
	bool deleteName(const char* Name) {
		if (isFreeName(Name)) {
			return false;
		}
		std::vector<std::string>::iterator It;
		for (It = m_List.begin(); It != m_List.end(); It++) {
			if (It->compare(Name) == 0) {
				m_List.erase(It);
				return true;
			}
		}
		return false;
	}
	bool deleteName(int Index) {
		if (Index < 0 || Index > m_List.size()) {
			return false;
		}
		m_List.erase(m_List.begin() + Index);
		return true;
	}
	bool isFreeName(const char* Name) {
		std::vector<std::string>::iterator It;
		for (It = m_List.begin(); It != m_List.end(); It++) {
			if (It->compare(Name) == 0) {
				return false;
			}
		}
		return true;
	}
	std::vector<std::string> m_List;
};

// network data
struct GCAnswerUserChatBlockListHead { // C2 -> 6E -> 0
	PWMSG_HEAD2 h;
	BYTE ActionType; // 1: login, 2: add, 3: delete
	BYTE Result; // 0: for login action, 0 / 1: for other action
	BYTE Count;
};

struct GCAnswerUserChatBlockList {
	bool inUse; // 0: slot is free, 1: slot in use
	BYTE Index; // -1: index if not used
	char Name[11];
};

struct CGRequestUserChatBlockAdd { // C1 -> 6E -> 1
	PBMSG_HEAD2 h;
	char Name[11];
};

struct CGRequestUserChatBlockDelete { // C1 -> 6E -> 2
	PBMSG_HEAD2 h;
	BYTE Index;
	char Name[11];
};

// user chat black list feature emulation
class UserChatBlockMng {
public:
	/* constr / destr */
	UserChatBlockMng() {}
	~UserChatBlockMng() {}

	/* pool managment */
	void addSlot(short UserIndex) 
	{
		if (!isFreeSlot(UserIndex)) 
		{
			return;
		}
		dataUserChatBlockListSet newSlot;
		m_Pool.insert(std::pair<short, dataUserChatBlockListSet>(UserIndex, newSlot));
		GDRequestList(UserIndex);
	}

	void deleteSlot(short UserIndex) 
	{
		if (isFreeSlot(UserIndex)) 
		{
			return;
		}
		std::map<short, dataUserChatBlockListSet>::iterator It = m_Pool.find(UserIndex);
		if (It != m_Pool.end()) 
		{
			GDSaveList(UserIndex);
			m_Pool.erase(It);
		}
	}

	dataUserChatBlockListSet* getSlot(short UserIndex) 
	{
		if (isFreeSlot(UserIndex)) 
		{
			return NULL;
		}
		std::map<short, dataUserChatBlockListSet>::iterator It = m_Pool.find(UserIndex);
		if (It != m_Pool.end()) 
		{
			return &It->second;
		}
		return NULL;
	}

	bool isFreeSlot(short UserIndex) 
	{
		std::map<short, dataUserChatBlockListSet>::iterator It = m_Pool.find(UserIndex);
		if (It != m_Pool.end()) 
		{
			return false;
		}
		return true;
	}

	bool isCanReceiveChat(short SenderIndex, short TargetIndex) 
	{
		LPOBJ tmpSender = &gObj[SenderIndex];
		dataUserChatBlockListSet* targetList = getSlot(TargetIndex);
		if (targetList == NULL) 
		{
			return true;
		}
		if (!targetList->isFreeName(tmpSender->Name)) 
		{
			MsgOutput(SenderIndex, "Failure, %s add you to black list", gObj[TargetIndex].Name);
			return false;
		}
		return true;
	}

	/* network handlers for client */
	// request for add name in list
	static void CGRequestAddName(short UserIndex, CGRequestUserChatBlockAdd* inPacket) 
	{
		if (UserChatBlockMng::getInstance()->isFreeSlot(UserIndex)) 
		{
			return;
		}
		
		dataUserChatBlockListSet* tmpList = UserChatBlockMng::getInstance()->getSlot(UserIndex);
		
		if (tmpList == NULL) 
		{
			return;
		}
		
		if (!tmpList->isFreeName(inPacket->Name)) 
		{
			return;
		}

		if (tmpList->addName(inPacket->Name)) 
		{
			GCSendList(UserIndex, chatblockActionAdd, 1);
		} 
		else 
		{
			GCSendList(UserIndex, chatblockActionAdd, 0);
		}
	}
	
	// request for delete name from list
	static void CGRequestDeleteName(short UserIndex, CGRequestUserChatBlockDelete* inPacket) 
	{
		if (UserChatBlockMng::getInstance()->isFreeSlot(UserIndex)) 
		{
			return;
		}
		dataUserChatBlockListSet* tmpList = UserChatBlockMng::getInstance()->getSlot(UserIndex);
		
		if (tmpList == NULL) 
		{
			return;
		}
		
		if (tmpList->isFreeName(inPacket->Name)) 
		{
			return;
		}

		if (tmpList->deleteName(inPacket->Index)) 
		{
			GCSendList(UserIndex, chatblockActionDelete, 1);
		}
		else
		{
			GCSendList(UserIndex, chatblockActionDelete, 0);
		}
	}

	// sending of list (call event: enter on map, any list action), better use how result in dataserver protocol
	static void GCSendList(short UserIndex, UserChatBlockActionType Action, BYTE Result) 
	{
		if (UserChatBlockMng::getInstance()->isFreeSlot(UserIndex)) 
		{
			return;
		}

		dataUserChatBlockListSet* tmpList = UserChatBlockMng::getInstance()->getSlot(UserIndex);
		
		if (tmpList == NULL) 
		{
			return;
		}

		BYTE tmpBuffer[sizeof(GCAnswerUserChatBlockListHead) + sizeof(GCAnswerUserChatBlockList) * USERCHATBLOCK_LIST_LIMIT] = { 0 }; // 255 - max value of BYTE
		int tmpPosition = sizeof(GCAnswerUserChatBlockListHead);
		int tmpCount = 0;
		for (int i = 0; i < tmpList->m_List.size(); i++) {
			GCAnswerUserChatBlockList tmpMember = { 0 };
			tmpMember.Index = i;
			tmpMember.inUse = true;
			strcpy_s(tmpMember.Name, sizeof(tmpMember.Name), tmpList->m_List[i].c_str());
			memcpy(&tmpBuffer[tmpPosition], &tmpMember, sizeof(GCAnswerUserChatBlockList));
			tmpPosition += sizeof(GCAnswerUserChatBlockList);
			tmpCount++;
		}
		GCAnswerUserChatBlockListHead tmpAnswer = { 0 };
		tmpAnswer.h.set((LPBYTE)&tmpAnswer, 0x6E, 0, tmpPosition);
		tmpAnswer.Count = tmpCount;
		tmpAnswer.ActionType = (BYTE)Action;
		tmpAnswer.Result = Result;
		memcpy(&tmpBuffer[0], &tmpAnswer, sizeof(GCAnswerUserChatBlockListHead));
		IOCP.DataSend(UserIndex, tmpBuffer, tmpPosition);
	}

	/* network handlers for dataserver */
	// request db list
	static void GDRequestList(int UserIndex) {
#if (USERCHATBLOCK_DB_CONNECTED == 0)
		std::string tmpName("test");
		getInstance()->getSlot(UserIndex)->m_List.push_back(tmpName);
		GCSendList(UserIndex, chatblockActionInit, 1);
#endif
		// fill me
	}
	
	// receive db list
	static void GDReceiveList(/*packet*/) 
	{
		short UserIndex;

		if (UserChatBlockMng::getInstance()->isFreeSlot(UserIndex)) 
		{
			return;
		}
		// fill me
	}
	
	// save list to db
	static void GDSaveList(short UserIndex) 
	{
		if (UserChatBlockMng::getInstance()->isFreeSlot(UserIndex)) 
		{
			return;
		}
		// fill me
	}
	
	// singleton
	static UserChatBlockMng* getInstance() 
	{
		if (m_Instance == NULL) 
		{
			m_Instance = new UserChatBlockMng();
		}
		return m_Instance;
	}

private:
	std::map<short, dataUserChatBlockListSet> m_Pool;
	static UserChatBlockMng* m_Instance;
};