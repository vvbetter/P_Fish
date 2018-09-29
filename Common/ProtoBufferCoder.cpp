#include "stdafx.h"
#include "..\GameServer\FishLogic\NetCmd.h"
#include "ProtoBufferCoder.h"
#include "proto_source\LoginMessage.pb.h"
#include "proto_source\RpcMessage.pb.h"
#include "proto_source\Game60_FishingMessage.pb.h"
#include <json\json.h>
#include <memory>

using namespace com::game::proto;
const uint PBC_HEADER_SIZE = 10;
//proto buffer ���롣
//����Ϸ��������Ϸ�ͻ�����ϢͨѶ��Ϣ��һ����
//��Ϸ���� 2�ֽ���Ϣ���ȣ��������Ϣ���ȣ���������2�ֽڣ�+ 8�ֽ����ID + 2�ֽ���ϢID + n�ֽڵ�PBC��Ϣ
//�ͻ��� 2�ֽ���ϢID + 2�ֽ���Ϣ���ȣ�ֻ�Ǻ���PBC����Ϣ����) + n�ֽڵ�PBC��Ϣ
//�ڲ�ͨѶ��NetCmd 2�ֽڵ���Ϣ���ȣ�ȫ�����ȣ�+ 8�ֽڵ�UID + 2�ֽڵ���ϢID + ��Ϣ��
// proto buffer ����



void PrintLogInCmd( USHORT cmdType)
{
#ifdef _DEBUG
	USHORT logCmeType[] = { 6003, 6004, 6030, 6017, 6018, 6032, 6031, 6025, 6026 };
	for (USHORT& x : logCmeType)
	{
		if (cmdType == x)
		{
			Log("��ӡ��Ϣ cmdType=%d ", x);
			break;
		}
	}
#endif
}

