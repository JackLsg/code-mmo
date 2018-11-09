#include "MsgFctryMgr.h"


#define MSGCREATE(MSG) \
	case MessageId::MSGIDDEFINEBASE(MSG):\
		{ MessagePtr ptr = POOLDEF_NEW(MSG); return ptr; } \
		break;\



MsgFactryMgr::MsgFactryMgr()
{
	memset(hdlList, 0, sizeof(hdlList));
}

MsgFactryMgr::~MsgFactryMgr()
{

}

MsgFactryMgr gMsgFactryMgr;

MessagePtr MsgFactryMgr::CreatePacket(int cmd)
{
	if (hdlList[cmd])
	{
	 	return hdlList[cmd]();
	}
	return NULL;
}

bool MsgFactryMgr::DelPacket(Message*packet)
{
	return false;
}

bool MsgFactryMgr::RegCreatePacketHdl(int cmd, CreatePacketHdl func)
{
	if (cmd < 0 || cmd >=  sizeof(hdlList)/sizeof(hdlList[0]) )
	{
		return false;
	}

	if (hdlList[cmd])
	{
		return false;
	}

	hdlList[cmd] = func;

	return true;
}
