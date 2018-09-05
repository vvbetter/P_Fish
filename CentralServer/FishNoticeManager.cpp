#include "Stdafx.h"
#include "FishNoticeManager.h"
#include "FishServer.h"
FishNoticeManager::FishNoticeManager()
{

}
FishNoticeManager::~FishNoticeManager()
{

}
void FishNoticeManager::OnReLoadFishNotcieConfig()
{
	//���¼���ȫ���Ĺ�������
	g_FishServer.GetFishConfig().LoadFishNoticeConfig(TEXT("FishNoticeConfig.xml"));
}
void FishNoticeManager::OnUpdateByMin(DWORD dwTimer)
{
	SYSTEMTIME time;
	GetLocalTime(&time);
	static BYTE LogNoticeUpdateMin = 0xff;//�ϴθ��¼�¼�ķ�����
	if (time.wSecond == 0 && (LogNoticeUpdateMin == 0xff || time.wMinute != LogNoticeUpdateMin))
	{
		LogNoticeUpdateMin = (BYTE)time.wMinute;//��¼�ϴθ���
		OnUpdateByZeroSec();
	}
}
void FishNoticeManager::OnUpdateByZeroSec()
{
	//ÿ����0����и���	
	HashMap<DWORD, tagNotice>& pMap = g_FishServer.GetFishConfig().GetAllNoticeInfo();
	if (pMap.empty())
	{
		return;
	}
	time_t NowTime = time(null);
	HashMap<DWORD, tagNotice>::iterator Iter = pMap.begin();
	for (; Iter != pMap.end(); ++Iter)
	{
		//����ÿһ������
		OnHandleOnceNotice(Iter->second, NowTime);
	}
}
void FishNoticeManager::OnHandleOnceNotice(tagNotice& pNotcie,time_t NowTime)
{
	//����ǰ����
	if (pNotcie.BeginTime > NowTime || (pNotcie.BeginTime + (pNotcie.StepNum - 1) *pNotcie.StepMin*60) < NowTime)
		return;
	DWORD DiffSec = static_cast<DWORD>(NowTime - pNotcie.BeginTime);
	//�������ʱ�� ��Ϊ�ⲿ��֤��һ����һ�ε� ���ǿ������ڲ������ӽ��н���
	DWORD ChangeSec = (DiffSec % (pNotcie.StepMin * 60));
	if (ChangeSec < 60)
	{
		g_FishServer.SendMessageByType(pNotcie.MessageInfo, _tcslen(pNotcie.MessageInfo), MT_Center, pNotcie.MessageInfoColor, pNotcie.OnceStepNum, pNotcie.OnceStepSec, 0);
	}
}