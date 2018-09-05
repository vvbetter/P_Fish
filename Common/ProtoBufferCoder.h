#pragma once
#include "NetCmd.h"
#include "Cmd_Fish.h"
#include "proto_source/LoginMessage.pb.h"
using namespace std;

//proto buffer ½âÂë
NetCmd* PBC_Decode(const USHORT cmdType, const char* buffer, const int length,bool& isPBC);
// proto buffer ±àÂë
char* PBC_Encode(NetCmd* pCmd,uint& dataLenth, bool& isPBC);
