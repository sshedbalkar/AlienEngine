#pragma once //Makes sure this header is only included once
#include "Composition.h"
#include "Message.h"
#include <iostream>
#include "Physics.h"

namespace Framework
{
	class RopeComponent: public GameComponent
	{
		friend class TweakBarManager;
		friend class LoadLevelClass;
		friend class SaveLevelClass;
	public:
		RopeComponent();
		~RopeComponent(){}
		virtual void SendMessage(Message *);
		virtual void Initialize();
		virtual void Serialize(ISerializer& stream);
		virtual void RemoveComponentFromGame();
		void Update(float &);	
		int GetId(){return Id;}
	private:
		RigidBody * m_Body;
		bool m_isStartPoint;
		int Id;
		int AttachedId;		
	};
}
