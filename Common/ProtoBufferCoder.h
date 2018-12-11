#pragma once
#include "NetCmd.h"
#include "Cmd_Fish.h"
#include "proto_source/LoginMessage.pb.h"
using namespace std;

//#define __NGNEX_ZHUANFA
#ifdef __NGNEX_ZHUANFA
#	define _HTONLL_ 
#	define _NTOHLL_ 
#else
#	define _HTONLL_ htonll
#	define _NTOHLL_ ntohll
#endif // __NGNEX_ZHUANFA

//proto buffer ½âÂë
NetCmd* PBC_Decode(const USHORT cmdType, const char* buffer, const int length);
// proto buffer ±àÂë
char* PBC_Encode(NetCmd* pCmd,uint& dataLenth);
