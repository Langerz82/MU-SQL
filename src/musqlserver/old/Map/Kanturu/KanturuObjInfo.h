////////////////////////////////////////////////////////////////////////////////
// KanturuObjInfo.h
#ifndef KANTURU_OBJ_INFO_H___
#define KANTURU_OBJ_INFO_H___

#define MAX_KANTURU_OBJ_INDEX	800

#include "custTypedef.h"

class CKanturuObjInfo
{

public:

	CKanturuObjInfo()
	{
		this->Reset();
	}

	virtual ~CKanturuObjInfo(){return;}

	void Reset()
	{
		for ( int iCount=0;iCount<MAX_KANTURU_OBJ_INDEX;iCount++)
		{
			this->m_iObjIndex[iCount]=-1;
		}

		this->m_iObjCount=0;
	}

	BOOL AddObj(int iIndex)
	{
		if ( this->m_iObjCount >= MAX_KANTURU_OBJ_INDEX )
		{
			return FALSE;
		}

		this->m_iObjIndex[this->m_iObjCount] = iIndex;
		this->m_iObjCount++;

		return TRUE;
	}

	int GetCount()
	{
		return this->m_iObjCount;
	}
	
public:

	int m_iObjCount;	// 4
	int m_iObjIndex[MAX_KANTURU_OBJ_INDEX];	// 8
};

#endif /*KANTURU_OBJ_INFO_H___*/

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

