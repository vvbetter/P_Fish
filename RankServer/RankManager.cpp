#include "Stdafx.h"
#include "RankManager.h"
#include "FishServer.h"
#define RankFilePath "xml\\"
RankManager::RankManager()
{
	m_LoadSum = 0;
	m_RankArray.clear();
}
RankManager::~RankManager()
{
}
void RankManager::OnInit(bool IsNeedLoadNowDayRank)
{
	if (IsNeedLoadNowDayRank)
	{
		OnLoadRankInfoByDB();//�Ƿ���Ҫ�������������ļ�
	}
}
void RankManager::OnDayChange()
{
	//����Ӧ����ǰ�������а��ļ� ��ͷһ���23:50 ���������ļ� ��ǰ�������а��ļ� �����ϴ���FTP������ 23:50
	//23:50�������23:50  �������� 23:50֮ǰ���ڽ���
	OnLoadRankInfoByDB();//����������ص�������� ���а�����
}
void RankManager::OnLoadRankInfoByDB()
{
	m_LoadSum = 0;
	m_RankArray.clear();
	HashMap<BYTE, tagRankConfig>::iterator Iter = g_FishServer.GetFishConfig().GetRankConfig().m_RankMap.begin();
	for (; Iter != g_FishServer.GetFishConfig().GetRankConfig().m_RankMap.end(); ++Iter)
	{
		DBR_Cmd_LoadRankInfo msg;
		SetMsgInfo(msg,DBR_LoadRankInfo, sizeof(DBR_Cmd_LoadRankInfo));
		msg.RankID = Iter->second.RankID;
		msg.RandRowCount = Iter->second.RowCount;
		g_FishServer.SendNetCmdToDB(&msg);
	}
}
void RankManager::OnLoadRankInfoResult(DBO_Cmd_LoadRankInfo* pDB)
{
	//���ؼ��صĽ�� ���ǽ���������ݱ����ڼ�������ȥ
	if (!pDB)
	{
		ASSERT(false);
		return;
	}
	if (pDB->Sum > 0)
	{
		HashMap<BYTE, std::vector<tagRankInfo>>::iterator Iter = m_RankArray.find(pDB->RankID);
		if (Iter == m_RankArray.end())
		{
			std::vector<tagRankInfo> pVec;
			for (int i = 0; i < pDB->Sum; ++i)
			{
				pVec.push_back(pDB->Array[i]);
			}
			m_RankArray.insert(std::map<BYTE, std::vector<tagRankInfo>>::value_type(pDB->RankID, pVec));
		}
		else
		{
			for (int i = 0; i < pDB->Sum; ++i)
			{
				Iter->second.push_back(pDB->Array[i]);
			}
		}
	}
	if ((pDB->States & MsgEnd) != 0)
	{
		++m_LoadSum;
	}
	if (ConvertIntToDWORD(m_LoadSum) == g_FishServer.GetFishConfig().GetRankConfig().m_RankMap.size())
	{
		//�����Ѿ���ȡ����� ���ǿ��������ļ���
		SaveNowDayRankInfo();
	}
}
void RankManager::SaveNowDayRankInfo()
{
	//���ļ����б���
	if (m_RankArray.empty())
		return;
	//�հ׵�����д�����ļ�  3:50  ����Ӧ�ñ��������ļ�  �п������м��ʱ��
	DWORD AllRankSum = 0;

	time_t tm = time(NULL) + Diff_Update_Sec - g_FishServer.GetFishConfig().GetWriteSec();
	SYSTEMTIME time = TimeTToSystemTime(tm);

	char FileName[32] = { 0 };
	sprintf_s(FileName, CountArray(FileName), "Rank_%d_%d_%d.xml", time.wYear, time.wMonth, time.wDay);
	tinyxml2::XMLDocument xmlDoc;
	XMLDeclaration* Declaration = xmlDoc.NewDeclaration("xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"");
	if (!Declaration)
	{
		ASSERT(false);
		return;
	}
	xmlDoc.InsertEndChild(Declaration);
	tinyxml2::XMLElement* elmRank = xmlDoc.NewElement("FishRanks");
	if (!elmRank)
	{
		ASSERT(false);
		return;
	}
	HashMap<BYTE, std::vector<tagRankInfo>>::iterator Iter = m_RankArray.begin();
	for (; Iter != m_RankArray.end(); ++Iter)
	{
		if (Iter->second.empty())
			continue;
		//1.��ʼд��ڵ�
		tinyxml2::XMLElement* elmRook = xmlDoc.NewElement("FishRook");
		if (!elmRook)
			continue;
		elmRook->SetAttribute("TypeID", Iter->first);
		std::vector<tagRankInfo>::iterator IterVec = Iter->second.begin();
		for (int i = 1; IterVec != Iter->second.end(); ++IterVec, ++i)
		{
			//�������� 
			tinyxml2::XMLElement* Info = xmlDoc.NewElement("Info");
			if (!Info)
				continue;
			Info->SetAttribute("Index", i);
			Info->SetAttribute("UserID", (unsigned int)IterVec->dwUserID);
			Info->SetAttribute("FaceID", (unsigned int)IterVec->dwFaceID);
			Info->SetAttribute("Gender", IterVec->Gender ? 1 : 0);
			Info->SetAttribute("TitleID", IterVec->TitleID);
			char Int64[64] = { 0 };
			errno_t Error = _i64toa_s(IterVec->Param, Int64, CountArray(Int64), 10);
			if (Error != 0)
			{
				ASSERT(false);
				continue;
			}
			Info->SetAttribute("Param", Int64);//Int64
			char * UName = UnicodeToutf8((char*)IterVec->NickName);
			Info->SetAttribute("Name", UName);
			free(UName);
			elmRook->InsertEndChild(Info);

			++AllRankSum;
		}
		elmRank->InsertEndChild(elmRook);
	}
	xmlDoc.InsertEndChild(elmRank);

	//���浽ָ��·��
	string FilePath = RankFilePath;
	FilePath  += FileName;
	xmlDoc.SaveFile(FilePath.c_str());
	SaveFileToFtp(FilePath.c_str(),FileName);//���ļ����䵽FTPȥ
	if (time.wDayOfWeek == g_FishServer.GetFishConfig().GetRankConfig().RankWeekDay)//����һ���������а�Ľ����ļ�
	{
		g_FishServer.ShowInfoToWin("�����µ����а��ļ� ����Ϊ��ǰ�ܵ����а�: %s", FileName);

		DWORD VersionID = (time.wYear) * 10000 + time.wMonth * 100 + time.wDay;
		//�����ݱ��浽���ݿ�ȥ ͨ��һ��������з���
		DWORD PageSize = sizeof(DBR_Cmd_SaveWeekRankInfo)+(AllRankSum - 1)*sizeof(tagRankWeekRankInfo);
		DBR_Cmd_SaveWeekRankInfo* msg = (DBR_Cmd_SaveWeekRankInfo*)malloc(PageSize);
		if (!msg)
		{
			ASSERT(false);
			return;
		}
		msg->SetCmdType(DBR_SaveWeekRankInfo);
		HashMap<BYTE, std::vector<tagRankInfo>>::iterator Iter = m_RankArray.begin();
		for (int Index =0; Iter != m_RankArray.end(); ++Iter)
		{
			if (Iter->second.empty())
				continue;
			std::vector<tagRankInfo>::iterator IterVec = Iter->second.begin();
			for (int i = 1; IterVec != Iter->second.end(); ++IterVec, ++i)
			{
				msg->Array[Index].dwUserID = IterVec->dwUserID;
				msg->Array[Index].IsReward = false;
				msg->Array[Index].RankID = Iter->first;
				msg->Array[Index].RankIndex = ConvertIntToBYTE(i);
				msg->Array[Index].VersionID = VersionID;
				++Index;
			}
		}
		std::vector<DBR_Cmd_SaveWeekRankInfo*> pVec;
		SqlitMsg(msg, PageSize, AllRankSum,false, pVec);
		free(msg);
		if (!pVec.empty())
		{
			std::vector<DBR_Cmd_SaveWeekRankInfo*>::iterator Iter = pVec.begin();
			for (; Iter != pVec.end(); ++Iter)
			{
				g_FishServer.SendNetCmdToDB(*Iter);
				free(*Iter);
			}
			pVec.clear();
		}
	}
	else
		g_FishServer.ShowInfoToWin("�����µ����а��ļ�: %s", FileName);
	m_RankArray.clear();//��ǰ������������յ�
}
void RankManager::SaveFileToFtp(const char* FilePath,const char* FileName)
{
	//��ָ���ļ����ɵ�FTP������ȥ 
	//1.����ָ���ļ����ڴ�����
	FILE* pFile = null;
	errno_t Error = fopen_s(&pFile, FilePath, "rb+");
	if (Error != 0 || !pFile)
		return;
	//��ȡ�ļ��ĳ��� 
	fseek(pFile, 0, SEEK_END);
	int FileSize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);
	DWORD PageSize = sizeof(CF_Cmd_UpLoadFile)+(FileSize - 1)* sizeof(BYTE);
	CF_Cmd_UpLoadFile* msg = (CF_Cmd_UpLoadFile*)malloc(PageSize);
	if (!msg)
	{
		ASSERT(false);
		return;
	}
	msg->SetCmdSize(ConvertDWORDToWORD(PageSize));
	msg->SetCmdType(GetMsgType(Main_Ftp, CF_UpLoadFile));
	msg->FileSum = FileSize;
	fread_s(msg->Array, FileSize,1, FileSize, pFile);
	fclose(pFile);
	DWORD SrcLength = CountArray(msg->FileName);
	DWORD DestLength = strlen(FileName);
	strncpy_s(msg->FileName, SrcLength, FileName, DestLength);

	std::vector<CF_Cmd_UpLoadFile*> pVec;
	SqlitMsg(msg, PageSize, FileSize, false, pVec);
	free(msg);
	if (!pVec.empty())
	{
		std::vector<CF_Cmd_UpLoadFile*>::iterator Iter = pVec.begin();
		for (; Iter != pVec.end(); ++Iter)
		{
			g_FishServer.SendNetCmdToFtpServer(*Iter);
			free(*Iter);
		}
		pVec.clear();
	}
}