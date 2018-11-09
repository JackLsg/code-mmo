#pragma once
#include "Message.h"
#include "Service/GeneralMessage.h"
#include "Service/MessageId.h"

typedef MessagePtr (*CreatePacketHdl)();

class MsgFactryMgr
{
public:
	MsgFactryMgr();
	~MsgFactryMgr();

	virtual MessagePtr CreatePacket(int cmd);
	virtual bool DelPacket(Message*packet);

	bool RegCreatePacketHdl(int cmd, CreatePacketHdl func);
private:
	CreatePacketHdl hdlList[MessageId::MaxMessageId];
};

extern MsgFactryMgr gMsgFactryMgr;
