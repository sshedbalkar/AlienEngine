#include "StoreMessage.h"

//
namespace Framework
{
	StoreMessage::StoreMessage()
		:m_msgCurr(0),
		m_msgPrev(0),
		m_msgIdToStore(Mid::None)
	{
		//
	}
	StoreMessage::~StoreMessage()
	{
		delete m_msgCurr;
		delete m_msgPrev;
	}
	void StoreMessage::storeMsgType(int id)
	{
		m_msgIdToStore = id;
	}
	void StoreMessage::SendMessage(Message *msg)
	{
		if(msg->MessageId == Mid::Collide )
		{
			m_msgPrev = m_msgCurr;
			m_msgCurr = msg->clone();
		}
	}
	void StoreMessage::Initialize()
	{
		//
	}
	void StoreMessage::Serialize(ISerializer& stream)
	{
		static_cast<void>(stream);
		//ISerializer* str = stream.clone();
		//if(str->isGood())
		//{
		//	int id;
		//	str->readProperty("MsgId", *str);
		//	str->readValue("value", id);
		//	//
		//	m_msgIdToStore = id;
		//}
		//delete str;
	}
	Message* StoreMessage::getStoredMsg()
	{
		return m_msgPrev;
	}
}