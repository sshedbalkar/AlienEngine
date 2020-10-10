#include "Precompiled.h"
#include "DebugComponent.h"
#include "Hero.h"
#include "GameLogic.h"
#include "ModelComponent.h"

namespace Framework
{

	DebugComponent::DebugComponent()
	{
		m_IsCube=false;
	}

	void DebugComponent::Initialize()
	{
	}


	

	void DebugComponent::SendMessage(Message *m)
	{

		switch( m->MessageId )
		{

		case Mid::Collide:
		{
			
			break;
		}				

		}
	}



	void DebugComponent::Serialize(ISerializer& stream)
	{

	}

	void DebugComponent::Update(float & dt)
	{
		
	}

}