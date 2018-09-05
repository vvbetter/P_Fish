#include "Stdafx.h"
#include "FishServer.h"
#include "FishMonthManager.h"
#include <algorithm>
extern void SendLogDB(NetCmd* pCmd);
#define RankFilePath "xml\\"
bool SortByMonthSocre(tagRoleMonthCenterInfo* obj1, tagRoleMonthCenterInfo* obj2)//������
{
	if (!obj1 || !obj2)
		return false;
	return (obj1->MonthInfo.dwMonthScore*obj1->VipAddScore) > (obj2->MonthInfo.dwMonthScore*obj2->VipAddScore);//��Ҫ��ӻ��ֵı���
}
FishMonth::FishMonth()
{
	m_MonthID = 0;
	m_IsInSignUp = false;
	m_IsStart = false;
	m_NowMonthTimeID = 0;
	//m_MonthRankSum = (RandUInt() % 30) + 20;
}
FishMonth::~FishMonth()
{
	OnDestroy();
}
bool FishMonth::OnInit(BYTE MontID)
{
	HashMap<BYTE, tagMonthConfig>::iterator Iter = g_FishServer.GetFishConfig().GetMonthConfig().m_MonthMap.find(MontID);
	if (Iter == g_FishServer.GetFishConfig().GetMonthConfig().m_MonthMap.end())
	{
		ASSERT(false);
		return false;
	}
	//m_FishMonthConfig = &Iter->second;
	m_MonthID = MontID;
	m_RoleVec.clear();
	m_MonthRoleMap.clear();
	m_IsInSignUp = false;
	m_IsStart = false;
	return true;
}
void FishMonth::OnLoadRobotInfo()
{
	if (!g_FishServer.GetMonthManager().RobotIsFinish())
		return;
	if (!m_IsInSignUp && !m_IsStart)//�Ǳ����׶β����ػ�����
		return;
	HashMap<BYTE, tagMonthConfig>::iterator IterConfig = g_FishServer.GetFishConfig().GetMonthConfig().m_MonthMap.find(m_MonthID);
	if (IterConfig == g_FishServer.GetFishConfig().GetMonthConfig().m_MonthMap.end())
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, tagMonthRobot>::iterator IterRobot = IterConfig->second.m_RobotMap.begin();
	for (; IterRobot != IterConfig->second.m_RobotMap.end(); ++IterRobot)
	{
		float fValue = 1.0f;
		if (IterRobot->second.SumRate != 0)
			fValue = (RandUInt() % (IterRobot->second.SumRate * 2) + (100 - IterRobot->second.SumRate)) / 100.0f;
		DWORD RobotSum = static_cast<DWORD>(fValue* IterRobot->second.RobotSum);
		for (DWORD i = 0; i < RobotSum; ++i)
		{
			RobotUserInfo * pInfo = g_FishServer.GetRobotManager().GetNonUseRobot();
			if (!pInfo)
			{
				continue;
			}
			//DWORD Level = IterRobot->first;

			tagRoleMonthCenterInfo* pRole = new tagRoleMonthCenterInfo;
			if (!pRole)
			{
				ASSERT(false);
				return;
			}
			pRole->MonthInfo.bMonthID = m_MonthID;
			pRole->MonthInfo.dwIndex = m_RoleVec.size();
			pRole->MonthInfo.dwMonthGlobel = IterConfig->second.InitMonthGlobel;//������ʱ��ͳ�ʼ���û���
			pRole->MonthInfo.dwMonthScore = 0;
			pRole->MonthInfo.dwUserID = pInfo->dwUserID;
			pRole->MonthInfo.wUserAddGlobelNum = 0;
			TCHARCopy(pRole->NickName, CountArray(pRole->NickName), pInfo->NickName, _tcslen(pInfo->NickName));//����������ڼ�¼xml�ļ�ʹ�õ�
			pRole->FaceID = pInfo->dwFaceID;//ͷ��ID
			pRole->VipAddScore = g_FishServer.GetFishConfig().GetVipConfig().AddMonthScoreRate(pInfo->VipLevel);//�������ֵ�����
			pRole->MonthInfo.SkillSum = 0;
			m_RoleVec.push_back(pRole);
			m_MonthRoleMap.insert(std::map<DWORD, tagRoleMonthCenterInfo*>::value_type(pRole->MonthInfo.dwUserID, pRole));

			tagRobotInfo pRobotInfo;
			pRobotInfo.dwUserID = pInfo->dwUserID;
			pRobotInfo.Level = IterRobot->first;
			pRobotInfo.SkillSum = 0;

			//���õ�ǰδ������ ��¼��
			m_RobotMap.insert(HashMap<DWORD, tagRobotInfo>::value_type(pInfo->dwUserID, pRobotInfo));
		}
	}
	OnRoleSumChange();
}
void FishMonth::OnLoadRobotFinish()
{
	OnLoadRobotInfo();//�������ػ�����
}
void FishMonth::SendMonthStatesToGameServer(BYTE GameConfigID, BYTE MonthStates)
{
	//��������״̬���͵�GameServerȥ
	CG_Cmd_MonthStates msg;
	SetMsgInfo(msg, GetMsgType(Main_Month, CG_MonthStates), sizeof(CG_Cmd_MonthStates));
	msg.MonthID = m_MonthID;
	msg.MonthStates = MonthStates;

	if (GameConfigID == 0)
	{
		g_FishServer.GetCenterManager().SendNetCmdToAllGameServer(&msg);
	}
	else
	{
		g_FishServer.GetCenterManager().SendNetCmdToGameServer(GameConfigID, &msg);
	}
}
void FishMonth::OnUpdateRobotInfo()
{
	//30s����һ�� 
	//����ȫ���Ļ����� ��ӻ����˵Ļ���
	if (!IsInStart())
		return;
	if (m_RobotMap.empty())
		return;
	HashMap<BYTE, tagMonthConfig>::iterator IterConfig = g_FishServer.GetFishConfig().GetMonthConfig().m_MonthMap.find(m_MonthID);
	if (IterConfig == g_FishServer.GetFishConfig().GetMonthConfig().m_MonthMap.end())
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, tagRobotInfo>::iterator Iter = m_RobotMap.begin();
	for (; Iter != m_RobotMap.end(); ++Iter)
	{
		HashMap<DWORD, tagRoleMonthCenterInfo*>::iterator IterFind = m_MonthRoleMap.find(Iter->first);
		if (IterFind == m_MonthRoleMap.end())
			continue;
		HashMap<DWORD, tagMonthRobot>::iterator IterRobot = IterConfig->second.m_RobotMap.find(Iter->second.Level);
		if (IterRobot == IterConfig->second.m_RobotMap.end())
			continue;
		if (IterFind->second->MonthInfo.dwMonthScore >= IterRobot->second.MaxScore)
			continue;

		float fValue = 1.0f;
		if (IterRobot->second.Rate != 0)
			fValue = (RandUInt() % (IterRobot->second.Rate * 2) + (100 - IterRobot->second.Rate)) / 100.0f;
		DWORD AddScore = static_cast<DWORD>(IterRobot->second.AddScore * fValue);
		IterFind->second->MonthInfo.dwMonthScore += AddScore;

		//������ ���� ��ͨʹ�û��� ���� �п��� ��ÿ�θ��µ�ʱ�� ʹ�ü��� 
		if (Iter->second.SkillSum < IterRobot->second.SkillSum)
		{
			if (RandUInt() % 100 <= IterRobot->second.SkillUseRate)
			{
				fValue = 1.0f;
				if (IterRobot->second.SkillScoreRate != 0)
					fValue = (RandUInt() % (IterRobot->second.SkillScoreRate * 2) + (100 - IterRobot->second.SkillScoreRate)) / 100.0f;
				DWORD AddScore = static_cast<DWORD>(IterRobot->second.SkillScore * fValue);
				IterFind->second->MonthInfo.dwMonthScore += AddScore;
				IterFind->second->MonthInfo.SkillSum += 1;//���ܴ������1
				Iter->second.SkillSum += 1;
			}
		}
	}
}
void FishMonth::OnDestroyRobotInfo()
{
	//�ڱ���������ʱ�� ���ǽ�������ȫ���黹
	if (m_RobotMap.empty())
		return;
	HashMap<DWORD, tagRobotInfo>::iterator Iter = m_RobotMap.begin();
	for (; Iter != m_RobotMap.end(); ++Iter)
	{
		g_FishServer.GetRobotManager().OnNonUseRobot(Iter->first);//�黹������
	}
}
void FishMonth::OnDestroy()
{
	//�����ݽ�������
	if (m_RoleVec.empty())
		return;
	std::vector<tagRoleMonthCenterInfo*>::iterator Iter = m_RoleVec.begin();
	for (; Iter != m_RoleVec.end(); ++Iter)
	{
		if (!(*Iter))
			continue;
		delete (*Iter);
	}
	m_RoleVec.clear();
	m_MonthRoleMap.clear();
}
BYTE FishMonth::GetMonthID()
{
	return m_MonthID;
}
void FishMonth::SaveFileToFtp(const char* FilePath,const char* FileName)//����ǰ���������������ϴ���FTPȥ
{
	if (!FileName || !FilePath)
	{
		ASSERT(false);
		return;
	}
	if (strlen(FileName) == 0 || strlen(FilePath) == 0)
	{
		ASSERT(false);
		return;
	}
	FILE* pFile = NULL;
	errno_t Error = fopen_s(&pFile, FilePath, "rb+");
	if (Error !=0 || !pFile)
	{
		ASSERT(false);
		return;
	}
	//��ȡ�ļ��ĳ��� 
	fseek(pFile, 0, SEEK_END);
	long FileSize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);
	DWORD PageSize = sizeof(CF_Cmd_UpLoadFile)+(FileSize - 1)* sizeof(BYTE);
	CF_Cmd_UpLoadFile* msg = (CF_Cmd_UpLoadFile*)malloc(PageSize);
	if (!msg)
	{
		ASSERT(false);
		return;
	}
	msg->SetCmdType(GetMsgType(Main_Ftp, CF_UpLoadFile));
	fread_s(msg->Array, FileSize, 1, FileSize, pFile);
	msg->FileSum = FileSize;
	fclose(pFile);
	strncpy_s(msg->FileName, CountArray(msg->FileName), FileName, strlen(FileName));
	
	//�����������
	std::vector<CF_Cmd_UpLoadFile*> pVec;
	SqlitMsg(msg, PageSize, FileSize, false, pVec);
	free(msg);
	if (!pVec.empty())
	{
		std::vector<CF_Cmd_UpLoadFile*>::iterator Iter = pVec.begin();
		for (; Iter != pVec.end(); ++Iter)
		{
			g_FishServer.SendNetCmdToFtp(*Iter);
			free(*Iter);
		}
		pVec.clear();
	}
}
void FishMonth::UpdateByMin()//�����ĸ��� Ӧ�úͿͻ��˱���һ�� ���� 0���ʱ����� ���ǰ�������
{
	HashMap<BYTE, tagMonthConfig>::iterator IterConfig = g_FishServer.GetFishConfig().GetMonthConfig().m_MonthMap.find(m_MonthID);
	if (IterConfig == g_FishServer.GetFishConfig().GetMonthConfig().m_MonthMap.end())
	{
		//������������ �����������ļ������� ���ǹرձ���
		OnLeaveSignUp();//���������׶�
		OnLeaveStart();//���������׶�
		ASSERT(false);
		return;
	}

	//�׶ν��и��´��� �ܶ�׶� 
	//������ͬһ���ӽ��д���� ������Ҫ�ж���
	//��Ҫһ������Ψһ��ǵ�ID ���ڱ�Ǳ����Ľ׶�
	DWORD MonthTimeID = IterConfig->second.GetMonthTimeID();
	if (m_NowMonthTimeID != 0)
	{
		//��ǰ�ڱ����� �����жϱ����Ƿ���Ҫ����
		if (m_NowMonthTimeID != MonthTimeID)
		{
			//��ǰ�׶��Ѿ�������
			//�õ�ǰ�׶ν���
			OnLeaveSignUp();//���������׶�
			OnLeaveStart();//���������׶�
			m_NowMonthTimeID = MonthTimeID;
		}
		else
		{
			//���ڵ�ǰ�׶� �����ж���
			BYTE TimeID = (BYTE)(MonthTimeID >> 16);
			HashMap<BYTE, tagMonthTime>::iterator Iter = IterConfig->second.m_MonthTime.find(TimeID);
			if (Iter == IterConfig->second.m_MonthTime.end())
			{
				ASSERT(false);
				//ʱ�䲻���� ���̽�������
				OnLeaveSignUp();
				OnLeaveStart();
			}
			else
			{
				if (!Iter->second.IsInSignTime())
					OnLeaveSignUp();
				if (!Iter->second.IsInStarTime())
					OnLeaveStart();
			}
		}
	}
	else
		m_NowMonthTimeID = MonthTimeID;
	//�ɵı��� �Ѿ������� ��ʼ�����µı���
	if (m_NowMonthTimeID != 0)
	{
		//�µĽ׶�  �ж� �Ƿ��������� ���� ��ʼ������
		BYTE TimeID = (BYTE)(MonthTimeID >> 16);
		HashMap<BYTE, tagMonthTime>::iterator Iter = IterConfig->second.m_MonthTime.find(TimeID);
		if (Iter == IterConfig->second.m_MonthTime.end())
		{
			ASSERT(false);
			//ʱ�䲻���� ���̽�������
			OnLeaveSignUp();
			OnLeaveStart();
		}
		else
		{
			if (Iter->second.IsInSignTime())
				OnJoinSignUp();
			if (Iter->second.IsInStarTime())
				OnJoinStart();
		}
	}
}
BYTE FishMonth::IsInSignUp()
{
	HashMap<BYTE, tagMonthConfig>::iterator IterConfig = g_FishServer.GetFishConfig().GetMonthConfig().m_MonthMap.find(m_MonthID);
	if (IterConfig == g_FishServer.GetFishConfig().GetMonthConfig().m_MonthMap.end())
	{
		ASSERT(false);
		return 0;
	}
	//�жϵ�ǰʱ���Ƿ��ڱ����׶� �����ж�
	HashMap<BYTE, tagMonthTime>::iterator Iter = IterConfig->second.m_MonthTime.begin();
	for (; Iter != IterConfig->second.m_MonthTime.end(); ++Iter)
	{
		if (Iter->second.IsInSignTime())
		{
			return Iter->first;//��IDΪ���
		}
	}
	return 0;
}
BYTE FishMonth::IsInStart()
{
	HashMap<BYTE, tagMonthConfig>::iterator IterConfig = g_FishServer.GetFishConfig().GetMonthConfig().m_MonthMap.find(m_MonthID);
	if (IterConfig == g_FishServer.GetFishConfig().GetMonthConfig().m_MonthMap.end())
	{
		ASSERT(false);
		return 0;
	}
	HashMap<BYTE, tagMonthTime>::iterator Iter = IterConfig->second.m_MonthTime.begin();
	for (; Iter != IterConfig->second.m_MonthTime.end(); ++Iter)
	{
		if (Iter->second.IsInStarTime())
		{
			return Iter->first;//��IDΪ���
		}
	}
	return 0;
}
void FishMonth::OnJoinSignUp()
{
	//���������뵽�����׶ε�ʱ�� ���ǿ�ʼ��һЩ׼������
	if (m_IsInSignUp)
		return;
	if (!m_RoleVec.empty())
	{
		ASSERT(false);
	}
	m_IsInSignUp = true;

	OnLoadRobotInfo();//���ػ�����

	SendMonthStatesToGameServer(0,1);
	/*SYSTEMTIME pTime;
	GetLocalTime(&pTime);
	char IpStr[128] = { 0 };
	sprintf_s(IpStr, sizeof(IpStr), "����ID:%d  TimeID:%d  ���뱨���׶�  %d:%d \n", m_FishMonthConfig->MonthID,(m_NowMonthTimeID >> 16), pTime.wHour, pTime.wMinute);
	std::cout << IpStr;*/
}
void FishMonth::OnLeaveSignUp()
{
	if (!m_IsInSignUp)
		return;
	//�뿪�����Ľ׶ε�ʱ��
	m_IsInSignUp = false;
	SendMonthStatesToGameServer(0,2);
	/*SYSTEMTIME pTime;
	GetLocalTime(&pTime);
	char IpStr[128] = { 0 };
	sprintf_s(IpStr, sizeof(IpStr), "����ID:%d  TimeID:%d  �뿪�����׶�  %d:%d \n", m_FishMonthConfig->MonthID, (m_NowMonthTimeID >> 16), pTime.wHour, pTime.wMinute);
	std::cout << IpStr;*/
}
void FishMonth::OnJoinStart()//����������ʽ�׶�
{
	if (m_IsStart)
		return;
	HashMap<BYTE, tagMonthConfig>::iterator IterConfig = g_FishServer.GetFishConfig().GetMonthConfig().m_MonthMap.find(m_MonthID);
	if (IterConfig == g_FishServer.GetFishConfig().GetMonthConfig().m_MonthMap.end())
	{
		ASSERT(false);
		return;
	}

	//�жϱ��������� ���ǽ�������
	m_IsStart = true;
	SendMonthStatesToGameServer(0,4);
	/*SYSTEMTIME pTime;
	GetLocalTime(&pTime);
	char IpStr[128] = { 0 };
	sprintf_s(IpStr, sizeof(IpStr), "����ID:%d  TimeID:%d  ��������׶�  %d:%d \n", m_FishMonthConfig->MonthID, (m_NowMonthTimeID >> 16), pTime.wHour, pTime.wMinute);
	std::cout << IpStr;*/

	if (m_RoleVec.empty())
		return;//��ǰ�������˲μ� ���봦��
	std::vector<tagRoleMonthCenterInfo*>::iterator Iter = m_RoleVec.begin();
	for (int i = 0; Iter != m_RoleVec.end(); ++Iter, ++i)
	{
		if (!(*Iter))
			continue;
		//���õ�ǰ��ҵ�һЩ����������
		(*Iter)->MonthInfo.dwMonthGlobel = IterConfig->second.InitMonthGlobel;
		(*Iter)->MonthInfo.dwMonthScore = 0;
		(*Iter)->MonthInfo.wUserAddGlobelNum = 0;
		(*Iter)->MonthInfo.dwIndex = i;//���ú�����
	}
}
void FishMonth::OnSaveMonthXml(SYSTEMTIME& pTime, std::vector<tagRoleMonthCenterInfo*>& pVec)
{
	//��һ�ֱ��������� ������Ҫд��XMl�ļ� Month_ID_Year_Month_Day_Hour_Min.xml
	HashMap<BYTE, tagMonthConfig>::iterator IterConfig = g_FishServer.GetFishConfig().GetMonthConfig().m_MonthMap.find(m_MonthID);
	if (IterConfig == g_FishServer.GetFishConfig().GetMonthConfig().m_MonthMap.end())
	{
		ASSERT(false);
		return;
	}
	if (pVec.empty())
	{
		//����Ҫ���е����� ���������ļ�
		return;
	}
	BYTE RankSum = IterConfig->second.RankSum;
	char FileName[32] = { 0 };
	sprintf_s(FileName, CountArray(FileName), "Month_%d_%d_%d_%d_%d_%d.xml",m_MonthID, pTime.wYear, pTime.wMonth, pTime.wDay, pTime.wHour, pTime.wMinute);

	tinyxml2::XMLDocument xmlDoc;
	XMLDeclaration* Declaration = xmlDoc.NewDeclaration("xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"");
	if (!Declaration)
	{
		ASSERT(false);
		return;
	}
	xmlDoc.InsertEndChild(Declaration);
	tinyxml2::XMLElement* elmRank = xmlDoc.NewElement("FishMonth");
	if (!elmRank)
	{
		ASSERT(false);
		return;
	}
	elmRank->SetAttribute("MonthID", m_MonthID);
	std::vector<tagRoleMonthCenterInfo*>::iterator Iter = pVec.begin();
	for (int i = 1; Iter != pVec.end() && i <= RankSum; ++Iter)
	{
		if (!(*Iter))
			continue;
		if ((*Iter)->MonthInfo.dwMonthScore == 0)//����Ϊ0����������
			continue;
		tinyxml2::XMLElement* elmRook = xmlDoc.NewElement("RoleInfo");
		if (!elmRook)
			continue;
		elmRook->SetAttribute("Index", i);
		elmRook->SetAttribute("UserID", (unsigned int)(*Iter)->MonthInfo.dwUserID);
		elmRook->SetAttribute("FaceID", (unsigned int)(*Iter)->FaceID);
		char * UNickName = UnicodeToutf8((char*)(*Iter)->NickName);
		elmRook->SetAttribute("NickName", UNickName);
		free(UNickName);
		elmRook->SetAttribute("Socre", (unsigned int)(*Iter)->MonthInfo.dwMonthScore);
		elmRook->SetAttribute("VipSocre", (unsigned int)((*Iter)->MonthInfo.dwMonthScore * (*Iter)->VipAddScore - (*Iter)->MonthInfo.dwMonthScore));
		elmRank->InsertEndChild(elmRook);
		++i;
	}
	xmlDoc.InsertEndChild(elmRank);

	string FilePath = RankFilePath;
	FilePath += FileName;
	xmlDoc.SaveFile(FilePath.c_str());

	SaveFileToFtp(FilePath.c_str(),FileName);
}
void FishMonth::OnLeaveStart()
{
	if (!m_IsStart)
		return;
	HashMap<BYTE, tagMonthConfig>::iterator IterConfig = g_FishServer.GetFishConfig().GetMonthConfig().m_MonthMap.find(m_MonthID);
	if (IterConfig == g_FishServer.GetFishConfig().GetMonthConfig().m_MonthMap.end())
	{
		ASSERT(false);
		return;
	}
	//�����б������� ���Ƿ��ͽ�������ҵ�����ȥ
	m_IsStart = false;
	if (m_RoleVec.empty())
		return;
	//����ǰ�����ļ�¼�������� ��������β����
	m_LastMonthInfo.OnAddInfo(m_NowMonthTimeID,m_RoleVec);//����ǰ�ķ��뵽��������ȥ

	//�������� ��ȫ����GameServer ��ȫ����GameServer�ϴ�����
	CG_Cmd_GetUserMonthInfo msg;
	SetMsgInfo(msg, GetMsgType(Main_Month, CG_GetUserMonthInfo), sizeof(CG_Cmd_GetUserMonthInfo));
	msg.MonthID = m_MonthID;
	g_FishServer.GetCenterManager().SendNetCmdToAllGameServer(&msg);

	m_RoleVec.clear();//��ռ��� ����ɾ������
	m_MonthRoleMap.clear();//һ������������ �������
	OnRoleSumChange();//������0

	OnDestroyRobotInfo();

	SendMonthStatesToGameServer(0,8);
}
void FishMonth::OnChangeRolePoint(DWORD dwUserID, DWORD Point)
{
	if (!m_IsStart)
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, tagRoleMonthCenterInfo*>::iterator IterFind = m_MonthRoleMap.find(dwUserID);
	if (IterFind == m_MonthRoleMap.end() || !IterFind->second)
	{
		ASSERT(false);
		return;
	}
	std::vector<tagRoleMonthCenterInfo*>::iterator Iter = m_RoleVec.begin() + IterFind->second->MonthInfo.dwIndex;
	if (Iter == m_RoleVec.end() || !(*Iter) || (*Iter)->MonthInfo.dwUserID != dwUserID)
	{
		ASSERT(false);
		return;
	}
	//ֱ���޸���ҵĻ���
	(*Iter)->MonthInfo.dwMonthScore = Point;
}
void FishMonth::OnChangeRoleGlobel(DWORD dwUserID, DWORD Globel)
{
	//�޸���ҵĽ��
	HashMap<DWORD, tagRoleMonthCenterInfo*>::iterator Iter = m_MonthRoleMap.find(dwUserID);
	if (Iter != m_MonthRoleMap.end() && Iter->second)
	{
		Iter->second->MonthInfo.dwMonthGlobel = Globel;
	}
	else
	{
		ASSERT(false);
	}
}
void FishMonth::OnChangeRoleInfo(DWORD dwUserID, DWORD Point, DWORD Globel, DWORD SkillSum)
{
	HashMap<DWORD, tagRoleMonthCenterInfo*>::iterator Iter = m_MonthRoleMap.find(dwUserID);
	if (Iter != m_MonthRoleMap.end() && Iter->second)
	{
		Iter->second->MonthInfo.dwMonthGlobel = Globel;
		Iter->second->MonthInfo.dwMonthScore = Point;
		Iter->second->MonthInfo.SkillSum = SkillSum;
	}
	else
	{
		ASSERT(false);
	}
}
void FishMonth::OnAddMonthGlobelNum(DWORD dwUserID)
{
	HashMap<BYTE, tagMonthConfig>::iterator IterConfig = g_FishServer.GetFishConfig().GetMonthConfig().m_MonthMap.find(m_MonthID);
	if (IterConfig == g_FishServer.GetFishConfig().GetMonthConfig().m_MonthMap.end())
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, tagRoleMonthCenterInfo*>::iterator Iter = m_MonthRoleMap.find(dwUserID);
	if (Iter != m_MonthRoleMap.end() && Iter->second)
	{
		Iter->second->MonthInfo.wUserAddGlobelNum += 1;
		//���Һ� �����ҵĽ�� �ֱҵĿ۳� ��GameServer����
		HashMap<BYTE, tagMonthConvert>::iterator IterFind = IterConfig->second.m_ConvertMap.find(Iter->second->MonthInfo.wUserAddGlobelNum);
		if (IterFind != IterConfig->second.m_ConvertMap.end())
		{
			Iter->second->MonthInfo.dwMonthGlobel += IterFind->second.AddMonthGlobel;
		}
		else
		{
			ASSERT(false);
		}
	}
	else
	{
		ASSERT(false);
	}
}
void FishMonth::OnLoadAllGameUserMonthInfo()
{
	HashMap<BYTE, tagMonthConfig>::iterator IterConfig = g_FishServer.GetFishConfig().GetMonthConfig().m_MonthMap.find(m_MonthID);
	if (IterConfig == g_FishServer.GetFishConfig().GetMonthConfig().m_MonthMap.end())
	{
		ASSERT(false);
		return;
	}
	//һ��GameServer �Ѿ���ȫ���Ļ����ϴ��� 
	if (m_LastMonthInfo.OnAddGameServerFinish())
	{
		/*char IpStr[128] = { 0 };
		sprintf_s(IpStr, sizeof(IpStr), "����ID:%d  TimeID:%d  �뿪�����׶�  %d:%d \n", m_FishMonthConfig->MonthID, (m_LastMonthInfo.m_NowMonthTimeID >> 16), m_LastMonthInfo.pTime.wHour, m_LastMonthInfo.pTime.wMinute);
		std::cout << IpStr;*/

		if (m_LastMonthInfo.Lastm_RoleVec.empty())
			return;
		//��vector �������� �����������
		sort(m_LastMonthInfo.Lastm_RoleVec.begin(), m_LastMonthInfo.Lastm_RoleVec.end(), SortByMonthSocre);//���������� �������
		OnSaveMonthXml(m_LastMonthInfo.pTime,m_LastMonthInfo.Lastm_RoleVec);
		std::vector<tagRoleMonthCenterInfo*>::iterator Iter = m_LastMonthInfo.Lastm_RoleVec.begin();
		for (int i = 0; Iter != m_LastMonthInfo.Lastm_RoleVec.end(); ++Iter)
		{
			//�������GameServer ������뿪����
			if (!(*Iter))
				continue;


			if ((*Iter)->MonthInfo.dwMonthScore == 0)
			{
				//��һ���Ϊ 0  ���ǽ��д���
			
				CG_Cmd_UserMonthEnd msgEnd;
				SetMsgInfo(msgEnd, GetMsgType(Main_Month, CG_UserMonthEnd), sizeof(CG_Cmd_UserMonthEnd));
				msgEnd.dwUserID = (*Iter)->MonthInfo.dwUserID;
				msgEnd.MonthID = m_MonthID;
				msgEnd.MonthIndex = 0xffffffff;//���һ��
				msgEnd.MonthScores = 0;
				msgEnd.VipScores = 0;
				CenterRole* pRole = g_FishServer.GetRoleManager().QueryCenterUser(msgEnd.dwUserID);
				if (pRole)
					pRole->SendDataToGameServer(&msgEnd);
				continue;
			}
			CG_Cmd_UserMonthEnd msgEnd;
			SetMsgInfo(msgEnd,GetMsgType(Main_Month, CG_UserMonthEnd), sizeof(CG_Cmd_UserMonthEnd));
			msgEnd.dwUserID = (*Iter)->MonthInfo.dwUserID;
			msgEnd.MonthID = m_MonthID;
			msgEnd.MonthIndex = i + 1;//����
			msgEnd.MonthScores = (*Iter)->MonthInfo.dwMonthScore;
			msgEnd.VipScores = static_cast<DWORD>(((*Iter)->VipAddScore - 1.0f) * (*Iter)->MonthInfo.dwMonthScore);
			CenterRole* pRole = g_FishServer.GetRoleManager().QueryCenterUser(msgEnd.dwUserID);
			if (pRole)
				pRole->SendDataToGameServer(&msgEnd);
			else
			{
				//��Ϊ��Ҳ����� ������Ҫ������ ��һ�����ε�ʱ�� ����� 
				//1.��һ�ñȽ��� ��һ�ñ����Ľ��ƽ����� ���ݼ�¼��� DBR����
				DBR_Cmd_ChangeGameDataByMonth msg;
				SetMsgInfo(msg, DBR_ChangeGameDataByMonth, sizeof(DBR_Cmd_ChangeGameDataByMonth));
				msg.dwUserID = msgEnd.dwUserID;
				msg.MonthIndex = ConvertIntToBYTE(i + 1);
				g_FishServer.SendNetCmdToDB(&msg);
			}

			if (i <= 2)
			{
				//ǰ����������ʾ����
				TCHAR tempMsg[128] = { 0 };
				_sntprintf_s(tempMsg, CountArray(tempMsg), TEXT("��ϲ%s��ñ����ĵ�%u��"), (*Iter)->NickName, i + 1);
				g_FishServer.SendMessageByType(tempMsg, _tcslen(tempMsg), MT_Center, 4294967295, 1, 3,0);
			}

			//��ָ����ҷ���ϵͳ�ʼ� �ҵ�������ζ�Ӧ�Ľ���
			HashMap<WORD, tagMonthReward>::iterator IterReward = IterConfig->second.m_MonthReward.find(ConvertDWORDToWORD(i + 1));
			if (IterReward == IterConfig->second.m_MonthReward.end())
			{
				continue;
			}
			//�����ʼ����� ϵͳ�ʼ�ֱ��Я��RewardID ���д���
			tagRoleMail	MailInfo;
			MailInfo.bIsRead = false;
			//������������Ҫ����Ĵ��� ������Ҫһ�� �����ת���ַ��� �ͻ��� �� ������ͨ�õ� 
			_sntprintf_s(MailInfo.Context, CountArray(MailInfo.Context), TEXT("��ϲ�����{MonthName:MonthID=%d}������� �ĵ�%d�� �õ����½���"), m_MonthID, i + 1);
			MailInfo.RewardID = IterReward->second.RewardID;
			MailInfo.RewardSum = 1;
			MailInfo.MailID = 0;
			MailInfo.SendTimeLog = time(NULL);
			MailInfo.SrcFaceID = 0;
			TCHARCopy(MailInfo.SrcNickName, CountArray(MailInfo.SrcNickName), TEXT(""), 0);
			MailInfo.SrcUserID = 0;//ϵͳ����
			MailInfo.bIsExistsReward = (MailInfo.RewardID != 0 && MailInfo.RewardSum!=0);
			DBR_Cmd_AddUserMail msg;
			SetMsgInfo(msg,DBR_AddUserMail, sizeof(DBR_Cmd_AddUserMail));
			msg.dwDestUserID = (*Iter)->MonthInfo.dwUserID;
			msg.MailInfo = MailInfo;
			g_FishServer.SendNetCmdToDB(&msg);	

			++i;
		}
		m_LastMonthInfo.OnDestroy();
	}
}
void FishMonth::SignUp(DWORD dwUserID)
{
	//1.�ж��Ƿ�Ϊ����ʱ��
	if (!m_IsInSignUp)
	{
		CG_Cmd_SignUpMonth msg;
		SetMsgInfo(msg,GetMsgType(Main_Month, CG_SignUpMonth), sizeof(CG_Cmd_SignUpMonth));
		msg.dwUserID = dwUserID;
		msg.MonthID = m_MonthID;
		msg.Result = false;
		CenterRole* pRole = g_FishServer.GetRoleManager().QueryCenterUser(dwUserID);
		if (pRole)
			pRole->SendDataToGameServer(&msg);
		else
		{
			ASSERT(false);
		}
		return;
	}
	//�ж�����Ƿ��Ѿ�������
	HashMap<DWORD, tagRoleMonthCenterInfo*>::iterator IterFind = m_MonthRoleMap.find(dwUserID);
	if (IterFind != m_MonthRoleMap.end() && IterFind->second)
	{
		CG_Cmd_SignUpMonth msg; 
		SetMsgInfo(msg,GetMsgType(Main_Month, CG_SignUpMonth), sizeof(CG_Cmd_SignUpMonth));
		msg.dwUserID = dwUserID;
		msg.MonthID = m_MonthID;
		msg.Result = false;
		CenterRole* pRole = g_FishServer.GetRoleManager().QueryCenterUser(dwUserID);
		if (pRole)
			pRole->SendDataToGameServer(&msg);
		else
		{
			ASSERT(false);
		}
		return;
	}
	CG_Cmd_SignUpMonth msg;  //��������ͻ��� ȥѯ������Ƿ�������㹻����Ʊ�������
	SetMsgInfo(msg,GetMsgType(Main_Month, CG_SignUpMonth), sizeof(CG_Cmd_SignUpMonth));
	msg.dwUserID = dwUserID;
	msg.MonthID = m_MonthID;
	msg.Result = true;
	CenterRole* pRole = g_FishServer.GetRoleManager().QueryCenterUser(dwUserID);
	if (pRole)
		pRole->SendDataToGameServer(&msg);
	else
	{
		ASSERT(false);
	}
}
void FishMonth::SignUpResult(GC_Cmd_SignUpMonthResult* pMsg)
{
	//GameServer�Ѿ��۳���ҵı���������
	HashMap<BYTE, tagMonthConfig>::iterator IterConfig = g_FishServer.GetFishConfig().GetMonthConfig().m_MonthMap.find(m_MonthID);
	if (IterConfig == g_FishServer.GetFishConfig().GetMonthConfig().m_MonthMap.end())
	{
		ASSERT(false);
		return;
	}
	if (!pMsg || !m_IsInSignUp || m_MonthID != pMsg->MonthID)
	{
		ASSERT(false);
		return;
	}
	CenterRole* pUser = g_FishServer.GetRoleManager().QueryCenterUser(pMsg->dwUserID);
	if (!pUser)
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, tagRoleMonthCenterInfo*>::iterator IterFind = m_MonthRoleMap.find(pMsg->dwUserID);
	if (IterFind != m_MonthRoleMap.end())
	{
		//ASSERT(false);
		return;
	}
	//��ҿ��Խ��б����� 
	tagRoleMonthCenterInfo* pRole = new tagRoleMonthCenterInfo;
	if (!pRole)
	{
		ASSERT(false);
		return;
	}
	pRole->MonthInfo.bMonthID = m_MonthID;
	pRole->MonthInfo.dwIndex = m_RoleVec.size();
	pRole->MonthInfo.dwMonthGlobel = IterConfig->second.InitMonthGlobel;//������ʱ��ͳ�ʼ���û���
	pRole->MonthInfo.dwMonthScore = 0;
	pRole->MonthInfo.dwUserID = pMsg->dwUserID;
	pRole->MonthInfo.wUserAddGlobelNum = 0;
	TCHARCopy(pRole->NickName, CountArray(pRole->NickName), pUser->GetRoleInfo().NickName, _tcslen(pUser->GetRoleInfo().NickName));//����������ڼ�¼xml�ļ�ʹ�õ�
	pRole->FaceID = pUser->GetRoleInfo().dwFaceID;//ͷ��ID
	pRole->VipAddScore = g_FishServer.GetFishConfig().GetVipConfig().AddMonthScoreRate(pUser->GetRoleInfo().VipLevel);//�������ֵ�����
	pRole->MonthInfo.SkillSum = 0;
	m_RoleVec.push_back(pRole);
	m_MonthRoleMap.insert(std::map<DWORD, tagRoleMonthCenterInfo*>::value_type(pRole->MonthInfo.dwUserID, pRole));

	CG_Cmd_SignUpMonthSucess msg; //����GameServer ��ұ��������ɹ���
	SetMsgInfo(msg,GetMsgType(Main_Month, CG_SignUpMonthSucess), sizeof(CG_Cmd_SignUpMonthSucess));
	msg.dwUserID = pMsg->dwUserID;
	msg.MonthID = pRole->MonthInfo.bMonthID;
	pUser->SendDataToGameServer(&msg);

	OnRoleSumChange();
}
void FishMonth::ResetUserMonthInfo(DWORD dwUserID)
{
	HashMap<BYTE, tagMonthConfig>::iterator IterConfig = g_FishServer.GetFishConfig().GetMonthConfig().m_MonthMap.find(m_MonthID);
	if (IterConfig == g_FishServer.GetFishConfig().GetMonthConfig().m_MonthMap.end())
	{
		ASSERT(false);
		return;
	}
	CenterRole* pUser = g_FishServer.GetRoleManager().QueryCenterUser(dwUserID);
	if (!pUser)
	{
		ASSERT(false);
		return;
	}

	CG_Cmd_ResetMonthInfo msg;
	SetMsgInfo(msg, GetMsgType(Main_Month, CG_ResetMonthInfo), sizeof(CG_Cmd_ResetMonthInfo));
	msg.dwUserID = dwUserID;
	msg.MonthID = m_MonthID;
	msg.Result = false;
	//����������Ѿ����� ���� �Ѿ���ʼ������
	if (!m_IsStart)
	{
		ASSERT(false);
		pUser->SendDataToGameServer(&msg);
		return;
	}
	HashMap<DWORD, tagRoleMonthCenterInfo*>::iterator Iter = m_MonthRoleMap.find(dwUserID);
	if (Iter == m_MonthRoleMap.end() || !Iter->second)
	{
		ASSERT(false);
		pUser->SendDataToGameServer(&msg);
		return;
	}
	Iter->second->MonthInfo.dwMonthScore = 0;
	Iter->second->MonthInfo.dwMonthGlobel = IterConfig->second.InitMonthGlobel;
	Iter->second->MonthInfo.wUserAddGlobelNum = 0;

	msg.Result = true;
	pUser->SendDataToGameServer(&msg);
}
bool FishMonth::IsCanJoinMonth(DWORD dwUserID, tagRoleMonthInfo& MonthInfo)
{
	if (!m_IsStart)
		return false;
	HashMap<DWORD, tagRoleMonthCenterInfo*>::iterator Iter = m_MonthRoleMap.find(dwUserID);
	if (Iter == m_MonthRoleMap.end() || !Iter->second)//δ���� �޷�����
		return false;
	MonthInfo = Iter->second->MonthInfo;
	return true;
}
void FishMonth::JoinMonth(DWORD dwUserID,WORD TableID)
{
	HashMap<BYTE, tagMonthConfig>::iterator IterConfig = g_FishServer.GetFishConfig().GetMonthConfig().m_MonthMap.find(m_MonthID);
	if (IterConfig == g_FishServer.GetFishConfig().GetMonthConfig().m_MonthMap.end())
	{
		ASSERT(false);
		return;
	}
	//�����Ҫ�������
	CenterRole* pUser = g_FishServer.GetRoleManager().QueryCenterUser(dwUserID);
	if (!pUser)
	{
		ASSERT(false);
		return;
	}
	if (pUser->IsRobot())
	{
		//���Ϊ������ ���뱨�� ֱ�ӾͿ��Խ������ 
		HashMap<DWORD, tagRoleMonthCenterInfo*>::iterator IterFind = m_MonthRoleMap.find(pUser->GetRoleID());
		if (IterFind == m_MonthRoleMap.end())
		{
			tagRoleMonthCenterInfo* pRole = new tagRoleMonthCenterInfo;
			if (!pRole)
			{
				ASSERT(false);
				return;
			}
			pRole->MonthInfo.bMonthID = m_MonthID;
			pRole->MonthInfo.dwIndex = m_RoleVec.size();
			pRole->MonthInfo.dwMonthGlobel = IterConfig->second.InitMonthGlobel;//������ʱ��ͳ�ʼ���û���
			pRole->MonthInfo.dwMonthScore = 0;
			pRole->MonthInfo.dwUserID = dwUserID;
			pRole->MonthInfo.wUserAddGlobelNum = 0;
			TCHARCopy(pRole->NickName, CountArray(pRole->NickName), pUser->GetRoleInfo().NickName, _tcslen(pUser->GetRoleInfo().NickName));//����������ڼ�¼xml�ļ�ʹ�õ�
			pRole->FaceID = pUser->GetRoleInfo().dwFaceID;//ͷ��ID
			pRole->VipAddScore = g_FishServer.GetFishConfig().GetVipConfig().AddMonthScoreRate(pUser->GetRoleInfo().VipLevel);
			pRole->MonthInfo.SkillSum = 0;
			m_RoleVec.push_back(pRole);
			m_MonthRoleMap.insert(std::map<DWORD, tagRoleMonthCenterInfo*>::value_type(pRole->MonthInfo.dwUserID, pRole));
			//���ݲ������ ���ǿ�������ҽ��������
			OnRoleSumChange();//���������仯��
			
			CG_Cmd_JoinMonth msg;
			SetMsgInfo(msg, GetMsgType(Main_Month, CG_JoinMonth), sizeof(CG_Cmd_JoinMonth));
			msg.Result = true;
			msg.MonthInfo = pRole->MonthInfo;
			msg.TableID = TableID;
			pUser->SendDataToGameServer(&msg);
		}
		else
		{
			CG_Cmd_JoinMonth msg;
			SetMsgInfo(msg, GetMsgType(Main_Month, CG_JoinMonth), sizeof(CG_Cmd_JoinMonth));
			msg.Result = true;
			msg.MonthInfo = IterFind->second->MonthInfo;
			msg.TableID = TableID;
			pUser->SendDataToGameServer(&msg);
		}
		return;
	}
	if (!m_IsStart)
	{
		CG_Cmd_JoinMonth msg; 
		SetMsgInfo(msg,GetMsgType(Main_Month, CG_JoinMonth), sizeof(CG_Cmd_JoinMonth));
		msg.Result = false;
		msg.MonthInfo.dwUserID = dwUserID;
		msg.TableID = TableID;
		pUser->SendDataToGameServer(&msg);
		return;
	}
	HashMap<DWORD, tagRoleMonthCenterInfo*>::iterator Iter = m_MonthRoleMap.find(dwUserID);
	if (Iter == m_MonthRoleMap.end() || !Iter->second)//δ���� �޷�����
	{
		CG_Cmd_JoinMonth msg; 
		SetMsgInfo(msg,GetMsgType(Main_Month, CG_JoinMonth), sizeof(CG_Cmd_JoinMonth));
		msg.Result = false;
		msg.MonthInfo.dwUserID = dwUserID;
		msg.TableID = TableID;
		pUser->SendDataToGameServer(&msg);
		return;
	}
	//�Ѿ������ɹ��� ���ҵ�ǰ�Ѿ����ڱ�����
	CG_Cmd_JoinMonth msg; 
	SetMsgInfo(msg,GetMsgType(Main_Month, CG_JoinMonth), sizeof(CG_Cmd_JoinMonth));
	msg.Result = true;
	msg.MonthInfo = Iter->second->MonthInfo;
	msg.TableID = TableID;
	pUser->SendDataToGameServer(&msg);
}
void FishMonth::UpdateMonthIndex()
{
	//��vector��������
	if (m_RoleVec.empty() || !m_IsStart)//ֻ���ڱ����� ������������0�Ž��и���
		return;
	sort(m_RoleVec.begin(), m_RoleVec.end(), SortByMonthSocre);
	//������Ϻ���� �Ӵ�С
	DWORD UpperMonthSocre = 0;
	std::vector<tagRoleMonthCenterInfo*>::iterator Iter = m_RoleVec.begin();
	for (size_t i = 0; Iter != m_RoleVec.end(); ++Iter,++i)//����ȫ������� ����ʹ��vector  ��֤ ���������� HashMap��Ϊ����֤˳�� 
	{
		if (!(*Iter))
			continue;
		if ((*Iter)->MonthInfo.dwIndex != i)
		{
			//��ҵ������ı��� �����޸���ҵ�����
			(*Iter)->MonthInfo.dwIndex = i;
			CG_Cmd_UserChangeIndex msgSrc; 
			SetMsgInfo(msgSrc,GetMsgType(Main_Month, CG_UserChangeIndex), sizeof(CG_Cmd_UserChangeIndex));
			msgSrc.dwUserID = (*Iter)->MonthInfo.dwUserID;
			msgSrc.dwIndex = i;
			msgSrc.dwUpperSocre = UpperMonthSocre;
			CenterRole* pRole = g_FishServer.GetRoleManager().QueryCenterUser(msgSrc.dwUserID);
			if (pRole)
				pRole->SendDataToGameServer(&msgSrc);
		}
		UpperMonthSocre = (*Iter)->MonthInfo.dwMonthScore;
	}

	//������Ϻ� ��������һ���б��͵�GameServerȥ ���� ת�����ͻ���ȥ
	CG_Cmd_MonthList msg;
	SetMsgInfo(msg, GetMsgType(Main_Month, CG_MonthList), sizeof(CG_Cmd_MonthList));
	msg.MonthID = GetMonthID();
	ZeroMemory(&msg.MonthInfo, sizeof(tagMonthRoleInfo)*MAX_MonthList_Length);
	for (BYTE i = 0; i < MAX_MonthList_Length && i<m_RoleVec.size(); ++i)
	{
		msg.MonthInfo[i].dwUserID = m_RoleVec[i]->MonthInfo.dwUserID;
		msg.MonthInfo[i].FaceID = m_RoleVec[i]->FaceID;
		msg.MonthInfo[i].Score = m_RoleVec[i]->MonthInfo.dwMonthScore;
		msg.MonthInfo[i].VipScore = static_cast<DWORD>(msg.MonthInfo[i].Score * m_RoleVec[i]->VipAddScore - msg.MonthInfo[i].Score);
		msg.MonthInfo[i].Index = m_RoleVec[i]->MonthInfo.dwIndex;
		TCHARCopy(msg.MonthInfo[i].NickName, CountArray(msg.MonthInfo[i].NickName), m_RoleVec[i]->NickName, _tcslen(m_RoleVec[i]->NickName));//����������ڼ�¼xml�ļ�ʹ�õ�
	}
	g_FishServer.GetCenterManager().SendNetCmdToAllGameServer(&msg);//������͵�ȫ����GameServerȥ
}
void FishMonth::OnRoleSumChange()
{
	HashMap<BYTE, tagMonthConfig>::iterator IterConfig = g_FishServer.GetFishConfig().GetMonthConfig().m_MonthMap.find(m_MonthID);
	if (IterConfig == g_FishServer.GetFishConfig().GetMonthConfig().m_MonthMap.end())
	{
		ASSERT(false);
		return;
	}
	CG_Cmd_SendMonthRoleSum msg;
	SetMsgInfo(msg, GetMsgType(Main_Month, CG_SendMonthRoleSum), sizeof(CG_Cmd_SendMonthRoleSum));
	msg.Sum = 1;
	msg.Array[0].MonthID = IterConfig->second.MonthID;
	msg.Array[0].MonthUserSum = GetSigupSum();
	msg.States = MsgEnd;
	g_FishServer.GetCenterManager().SendNetCmdToAllGameServer(&msg);
}
//----------------------------------------------------------------------------------------------------------------------Manager
FishMonthManager::FishMonthManager()
{
	m_FishMonthMap.clear();
	m_RobotIsFinish = false;
}
FishMonthManager::~FishMonthManager()
{
	if (!m_FishMonthMap.empty())
	{
		HashMap<BYTE, FishMonth*>::iterator Iter = m_FishMonthMap.begin();
		for (; Iter != m_FishMonthMap.end(); ++Iter)
		{
			delete Iter->second;
		}
		m_FishMonthMap.clear();
	}
}
bool FishMonthManager::OnInit()
{
	//�����еı�����������
	tagMonthMap& pMap = g_FishServer.GetFishConfig().GetMonthConfig();
	HashMap<BYTE, tagMonthConfig>::iterator Iter = pMap.m_MonthMap.begin();
	for (; Iter != pMap.m_MonthMap.end(); ++Iter)
	{
		FishMonth* pMonth = new FishMonth();
		if (!pMonth)
		{
			ASSERT(false);
			continue;
		}
		if (!pMonth->OnInit(Iter->first))//���������úõı�����������
		{
			delete pMonth;
			ASSERT(false);
			return false;
		}
		m_FishMonthMap.insert(HashMap<BYTE, FishMonth*>::value_type(pMonth->GetMonthID(), pMonth));
	}
	//OnSendMonthDataToGame(0);//��ʼ����ʱ��ͬ����������
	return true;
}
void FishMonthManager::OnLoadRobotFinish()
{
	m_RobotIsFinish = true;
	HashMap<BYTE, FishMonth*>::iterator Iter = m_FishMonthMap.begin();
	for (; Iter != m_FishMonthMap.end(); ++Iter)
	{
		Iter->second->OnLoadRobotFinish();
	}
}
void FishMonthManager::Update(DWORD dwTimer)
{
	UpdateByMin(dwTimer);//һ���Ӹ���һ�� ���ڿ��Ʊ����Ŀ��� �ͽ���
	UpdateRobot(dwTimer);//30s ����һ�� ���»����˵�����
	UpdateMonthIndex(dwTimer);//10S ����һ�� ���ڿ������б�������ҵĻ�������
	//UpdateMonthRoleSunToGameServer(dwTimer);//���μӱ������������͵�GameServerȥ 10Sһ��
}
void FishMonthManager::UpdateByMin(DWORD dwTimer)//����ֻ�Ƿ��� ������ÿһ���ӵ�0���ʱ��(0-59) 
{
	SYSTEMTIME time;
	GetLocalTime(&time);
	static BYTE LogUpdateMin = 0xff;//�ϴθ��¼�¼�ķ�����
	if (time.wSecond == 0 && (LogUpdateMin == 0xff || time.wMinute != LogUpdateMin))
	{
		LogUpdateMin = (BYTE)time.wMinute;//��¼�ϴθ���
		if (m_FishMonthMap.empty())
			return;
		HashMap<BYTE, FishMonth*>::iterator Iter = m_FishMonthMap.begin();
		for (; Iter != m_FishMonthMap.end(); ++Iter)
		{
			Iter->second->UpdateByMin();
		}
	}
	else if (LogUpdateMin == 0xff)
	{
		//Ϊ0xff ��ʱ��  �ǳ�ʼ������ ֱ�Ӵ���
		LogUpdateMin = (BYTE)time.wMinute;//��¼�ϴθ���
		if (m_FishMonthMap.empty())
			return;
		HashMap<BYTE, FishMonth*>::iterator Iter = m_FishMonthMap.begin();
		for (; Iter != m_FishMonthMap.end(); ++Iter)
		{
			Iter->second->UpdateByMin();
		}
	}
}
void FishMonthManager::UpdateMonthIndex(DWORD dwTimer)//���¸�������������
{
	static DWORD MonthUpdateIndex = 0;
	if (dwTimer - MonthUpdateIndex < 30000)
		return;
	MonthUpdateIndex = dwTimer;
	if (m_FishMonthMap.empty())
		return;
	HashMap<BYTE, FishMonth*>::iterator Iter = m_FishMonthMap.begin();
	for (; Iter != m_FishMonthMap.end(); ++Iter)
	{
		if (!Iter->second)
			continue;
		Iter->second->UpdateMonthIndex();
	}
}
void FishMonthManager::UpdateRobot(DWORD dwTimer)
{
	static DWORD MonthUpdateRobot = 0;
	if (dwTimer - MonthUpdateRobot < 30000)
		return;
	MonthUpdateRobot = dwTimer;
	if (m_FishMonthMap.empty())
		return;
	HashMap<BYTE, FishMonth*>::iterator Iter = m_FishMonthMap.begin();
	for (; Iter != m_FishMonthMap.end(); ++Iter)
	{
		if (!Iter->second)
			continue;
		Iter->second->OnUpdateRobotInfo();
	}
}
FishMonth* FishMonthManager::QueryMonth(BYTE MonthID)
{
	HashMap<BYTE, FishMonth*>::iterator Iter = m_FishMonthMap.find(MonthID);
	if (Iter == m_FishMonthMap.end() || !Iter->second)
		return NULL;
	else
		return Iter->second;
}
void FishMonthManager::SendRoleSignUpInfoToGameServer(DWORD dwUserID)
{
	CenterRole* pRole = g_FishServer.GetRoleManager().QueryCenterUser(dwUserID);
	if (!pRole)
	{
		ASSERT(false);
		return;
	}
	DWORD PageSize = sizeof(CG_Cmd_LoadRoleSignUp)+(m_FishMonthMap.size() - 1)*sizeof(BYTE);
	CG_Cmd_LoadRoleSignUp* msg = (CG_Cmd_LoadRoleSignUp*)malloc(PageSize);
	if (!msg)
	{
		ASSERT(false);
		return;
	}
	msg->dwUserID = dwUserID;
	msg->SetCmdType(GetMsgType(Main_Month, CG_LoadRoleSignUp));
	HashMap<BYTE, FishMonth*>::iterator Iter = m_FishMonthMap.begin();
	WORD SignUpMonthSum = 0;
	for (; Iter != m_FishMonthMap.end(); ++Iter)
	{
		if (!Iter->second)
			continue;
		if (Iter->second->RoleIsInMonth(dwUserID))
		{
			msg->Array[SignUpMonthSum] = Iter->second->GetMonthID();
			++SignUpMonthSum;
		}
	}
	PageSize = sizeof(CG_Cmd_LoadRoleSignUp)+(SignUpMonthSum - 1)*sizeof(BYTE);

	std::vector<CG_Cmd_LoadRoleSignUp*> pVec;
	SqlitMsg(msg, PageSize, SignUpMonthSum,false, pVec);
	free(msg);
	if (!pVec.empty())
	{
		std::vector<CG_Cmd_LoadRoleSignUp*>::iterator Iter = pVec.begin();
		for (; Iter != pVec.end(); ++Iter)
		{
			pRole->SendDataToGameServer(*Iter);
			free(*Iter);
		}
		pVec.clear();
	}
}
void FishMonthManager::ResetUserMonthInfo(DWORD dwUserID, BYTE MonthID)
{
	//����ָ���������� 
	FishMonth* pMonth = QueryMonth(MonthID);
	if (!pMonth)
	{
		ASSERT(false);
		return;
	}
	pMonth->ResetUserMonthInfo(dwUserID);
}
void FishMonthManager::OnGameRsg(BYTE GameID)
{
	HashMap<BYTE, FishMonth*>::iterator Iter = m_FishMonthMap.begin();
	for (; Iter != m_FishMonthMap.end(); ++Iter)
	{
		BYTE States = 0;
		if (Iter->second->IsInSignUp())
			States |= 1;
		else
			States |= 2;

		if (Iter->second->IsInStart())
			States |= 4;
		else
			States |= 8;

		Iter->second->SendMonthStatesToGameServer(GameID, States);
	}
}
void FishMonthManager::OnSendMonthDataToGame(BYTE GameID)
{
	//������������ͬ����GameServerȥ
	//GameSocketID ==0 ��ʱ�� ��ʾ��ȫ����GameServer ����ָ���ǵ�����һ��GameServerΪ
	//0.�����������͸�GameServer �����ڸ��µ�ʱ�� ���и��� 
	//1.��֯����
	DWORD PageSize = sizeof(CG_Cmd_SendMonthRoleSum)+sizeof(tagMonthRoleSum)* (m_FishMonthMap.size() - 1);
	CG_Cmd_SendMonthRoleSum * msg = (CG_Cmd_SendMonthRoleSum*)malloc(PageSize);
	if (!msg)
	{
		ASSERT(false);
		return;
	}
	msg->SetCmdType(GetMsgType(Main_Month, CG_SendMonthRoleSum));
	HashMap<BYTE, FishMonth*>::iterator Iter = m_FishMonthMap.begin();
	for (WORD i = 0; Iter != m_FishMonthMap.end(); ++Iter, ++i)
	{
		if (!Iter->second)
			continue;
		msg->Array[i].MonthID = Iter->first;
		msg->Array[i].MonthUserSum = Iter->second->GetSigupSum();//���������� ��ǰ�׶ε�����
	}

	std::vector<CG_Cmd_SendMonthRoleSum*> pVec;
	SqlitMsg(msg, PageSize, m_FishMonthMap.size(),false, pVec);
	free(msg);
	if (!pVec.empty())
	{
		std::vector<CG_Cmd_SendMonthRoleSum*>::iterator Iter = pVec.begin();
		for (; Iter != pVec.end(); ++Iter)
		{
			if (GameID == 0)
			{
				g_FishServer.GetCenterManager().SendNetCmdToAllGameServer(*Iter);
			}
			else
			{
				g_FishServer.GetCenterManager().SendNetCmdToGameServer(GameID, *Iter);
			}
			free(*Iter);
		}
		pVec.clear();
	}
}
//void FishMonthManager::OnLogonToMonth(ServerClientData* pClient,DWORD dwUserID, BYTE MonthID)
//{
//	FishMonth* pMonth = QueryMonth(MonthID);
//	tagRoleMonthInfo MonthInfo;
//	if (!pMonth || !pMonth->IsCanJoinMonth(dwUserID, MonthInfo))
//	{
//		ASSERT(false);
//		CG_Cmd_LogonToMonthTable msg;
//		SetMsgInfo(msg, GetMsgType(Main_Month, CG_LogonToMonthTable), sizeof(CG_Cmd_LogonToMonthTable));
//		msg.dwUserID = dwUserID;
//		msg.MonthID = MonthID;
//		msg.Result = false;
//		g_FishServer.SendNetCmdToClient(pClient, &msg);
//		return;
//	}
//	//��ҿ��Լ������
//	CG_Cmd_LogonToMonthTable msg;
//	SetMsgInfo(msg, GetMsgType(Main_Month, CG_LogonToMonthTable), sizeof(CG_Cmd_LogonToMonthTable));
//	msg.dwUserID = dwUserID;
//	msg.MonthID = MonthID;
//	msg.Result = true;
//	msg.MonthInfo = MonthInfo;
//	g_FishServer.SendNetCmdToClient(pClient, &msg);
//	return;
//}


bool LastMonthRewardInfo::OnAddGameServerFinish()
{
	if (Lastm_RoleVec.empty())
	{
		ASSERT(false);
		return false;
	}
	LoadSum++;
	if (LoadSum == g_FishServer.GetCenterManager().GetGameServerSum())
		return true;
	else
		return false;
}
