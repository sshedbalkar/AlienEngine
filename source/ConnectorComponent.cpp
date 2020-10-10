#include "Precompiled.h"
#include "ConnectorComponent.h"
#include "Hero.h"
#include "GameLogic.h"
#include "ModelComponent.h"

namespace Framework
{

	ConnectorComponent::ConnectorComponent()
	{
		m_isVertical=false;
		m_canCollideWithStatic=false;
	}

	void ConnectorComponent::Initialize()
	{

	}

	void ConnectorComponent::SendMessage(Message *m)
	{

		switch( m->MessageId )
		{

		case Mid::Collide:
		{
			break;
		}				

		}
	}

	void ConnectorComponent::RemoveComponentFromGame()
	{

	}

	

	void ConnectorComponent::Serialize(ISerializer& stream)
	{

	}




	void ConnectorComponent::Update(const float & dt)
	{


	}
}