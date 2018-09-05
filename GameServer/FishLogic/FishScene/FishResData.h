#pragma once
#define FISH_DURATION_TIME	0.1f		//��С��λ0.5�����ʱ��
#define MAX_FISH_NUM		USHRT_MAX	//ÿ��������ͬʱ����������
#define MAX_BULLET_NUM		UCHAR_MAX	//ÿ��������ͬʱ�����ӵ�����
enum PathEventType
{
	EVENT_NONE = 0,
	EVENT_STAY,     //ͣ��
	EVENT_LAUGH,    //����
	EVENT_ATTACK,   //�ܻ�
};

enum FishClipType
{
	CLIP_YOUYONG = 0,
	CLIP_SIWANG,
	CLIP_CHAOFENG,
	CLIP_GONGJI,
	CLIP_MAX
};
//�ṹ����
struct Ray
{
	Vector3 Pos;
	Vector3 Dir;
};
struct FishData
{
	Vector3		Size;
	Quaternion	Rot;	//��ģ�͵���ת
	float		ClipLength[CLIP_MAX];	//��Ӿ����������������ʱ�䳤�ȣ��룩
};

struct GroupData
{
	struct GroupItem
	{
		GroupItem()
		{
			pFishPosList = NULL;
		}
		bool	 ActionUnite;
		float	 Action;
		float    SpeedScaling;
		USHORT   FishNum;
		BYTE	 FishIndex;
		float    FishScaling;
		Vector3  *pFishPosList;
	};
	GroupData()
	{
		pGroupDataArray = NULL;
		pPathList = NULL;
		TotalFishCount = 0;
	}
	USHORT  TotalFishCount;
	USHORT	PathCount;
	USHORT	GroupDataCount;
	Vector3 FrontPosition;
	GroupItem *pGroupDataArray;
	USHORT *pPathList;

	//��Ⱥ��·��Ⱥ��

	bool	FishGroupByPathGroup;
	USHORT  PathGroupIndex;
	float   Speed;
	BYTE    FishIndex;
	float   FishScaling;
	float   ActionSpeed;
	USHORT	RepeatNum;
	USHORT  Interval;//����
	bool	ActionUnite;
};
struct FlowGroupData
{
	int		GroupID;
	int		Probability;		//���ֵĸ���0 - 1000, Խ����ֻ���Խ��
	int		MaxCount;			//�����ִ���,-1����

};
struct FlowTimeData
{
	int      GroupID;
	float    StartTime;          //0 - 1
	float    EndTime;            //0 - 1
	int      Probability;        //���ֵĸ���0 - 100, 100�س���
	int      MaxCount;           //�����ִ���
	float    LaunchInterval;     //������������
	bool     ClearAll;           //��ճ���
};
struct FlowData
{
	FlowData()
	{
		pFlowGroupList = NULL;
		pFlowTimeList = NULL;
	}
	int		MaxTime;                //��
	int		LoopCount;
	float	LauncherInterval;		//��
	int		GroupCount;
	int		FlowTimeCount;
	FlowGroupData *pFlowGroupList;
	FlowTimeData *pFlowTimeList;

	int		GetMaxTimeSeconds()
	{
		return MaxTime;
	}
};

struct LinearSplineNode
{
	short x, y, z;
	short qx, qy, qz, qw;
	short ts;
	short nodeIdx;
};
struct LinearSplineNodeUnzip
{
	Vector3 Pos;
	Quaternion Rot;
	float ts;
	short nodeIdx;
};
struct LinearNodeData
{
	byte	EventType;
	int 	Times;
};
struct PathData
{
	
	PathData()
	{
		pSplineDataList = NULL;
	}
	bool		HasPathEvent;
	float		SampleMaxDistance;
	float		MaxDistance;
	float		MaxTimeScaling;
	Matrix		WorldRTS;
	Rotation	WorldRot;

	Vector3		MinPos;
	Vector3		RangePos;
	Rotation	MinRot;
	Rotation	RangeRot;
	int			SplineDataCount;
	float		Speed;
	int			NodeCount;
	LinearNodeData   *pNodeList;
	LinearSplineNode *pSplineDataList;
	LinearSplineNodeUnzip *pSplineDataListUnzip;
};

struct PathGroupData
{
	short PathCount;
	PathData *pPathList;
};
struct DeadStayTime
{
	float TianZai_DouDong_Time;           //���ֶ���ʱ��

	float TianZai_Stay_Time1;             //���ֵȴ���ʯ����ͣ�����ʱ��
	float TianZai_Stay_Time2;             //���ֵȴ���ʯ����ͣ���ʱ��

	float TianZai_Dead_Time1;             //��������ͣ�����ʱ��
	float TianZai_Dead_Time2;             //��������ͣ���ʱ��

	float Bullet_BingDong_Dead_Time1;     //�����ڻ�������ͣ�����ʱ��
	float Bullet_BingDong_Dead_Time2;     //�����ڻ�������ͣ������ʱ��

	float BingDong_Dead_Time1;            //���������������ʱ��
	float BingDong_Dead_Time2;            //�������������ʱ��

	float ShanDian_Dead_Time1;            //���缼���������ʱ��
	float ShanDian_Dead_Time2;            //���缼�������ʱ��

	float LongJuanFeng_Dead_Time1;        //����缼���������ʱ��
	float LongJuanFeng_Dead_Time2;        //����缼�������ʱ��

	float JiGuang_Stay_Time;
	float JiGuang_Dead_Time1;             //��������������ʱ��
	float JiGuang_Dead_Time2;             //������������ʱ��

	float ShandDian_Speed;
	float JiGuang_Speed;
	float JiGuang_BingDong_Speed;
	float TianZai_Speed;
	float BingDong_Speed;
};