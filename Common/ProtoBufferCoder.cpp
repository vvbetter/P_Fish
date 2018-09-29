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
//proto buffer 解码。
//与游戏大厅和游戏客户端消息通讯消息不一样。
//游戏大厅 2字节消息长度（后面的消息长度，不包含这2字节）+ 8字节玩家ID + 2字节消息ID + n字节的PBC消息
//客户端 2字节消息ID + 2字节消息长度（只是后面PBC的消息长度) + n字节的PBC消息
//内部通讯的NetCmd 2字节的消息长度（全部长度）+ 8字节的UID + 2字节的消息ID + 消息体
// proto buffer 编码



void PrintLogInCmd( USHORT cmdType)
{
#ifdef _DEBUG
	USHORT logCmeType[] = { 6003, 6004, 6030, 6017, 6018, 6032, 6031, 6025, 6026 };
	for (USHORT& x : logCmeType)
	{
		if (cmdType == x)
		{
			Log("打印消息 cmdType=%d ", x);
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
	//游戏大厅的这些请求不处理
	if (cmdType == GetMsgType(159, 24) || cmdType == GetMsgType(65, 24) || cmdType == GetMsgType(156, 24)) //6303 同步配置表// 6209 请求退出子游戏//6300 时时彩LS请求GS下注
	{
		return NULL;
		//Log("Ignore CmdType=%d", cmdType);
	}
	//大厅消息
	else if (cmdType == GetMsgType(162, 24))//6306 使用物品
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
	else if (cmdType == GetMsgType(Main_HallHeartBeat, 0))//游戏大厅连接消息，心跳
	{
		pCmd = CreateCmd(length + sizeof(USHORT), buffer);
	}
	else if (cmdType == GetMsgType(67, 24)) //6211 请求金币改变，一般情况是充值原因导致的
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
	else if (cmdType == GetMsgType(63, 24))//6207 请求进入子游戏
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
	else if (cmdType == GetMsgType(71, 24))//6215 通知关闭捕鱼服务器
	{
		pCmd = CreateCmd(sizeof(LG_StopFishServer));
		pCmd->SetCmdType(GetMsgType(Main_Hall, 71));
		LGStopFishServerMessage msg;
		msg.ParseFromArray(buffer + sizeof(NetCmd), length - PBC_HEADER_SIZE);
		if (msg.has_rpcid()) ((LG_StopFishServer*)pCmd)->rcpid = msg.rpcid();
		if (msg.has_closetime()) ((LG_StopFishServer*)pCmd)->closeTime = msg.closetime();
	}
	else if (cmdType == GetMsgType(72, 24)) //6216 通知加载捕鱼配置表
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
	else if (cmdType == GetMsgType(73, 24)) //6217 通知游戏大厅允许玩家进入
	{
		pCmd = CreateCmd(sizeof(CL_HallStartFishServer));
		pCmd->SetCmdType(GetMsgType(Main_Hall, 73));
		LGStartFishServerMessage msg;
		msg.ParseFromArray(buffer + sizeof(NetCmd), length - PBC_HEADER_SIZE);
		if (msg.has_starttime()) ((CL_HallStartFishServer*)pCmd)->starTime = msg.starttime();
	}
	//游戏客户端消息
	else if (cmdType == 6003)//请求进入房间
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
	else if (cmdType == 6009)//发送子弹
	{
		pCmd = CreateCmd(sizeof(NetCmdBullet));
		pCmd->SetCmdType(CMD_BULLET);
		ReqBulletMessage msg;
		msg.ParseFromArray(buffer + 4, length);
		if (msg.has_angle()) ((NetCmdBullet*)pCmd)->Angle = msg.angle();
		if (msg.has_lockfishid()) ((NetCmdBullet*)pCmd)->LockFishID = msg.lockfishid();
		if (msg.has_launchertype()) ((NetCmdBullet*)pCmd)->LauncherType = msg.launchertype();
	}
	else if (cmdType == 6017)//请求切换炮台
	{
		pCmd = CreateCmd(sizeof(NetCmdChangeLauncherType));
		pCmd->SetCmdType(CMD_CHANGE_LAUNCHER_TYPE);
		ReqChangeLauncherMessage msg;
		msg.ParseFromArray(buffer + 4, length);
		if (msg.has_seat()) ((NetCmdChangeLauncherType*)pCmd)->Seat = msg.seat();
		if (msg.has_launchertype()) ((NetCmdChangeLauncherType*)pCmd)->LauncherType = msg.launchertype();
	}
	else if (cmdType == 6019)//更改炮的倍率
	{
		pCmd = CreateCmd(sizeof(NetCmdClientChangeRate));
		pCmd->SetCmdType(CMD_CHANGE_CLIENT_RATE);
		ReqChangeRateMessage msg;
		msg.ParseFromArray(buffer + 4, length);
		if (msg.has_uprodownrate()) ((NetCmdClientChangeRate*)pCmd)->UpRoDownRate = msg.uprodownrate();
	}
	else if (cmdType == 6025)//请求离开游戏
	{
		pCmd = CreateCmd(sizeof(CL_LeaveTable));
		pCmd->SetCmdType(GetMsgType(Main_Table, CL_Sub_LeaveTable));
	}
	else if (cmdType == 6027)//请求查看玩家信息
	{
		pCmd = CreateCmd(sizeof(NetCmdClientShowOthers));
		pCmd->SetCmdType(GetMsgType(Main_Table, LC_Sub_ShowOthers));
		ReqLookUpPlayerInfoMessage msg;
		msg.ParseFromArray(buffer + 4, length);
		if (msg.has_playerid()) ((NetCmdClientShowOthers*)pCmd)->playerid = msg.playerid();
	}
	else if (cmdType == 6031)//客户端准备完成可以同步鱼了
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
	//大厅消息
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
	case GetMsgType(64, 24): //6208 请求进入子游戏返回
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
	case GetMsgType(68, 24): //同步玩家数据到游戏大厅 6212
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
		//解析成就信息
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
		//Log("玩家%lld退出游戏,赢的次数:%d,输的次数:%d, 总赢的钱%lf,总输的钱%lf", pmsg->uid, pmsg->winNum, pmsg->loseNum, pmsg->winMoney, pmsg->loseMoney);
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
	//游戏客户端消息
	case GetMsgType(Main_Table, LC_Sub_JoinTable): //返回进入房间消息 6004
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
	case GetMsgType(Main_Table, LC_Sub_OtherUserInfo): //玩家加入同步其他玩家信息 6016
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
	case CMD_SKILL_DOUBLE_FIRE: //使用狂暴道具 6036
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
	case CMD_SKILL_TIMESTOP: //玩家使用全屏冰冻道具 6038
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
	case CMD_SYNC_BULLET:  //同步场景中子弹 6024
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
	case CMD_FISH://同步场景中的鱼 6006
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
	case CMD_SYNC_FISH://同步场景中的异步的鱼 6008
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
	case CMD_BULLET://接收子弹 6010
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
	case CMD_CATCHED://命中鱼的消息	6012
	case CMD_BULLET_REDUCTION: //冰冻子弹命中6022
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
	case CMD_CLEAR_SCENE://清场消息 6014
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
	case CMD_CHANGE_LAUNCHER://切换炮台返回 6018
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
	case CMD_CHANGE_RATE://切换倍率返回 6020
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
	case CMD_PLAYER_LEAVE: //玩家离开游戏 6026
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
	case 6028://查看其它玩家信息
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
	case 6030: //通知玩家数据加载完成，可以进房间了
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
	case 6032: //踢玩家下线
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