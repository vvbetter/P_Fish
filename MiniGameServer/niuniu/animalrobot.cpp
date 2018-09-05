#include "Stdafx.h"
#include"animalrobot.h"
#include "math.h"
#include "..\FishServer.h"
//#include"NiuNiuManager.h"
//#include"..\DialManager.h"
#include"..\RoleManager.h"

//////////////////////////////////////////////////////////////////////////
//�����ע����
#define MAX_CHIP_TIME			  50
//ʱ���ʶ
#define IDI_BANK_OPERATE			3									//���ж�ʱ
#define IDI_CHECK_BANKER			108									//�����ׯ
#define IDI_REQUEST_BANKER			101									//���붨ʱ
#define IDI_GIVEUP_BANKER			102									//��ׯ��ʱ
#define IDI_PLACE_JETTON			110									//��ע���� (Ԥ��110-160)

DWORD CAnimalRobot::m_dwApplistChangeTime;
BYTE	CAnimalRobot::m_byAppCountLimit;

DWORD	CAnimalRobot::m_dwRobotCountChangeTime;
BYTE	CAnimalRobot::m_byRobotCount;

//////////////////////////////////////////////////////////////////////////
//���캯��
CAnimalRobot::CAnimalRobot(DWORD dwRobotid)
{	
	//��Ϸ����
	m_bAction = rand() % 100 < GameConfig().RobotActionRate;
	m_bBnkerRobot = rand() % 100 < GameConfig().RobotBankerRate;
	m_lMaxChipBanker = 0;
	m_lMaxChipUser = 0;
	m_nChipTime = 0;
	m_nChipTimeCount = 0;	
	ZeroMemory(m_lAreaChip, sizeof(m_lAreaChip));
	ZeroMemory(m_nChipLimit, sizeof(m_nChipLimit));

	//��ׯ����
	//m_bMeApplyBanker = false;
	m_nWaitBanker = 0;
	m_nRobotWaitBanker = ClipNum(GameConfig().RobotWaitBankerMin, GameConfig().RobotWaitBankerMax);
	m_nBankerCount = 0;
	m_dwRobotid = dwRobotid;
	m_nGameTime = ClipNum(GameConfig().RobotMinGameTime, GameConfig().RobotMaxGameTime) * 60;	
	m_nRobotBankerCount = ClipNum(GameConfig().RobotBankerCountMin, GameConfig().RobotBankerCountMax);
	return;
}

//��������
CAnimalRobot::~CAnimalRobot()
{
}

