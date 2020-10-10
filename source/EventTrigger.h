#pragma once //Makes sure this header is only included once

#include "PhysicsMath.h"
#include "Composition.h"

namespace Framework
{
	///The transform component provides a shared position and rotation.
	class EventTrigger : public GameComponent
	{
	public:	
		EventTrigger();
		~EventTrigger();

		virtual void Initialize();
		virtual void Serialize(ISerializer& stream);
		virtual void SendMessage(Message *m);

		EventTrigger * Next;
		EventTrigger * Prev;
	};
}
