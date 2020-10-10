#pragma once //Makes sure this header is only included once
#include "Composition.h"
#include "Transform.h"

namespace Framework
{
	
	class PickedObjectHolder : public GameComponent
	{
	public:
		PickedObjectHolder();
		~PickedObjectHolder();

		virtual void SendMessage(Message *);
		virtual void Initialize();
		virtual void Serialize(ISerializer& stream);
		virtual void RemoveComponentFromGame();
		void Update(const float &);
	private:
		Transform * m_Transform;
	};
}