NetCmd* PBC_Decode(const USHORT cmdType, const char* buffer, const int length, bool& isPBC)
{
	if (buffer == NULL)
	{
		ASSERT(false);
		return NULL;
	}
	PrintLogInCmd(cmdType);
	NetCmd *pCmd = NULL;
	isPBC = true;
	//��Ϸ��������Щ���󲻴���
	if (cmdType == GetMsgType(159, 24) || cmdType == GetMsgType(65, 24) || cmdType == GetMsgType(156, 24)) //6303 ͬ�����ñ�// 6209 �����˳�����Ϸ//6300 ʱʱ��LS����GS��ע
	{
		return NULL;
		//Log("Ignore CmdType=%d", cmdType);
	}
	//������Ϣ
	else if (cmdType == GetMsgType(162, 24))//6306 ʹ����Ʒ
	{
		pCmd = CreateCmd(sizeof(LG_UseGoods));
		pCmd->SetCmdType(GetMsgType(Main_Hall, 162));
		int64 uid = 0;
		memcpy_s(&uid, 8, buffer + 2, 8);
		((LG_UseGoods*)pCmd)->uid = _NTOHLL_(uid);
		LGUseGoodsMessage msg;
		msg.ParseFromArray(buffer + sizeof(NetCmd), length - PBC_HEADER_SIZE);
		if (msg.has_itemid()) ((LG_UseGoods*)pCmd)->itemId = msg.itemid();
		if (msg.has_itemmodelid()) ((LG_UseGoods*)pCmd)->itemModelId = msg.itemmodelid();
		if (msg.has_itemnum()) ((LG_UseGoods*)pCmd)->itemNum = msg.itemnum();
		if (msg.has_time())((LG_UseGoods*)pCmd)->keep_time = msg.time();
	}
	else if (cmdType == GetMsgType(Main_HallHeartBeat, 0))//��Ϸ����������Ϣ������
	{
		pCmd = CreateCmd(length + sizeof(USHORT), buffer);
	}
	else if (cmdType == GetMsgType(67, 24)) //6211 �����Ҹı䣬һ������ǳ�ֵԭ���µ�
	{
		pCmd = CreateCmd(sizeof(LG_UpdateMoney));
		pCmd->SetCmdType(GetMsgType(Main_Hall, 67));
		int64 uid = 0;
		memcpy_s(&uid, 8, buffer + 2, 8);
		((LG_UpdateMoney*)pCmd)->uid = _NTOHLL_(uid);
		LGUpdateMoney1Message msg;
		msg.ParseFromArray(buffer + sizeof(NetCmd), length - PBC_HEADER_SIZE);
		if (msg.has_money1()) ((LG_UpdateMoney*)pCmd)->money1 = msg.money1();
		if (msg.has_money2()) ((LG_UpdateMoney*)pCmd)->money2 = msg.money2();
		if (msg.has_reason()) ((LG_UpdateMoney*)pCmd)->reason = msg.reason();
	}
	else if (cmdType == GetMsgType(63, 24))//6207 �����������Ϸ
	{
		LGLoginSubGameMessage msg;
		msg.ParseFromArray(buffer + sizeof(NetCmd), length - PBC_HEADER_SIZE);
		UINT dataSize = sizeof(LG_UDPClientConnect);
		Json::Value json_obj;
		if (msg.has_achdatamap())
		{
			const string& achdata = msg.achdatamap();
			Json::CharReaderBuilder builder;
			Json::CharReader* reader = builder.newCharReader();
			JSONCPP_STRING errs;
			if (reader->parse(achdata.c_str(), achdata.c_str() + strlen(achdata.c_str()), &json_obj, &errs))
			{
				dataSize += json_obj.size() * sizeof(tagAchDataMap);
			}
		}
		pCmd = CreateCmd(dataSize);
		pCmd->SetCmdType(GetMsgType(Main_Hall, 63));
		int64 uid = 0;
		memcpy_s(&uid, 8, buffer + 2, 8);
		((LG_UDPClientConnect*)pCmd)->uid = _NTOHLL_(uid);
		if (msg.has_rpcid())((LG_UDPClientConnect*)pCmd)->rpcid = msg.rpcid();
		if (msg.has_money1()) ((LG_UDPClientConnect*)pCmd)->udata.money1 = msg.money1();
		if (msg.has_money2()) ((LG_UDPClientConnect*)pCmd)->udata.money2 = msg.money2();
		if (msg.has_name())
		{
			const std::string& name = msg.name();
			memset(((LG_UDPClientConnect*)pCmd)->udata.name, 0, MAX_NICKNAME + 1);
			memcpy_s(((LG_UDPClientConnect*)pCmd)->udata.name, MAX_NICKNAME, name.c_str(), MAX_NICKNAME);
		}
		if (msg.has_icon())
		{
			memcpy_s(((LG_UDPClientConnect*)pCmd)->udata.icon, ICON_LENGTH, msg.icon().c_str(), msg.icon().length());
		}
		if (msg.has_sex())((LG_UDPClientConnect*)pCmd)->udata.sex = msg.sex();
		if (msg.has_loginid())((LG_UDPClientConnect*)pCmd)->udata.loginid = msg.loginid();
		if (msg.has_deleteat())((LG_UDPClientConnect*)pCmd)->udata.deleteAt = msg.deleteat();
		if (msg.has_gmlevel())((LG_UDPClientConnect*)pCmd)->udata.gmLevel = msg.gmlevel();
		if (msg.has_robot())((LG_UDPClientConnect*)pCmd)->udata.robot = msg.robot();
		if (msg.has_titleindex())((LG_UDPClientConnect*)pCmd)->udata.titleIndex = msg.titleindex();
		if (msg.has_gamemaxwinmoney())((LG_UDPClientConnect*)pCmd)->udata.gameMaxWinMoney = msg.gamemaxwinmoney();
		if (msg.has_charmvalue())((LG_UDPClientConnect*)pCmd)->udata.charmValue = msg.charmvalue();
		if (msg.has_vip())((LG_UDPClientConnect*)pCmd)->udata.vip = msg.vip();
		if (msg.has_barragenum())((LG_UDPClientConnect*)pCmd)->udata.barrageNum = msg.barragenum();
		if (msg.has_taxratio())((LG_UDPClientConnect*)pCmd)->udata.taxRatio = msg.taxratio();
		if (msg.has_cardbg())((LG_UDPClientConnect*)pCmd)->udata.cardBg = msg.cardbg();
		if (msg.has_headicon())((LG_UDPClientConnect*)pCmd)->udata.headIcon = msg.headicon();
		if (msg.has_signature())
		{
			const std::string& signature = msg.signature();
			memset(((LG_UDPClientConnect*)pCmd)->udata.signature, 0, MAX_SIGNATURE + 1);
			memcpy_s(((LG_UDPClientConnect*)pCmd)->udata.signature, MAX_SIGNATURE, signature.c_str(), MAX_SIGNATURE);
		}
		if (msg.has_taxfreecardendtime())((LG_UDPClientConnect*)pCmd)->udata.taxFreeCardEndTime = msg.taxfreecardendtime();
		if (msg.has_taxfreecardrate())((LG_UDPClientConnect*)pCmd)->udata.taxFreeCardRate = msg.taxfreecardrate();
		if (msg.has_ratiovalue())((LG_UDPClientConnect*)pCmd)->udata.ratioValue = msg.ratiovalue();
		((LG_UDPClientConnect*)pCmd)->udata.enterTime = timeGetTime();
		((LG_UDPClientConnect*)pCmd)->udata.achSize = json_obj.size();
		int achIndex = 0;
		for (const auto& x : json_obj)
		{
			if (x.isNull()) continue;
			((LG_UDPClientConnect*)pCmd)->udata.achDataMap[achIndex].achtype = x["achtype"].asInt();
			((LG_UDPClientConnect*)pCmd)->udata.achDataMap[achIndex].finishtime = x["finishtime"].asUInt64();
			((LG_UDPClientConnect*)pCmd)->udata.achDataMap[achIndex].gameid = x["gameid"].asInt();
			((LG_UDPClientConnect*)pCmd)->udata.achDataMap[achIndex].liansheng = x["liansheng"].asInt();
			((LG_UDPClientConnect*)pCmd)->udata.achDataMap[achIndex].lianshu = x["lianshu"].asInt();
			((LG_UDPClientConnect*)pCmd)->udata.achDataMap[achIndex].qId = x["qId"].asInt();
			((LG_UDPClientConnect*)pCmd)->udata.achDataMap[achIndex].qachNum = x["qachNum"].asInt();
			((LG_UDPClientConnect*)pCmd)->udata.achDataMap[achIndex].qachfinishnum = x["qachfinishnum"].asInt();
			((LG_UDPClientConnect*)pCmd)->udata.achDataMap[achIndex].receivetime = x["receivetime"].asUInt64();
			((LG_UDPClientConnect*)pCmd)->udata.achDataMap[achIndex].rewardMoney = x["rewardMoney"].asFloat();
			((LG_UDPClientConnect*)pCmd)->udata.achDataMap[achIndex].titleID = x["titleID"].asInt();
			((LG_UDPClientConnect*)pCmd)->udata.achDataMap[achIndex].titlemoney = x["titlemoney"].asFloat();
			++achIndex;
		}
	}
	else if (cmdType == GetMsgType(71, 24))//6215 ֪ͨ�رղ��������
	{
		pCmd = CreateCmd(sizeof(LG_StopFishServer));
		pCmd->SetCmdType(GetMsgType(Main_Hall, 71));
		LGStopFishServerMessage msg;
		msg.ParseFromArray(buffer + sizeof(NetCmd), length - PBC_HEADER_SIZE);
		if (msg.has_rpcid()) ((LG_StopFishServer*)pCmd)->rcpid = msg.rpcid();
		if (msg.has_closetime()) ((LG_StopFishServer*)pCmd)->closeTime = msg.closetime();
	}
	else if (cmdType == GetMsgType(72, 24)) //6216 ֪ͨ���ز������ñ�
	{
		LGReloadFishTableMessage msg;
		msg.ParseFromArray(buffer + sizeof(NetCmd), length - PBC_HEADER_SIZE);
		UINT cfgStrSize = 0;
		if (msg.has_configstring())
		{
			cfgStrSize = msg.configstring().length() + 1;
		}
		UINT msgSize = sizeof(CL_Cmd_ReloadConfig)+cfgStrSize;
		pCmd = (NetCmd *)malloc(msgSize);
		memset(pCmd, 0, msgSize);
		pCmd->SetCmdSize(msgSize);
		pCmd->SetCmdType(GetMsgType(Main_Hall, 72));
		if (msg.has_configstring())
		{
			memcpy_s(((CL_Cmd_ReloadConfig*)pCmd)->tableStr, cfgStrSize, msg.configstring().c_str(), cfgStrSize);
		}
	}
	else if (cmdType == GetMsgType(73, 24)) //6217 ֪ͨ��Ϸ����������ҽ���
	{
		pCmd = CreateCmd(sizeof(CL_HallStartFishServer));
		pCmd->SetCmdType(GetMsgType(Main_Hall, 73));
		LGStartFishServerMessage msg;
		msg.ParseFromArray(buffer + sizeof(NetCmd), length - PBC_HEADER_SIZE);
		if (msg.has_starttime()) ((CL_HallStartFishServer*)pCmd)->starTime = msg.starttime();
	}
	//��Ϸ�ͻ�����Ϣ
	else if (cmdType == 6003)//������뷿��
	{
		pCmd = CreateCmd(sizeof(CL_JoinTable));
		pCmd->SetCmdType(GetMsgType(Main_Table, CL_Sub_JoinTable));
		ReqJoinRoomMessage msg;
		msg.ParseFromArray(buffer + 4, length);
		if (msg.has_roomid())
		{
			((CL_JoinTable*)pCmd)->bTableTypeID = msg.roomid();
		}
		if (msg.has_isarena())
		{
			((CL_JoinTable*)pCmd)->isArena = (msg.isarena() == 0 ? false : true);
		}
	}
	else if (cmdType == 6009)//�����ӵ�
	{
		pCmd = CreateCmd(sizeof(NetCmdBullet));
		pCmd->SetCmdType(CMD_BULLET);
		ReqBulletMessage msg;
		msg.ParseFromArray(buffer + 4, length);
		if (msg.has_angle()) ((NetCmdBullet*)pCmd)->Angle = msg.angle();
		if (msg.has_lockfishid()) ((NetCmdBullet*)pCmd)->LockFishID = msg.lockfishid();
		if (msg.has_launchertype()) ((NetCmdBullet*)pCmd)->LauncherType = msg.launchertype();
	}
	else if (cmdType == 6017)//�����л���̨
	{
		pCmd = CreateCmd(sizeof(NetCmdChangeLauncherType));
		pCmd->SetCmdType(CMD_CHANGE_LAUNCHER_TYPE);
		ReqChangeLauncherMessage msg;
		msg.ParseFromArray(buffer + 4, length);
		if (msg.has_seat()) ((NetCmdChangeLauncherType*)pCmd)->Seat = msg.seat();
		if (msg.has_launchertype()) ((NetCmdChangeLauncherType*)pCmd)->LauncherType = msg.launchertype();
	}
	else if (cmdType == 6019)//�����ڵı���
	{
		pCmd = CreateCmd(sizeof(NetCmdClientChangeRate));
		pCmd->SetCmdType(CMD_CHANGE_CLIENT_RATE);
		ReqChangeRateMessage msg;
		msg.ParseFromArray(buffer + 4, length);
		if (msg.has_uprodownrate()) ((NetCmdClientChangeRate*)pCmd)->UpRoDownRate = msg.uprodownrate();
	}
	else if (cmdType == 6025)//�����뿪��Ϸ
	{
		pCmd = CreateCmd(sizeof(CL_LeaveTable));
		pCmd->SetCmdType(GetMsgType(Main_Table, CL_Sub_LeaveTable));
	}
	else if (cmdType == 6027)//����鿴�����Ϣ
	{
		pCmd = CreateCmd(sizeof(NetCmdClientShowOthers));
		pCmd->SetCmdType(GetMsgType(Main_Table, LC_Sub_ShowOthers));
		ReqLookUpPlayerInfoMessage msg;
		msg.ParseFromArray(buffer + 4, length);
		if (msg.has_playerid()) ((NetCmdClientShowOthers*)pCmd)->playerid = msg.playerid();
	}
	else if (cmdType == 6031)//�ͻ���׼����ɿ���ͬ������
	{
		pCmd = CreateCmd(sizeof(LC_ReqStartSyncFish));
		pCmd->SetCmdType(GetMsgType(Main_Table, LC_Sub_StartSyncFish));
	}
	else if (cmdType > 0x8000)
	{
		isPBC = false;
		pCmd = CreateCmd(length, buffer);
		//Log("PCB_Decode unkown data:cmdSize=%d,cmdType=%d,mainType=%d,subType=%d", pCmd->GetCmdSize(), pCmd->GetCmdType(), pCmd->GetCmdType() >> 8, pCmd->GetCmdType() & 0xff);
	}
	else
	{
		Log("undefined cmdType = %d,cmdsize = %d", cmdType, length);
		return NULL;
	}
	return pCmd;
}
char* PBC_Encode(NetCmd* pCmd, uint& dataLenth, bool& isPBC)
{
	if (pCmd == NULL)
	{
		ASSERT(false);
		return false;
	}
	USHORT cmdType = pCmd->GetCmdType();
	PrintLogInCmd(cmdType);
	isPBC = true;
	char* ret = NULL;
	//Log("PBC_Encode cmdType =%d", cmdType);
	switch (cmdType)
	{
	//������Ϣ
	case GetMsgType(Main_HallHeartBeat, 0):
	{
		com::game::proto::GLRegisterLoginMessage retMessage;
		retMessage.set_msgid(com::game::proto::Protos_Login::GLRegisterLogin);
		Json::Value a;
		Json::StreamWriterBuilder jw;
		a["clientType"] = "Game60";
		a["state"] = 0;
		a["gameId"] = 80002;
		unique_ptr<Json::StreamWriter> writer(jw.newStreamWriter());
		ostringstream os;
		writer->write(a, &os);
		retMessage.set_serverinfo(os.str());
		dataLenth = retMessage.ByteSize() + sizeof(NetCmd);
		ret = (char*)malloc(dataLenth);
		USHORT pbcLen = htons(retMessage.ByteSize() + 10);
		memcpy_s(ret, 2, &pbcLen, 2);
		long long i = 0;
		memcpy_s(ret + 2, 8, &i, 8);
		USHORT id = htons(com::game::proto::Protos_Login::GLRegisterLogin);
		memcpy_s(ret + 10, 2, &id, 2);
		retMessage.SerializeToArray(ret + 12, retMessage.ByteSize());
		break;
	}
	case GetMsgType(64, 24): //6208 �����������Ϸ����
	{
		GLLoginSubGameMessage msg;
		GL_LoginSubGame* pmsg = (GL_LoginSubGame*)pCmd;
		msg.set_rpcid(pmsg->rpcId);
		char ipStr[30] = { 0 };
		GetIPString(pmsg->fishIp, ipStr);
		std::string tempStr(ipStr);
		msg.set_fiship(tempStr);
		msg.set_fishport(pmsg->fishPort);
		msg.set_loginsubgame(pmsg->loginSubGame);
		dataLenth = msg.ByteSize() + sizeof(NetCmd);
		ret = (char*)malloc(dataLenth);
		USHORT pbcLen = htons(msg.ByteSize() + 10);
		memcpy_s(ret, 2, &pbcLen, 2);
		int64 uid = _HTONLL_(pmsg->uid);
		memcpy_s(ret + 2, 8, &uid, 8);
		USHORT id = htons(com::game::proto::Protos_Rpc::GLLoginSubGame);
		memcpy_s(ret + 10, 2, &id, 2);
		msg.SerializeToArray(ret + 12, msg.ByteSize());
		break;
	}
	case GetMsgType(68, 24): //ͬ��������ݵ���Ϸ���� 6212
	{
		GL_SyncDataToHall* pmsg = (GL_SyncDataToHall*)pCmd;
		GLUpdateRoundInfoMessage msg;
		msg.set_money1(pmsg->money1);
		msg.set_money2(pmsg->money2);
		msg.set_subgameid(pmsg->subGameId);
		dataLenth = msg.ByteSize() + sizeof(NetCmd);
		ret = (char*)malloc(dataLenth);
		USHORT pbcLen = htons(msg.ByteSize() + 10);
		memcpy_s(ret, 2, &pbcLen, 2);
		int64 uid = _HTONLL_(pmsg->uid);
		memcpy_s(ret + 2, 8, &uid, 8);
		USHORT id = htons(com::game::proto::Protos_Rpc::GLUpdateRoundInfo);
		memcpy_s(ret + 10, 2, &id, 2);
		msg.SerializeToArray(ret + 12, msg.ByteSize());
		break;
	}
	case GetMsgType(70, 24):
	{
		GL_QuitSubGame* pmsg = (GL_QuitSubGame*)pCmd;
		GLQuitSubGameMessage msg;
		msg.set_rpcid(0);
		msg.set_quitsubgame(true);
		msg.set_money1(pmsg->money1);
		msg.set_money2(pmsg->money2);
		msg.set_offline(pmsg->quitSubGame);
		msg.set_daywinnum(pmsg->winNum);
		msg.set_daylosenum(pmsg->loseNum);
		msg.set_winmoney2(pmsg->winMoney);
		msg.set_losemoney2(pmsg->loseMoney);
		msg.set_continuewinnum(0);
		msg.set_gamemaxwinmoney(0);
		msg.set_charmvalue(pmsg->charmValue);
		msg.set_others(Format2String(pmsg->catchBossFishCount));
		//�����ɾ���Ϣ
		Json::Value achValue;
		Json::StreamWriterBuilder jw;
		for (int i = 0; i < pmsg->achSize; ++i)
		{
			Json::Value value_i;
			value_i["achtype"] = pmsg->achData[i].achtype;
			value_i["finishtime"] = pmsg->achData[i].finishtime;
			value_i["gameid"] = pmsg->achData[i].gameid;
			value_i["liansheng"] = pmsg->achData[i].liansheng;
			value_i["lianshu"] = pmsg->achData[i].lianshu;
			value_i["qId"] = pmsg->achData[i].qId;
			value_i["qachNum"] = pmsg->achData[i].qachNum;
			value_i["qachfinishnum"] = pmsg->achData[i].qachfinishnum;
			value_i["receivetime"] = pmsg->achData[i].receivetime;
			value_i["rewardMoney"] = pmsg->achData[i].rewardMoney;
			value_i["titleID"] = pmsg->achData[i].titleID;
			value_i["titlemoney"] = pmsg->achData[i].titlemoney;
			achValue[Format2String(pmsg->achData[i].qId)] = value_i;
		}
		unique_ptr<Json::StreamWriter> writer(jw.newStreamWriter());
		ostringstream os;
		writer->write(achValue, &os);
		msg.set_achdatamap(os.str());
		//cout << msg.achdatamap() << endl;
		//Log("���%lld�˳���Ϸ,Ӯ�Ĵ���:%d,��Ĵ���:%d, ��Ӯ��Ǯ%lf,�����Ǯ%lf", pmsg->uid, pmsg->winNum, pmsg->loseNum, pmsg->winMoney, pmsg->loseMoney);
		dataLenth = msg.ByteSize() + sizeof(NetCmd);
		ret = (char*)malloc(dataLenth);
		USHORT pbcLen = htons(msg.ByteSize() + 10);
		memcpy_s(ret, 2, &pbcLen, 2);
		int64 uid = _HTONLL_(pmsg->uid);
		memcpy_s(ret + 2, 8, &uid, 8);
		USHORT id = htons(com::game::proto::Protos_Rpc::GLQuitSubGame);
		memcpy_s(ret + 10, 2, &id, 2);
		msg.SerializeToArray(ret + 12, msg.ByteSize());
		break;
	}
	//��Ϸ�ͻ�����Ϣ
	case GetMsgType(Main_Table, LC_Sub_JoinTable): //���ؽ��뷿����Ϣ 6004
	{		
		LC_JoinTableResult* pmsg = (LC_JoinTableResult*)pCmd;
		ResJoinRoomMessage msg;
		msg.set_result(pmsg->Result);
		msg.set_roomid(pmsg->RoomID);
		PlayerData* pNewData = msg.mutable_playerdata();
		pNewData->set_playerid(pmsg->PlayerData.playerId);
		pNewData->set_playername(std::string(pmsg->PlayerData.playerName));
		pNewData->set_sex(pmsg->PlayerData.sex);
		pNewData->set_headicon(pmsg->PlayerData.headicon);
		pNewData->set_icon(std::string(pmsg->PlayerData.icon));
		pNewData->set_viplevel(pmsg->PlayerData.vipLevel);
		pNewData->set_goldnum(pmsg->PlayerData.goldNum);
		pNewData->set_seat(pmsg->PlayerData.seat);
		pNewData->set_launchertype(pmsg->PlayerData.launcherType);
		pNewData->set_rateindex(pmsg->PlayerData.rateIndex);
		pNewData->set_energy(pmsg->PlayerData.energy);
		dataLenth = msg.ByteSize() + 4;
		ret = (char*)malloc(dataLenth);
		USHORT msgID = Protos_Game60Fishing::ResJoinRoom;
		memcpy_s(ret, 2, &msgID, 2);
		USHORT pbcLen = msg.ByteSize();
		memcpy_s(ret + 2, 2, &pbcLen, 2);
		msg.SerializeToArray(ret + 4, msg.ByteSize());
		break;
	}
	case GetMsgType(Main_Table, LC_Sub_OtherUserInfo): //��Ҽ���ͬ�����������Ϣ 6016
	{
		LC_OtherUserInfo* pmsg = (LC_OtherUserInfo*)pCmd;
		ResPlayerJoinMessage msg;
		PlayerData* pNewData = msg.mutable_playerdata();
		pNewData->set_playerid(pmsg->UserInfo.playerId);
		pNewData->set_playername(std::string(pmsg->UserInfo.playerName));
		pNewData->set_sex(pmsg->UserInfo.sex);
		pNewData->set_headicon(pmsg->UserInfo.headicon);
		pNewData->set_icon(std::string(pmsg->UserInfo.icon));
		pNewData->set_viplevel(pmsg->UserInfo.vipLevel);
		pNewData->set_goldnum(pmsg->UserInfo.goldNum);
		pNewData->set_seat(pmsg->UserInfo.seat);
		pNewData->set_launchertype(pmsg->UserInfo.launcherType);
		pNewData->set_rateindex(pmsg->UserInfo.rateIndex);
		pNewData->set_energy(pmsg->UserInfo.energy);
		dataLenth = msg.ByteSize() + 4;
		ret = (char*)malloc(dataLenth);
		USHORT msgID = Protos_Game60Fishing::ResPlayerJoin;
		memcpy_s(ret, 2, &msgID, 2);
		USHORT pbcLen = msg.ByteSize();
		memcpy_s(ret + 2, 2, &pbcLen, 2);
		msg.SerializeToArray(ret + 4, msg.ByteSize());											   
		break;
	}
	case CMD_SKILL_DOUBLE_FIRE: //ʹ�ÿ񱩵��� 6036
	{
		LG_UseGoodsKB* pmsg = (LG_UseGoodsKB*)pCmd;
		ResUseGoodsKBMessage msg;
		msg.set_seatid(pmsg->seatID);
		msg.set_keeptime(pmsg->keepTime);
		dataLenth = msg.ByteSize() + 4;
		ret = (char*)malloc(dataLenth);
		USHORT msgID = Protos_Game60Fishing::ResUseGoodsKB;
		memcpy_s(ret, 2, &msgID, 2);
		USHORT pbcLen = msg.ByteSize();
		memcpy_s(ret + 2, 2, &pbcLen, 2);
		msg.SerializeToArray(ret + 4, msg.ByteSize());
		break;
	}
	case CMD_SKILL_TIMESTOP: //���ʹ��ȫ���������� 6038
	{
		LG_UseGoodsBD* pmsg = (LG_UseGoodsBD*)pCmd;
		ResUseGoodsBDMessage msg;
		msg.set_seatid(pmsg->seatID);
		msg.set_keeptime(pmsg->keepTime);
		dataLenth = msg.ByteSize() + 4;
		ret = (char*)malloc(dataLenth);
		USHORT msgID = Protos_Game60Fishing::ResUseGoodsBD;
		memcpy_s(ret, 2, &msgID, 2);
		USHORT pbcLen = msg.ByteSize();
		memcpy_s(ret + 2, 2, &pbcLen, 2);
		msg.SerializeToArray(ret + 4, msg.ByteSize());
		break;
	}
	case CMD_SYNC_BULLET:  //ͬ���������ӵ� 6024
	{
		NetCmdSyncBullet* pmsg = (NetCmdSyncBullet*)pCmd;
		ResSyncBulletDataMessage msg;
		UINT bulletCount = (pmsg->GetCmdSize() - sizeof(NetCmdSyncBullet)) / sizeof(tagSyncBulletData)+1;
		if (bulletCount > 0)
		{
			for (UINT i = 0; i < bulletCount; ++i)
			{
				SyncBulletData* nData = msg.add_syncbulletdata();
				nData->set_bulletid(pmsg->Bullets[i].BulletID);
				nData->set_degree(pmsg->Bullets[i].Degree);
				nData->set_time(pmsg->Bullets[i].Time);
				nData->set_bullettype(pmsg->Bullets[i].BulletType);
				nData->set_rateidx(pmsg->Bullets[i].RateIdx);
				nData->set_reboundcount(pmsg->Bullets[i].ReboundCount);
				nData->set_lockfishid(pmsg->Bullets[i].LockFishID);
			}
		}
		dataLenth = msg.ByteSize() + 4;
		ret = (char*)malloc(dataLenth);
		USHORT msgID = Protos_Game60Fishing::ResSyncBulletData;
		memcpy_s(ret, 2, &msgID, 2);
		USHORT pbcLen = msg.ByteSize();
		memcpy_s(ret + 2, 2, &pbcLen, 2);
		msg.SerializeToArray(ret + 4, msg.ByteSize());
		break;
	}
	case CMD_FISH://ͬ�������е��� 6006
	{
		NetCmdFish* pmsg = (NetCmdFish*)pCmd;
		ResCmdFishMessage msg;
		msg.set_groupid(pmsg->GroupID);
		msg.set_startid(pmsg->StartID);
		msg.set_pathid(pmsg->PathID);
		dataLenth = msg.ByteSize() + 4;
		ret = (char*)malloc(dataLenth);
		USHORT msgID = Protos_Game60Fishing::ResCmdFish;
		memcpy_s(ret, 2, &msgID, 2);
		USHORT pbcLen = msg.ByteSize();
		memcpy_s(ret + 2, 2, &pbcLen, 2);
		msg.SerializeToArray(ret + 4, msg.ByteSize());
		break;			  
	}
	case CMD_SYNC_FISH://ͬ�������е��첽���� 6008
	{
		NetCmdSyncFish* pmsg = (NetCmdSyncFish*)pCmd;
		ResCmdSyncFishMessage msg;
		msg.set_fishid(pmsg->FishID);
		msg.set_groupid(pmsg->GroupID);
		msg.set_fishtime(pmsg->FishTime);
		msg.set_pathgroup(pmsg->PathGroup);
		msg.set_pathidx(pmsg->PathIdx);
		msg.set_isactiveevent(pmsg->IsActiveEvent);
		msg.set_elapsedtime(pmsg->ElapsedTime);
		msg.set_delaytype(pmsg->DelayType);
		msg.set_delayscaling(pmsg->DelayScaling);
		msg.set_delayduration1(pmsg->DelayDuration1);
		msg.set_delayduration2(pmsg->DelayDuration2);
		msg.set_delayduration3(pmsg->DelayDuration3);
		msg.set_delaycurrenttime(pmsg->DelayCurrentTime);
		msg.set_package(pmsg->Package);
		dataLenth = msg.ByteSize() + 4;
		ret = (char*)malloc(dataLenth);
		USHORT msgID = Protos_Game60Fishing::ResCmdSyncFish;
		memcpy_s(ret, 2, &msgID, 2);
		USHORT pbcLen = msg.ByteSize();
		memcpy_s(ret + 2, 2, &pbcLen, 2);
		msg.SerializeToArray(ret + 4, msg.ByteSize());
		break;
	}
	case CMD_BULLET://�����ӵ� 6010
	{
		NetCmdBullet* pmsg = (NetCmdBullet*)pCmd;
		ResBulletMessage msg;
		msg.set_bulletid(pmsg->BulletID);
		msg.set_launchertype(pmsg->LauncherType);
		msg.set_angle(pmsg->Angle);
		msg.set_energy(pmsg->Energy);
		msg.set_reboundcount(pmsg->ReboundCount);
		msg.set_lockfishid(pmsg->LockFishID);
		dataLenth = msg.ByteSize() + 4;
		ret = (char*)malloc(dataLenth);
		USHORT msgID = Protos_Game60Fishing::ResBullet;
		memcpy_s(ret, 2, &msgID, 2);
		USHORT pbcLen = msg.ByteSize();
		memcpy_s(ret + 2, 2, &pbcLen, 2);
		msg.SerializeToArray(ret + 4, msg.ByteSize());
		break;
	}
	case CMD_CATCHED://���������Ϣ	6012
	case CMD_BULLET_REDUCTION: //�����ӵ�����6022
	{
		NetCmdCatched* pmsg = (NetCmdCatched*)pCmd;
		ResCatchedMessage msg;
		msg.set_bulletid(pmsg->BulletID);
		msg.set_combo(pmsg->Combo);
		msg.set_goldnum(pmsg->GoldNum);
		msg.set_totalnum(static_cast<int>(pmsg->TotalNum));
		UINT fc_size = (pmsg->GetCmdSize() - sizeof(NetCmdCatched));
		UINT fishCount = fc_size / sizeof(FishCatched)+1;
		if (fishCount > 0)
		{
			for (UINT i = 0; i < fishCount; ++i)
			{
				FishCatchedInfo* oneFihs = msg.add_fishcatchedlist();
				oneFihs->set_catchevent(pmsg->Fishs[i].CatchEvent);
				oneFihs->set_fishid(pmsg->Fishs[i].FishID);
				oneFihs->set_nreward(pmsg->Fishs[i].nReward);
				oneFihs->set_lightingfishid(pmsg->Fishs[i].LightingFishID);
			}
		}
		dataLenth = msg.ByteSize() + 4;
		ret = (char*)malloc(dataLenth);
		USHORT msgID = ((cmdType == CMD_CATCHED) ? Protos_Game60Fishing::ResCatched : Protos_Game60Fishing::ResCatchedFreeze);
		memcpy_s(ret, 2, &msgID, 2);
		USHORT pbcLen = msg.ByteSize();
		memcpy_s(ret + 2, 2, &pbcLen, 2);
		msg.SerializeToArray(ret + 4, msg.ByteSize());
		break;
	}
	case CMD_CLEAR_SCENE://�峡��Ϣ 6014
	{
		NetCmdClearScene* pmsg = (NetCmdClearScene*)pCmd;
		ResClearSceneMessage msg;
		msg.set_cleartype(pmsg->ClearType);
		dataLenth = msg.ByteSize() + 4;
		ret = (char*)malloc(dataLenth);
		USHORT msgID = Protos_Game60Fishing::ResClearScene;
		memcpy_s(ret, 2, &msgID, 2);
		USHORT pbcLen = msg.ByteSize();
		memcpy_s(ret + 2, 2, &pbcLen, 2);
		msg.SerializeToArray(ret + 4, msg.ByteSize());
		break;
	}
	case CMD_CHANGE_LAUNCHER://�л���̨���� 6018
	{
		NetCmdChangeLauncher* pmsg = (NetCmdChangeLauncher*)pCmd;
		ResChangeLauncherMessage msg;
		msg.set_seat(pmsg->Seat);
		msg.set_launchertype(pmsg->LauncherType);
		dataLenth = msg.ByteSize() + 4;
		ret = (char*)malloc(dataLenth);
		USHORT msgID = Protos_Game60Fishing::ResChangeLauncher;
		memcpy_s(ret, 2, &msgID, 2);
		USHORT pbcLen = msg.ByteSize();
		memcpy_s(ret + 2, 2, &pbcLen, 2);
		msg.SerializeToArray(ret + 4, msg.ByteSize());
		break;
	}
	case CMD_CHANGE_RATE://�л����ʷ��� 6020
	{
		NetCmdChangeRate* pmsg = (NetCmdChangeRate*)pCmd;
		ResChangeRateMessage msg;
		msg.set_seat(pmsg->Seat);
		msg.set_rateindex(pmsg->RateIndex);
		msg.set_iscanuserate(pmsg->IsCanUseRate);
		dataLenth = msg.ByteSize() + 4;
		ret = (char*)malloc(dataLenth);
		USHORT msgID = Protos_Game60Fishing::ResChangeRate;
		memcpy_s(ret, 2, &msgID, 2);
		USHORT pbcLen = msg.ByteSize();
		memcpy_s(ret + 2, 2, &pbcLen, 2);
		msg.SerializeToArray(ret + 4, msg.ByteSize());
		break;
	}
	case CMD_PLAYER_LEAVE: //����뿪��Ϸ 6026
	{
		NetCmdPlayerLeave* pmsg = (NetCmdPlayerLeave*)pCmd;
		ResLeaveGameMessage msg;
		msg.set_seat(pmsg->Seat);
		dataLenth = msg.ByteSize() + 4;
		ret = (char*)malloc(dataLenth);
		USHORT msgID = Protos_Game60Fishing::ResLeaveGame;
		memcpy_s(ret, 2, &msgID, 2);
		USHORT pbcLen = msg.ByteSize();
		memcpy_s(ret + 2, 2, &pbcLen, 2);
		msg.SerializeToArray(ret + 4, msg.ByteSize());
		break;
	}
	case 6028://�鿴���������Ϣ
	{
		NetCmdOtherInfo* pmsg = (NetCmdOtherInfo*)pCmd;
		ResLookUpPlayerInfoMessage msg;
		msg.set_playerid(pmsg->playerId);
		msg.set_viplevel(pmsg->vipLevel);
		msg.set_icon(std::string(pmsg->icon));
		msg.set_name(std::string(pmsg->name));
		msg.set_sex(pmsg->sex);
		msg.set_titleid(pmsg->titleId);
		msg.set_gamewinmaxmoney(pmsg->gameWinMaxMoney);
		msg.set_money(pmsg->money);
		msg.set_charmvalue(pmsg->charmValue);
		msg.set_signature(std::string(pmsg->signature));
		msg.set_headicon(pmsg->headIcon);
		dataLenth = msg.ByteSize() + 4;
		ret = (char*)malloc(dataLenth);
		USHORT msgID = Protos_Game60Fishing::ResLookUpPlayerInfo;
		memcpy_s(ret, 2, &msgID, 2);
		USHORT pbcLen = msg.ByteSize();
		memcpy_s(ret + 2, 2, &pbcLen, 2);
		msg.SerializeToArray(ret + 4, msg.ByteSize());
		break;
	}
	case 6030: //֪ͨ������ݼ�����ɣ����Խ�������
	{
		ResConnectFinishMessage msg;
		dataLenth = msg.ByteSize() + 4;
		ret = (char*)malloc(dataLenth);
		USHORT msgID = Protos_Game60Fishing::ResConnectFinish;
		memcpy_s(ret, 2, &msgID, 2);
		USHORT pbcLen = msg.ByteSize();
		memcpy_s(ret + 2, 2, &pbcLen, 2);
		msg.SerializeToArray(ret + 4, msg.ByteSize());
		break;
	}
	case 6032: //���������
	{
		ResPlayerTickOutMessage msg;
		dataLenth = msg.ByteSize() + 4;
		ret = (char*)malloc(dataLenth);
		USHORT msgID = Protos_Game60Fishing::ResPlayerTickOut;
		memcpy_s(ret, 2, &msgID, 2);
		USHORT pbcLen = msg.ByteSize();
		memcpy_s(ret + 2, 2, &pbcLen, 2);
		msg.SerializeToArray(ret + 4, msg.ByteSize());
		break;
	}
	case 6034:
	{
		LG_SyncAddMoney* pmsg = (LG_SyncAddMoney*)pCmd;
		ResSyncAddMoneyMessage msg;
		msg.set_add_goldnum(pmsg->add_goldNum);
		msg.set_reason(pmsg->reason);
		dataLenth = msg.ByteSize() + 4;
		ret = (char*)malloc(dataLenth);
		USHORT msgID = Protos_Game60Fishing::ResSyncAddMoney;
		memcpy_s(ret, 2, &msgID, 2);
		USHORT pbcLen = msg.ByteSize();
		memcpy_s(ret + 2, 2, &pbcLen, 2);
		msg.SerializeToArray(ret + 4, msg.ByteSize());
		break;
	}
	default:
		isPBC = false;
		ret = (char*)pCmd;
		//Log("PCB_Eecode unkown data:cmdSize=%d,cmdType=%d,mainType=%d,subType=%d", pCmd->GetCmdSize(), pCmd->GetCmdType(), pCmd->GetCmdType() >> 8, pCmd->GetCmdType() & 0xff);
		dataLenth = pCmd->GetCmdSize();
	}
	return ret;
}