//ʱ����Ϣ
bool  CAnimalRobot::OnEventTimer(UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_CHECK_BANKER:		//�����ׯ
	{
									

									if (BankerId() == UserId())
										return false;

									if (ApplyListLength()<CAnimalRobot::m_byAppCountLimit&&!g_FishServer.GetDialManager().HaveApply(UserId()))//BankerId() == 0 ||
									{
										//��ׯ
										m_nWaitBanker++;

										//MyDebug(_T("��������ׯ(End) %d [%d %d] [%d %d]"), m_pIAndroidUserItem->GetChairID(), m_nWaitBanker, 
										//	m_nRobotWaitBanker, m_stlApplyBanker, m_nRobotApplyBanker);

										//��������ׯ
										if (m_bBnkerRobot&& m_nWaitBanker > m_nRobotWaitBanker)
										{
											//�Ϸ��ж�											
											if (UserGold() > GameConfig().JoinBankerGlobelSum)
											{
												//��������ׯ
												m_nBankerCount = 0;//������
												m_nWaitBanker = 0;
												g_FishServer.GetDialManager().OnRoleJoinWriteBankerList(UserId());												
											}
										}
									}
									//else if (BankerId() != 0)
									//{
									//	//��������ׯ
									//	
									//}

									return false;
	}
	case IDI_GIVEUP_BANKER:		//������ׯ
	{									
									g_FishServer.GetDialManager().OnRoleCanelBanker(UserId());								
									return false;
	}
	case IDI_BANK_OPERATE:		//���в���
	{
									if (m_bBnkerRobot)
									{
										if (UserGold() < GameConfig().RobotBankerScoreMin || UserGold() < GameConfig().RobotBankerScoreMax)
										{
											AdjustGold(ClipNum(GameConfig().RobotBankerScoreMin, GameConfig().RobotBankerScoreMax));																			 											
										}
									}
									else
									{
										if (UserGold() < GameConfig().RobotPlayerScoreMin || UserGold() < GameConfig().RobotPlayerScoreMax)
										{																															
											AdjustGold(ClipNum(GameConfig().RobotPlayerScoreMin, GameConfig().RobotPlayerScoreMax));
										}
									}																
									return false;
	}
	default:
	{
			   if (nTimerID >= IDI_PLACE_JETTON && nTimerID <= IDI_PLACE_JETTON + MAX_CHIP_TIME)
			   {				  				 
				   //��������
				   int nRandNum = 0, nChipArea = 0, nCurChip = 0, nACTotal = 0, nCurJetLmt[2] = {};
				   LONGLONG lMaxChipLmt = __min(m_lMaxChipBanker, m_lMaxChipUser);			//������עֵ				   
				   for (int i = 0; i <MAX_DIAL_ClientSum; i++)
					   nACTotal += m_RobotInfo.nAreaChance[i];

				   //ͳ�ƴ���
				   m_nChipTimeCount++;

				   //����˳�
				   if (lMaxChipLmt < m_RobotInfo.nChip[m_nChipLimit[0]])	return false;
				   for (int i = 0; i < MAX_DIAL_ClientSum; i++)
				   {
					   if (m_lAreaChip[i] >= m_RobotInfo.nChip[m_nChipLimit[0]])	break;
					   if (i == MAX_DIAL_ClientSum - 1)	return false;
				   }

				   //��ע����
				   ASSERT(nACTotal>0);
				   static int nStFluc = 1;				//�������
				   if (nACTotal <= 0)	return false;
				   do {
					   nRandNum = (rand()  + nStFluc * 3) % nACTotal;
					   for (int i = 0; i < MAX_DIAL_ClientSum; i++)
					   {
						   nRandNum -= m_RobotInfo.nAreaChance[i];
						   if (nRandNum < 0)
						   {
							   nChipArea = i;
							   break;
						   }
					   }
				   } while (m_lAreaChip[nChipArea] < m_RobotInfo.nChip[m_nChipLimit[0]]);
				   nStFluc = nStFluc % 3 + 1;

				   //��ע��С
				   if (m_nChipLimit[0] == m_nChipLimit[1])
					   nCurChip = m_nChipLimit[0];
				   else
				   {
					   //���ñ���
					   lMaxChipLmt = __min(lMaxChipLmt, m_lAreaChip[nChipArea]);
					   nCurJetLmt[0] = m_nChipLimit[0];
					   nCurJetLmt[1] = m_nChipLimit[0];

					   //���㵱ǰ������
					   for (int i = m_nChipLimit[1]; i > m_nChipLimit[0]; i--)
					   {
						   if (lMaxChipLmt > m_RobotInfo.nChip[i])
						   {
							   nCurJetLmt[1] = i;
							   break;
						   }
					   }

					   //�����ע
					   nRandNum = (rand()) % (nCurJetLmt[1] - nCurJetLmt[0] + 1);
					   nCurChip = nCurJetLmt[0] + nRandNum;

					   //���¿��� (��ׯ�ҽ�ҽ���ʱ�ᾡ����֤�������)
					   if (m_nChipTimeCount < m_nChipTime)
					   {
						   LONGLONG lLeftJetton = LONGLONG((lMaxChipLmt - m_RobotInfo.nChip[nCurChip]) / (m_nChipTime - m_nChipTimeCount));

						   //�������� (��ȫ����С���Ƴ�����עҲ����)
						   if (lLeftJetton < m_RobotInfo.nChip[m_nChipLimit[0]] && nCurChip > m_nChipLimit[0])
							   nCurChip--;
					   }
				   }
				   int nJetton = m_RobotInfo.nChip[nCurChip];
				   g_FishServer.GetDialManager().OnRoleBetGlobel(UserId(), nChipArea, nJetton);

				   //���ñ���
				   m_lMaxChipBanker -= nJetton;
				   m_lAreaChip[nChipArea] -= nJetton;
				   if (BankerId() == UserId())
				   {
					   m_lMaxChipUser -= nJetton;
				   }					   

				   /*ASSERT( MyDebug(_T("��������ע %d ��ע���� [%d/%d] ��ע [%d %d] ��Χ [%d %d] ���� [%I64d %I64d %I64d]"), wMyID, nTimerID-IDI_PLACE_JETTON, m_nChipTime,
				   nChipArea, m_RobotInfo.nChip[nCurChip], m_nChipLimit[0], m_nChipLimit[1], m_lMaxChipBanker, m_lMaxChipUser, lMaxChipLmt) );*/

				   //��������
				   //CMD_C_PlaceJetton PlaceJetton = {};

				   ////�������
				   //PlaceJetton.cbJettonArea = nChipArea + 1;		//������1��ʼ
				   //PlaceJetton.lJettonScore = m_RobotInfo.nChip[nCurChip];

				   ////������Ϣ
				   //m_pIAndroidUserItem->SendSocketData(SUB_C_PLACE_JETTON, &PlaceJetton, sizeof(PlaceJetton));
			   }

			   //m_pIAndroidUserItem->KillGameTimer(nTimerID);
			   return false;
	}
	}
	return false;
}


