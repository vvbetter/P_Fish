#pragma once
#include "Stdafx.h"
#include <map>
#include <vector>
struct tagRankInfo;
class RankManager
{
public:
	RankManager();
	virtual ~RankManager();
	void OnInit(bool IsNeedLoadNowDayRank);
	void OnLoadRankInfoByDB();
	void OnLoadRankInfoResult(DBO_Cmd_LoadRankInfo* pDB);
	void OnDayChange();
private:
	void SaveNowDayRankInfo();
	void SaveFileToFtp(const char* FilePath,const char* FileName);
private:
	//��������а�����
	int													m_LoadSum;//�����ж����а��Ƿ��ȡ�����
	HashMap<BYTE, std::vector<tagRankInfo>>				m_RankArray;//���غ󲻿��ܽ��иı������
};