#pragma once
enum  CatchType
{
	CATCH_SKILL,
	CATCH_LASER,
	CATCH_BULLET,
};
enum LaunchFailedType
{
	LFT_OK,
	LFT_CD,
	LFT_INVALID,
	LFT_ENERGY,
};

enum SkillFailedType
{
	SFT_OK,
	SFT_CD,
	SFT_INVALID,
	SFT_COUNT,
	SFT_UNLOCK,
	SFT_BIND
};
//�������Խӿ�
class Fish;
class Bullet;
class IFishSetting{
public:
	virtual bool LoadConfig(char szDir[]) = 0;
	virtual UINT  GetVersion() = 0;
	virtual float BulletRadius(USHORT bulletID) = 0;					//�ӵ������뾶
	virtual float BulletCatchRadius(USHORT bulletID) = 0;					//�ӵ���ײ֮��Ĳ���뾶
	virtual UINT  MaxCatch(USHORT bulletID) = 0;				//�ӵ������������
	virtual float CatchChance(PlayerID player, int maxCatch, Fish* pFish, Bullet* pBullet) = 0;	//�ӵ��Ĳ������(0 - 1)
	virtual SkillFailedType  UseSkill(PlayerID playerID, SkillType skill) = 0;
	virtual LaunchFailedType  UseLaser(PlayerID playerID, byte launcherType) = 0;
	virtual void  GetSkillRange(SkillType skill, int &minNum, int &maxNum) = 0;
	virtual void  GetLaserRange(byte laser, int &minNum, int &maxNum) = 0;
	virtual void  GetSkillFreezeReduction(byte &speedScaling, byte &duration1, byte& duration2, byte &duration3) = 0;
	virtual float GetSkillChance(SkillType skill, USHORT fishIndex, BYTE byPackageType) = 0;
	virtual float GetLaserChance(byte launcherType, USHORT fishIndex, BYTE byPackageType) = 0;
	virtual float GetLaserRadius(byte launcherType) = 0;
	//LauncherType��̨����
	virtual BulletType GetBulletType(USHORT bulletID) = 0;
	virtual void  GetBulletFreezeReduction(byte &speedScaling, byte &duration1, byte& duration2, byte &duration3) = 0;
	//�����㣬���ؽ������
	virtual UINT  CatchFish(PlayerID playerID, USHORT fishIndx, CatchType catchType, byte subType, byte rate, bool result) = 0;
	virtual USHORT    GetCombo(PlayerID playerID, USHORT bulletID) = 0;
	//�Ƿ������ϴ�ͷ��
	virtual bool CanUploadImg(PlayerID playerID, USHORT imgSize) = 0;
	//���ܵ�cdʱ��
	virtual byte  GetSkillCDTime(SkillType st) = 0;
	//��̨���еļ��ٲ���
	virtual void  GetLaserReduction(byte LauncherType, byte &speedScaling, byte &duration1, byte& duration2, byte &duration3) = 0;
	//��̨���ӵ��ٶ�
	virtual byte  GetBulletSpeed(byte launcherType) = 0;
	//��̨�ķ�����
	virtual byte  GetLauncherInterval(byte LauncherType) = 0;
	//���е�CDʱ��
	virtual byte  GetLaserCDTime(byte LauncherType) = 0;
	virtual UINT  LaunchPackage(UINT fishid_packageid)  = 0;
	virtual byte  GetRateIndex(byte seat, PlayerID id) = 0;
	virtual USHORT FishRewardDrop(PlayerID id, BYTE byPackageType,USHORT byType) = 0;

	//�Ƿ��������㣬�������ж�һ�¡�
	virtual bool IsLightingFish(USHORT fishIndex) = 0;
	//�Ƿ��������㣬�����ؿ��Բ����������fishIndex���ID�����id,����0 ��ʾ���ǣ����ش���0��ʾ���������������
	virtual int  IsLightingFish(USHORT fishIndex, PlayerID id) = 0;
	//ĳһ�����Ƿ���Ա����粶��fishIndex��ID�����ID��dist:��Ļ���롣
	virtual bool CanCatchLightingFish(USHORT fishIndex, PlayerID id, float dist) = 0;
	virtual bool FishSpecialRateChange(const USHORT fishType, float rate) = 0;
	virtual bool FishConnonPowerChange(const UINT id, float power) = 0;
};

struct NetCmd;
typedef void(*SendDataCallback)(PlayerID id, NetCmd *pCmd);
typedef void(*RecvDataCallback)(PlayerID id, NetCmd *pCmd);
typedef void(*FishLogCallback)(const WCHAR *pcStr);
typedef void(*KickPlayerCallback)(PlayerID id);
typedef IFishSetting* (*BulletDataCallback)(USHORT bulletID);

class FishCallback
{
public:

	//�ⲿ�ṩ��FishSceneʹ�õĽӿ�
	//===============================================
	static void SetSendDataFunc(SendDataCallback pFunc);
	static void SendData(PlayerID id, NetCmd *pCmd);
	
	static void SetFishLogFunc(FishLogCallback pFunc);
	static void FishLog(const WCHAR *pcStr);


	static void SetKickPlayerFunc(KickPlayerCallback pFunc);
	static void KickPlayer(PlayerID id);

	static void SetFishSetting(IFishSetting *pSetting);
	static IFishSetting* GetFishSetting();

	//FishScene�ṩ���ⲿ�Ľӿ�
	//===============================================
	static void SetRecvDataFunc(RecvDataCallback pFunc);
	static void RecvData(PlayerID id, NetCmd *pCmd);

};