//��Ϸ����
void CAnimalRobot::EndGame()
{
	if (!m_bAction)
	{
		return;
	}
	m_VecOxTimer.push_back(new AnimalTimer(IDI_BANK_OPERATE, (WORD)rand() % (GameConfig().EndWriteSec / 2) + 1));
	bool bMeGiveUp = false;
	if (BankerId() == UserId())
	{
		m_nBankerCount++;
		if (m_nBankerCount >= m_nRobotBankerCount)
		{			
			m_nBankerCount = 0;//��������ׯ
			m_VecOxTimer.push_back(new AnimalTimer(IDI_GIVEUP_BANKER, (WORD)rand() % (GameConfig().EndWriteSec / 2) + 1));
			bMeGiveUp = true;
		}
	}
	//�����ׯ
	if (BankerId() != UserId() || bMeGiveUp)
	{
		
		m_VecOxTimer.push_back(new AnimalTimer(IDI_CHECK_BANKER, (WORD)rand() % (GameConfig().EndWriteSec / 2) + 1));
	}
}
void CAnimalRobot::AdjustGold(DWORD dwGold)
{
	Role *pRole = g_FishServer.GetRoleManager().QueryRole(m_dwRobotid);
	if (pRole)
	{
		pRole->OnChangeRoleGlobelSum(dwGold,true);
	}
}

//��Ϸ��ʼ
void CAnimalRobot::StartGame()
{
	if (!m_bAction)
	{
		return;
	}
	//�Լ���ׯ������ע������
	if (UserId() == BankerId())
		return;

	if (BankerId() == 0)
	{
		m_lMaxChipBanker = GameConfig().MaxAddGlobel;// / GameConfig().MaxRate;
	}
	else
	{
		m_lMaxChipBanker = BankerGold() / GameConfig().MaxRate;
	}
	m_lMaxChipUser = UserGold() / GameConfig().MaxRate;
	m_lMaxChipUser = __min(m_lMaxChipUser, m_lMaxChipBanker);
	//���ñ���
	
	m_nChipTimeCount = 0;
	ZeroMemory(m_nChipLimit, sizeof(m_nChipLimit));
	for (int i = 0; i <MAX_DIAL_ClientSum; i++)
		m_lAreaChip[i] = m_lMaxChipUser;

	//������ע����
	int nElapse = 0;

	if (GameConfig().RobotMinBetTime == GameConfig().RobotMaxBetTime)
		m_nChipTime = GameConfig().RobotMinBetTime;
	else
		m_nChipTime = (rand()) % (GameConfig().RobotMaxBetTime - GameConfig().RobotMinBetTime + 1) + GameConfig().RobotMinBetTime;
	ASSERT(m_nChipTime >= 0);
	if (m_nChipTime <= 0)	return ;								//��ȷ,2����������
	if (m_nChipTime > MAX_CHIP_TIME)	m_nChipTime = MAX_CHIP_TIME;	//�޶�MAX_CHIP����ע

	LONGLONG lRobotJettonLimit[2] = {0};
	lRobotJettonLimit[0] = GameConfig().RobotMinJetton;
	lRobotJettonLimit[1] = GameConfig().RobotMaxJetton;

	//���㷶Χ
	if (!CalcJettonRange(__min(m_lMaxChipBanker, m_lMaxChipUser), lRobotJettonLimit, m_nChipTime, m_nChipLimit))
		return ;

	//����ʱ��
	int nTimeGrid = int(GameConfig().BeginWriteSec - 2) * 800 / m_nChipTime;		//ʱ���,ǰ2�벻��ע,����-2,800��ʾ��������עʱ�䷶Χǧ�ֱ�
	for (int i = 0; i < m_nChipTime; i++)
	{
		int nRandRage = int(nTimeGrid * i / (1500 * sqrt((double)m_nChipTime))) + 1;		//������Χ
		nElapse = 2 + (nTimeGrid*i) / 1000 + ((rand()) % (nRandRage * 2) - (nRandRage - 1));
		//ASSERT(nElapse >= 2 && nElapse <= pGameStart->cbTimeLeave);
		if (nElapse < 2 || nElapse > (int)GameConfig().BeginWriteSec)	continue;

		m_VecOxTimer.push_back(new AnimalTimer(IDI_PLACE_JETTON + i + 1, nElapse));
	}

	//ASSERT( MyDebug(_T("������ %d ��ע���� %d ��Χ [%d %d] ������ %d ���� [%I64d %I64d] ��ׯ [%d %d]"), wMyID, m_nChipTime, m_nChipLimit[0], m_nChipLimit[1], 
	//	pGameStart->nChipRobotCount, m_lMaxChipBanker, m_lMaxChipUser, m_stlApplyBanker, m_nRobotApplyBanker) );

	return ;
}

