#include "Precompiled.h"
#include "Dying.h"
#include "Physics.h"
#include "Hero.h"
#include "GameLogic.h"
#include "ModelComponent.h"
#include "TimeMechanicManager.h"
#include "GameHistory.h"

namespace Framework
{

	Dying::Dying()
	{
		m_isDying=false;
		m_dyingRate=1.0f;
	}

	Dying::~Dying()
	{
		if(!this->GetOwner()->OnRemovedList)
		{
			LOGIC->DyingComponentList.erase(this);
		}

	}
	
	void Dying::Initialize()
	{
		LOGIC->DyingComponentList.push_back(this);
		if(GetOwner()->CompositionTypeId==GOC_Type_Rope)
		{
			m_dyingRate=3.0f;
		}
	}

	void Dying::RemoveComponentFromGame()
	{
		LOGIC->DyingComponentList.erase(this);
	}
	void Dying::RestoreComponentToGame ()
	{
		LOGIC->DyingComponentList.push_back(this);
	}

	void Dying::SendMessage(Message *m)
	{

		switch( m->MessageId )
		{

		case Mid::Collide:
		{
			
			break;
		}				

		}
	}

	void Dying::MarkAsDying()
	{
		if(!m_isDying)//not already marked
		{
			if(POWERGUN->gravityGun.GetPickedItem() && POWERGUN->gravityGun.GetPickedItem()==this->GetOwner())
			{
				POWERGUN->gravityGun.RemovePickedItem();
			}
			if(this->GetOwner()->CompositionTypeId==GOC_Type_Cube )
			{
				this->GetOwner()->has(RigidBody)->canCollide=false;
			}
			this->GetOwner()->has(ModelComponent)->SwapTexture();
			this->GetOwner()->has(ModelComponent)->Transparency=1.0; 
			m_isDying=true;
		}
	}

	void Dying::SetIsDying(bool t)//used for timemechanic
	{
		if(m_isDying!=t)
		{
			this->GetOwner()->has(ModelComponent)->SwapTexture();
		}
		m_isDying=t;
		if (!t)
			this->GetOwner()->has(RigidBody)->canCollide=true;

	}

	void Dying::SetTransparrency(float t)//used for timemechanic
	{
		this->GetOwner()->has(ModelComponent)->Transparency=t;
	}


	void Dying::Serialize(ISerializer& stream)
	{

	}

	void Dying::Update(const float & dt)
	{
		if(m_isDying)
		{
			if(TIMEMANAGER->GetTimeState()==TIME_NORMAL)
			{
				this->GetOwner()->has(ModelComponent)->Transparency-=m_dyingRate*dt; 
				if(this->GetOwner()->has(ModelComponent)->Transparency<0.0f)
				{
					this->GetOwner()->has(ModelComponent)->Transparency=0.0f;
					this->GetOwner()->Destroy();
				}
			}
			
		}
	}

}