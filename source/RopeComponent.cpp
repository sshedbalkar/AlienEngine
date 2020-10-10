#include "Precompiled.h"
#include "RopeComponent.h"
#include "Hero.h"
#include "GameLogic.h"
#include "ModelComponent.h"

namespace Framework
{

	RopeComponent::RopeComponent()
	{
		m_Body=0;
		m_isStartPoint=false;
		Id=0;
		AttachedId=0;		
	}

	void RopeComponent::Initialize()
	{//when we change or restart level
		m_Body = this->GetOwner()->has(RigidBody);
	}

	void RopeComponent::SendMessage(Message *m)
	{

		switch( m->MessageId )
		{

		case Mid::Collide:
		{
			
			break;
		}				

		}
	}
	void RopeComponent::RemoveComponentFromGame()
	{}



	void RopeComponent::Serialize(ISerializer& stream)
	{

	}

	void RopeComponent::Update(float & dt)
	{
		
	}

}