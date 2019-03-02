////////////////////////////////////////////////////////////////////////////////
// CustomItemValue.h
// ItemValue.h: interface for the CItemObjectValue class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "ItemObject.h"

#define FILE_CUSTOM_ITEMVAL "IGC_ItemValue.ini"

struct ITEM_VALUE_INFO
{
	int Index;
	int Level;
	int Grade;
	int Value;
};

class CItemObjectValue
{
public:
	CItemObjectValue();
	virtual ~CItemObjectValue();
	void Load(char* path);
	bool GetItemValue(CItemObject* lpItem, int* value);
private:
	std::vector<ITEM_VALUE_INFO> m_ItemValueInfo;
};

extern CItemObjectValue gItemValue;


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