//���㷶Χ	(����ֵ��ʾ�Ƿ����ͨ���½����޴ﵽ��ע)
bool CAnimalRobot::CalcJettonRange(LONGLONG lMaxScore, LONGLONG lChipLmt[], int & nChipTime, int lJetLmt[])
{
	//�������
	bool bHaveSetMinChip = false;

	//����һע
	if (lMaxScore < m_RobotInfo.nChip[0])	return false;

	//���÷�Χ
	for (int i = 0; i < CountArray(m_RobotInfo.nChip); i++)
	{
		if (!bHaveSetMinChip && m_RobotInfo.nChip[i] >= lChipLmt[0])
		{
			lJetLmt[0] = i;
			bHaveSetMinChip = true;
		}
		if (m_RobotInfo.nChip[i] <= lChipLmt[1])
			lJetLmt[1] = i;
	}
	if (lJetLmt[0] > lJetLmt[1])	lJetLmt[0] = lJetLmt[1];

	//�Ƿ񽵵�����
	if (m_bReduceJettonLimit)
	{
		if (nChipTime * m_RobotInfo.nChip[lJetLmt[0]] > lMaxScore)
		{
			//�Ƿ񽵵���ע����
			if (nChipTime * m_RobotInfo.nChip[0] > lMaxScore)
			{
				nChipTime = int(lMaxScore / m_RobotInfo.nChip[0]);
				lJetLmt[0] = 0;
				lJetLmt[1] = 0;
			}
			else
			{
				//���͵���������
				while (nChipTime * m_RobotInfo.nChip[lJetLmt[0]] > lMaxScore)
				{
					lJetLmt[0]--;
					ASSERT(lJetLmt[0] >= 0);
				}
			}
		}
	}

	return true;
}

bool CAnimalRobot::Update()
{
	for (vector<AnimalTimer*>::iterator it = m_VecOxTimer.begin(); it != m_VecOxTimer.end();)
	{		
		if (--(*it)->m_nTime == 0)
		{
			OnEventTimer((*it)->m_byTimerid);
			delete (*it);
			it = m_VecOxTimer.erase(it);
		}
		else
		{
			it++;
		}
	}
	if (m_nGameTime > 0)
	{
		m_nGameTime--;
	}	
	return m_nGameTime != 0;
}

DWORD CAnimalRobot::UserId()
{
	return m_dwRobotid;
}
DWORD CAnimalRobot::UserGold()
{
	Role* pRole=g_FishServer.GetRoleManager().QueryRole(m_dwRobotid);
	if (pRole)
	{
		return pRole->GetGlobelSum();
	}
	return 0;
}
DWORD CAnimalRobot::BankerId()
{
	return g_FishServer.GetDialManager().BankerId();
}

