#include "Precompiled.h"
#include "ConstraintsComponent.h"
#include "Hero.h"
#include "GameLogic.h"
#include "ModelComponent.h"

namespace Framework
{

	ConstraintComponent::ConstraintComponent()
	{
		m_Body=0;
		Id=-1;
		JoinedToIds.clear();
		PosRelative=Vector3(0,0,0);
		AttachedToId=-1;
		err=0.0;
		isInsideBody=false;
		multiplier=Vec3(0,0,0);
	}

	void ConstraintComponent::Initialize()
	{//when we change or restart level
		m_Body = this->GetOwner()->has(RigidBody);
		/*WorldPosition=m_Body->getPosition();
		Id=-1;
		Constrains.clear();
		JoinedToIds.clear();
		PosRelative=Vector3(0,0,0);
		err=0.0;*/
	}

	void ConstraintComponent::SendMessage(Message *m)
	{

		switch( m->MessageId )
		{

		case Mid::Collide:
		{
			
			break;
		}				

		}
	}



	void ConstraintComponent::Serialize(ISerializer& stream)
	{

	}

	void ConstraintComponent::RemoveComponentFromGame()
	{

	}

	

	void ConstraintComponent::Update(float & dt)
	{

	}

}