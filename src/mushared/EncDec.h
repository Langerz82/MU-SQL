////////////////////////////////////////////////////////////////////////////////
// EncDec.h
#ifndef ENCDEC_H
#define ENCDEC_H

class CEncDec{
	bool m_CGInitialized;
	bool m_GCInitialized;

	BYTE m_CGXorKey[256];
	BYTE m_CGSubKey[256];

	BYTE m_GCXorKey[256];
	BYTE m_GCSubKey[256];
public:
	CEncDec();
	void OnConnect();
	void CreateServerKey(DWORD xorSeed, DWORD subSeed);
	void CreateClientKey(DWORD xorSeed, DWORD subSeed);

	void EncCGBuff(BYTE* buff, int len);
	bool DecCGBuff(BYTE* buff, int len);

	void EncGCBuff(BYTE* buff, int len);
	void DecGCBuff(BYTE* buff, int len);
};

extern CEncDec* g_EncDec;
#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