DWORD CAnimalRobot::BankerGold()
{
	return g_FishServer.GetDialManager().BankerGold();
}

WORD  CAnimalRobot::ApplyListLength()
{
	return g_FishServer.GetDialManager().ApplyListLength();
}


tagDialConfig CAnimalRobot::GameConfig()
{
	return g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig;
}
/////////////

CAnimalRobotManager::CAnimalRobotManager()
{
	srand(GetTickCount());
}

CAnimalRobotManager::~CAnimalRobotManager()
{

}
//void CAnimalRobotManager::Init(NiuNiuManager *pgame)
//{
//	m_pGame = pgame;
//}

void CAnimalRobotManager::Update()
{
	if ((GetTickCount() - CAnimalRobot::m_dwRobotCountChangeTime) / 1000 > (DWORD)(g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.RobotCountInterval * 60))//����������
	{
		CAnimalRobot::m_dwRobotCountChangeTime = GetTickCount();
		SYSTEMTIME st;
		GetLocalTime(&st);
		for (vector<RobotAnimalCountControl>::iterator it = g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.VecRobotCount.begin(); it != g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.VecRobotCount.end(); it++)
		{
			if (st.wHour >= (*it).StartTime&&st.wHour <= (*it).EndTime)
			{
				BYTE byCount1 = (*it).RobotCountMin;
				BYTE byCount2 = (*it).RobotCountMax;
				if (byCount1<=byCount2)
				{
					CAnimalRobot::m_byRobotCount = byCount1 + rand() % (byCount2 - byCount1 + 1);
				}					
				break;
			}
		}
	}

	if ((GetTickCount() - CAnimalRobot::m_dwApplistChangeTime) / 1000>(DWORD)(g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.RobotListCountInterval * 60))//�����б���������
	{
		CAnimalRobot::m_dwApplistChangeTime = GetTickCount();
		BYTE  bymlistmin = g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.RobotListCountMin;
		BYTE  bylistmax = g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.RobotListCountMax;
		if (bymlistmin <= bylistmax)
		{
			CAnimalRobot::m_byAppCountLimit = bymlistmin + rand() % (bylistmax - bymlistmin + 1);
		}
	}

	if (m_allrobot.size() < CAnimalRobot::m_byRobotCount)
	{
		RobotUserInfo *pInfo=g_FishServer.GetRobotManager().GetNonUseRobot();
		if (pInfo)
		{	
			CAnimalRobot *probot = new CAnimalRobot(pInfo->dwUserID);//���ӵ���
			g_FishServer.GetDialManager().OnRoleJoinDialTable(pInfo->dwUserID);			
			m_allrobot.push_back(probot);
		}		
	}

	for (vector<CAnimalRobot*>::iterator it = m_allrobot.begin(); it != m_allrobot.end();)
	{		
		if (!(*it)->Update() && g_FishServer.GetDialManager().IsGameEnd())
		{		
			g_FishServer.GetRobotManager().OnNonUseRobot((*it)->UserId());
			g_FishServer.GetDialManager().OnRoleLeaveDialTable((*it)->UserId());
			delete (*it);
			it=m_allrobot.erase(it);
		}
		else
		{
			it++;
		}
	}	
}

void CAnimalRobotManager::StartGame()
{
	for (vector<CAnimalRobot*>::iterator it = m_allrobot.begin(); it != m_allrobot.end();it++)
	{
		(*it)->StartGame();
	}
}

void CAnimalRobotManager::EndGame()
{
	for (vector<CAnimalRobot*>::iterator it = m_allrobot.begin(); it != m_allrobot.end();it++)
	{
		(*it)->EndGame();
	}
}
bool CAnimalRobotManager::IsRobot(DWORD dwUserid)
{
	for (vector<CAnimalRobot*>::iterator it = m_allrobot.begin(); it != m_allrobot.end(); it++)
	{
		if ((*it)->UserId() == dwUserid)
		{
			return true;
		}
	}
	return false;
}
USHORT CAnimalRobotManager::RobotCount()
{
	return m_allrobot.size();
}