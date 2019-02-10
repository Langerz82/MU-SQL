////////////////////////////////////////////////////////////////////////////////
// PacketCheckSum.h
#ifndef PACKETCHECKSUM_H
#define PACKETCHECKSUM_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_PACKET_CHECKSUM_FUNCTION_INDEX	22
#define MAX_CHECKSUM_TABLE_AVG	100

struct PAKETCHECKSUM
{
	DWORD Check[MAX_PACKET_CHECKSUM_FUNCTION_INDEX];	// 0
};

class CPacketCheckSum
{

public:

	CPacketCheckSum();
	virtual ~CPacketCheckSum();

	void Init();
	void Check(int aIndex);
	void ClearCheckSum(int aIndex);
	int Add(LPGameObject &lpObj, int funcindex, DWORD checksum);
	void AddCheckSum(LPGameObject &lpObj, int funcindex, DWORD checksum);
	void SetClearChecksumFunc(int funcindex);

private:

	PAKETCHECKSUM * PaketChecksum;	// 4
	int m_ChecksumTableAllClearState;	// 9EFC4
	int m_ChecksumTableClearState[MAX_PACKET_CHECKSUM_FUNCTION_INDEX];	// 9EFC8
	int m_ChecksumTableAVGCount[MAX_PACKET_CHECKSUM_FUNCTION_INDEX];	// 9F020
	DWORD m_ChecksumTableAVG[MAX_PACKET_CHECKSUM_FUNCTION_INDEX][MAX_CHECKSUM_TABLE_AVG];	// 9F078
	DWORD m_ChecksumTable[MAX_PACKET_CHECKSUM_FUNCTION_INDEX];	// A12D8

};

extern CPacketCheckSum gPacketCheckSum;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

