#include "Precompiled.h"
#include "PickedObjectHolder.h"
#include "GameLogic.h"
//#include "Physics.h"

namespace Framework
{

	PickedObjectHolder::PickedObjectHolder()
	{
		
	}

	PickedObjectHolder::~PickedObjectHolder()
	{
	}


	void PickedObjectHolder::Initialize()
	{ 
		m_Transform = this->GetOwner()->has(Transform);
	}



	void PickedObjectHolder::Update(const float & dt)
	{
		if(POWERGUN->gravityGun.GetPreviouslyPickedItem())
		{
			Transform * pickedTranf=POWERGUN->gravityGun.GetPreviouslyPickedItem()->has(Transform);
			RigidBody * pickedBody=POWERGUN->gravityGun.GetPreviouslyPickedItem()->has(RigidBody);
			Vector3 dist=m_Transform->Position-pickedTranf->Position;
			real d=dist.magnitude();
			if(d<1.7)
			{
				pickedTranf->QUATERNION=D3DXQUATERNION(0,0,0,1);
				pickedTranf->Position=m_Transform->Position;
				pickedTranf->Position.y+=m_Transform->Scale.y*0.5f + pickedTranf->Scale.y*0.5f ;
				pickedBody->setPosition(pickedTranf->Position);
				pickedBody->setOrientation(pickedTranf->QUATERNION);
				POWERGUN->gravityGun.SetPreviouslyPickedItem(0);
			}
		}
	}

	void PickedObjectHolder::SendMessageA(Message *m)
	{
	}


	void PickedObjectHolder::Serialize(ISerializer& stream)
	{	

	}

	void PickedObjectHolder::RemoveComponentFromGame()
	{	

	}


}

