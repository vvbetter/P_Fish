#pragma once
#pragma pack(push)
#pragma pack(1)

struct NetCmd
{
private:
	USHORT	CmdSize;
	BYTE    Uid[8];
	BYTE	SubCmdType;
	BYTE	CmdType;
	/*USHORT  Rand;
	USHORT  Identity;*/
public:
	const BYTE GetMainType() const
	{
		return CmdType;
	}
	const BYTE GetSubType() const
	{
		return SubCmdType;
	}
	void CreateRandMask()
	{
		//Rand = rand() % USHRT_MAX;
		//Identity = (~Rand) ^ (~CmdSize) ^ (~SubCmdType) ^ (~CmdType);
	}
	bool CheckRandMask()
	{
		return true;
		//return Identity == (~Rand) ^ (~CmdSize) ^ (~SubCmdType) ^ (~CmdType);
	}
	void SetCmdType(USHORT cmdType)
	{
		CmdType = static_cast<BYTE>(cmdType >> 8);
		SubCmdType = static_cast<BYTE>(cmdType);
	}
	USHORT GetCmdType()
	{
		return static_cast<USHORT>((CmdType << 8) | SubCmdType);
	}
	void SetCmdSize(USHORT size)
	{
		CmdSize = htons(size);
	}
	USHORT GetCmdSize()
	{
		return ntohs(CmdSize);
	}
	NetCmd()
	{
		CmdSize = 0;
		memset(Uid, 0, 8);
		SubCmdType = 0;
		CmdType = 0;
	}
};

inline NetCmd* CreateCmd(USHORT size)
{
	NetCmd* pCmd = (NetCmd*)malloc(size);
	memset(pCmd, 0, size);
	pCmd->SetCmdSize(size);
	return pCmd;
}
inline NetCmd* CreateCmd(const USHORT size,const void *pSrc)
{
	NetCmd* pCmd = (NetCmd*)malloc(size);
	if (!pCmd)
		return NULL;
	memcpy(pCmd, pSrc, size);
	return pCmd;
}
inline NetCmd* CreateCmd(USHORT cmdType, USHORT size)
{
	NetCmd* pCmd = (NetCmd*)malloc(size);
	memset(pCmd, 0, size);
	pCmd->SetCmdSize(size);
	pCmd->SetCmdType(cmdType);
	return pCmd;
}
inline void DeleteCmd(NetCmd *pCmd)
{
	free(pCmd);
}
#pragma pack(pop)