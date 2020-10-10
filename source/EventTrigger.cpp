#include "Precompiled.h"
#include "EventTrigger.h"
#include "Physics.h"
#include "TimeMechanicManager.h"

namespace Framework
{
	EventTrigger::EventTrigger()
	{

	}

	EventTrigger::~EventTrigger()
	{
		PHYSICS->Events.erase(this);
	}

	void EventTrigger::Serialize(ISerializer& stream)
	{
		
	}

	void EventTrigger::Initialize()
	{
		PHYSICS->Events.push_back(this);
	}

	void EventTrigger::SendMessage(Message *m)
	{
		switch( m->MessageId )
		{
			case Mid::EventTrigger:
				{
					TimeState t =TimeState::TIME_NORMAL;
					MessageTimeStateChange* newMsg = new MessageTimeStateChange(t); 
					TIMEMANAGER->SendMessageA(newMsg);
					//GetOwner()->Destroy();

				}
		}
	}
}



