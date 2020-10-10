#include "Precompiled.h"
#include "LightComponent.h"
#include "Transform.h"

namespace Framework
{

	LightComponent::LightComponent()
	{
		Color = Vec3(1.0,1.0,1.0);
		fallout = 5.0f;
		CanMove=false;
	}

	void LightComponent::Initialize()
	{//when we change or restart level
		prevPos=this->GetOwner()->has(Transform)->Position;
	}

	void LightComponent::SendMessage(Message *m)
	{

		
	}
	bool LightComponent::UpdatePrevPos()
	{
		if (this->GetOwner()->has(Transform)->Position!=prevPos && CanMove)
		{
			prevPos=this->GetOwner()->has(Transform)->Position;
			return true;
		}
		else
			return false;
	}


	void LightComponent::Serialize(ISerializer& stream)
	{

	}

	void LightComponent::RemoveComponentFromGame()
	{
	}

	void LightComponent::Update(float & dt)
	{
		
		
	}

}