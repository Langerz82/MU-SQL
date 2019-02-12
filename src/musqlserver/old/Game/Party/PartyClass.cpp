////////////////////////////////////////////////////////////////////////////////
// PartyClass.cpp
#include "StdAfx.h"
#include "PartyClass.h"
#include "Logging/Log.h"
#include "Protocol.h"
#include "util.h"
#include "configread.h"
#include "GameMain.h"
#include "IllusionTempleEvent_Renewal.h"
// GS-N 0.99.60T 0x0048E650
//	GS-N	1.00.18	JPN	0x004A9EB0	-	Completed

PartyClass::PartyClass()
{
	return;
}

PartyClass::~PartyClass()
{
	return;
}

void PartyClass::Init()
{
	this->m_PartyS = new PARTY_STRUCT[g_ConfigRead.server.GetObjectMaxUser()];
}

void PartyClass::Delete()
{
	delete [] this->m_PartyS;
}

BOOL PartyClass::IsParty(int party_number)
{
	if ( party_number < 0 || party_number > g_ConfigRead.server.GetObjectMaxUser()-1 )	// #error ObjectMax-1
	{
		return FALSE;
	}

	if ( this->m_PartyS[party_number].Count == 0 )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL PartyClass::Isleader(int party_number, int usernumber, int dbnumber)
{
	if ( this->IsParty(party_number) == FALSE )
	{
		return FALSE;
	}

	if ( this->m_PartyS[party_number].Number[0] == usernumber && this->m_PartyS[party_number].DbNumber[0] == dbnumber )
	{
		return TRUE;
	}

	return FALSE;
}

int PartyClass::GetCount(int party_number)
{
	if ( this->IsParty(party_number) == FALSE )
	{
		return 0;
	}

	return this->m_PartyS[party_number].Count;
}

int PartyClass::GetMemberIndex(int index, int number) // OK
{
	if (this->IsParty(index) == 0)
	{
		return -1;
	}

	if (number < 0 || number >= MAX_USER_IN_PARTY)
	{
		return -1;
	}

	return this->m_PartyS[index].Number[number];
}

BOOL PartyClass::RevisionIndexUser(int party_number, BYTE *index, char *szName)
{
	if (this->IsParty(party_number) == FALSE)
	{
		return FALSE;
	}

	char szMemberName[MAX_ACCOUNT_LEN + 1];
	memcpy(szMemberName, szName, MAX_ACCOUNT_LEN + 1);

	for (int i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		int nNumber = this->m_PartyS[party_number].Number[i];

		if (ObjectMaxRange(nNumber) == false)
		{
			continue;
		}

		if (!strcmp(szMemberName, gGameObjects[nNumber]->Name))
		{
			*index = i;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL PartyClass::GetIndexUser(int party_number, int index, int& usernumber,  int& dbnumber)
{
	if ( this->IsParty(party_number) == FALSE )
	{
		return FALSE;
	}

	if ( index <0 || index > MAX_USER_IN_PARTY-1 )
	{
		return FALSE;
	}

	usernumber = this->m_PartyS[party_number].Number[index];
	dbnumber = this->m_PartyS[party_number].DbNumber[index];

	return TRUE;
}


int PartyClass::GetIndex(int party_number, int usernumber, int dbnumber)
{
	if ( this->IsParty(party_number) == FALSE )
	{
		return 0;
	}

	for ( int n=0; n<MAX_USER_IN_PARTY;n++)
	{
		if ( this->m_PartyS[party_number].Number[n] == usernumber || this->m_PartyS[party_number].DbNumber[n] == dbnumber )
		{
			return n;
		}
	}

	return -1;
}

int PartyClass::Create(int usernumber, int dbnumber, int level)
{
	int n = this->m_PartyCount;
	int loopcount = 0;

	while ( true )
	{
		if ( this->m_PartyS[n].Count == 0 )
		{
			this->m_PartyS[n].Count = 1;
			this->m_PartyS[n].Number[0] = usernumber;
			this->m_PartyS[n].DbNumber[0] = dbnumber;

			if (gGameObjects[usernumber]->m_PlayerData->m_bUsePartyMatching && gGameObjects[usernumber]->m_PlayerData->m_bPartyMatchingLeader)
			{
				this->m_PartyS[n].bUsePartyMatching = true;
				this->m_PartyS[n].nMatchingPartyCount = gGameObjects[usernumber]->m_PlayerData->m_nPartyMatchingMemberCount;
			}

			else
			{
				this->m_PartyS[n].bUsePartyMatching = false;
				this->m_PartyS[n].nMatchingPartyCount = 0;
			}

			for ( int i=1;i<MAX_USER_IN_PARTY;i++)
			{
				this->m_PartyS[n].DbNumber[i] = -1;
				this->m_PartyS[n].Number[i] = -1;
				this->m_PartyS[n].bITLEnterOK[i] = 0;
				this->m_PartyS[n].bITRWithYouOk[i] = 0;
				this->m_PartyS[n].bDSFEnterOK[i] = 0;
			}

			this->m_PartyCount++;
			this->m_PartyS[n].m_MaxLevel = 0;
			this->m_PartyS[n].m_MinLevel = 0;
			this->SetLevel(n, level);

			if ( this->m_PartyCount > g_ConfigRead.server.GetObjectMaxUser()-1 )
			{
				this->m_PartyCount=0;
			}

			this->m_PartyS[n].m_UserPKLevel[0] = gGameObjects[usernumber]->m_PK_Level;

			if (this->m_PartyS[n].m_UserPKLevel[0] < 5)
			{
				this->m_PartyS[n].m_PKPartyLevel = 3;
				this->m_PartyS[n].m_nPKUserCount = 0;
			}

			else
			{
				this->m_PartyS[n].m_PKPartyLevel = this->m_PartyS[n].m_UserPKLevel[0];
				this->m_PartyS[n].m_nPKUserCount = 1;
			}

			return n;
		}

		n++;

		if ( n> g_ConfigRead.server.GetObjectMaxUser()-1 )
		{
			n=0;
		}

		loopcount++;

		if ( loopcount > g_ConfigRead.server.GetObjectMaxUser()-1 )
		{
			break;
		}
	}

	return -1;
}

BOOL PartyClass::SetLevel(int party_number, int level)
{
	if ( this->m_PartyS[party_number].m_MaxLevel  < level )
	{
		this->m_PartyS[party_number].m_MaxLevel  = level;
	}

	if ( this->m_PartyS[party_number].m_MinLevel  == 0 )
	{
		this->m_PartyS[party_number].m_MinLevel = level;
	}

	if ( this->m_PartyS[party_number].m_MinLevel > level )	// In Case of Error
	{
		this->m_PartyS[party_number].m_MinLevel = level;
	}

	return TRUE;
}

BOOL PartyClass::GetLevel(int party_number, int& maxlevel, int& minlevel)
{
	if ( this->IsParty(party_number) == FALSE )
	{
		return FALSE;
	}

	maxlevel = this->m_PartyS[party_number].m_MaxLevel;
	minlevel = this->m_PartyS[party_number].m_MinLevel;

	return TRUE;
}

BOOL PartyClass::Destroy(int party_number)
{
	if ( this->IsParty(party_number) == FALSE )
	{
		return FALSE;
	}

	this->m_PartyS[party_number].Count = 0;
	this->m_PartyS[party_number].Number[0] = -1;
	this->m_PartyS[party_number].DbNumber[0] = -1;
	this->m_PartyS[party_number].bUsePartyMatching = false;
	this->m_PartyS[party_number].nMatchingPartyCount = 0;
	this->m_PartyS[party_number].bITLEnterOK[0] = 0;
	this->m_PartyS[party_number].bITRWithYouOk[0] = 0;
	this->m_PartyS[party_number].bDSFEnterOK[0] = 0;
	this->m_PartyCount--;

	return FALSE;
}

int PartyClass::Add(int party_number, int usernumber, int dbnumber, int level)
{
	if ( this->IsParty(party_number ) == FALSE )
	{
		return -1;
	}

	if ( this->m_PartyS[party_number].Count >= MAX_USER_IN_PARTY)	// Useless
	{
		return -1;
	}

	if (this->m_PartyS[party_number].nMatchingPartyCount >= 5 && this->m_PartyS[party_number].bUsePartyMatching)
	{
		return -1;
	}

	for ( int i = 0; i<MAX_USER_IN_PARTY ; i++)
	{
		if ( (this->m_PartyS[party_number].Number[i] >= 0) && (this->m_PartyS[party_number].Number[i] == usernumber) && (this->m_PartyS[party_number].DbNumber[i] == dbnumber) )
		{
			return -2;
		}
	}

	for (int i = 0; i<MAX_USER_IN_PARTY ; i++ )
	{
		if ( this->m_PartyS[party_number].Number[i] == -1 )
		{
			this->m_PartyS[party_number].Number[i] = usernumber;
			this->m_PartyS[party_number].DbNumber[i] = dbnumber;
			this->m_PartyS[party_number].Count++;
			this->SetLevel(party_number, level);
			this->m_PartyS[party_number].m_UserPKLevel[i] = gGameObjects[usernumber]->m_PK_Level;
			if (this->m_PartyS[party_number].m_UserPKLevel[i] >= 5)
			{
				if (this->m_PartyS[party_number].m_PKPartyLevel < this->m_PartyS[party_number].m_UserPKLevel[i])
				{
					this->m_PartyS[party_number].m_PKPartyLevel = this->m_PartyS[party_number].m_UserPKLevel[i];
				}

				this->m_PartyS[party_number].m_nPKUserCount++;
			}

			return i;
		}
	}

	return -1;
}

int PartyClass::Delete(int party_number, int usernumber, int dbnumber)	// Kick a User from Party
{
	if ( this->IsParty(party_number) == FALSE )
	{
		return -1;
	}

	for ( int i = 0 ; i<MAX_USER_IN_PARTY ; i++ )
	{
		if ( (this->m_PartyS[party_number].Number[i] == usernumber) && (this->m_PartyS[party_number].DbNumber[i] == dbnumber) )
		{
			this->m_PartyS[party_number].Number[i] = -1;
			this->m_PartyS[party_number].DbNumber[i] = -1;
			this->m_PartyS[party_number].Count--;
			this->m_PartyS[party_number].m_UserPKLevel[i] = 3;
			return i;
		}
	}

	return -1;
}

void PartyClass::Delete(int party_number, int index)	// Kick a User Again
{
	if ( this->IsParty(party_number) == FALSE )
	{
		return;
	}

	if ( index < 0 || index > MAX_USER_IN_PARTY -1 )
	{
		return;
	}

	if ( this->m_PartyS[party_number].Number[index] >= 0 )
	{
		this->m_PartyS[party_number].Number[index] = -1;
		this->m_PartyS[party_number].DbNumber[index] = -1;
		this->m_PartyS[party_number].Count--;
		this->m_PartyS[party_number].m_UserPKLevel[index] = 3;
	}
}

int PartyClass::GetPartyCount(int party_number)
{
	if ( this->IsParty( party_number ) == FALSE )
	{
		return -1;
	}

	return this->m_PartyS[party_number].Count;
}

void PartyClass::Paint(int party_number)
{
	if (this->IsParty(party_number) == FALSE )
	{
		return;
	}

	int usern = 0;
	wsprintf( this->m_szTempPaint, "PartyMember(%d) :", this->m_PartyS[party_number].Count);

	for ( int i = 0; i< MAX_USER_IN_PARTY ; i++ )
	{
		if ( (this->m_PartyS[party_number].Number[i] != 0 ) && (this->m_PartyS[party_number].DbNumber[i] != 0 ) )
		{
			usern = this->m_PartyS[party_number].Number[i];

			if ( !gObjIsConnected(usern) )
				continue;

			strcat(this->m_szTempPaint, "[");
			strcat(this->m_szTempPaint, gGameObjects[usern]->AccountID);
			strcat(this->m_szTempPaint, "][");
			strcat(this->m_szTempPaint, gGameObjects[usern]->Name);
			strcat(this->m_szTempPaint, "], ");
		}
	}

	sLog->outBasic(this->m_szTempPaint);
}


struct PMSG_DEFAULT_COUNT
{
	PBMSG_HEAD2 h;
	BYTE Count;	// 3
};

#pragma pack (1)
struct PMSG_PARTYLIFEALL
{
	BYTE Life;
	BYTE Mana;
	char Name[11];
};
#pragma pack ()

void PartyClass::PartyMemberLifeSend(int party_number)
{
	if ( this->IsParty(party_number) == FALSE )
	{
		return;
	}

	PMSG_DEFAULT_COUNT pCount;
	PMSG_PARTYLIFEALL pList;
	int usern;
	char sendbuf[512];
	int lOfs = 0;
	int tlife = 0;

	PHeadSetB((LPBYTE)&pCount, 0x44, sizeof(pCount));
	pCount.Count = this->m_PartyS[party_number].Count;
	lOfs += sizeof(pCount);

	int numbercount = 0;
	
	for ( int i = 0; i<MAX_USER_IN_PARTY ; i++ )
	{
		if ( (this->m_PartyS[party_number].Number[i] >= 0 ) && (this->m_PartyS[party_number].DbNumber[i] >= 0) )
		{
			usern = this->m_PartyS[party_number].Number[i];

			if ( gObjIsConnected(usern) != PLAYER_EMPTY )
			{
				pList.Life = ((gGameObjects[usern]->Life) * 100.0f / (gGameObjects[usern]->MaxLife+gGameObjects[usern]->AddLife));
				pList.Mana = ((gGameObjects[usern]->Mana) * 100.0f / (gGameObjects[usern]->MaxMana+gGameObjects[usern]->AddMana));
				
				if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
				{
					memcpy(pList.Name, gGameObjects[usern]->m_PlayerData->m_RealNameOfUBF, MAX_ACCOUNT_LEN+1);
				}

				else
				{
					memcpy(pList.Name, gGameObjects[usern]->Name, MAX_ACCOUNT_LEN+1);
				}

				memcpy(&sendbuf[lOfs], &pList, sizeof(pList));
				lOfs += sizeof(pList);
			}

			else
			{
				this->m_PartyS[party_number].Number[i] = -1;
				this->m_PartyS[party_number].DbNumber[i] = -1;
				this->m_PartyS[party_number].Count--;
			}
		}
	}

	pCount.h.size = lOfs;
	memcpy(sendbuf, &pCount, sizeof(pCount));

	for (int i=0 ; i<MAX_USER_IN_PARTY ; i++ )
	{
		usern = this->m_PartyS[party_number].Number[i];

		if ( usern >= 0 )
		{
			IOCP.DataSend(usern, (unsigned char*)sendbuf, lOfs);
		}
	}
}

char PartyClass::GetPKPartyPenalty(int party_number)
{
	return m_PartyS[party_number].m_PKPartyLevel;
}

void PartyClass::UpdatePKUserInfo(int party_number, int usernumber, int dbnumber, char userPKLevel)
{
	if( IsParty(party_number) == FALSE )	return;

	for(int i = 0; i<MAX_USER_IN_PARTY; i++)
	{
		if( m_PartyS[party_number].Number[i] == usernumber && 
			m_PartyS[party_number].DbNumber[i] == dbnumber )
		{
			m_PartyS[party_number].m_UserPKLevel[i] = userPKLevel;
			break;
		}
	}
}

void PartyClass::UpdatePKPartyPanalty(int party_number)
{
	int PKUserCount = 0;
	int usern = 0;
	for(int i = 0; i<MAX_USER_IN_PARTY; i++)
	{
		usern = m_PartyS[party_number].Number[i];
		if(usern >= 0 && m_PartyS[party_number].m_UserPKLevel[i] >= 5)
		{
			if( m_PartyS[party_number].m_PKPartyLevel < m_PartyS[party_number].m_UserPKLevel[i] )
			{
				m_PartyS[party_number].m_PKPartyLevel = m_PartyS[party_number].m_UserPKLevel[i];
			}
			PKUserCount++;
		}
	}
	
	m_PartyS[party_number].m_nPKUserCount = PKUserCount;
	if( PKUserCount == 0 )
	{
		m_PartyS[party_number].m_PKPartyLevel = 3;
	}
}

void PartyClass::ChangeLeader(int party_number)
{
	int usern = 0;
	for (int i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		usern = m_PartyS[party_number].Number[i];
		if (usern >= 0)
		{
			m_PartyS[party_number].Number[0] = m_PartyS[party_number].Number[i];
			m_PartyS[party_number].Number[i] = -1;
			m_PartyS[party_number].DbNumber[0] = m_PartyS[party_number].DbNumber[i];
			m_PartyS[party_number].DbNumber[i] = -1;
			m_PartyS[party_number].m_UserPKLevel[0] = m_PartyS[party_number].m_UserPKLevel[i];
			m_PartyS[party_number].m_UserPKLevel[i] = 3;
			break;
		}
	}
}

BOOL PartyClass::EnterITL_PartyAuth(int partynumber, int userindex)
{
	if (this->IsParty(partynumber) == FALSE)
	{
		return FALSE;
	}

	for (int i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		if (this->m_PartyS[partynumber].Number[i] == userindex)
		{
			this->m_PartyS[partynumber].bITLEnterOK[i] = TRUE;
			return TRUE;
		}
	}

	return FALSE;
}

char PartyClass::ReturnOKCount(int partynumber)
{
	if (this->IsParty(partynumber) == FALSE)
	{
		return -1;
	}

	char bReturn = 0;

	for (int i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		if (this->m_PartyS[partynumber].bITLEnterOK[i] == TRUE)
		{
			bReturn++;
		}
	}

	return bReturn;
}

void PartyClass::ClearPartyAuthITL(int partynumber)
{
	if (this->IsParty(partynumber) == FALSE)
	{
		return;
	}

	for (int i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		this->m_PartyS[partynumber].bITLEnterOK[i] = FALSE;
	}

	sLog->outBasic("[ ITL ][ClearPartyAuthITL]PartyID:%d", partynumber);
}

bool PartyClass::AllAgreeEnterITL(int partynumber)
{
	if (this->IsParty(partynumber) == FALSE)
	{
		return false;
	}

	char bReturn = 0;

	for (int i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		if (this->m_PartyS[partynumber].bITLEnterOK[i] == TRUE)
		{
			bReturn++;
		}
	}

	return bReturn == MAX_USER_IN_PARTY;
}

BOOL PartyClass::EnterITR_PartyAuth(int partynumber, int userindex)
{
	if (this->IsParty(partynumber) == FALSE)
	{
		return FALSE;
	}

	for (int i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		if (this->m_PartyS[partynumber].Number[i] == userindex)
		{
			this->m_PartyS[partynumber].bITRWithYouOk[i] = TRUE;
			return TRUE;
		}
	}

	return FALSE;
}

char PartyClass::ReturnOKCountITR(int partynumber)
{
	if (this->IsParty(partynumber) == FALSE)
	{
		return -1;
	}

	char bReturn = 0;

	for (int i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		if (this->m_PartyS[partynumber].bITRWithYouOk[i] == TRUE)
		{
			bReturn++;
		}
	}

	return bReturn;
}

void PartyClass::ClearPartyAuthITR(int partynumber)
{
	if (this->IsParty(partynumber) == FALSE)
	{
		return;
	}

	for (int i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		this->m_PartyS[partynumber].bITRWithYouOk[i] = FALSE;
	}

	sLog->outBasic("[ ITR ][ClearPartyAuthITR]PartyID:%d", partynumber);
}

bool PartyClass::AllAgreeEnterITR(int partynumber)
{
	if (this->IsParty(partynumber) == FALSE)
	{
		return false;
	}

	char bReturn = 0;

	for (int i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		if (this->m_PartyS[partynumber].bITRWithYouOk[i] == TRUE)
		{
			bReturn++;
		}
	}

	return bReturn == g_IT_Event.m_iITR_MinUserInParty;
}

BOOL PartyClass::EnterDSF_PartyAuth(int partynumber, int userindex)
{
	if (this->IsParty(partynumber) == FALSE)
	{
		return FALSE;
	}

	for (int i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		if (this->m_PartyS[partynumber].Number[i] == userindex)
		{
			this->m_PartyS[partynumber].bDSFEnterOK[i] = TRUE;
			return TRUE;
		}
	}

	return FALSE;
}

char PartyClass::DSFReturnOKCount(int partynumber)
{
	if (this->IsParty(partynumber) == FALSE)
	{
		return -1;
	}

	char bReturn = 0;

	for (int i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		if (this->m_PartyS[partynumber].bDSFEnterOK[i] == TRUE)
		{
			bReturn++;
		}
	}

	return bReturn;
}

void PartyClass::ClearPartyAuthDSF(int partynumber)
{
	if (this->IsParty(partynumber) == FALSE)
	{
		return;
	}

	for (int i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		this->m_PartyS[partynumber].bDSFEnterOK[i] = FALSE;
	}

	sLog->outBasic("[DSF][ClearPartyAuthDSF]PartyID:%d", partynumber);
}

bool PartyClass::AllAgreeEnterDSF(int partynumber)
{
	if (this->IsParty(partynumber) == FALSE)
	{
		return false;
	}

	char bReturn = 0;

	for (int i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		if (this->m_PartyS[partynumber].bDSFEnterOK[i] == TRUE)
		{
			bReturn++;
		}
	}

	return bReturn == this->GetLorenMarketUserCount(partynumber) && bReturn == 2;
}

char PartyClass::GetLorenMarketUserCount(int partynumber)
{
	if (this->IsParty(partynumber) == FALSE)
	{
		return -1;
	}

	char bReturn = 0;

	for (int i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		if (!ObjectMaxRange(this->m_PartyS[partynumber].Number[i]))
		{
			continue;
		}

		if (gGameObjects[this->m_PartyS[partynumber]->Number[i]]->MapNumber == MAP_INDEX_LORENMARKET)
		{
			bReturn++;
		}
	}

	return bReturn;
}

void PartyClass::ReadBonusInfo(LPSTR File) //FILE_PARTYBONUS
{
	pugi::xml_document Document;
	pugi::xml_parse_result Result = Document.load_file(File);
	// ----
	if (Result.status != pugi::status_ok)
	{
		sLog->outError("[Notice] File %s not found!", File);
		return;
	}
	// ----
	this->m_BonusInfo.m_NearbyBonus.clear();
	this->m_BonusInfo.m_ClassBonus.clear();
	// ----
	pugi::xml_node BonusInfo = Document.child("PartyBonus");
	pugi::xml_node NearbyBonus = BonusInfo.child("PartyMemberNearby");
	pugi::xml_node ClassBonus = BonusInfo.child("PartyMemberClass");
	pugi::xml_node Node;
	// ----
	for (Node = NearbyBonus.child("Bonus"); Node; Node = Node.next_sibling())
	{
		PartyBonusByNearby lpInfo;
		lpInfo.m_NearbyCount = Node.attribute("NearbyCount").as_int();
		lpInfo.m_AddExp = Node.text().as_int();
		this->m_BonusInfo.m_NearbyBonus.push_back(lpInfo);
	}
	// ----
	for (Node = ClassBonus.child("Bonus"); Node; Node = Node.next_sibling())
	{
		PartyBonusByClass lpInfo;
		lpInfo.m_ClassTable[0] = Node.attribute("DW").as_int();
		lpInfo.m_ClassTable[1] = Node.attribute("DK").as_int();
		lpInfo.m_ClassTable[2] = Node.attribute("ELF").as_int();
		lpInfo.m_ClassTable[3] = Node.attribute("MG").as_int();
		lpInfo.m_ClassTable[4] = Node.attribute("DL").as_int();
		lpInfo.m_ClassTable[5] = Node.attribute("SUM").as_int();
		lpInfo.m_ClassTable[6] = Node.attribute("RF").as_int();
		lpInfo.m_ClassTable[7] = Node.attribute("GL").as_int();
		lpInfo.m_AddExp = Node.text().as_int();
		this->m_BonusInfo.m_ClassBonus.push_back(lpInfo);
	}
}

int PartyClass::GetExpBonus(CGameObject &lpUser, CGameObject lpMonster, int PartyNumber)
{
	if (!lpUser)
	{
		return 0;
	}
	// ----
	int NearbyCount = 0;
	int NearbyBonus = 0;
	int ClassBonus = 0;
	BYTE ClassTable[MAX_TYPE_PLAYER] = { 0 };
	// ----
	for (int i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		int UserIndex = gParty.m_PartyS[PartyNumber].Number[i];
		// ----
		if (UserIndex < 0)
		{
			continue;
		}
		// ----
		CGameObject lpPartyUser = &gGameObjects[UserIndex];
		// ----
		if (!lpPartyUser)
		{
			continue;
		}
		// ----
		if (lpMonster->MapNumber != lpPartyUser->MapNumber)
		{
			continue;
		}
		// ----
		if (gObjCalDistance(lpMonster, lpPartyUser) >= 10)
		{
			continue;
		}
		// ----
		NearbyCount++;
		ClassTable[lpPartyUser->Class] = 1;
	}
	// ----
	for (int j = 0; j < this->m_BonusInfo.m_NearbyBonus.size(); j++)
	{
		if (this->m_BonusInfo.m_NearbyBonus[j].m_NearbyCount == NearbyCount)
		{
			NearbyBonus = this->m_BonusInfo.m_NearbyBonus[j].m_AddExp;
		}
	}
	// ----
	for (int n = 0; n < this->m_BonusInfo.m_ClassBonus.size(); n++)
	{
		if (memcmp(this->m_BonusInfo.m_ClassBonus[n].m_ClassTable,
			ClassTable, MAX_TYPE_PLAYER) == 0)
		{
			ClassBonus = this->m_BonusInfo.m_ClassBonus[n].m_AddExp;
		}
	}
	// ----
	return ClassBonus + NearbyBonus;
}

bool PartyClass::SetLeader(int OldLeader, int NewLeader)
{
	if (!ObjectMaxRange(OldLeader))
		return false;

	if (!ObjectMaxRange(NewLeader))
		return false;

	CGameObject lpObj = &gGameObjects[OldLeader];
	CGameObject lpTargetObj = &gGameObjects[NewLeader];
	// ----
	int PartyNumber = lpObj.PartyNumber;
	// ----
	if (!this->IsParty(PartyNumber))
	{
		GSProtocol.GCServerMsgStringSend("Your character is not in party", lpObj.m_Index, 1);
		return false;
	}
	// ----
	if (lpObj.PartyNumber != lpTargetObj.PartyNumber)
	{
		GSProtocol.GCServerMsgStringSend("Target is not party member", lpObj.m_Index, 1);
		return false;
	}
	// ----
	if (!this->Isleader(PartyNumber, lpObj.m_Index, lpObj.DBNumber))
	{
		GSProtocol.GCServerMsgStringSend("Your character is not party leader", lpObj.m_Index, 1);
		return false;
	}
	// ----
	int TempIndex = this->GetIndex(PartyNumber, lpTargetObj.m_Index, lpTargetObj.DBNumber);
	// ----
	this->m_PartyS[PartyNumber].Number[0] = this->m_PartyS[PartyNumber].Number[TempIndex];
	this->m_PartyS[PartyNumber].DbNumber[0] = this->m_PartyS[PartyNumber].DbNumber[TempIndex];
	this->m_PartyS[PartyNumber].m_UserPKLevel[0] = this->m_PartyS[PartyNumber].m_PKPartyLevel[&TempIndex];
	// ----
	this->m_PartyS[PartyNumber].Number[TempIndex] = lpObj.m_Index;
	this->m_PartyS[PartyNumber].DbNumber[TempIndex] = lpObj.DBNumber;
	this->m_PartyS[PartyNumber].m_PKPartyLevel[&TempIndex] = lpObj.m_PK_Level;
	// ----
	char Text[100] = { 0 };
	sprintf(Text, "New party leader: %s", lpTargetObj.Name);
	// ----
	for (int i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		int UserIndex = this->m_PartyS[PartyNumber].Number[i];
		// ----
		if (UserIndex >= 0 && gGameObjects[UserIndex]->Connected == 3)
		{
			GSProtocol.GCServerMsgStringSend(Text, this->m_PartyS[PartyNumber].Number[i], 1);
			GSProtocol.CGPartyList(this->m_PartyS[PartyNumber].Number[i]);
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

