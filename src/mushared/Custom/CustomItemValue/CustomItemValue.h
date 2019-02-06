////////////////////////////////////////////////////////////////////////////////
// CustomItemValue.h
// ItemValue.h: interface for the CItemValue class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "zzzitem.h"

#define FILE_CUSTOM_ITEMVAL "IGC_ItemValue.ini"

struct ITEM_VALUE_INFO
{
	int Index;
	int Level;
	int Grade;
	int Value;
};

class CItemValue
{
public:
	CItemValue();
	virtual ~CItemValue();
	void Load(char* path);
	bool GetItemValue(CItem* lpItem, int* value);
private:
	std::vector<ITEM_VALUE_INFO> m_ItemValueInfo;
};

extern CItemValue gItemValue;


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

