//�������˹������
#pragma once
class FishNoticeManager
{
public:
	FishNoticeManager();
	virtual ~FishNoticeManager();

	void OnReLoadFishNotcieConfig();//���¼��������ļ�
	void OnUpdateByMin(DWORD dwTimer);
private:
	void OnUpdateByZeroSec();//����  ÿ���ӵ�0����и��´���
	void OnHandleOnceNotice(tagNotice& pNotcie, time_t NowTime);
};