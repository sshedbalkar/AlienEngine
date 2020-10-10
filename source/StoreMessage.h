#ifndef STORE_MESSAGE_H
#define STORE_MESSAGE_H
//
#include "Composition.h"
#include "Message.h"
//
namespace Framework
{
	class StoreMessage: public GameComponent
	{
	public:
		StoreMessage();
		~StoreMessage();
		//
		void SendMessage(Message *msg);
		void Initialize();
		void Serialize(ISerializer& stream);
		void storeMsgType(int id);
		//
		Message* getStoredMsg();
	private:
		Message* m_msgCurr;
		Message* m_msgPrev;
		int m_msgIdToStore;
	};
}
//
#endif