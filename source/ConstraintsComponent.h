#pragma once //Makes sure this header is only included once
#include "Composition.h"
#include "Message.h"
#include <iostream>
#include "Physics.h"
#include <vector>

namespace Framework
{
	class ConstraintComponent: public GameComponent
	{
		friend class TweakBarManager;
		friend class SaveLevelClass;
		friend class LoadLevelClass;
	public:
		ConstraintComponent();
		~ConstraintComponent(){}
		virtual void SendMessage(Message *);
		virtual void Initialize();
		virtual void Serialize(ISerializer& stream);
		virtual void RemoveComponentFromGame();
		void Update(float &);	
		int GetId(){return Id;}
	private:
		RigidBody * m_Body;
		int AttachedToId;
		int Id;
		std::vector<int> JoinedToIds;
		Vector3 PosRelative;
		real err;
		bool isInsideBody;
		Vec3 multiplier;
	};
}