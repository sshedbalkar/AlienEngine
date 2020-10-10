#include "Precompiled.h"
#include "AutoMovingComponent.h"
#include "Hero.h"
#include "GameLogic.h"
#include "ModelComponent.h"
#include "TimeMechanicManager.h"
#include "ActivationProperties.h"

namespace Framework
{

	AutoMovingComponent::AutoMovingComponent()
	{
		m_timer=0.0f;
		m_autoCanMove=true;
		m_autoCanRotate=true;
		m_allowRotate=false;
		m_autoCanMove=false;
		m_Body=0;
		m_rotationAxis=Vec3(0,0,0);
		m_rotatingSpeed=0.0f;
		m_movingSpeed=0.0f;
		m_MovingAxis=Vec3(0,0,0);
		reverseRotation=1;
		reverseMovement=1;
	}

	void AutoMovingComponent::Initialize()
	{//when we change or restart level
		m_Body = this->GetOwner()->has(RigidBody);
		m_transform = this->GetOwner()->has(Transform);
		m_initialPos=m_Body->getPosition();
		objAttachedToThis.clear();
	}

	void AutoMovingComponent::SendMessage(Message *m)
	{

		switch( m->MessageId )
		{

		case Mid::Collide:
		{
			MessageCollide * msg = (MessageCollide*)m;
			if(msg->CollidedWith)
			{
			}
			break;
		}				

		}
	}

	void AutoMovingComponent::RemoveComponentFromGame()
	{
	}

	



	void AutoMovingComponent::Serialize(ISerializer& stream)
	{

	}


	void AutoMovingComponent::StartRotation()
	{
		for(unsigned int i=0;i<objAttachedToThis.size();++i)
		{
			objAttachedToThis[i]->has(ActivationProperties)->SetMovedThisFrame(true);
			
		}
		m_allowRotate=true;
	}


	void AutoMovingComponent::StopRotation()
	{
		for(unsigned int i=0;i<objAttachedToThis.size();++i)
		{
			objAttachedToThis[i]->has(ActivationProperties)->SetMovedThisFrame(false);
			
		}
		m_allowRotate=false;
	}

	void AutoMovingComponent::SetReverseRotation(bool t)
	{
		if(t)
			reverseRotation=-1;
		else
			reverseRotation= 1;
	}

	void AutoMovingComponent::StartMovement()
	{
		for(unsigned int i=0;i<objAttachedToThis.size();++i)
		{
			objAttachedToThis[i]->has(ActivationProperties)->SetMovedThisFrame(true);
			
		}
		m_allowMove=true;
	}


	void AutoMovingComponent::StopMovement()
	{
		for(unsigned int i=0;i<objAttachedToThis.size();++i)
		{
			objAttachedToThis[i]->has(ActivationProperties)->SetMovedThisFrame(false);
			
		}
		m_allowMove=false;
	}

	void AutoMovingComponent::SetReverseMovement(bool t)
	{
		if(t)
			reverseMovement=-1;
		else
			reverseMovement= 1;
	}


	void AutoMovingComponent::UpdateMovement(const float & dt)
	{
		if (m_autoCanMove && m_allowMove)
		{
			Vector3 vec=m_Body->getPosition ();
			Vector3 move;
			move.x = int(m_MovingAxis.x) * reverseMovement * dt*m_movingSpeed;
			move.y = int(m_MovingAxis.y) * reverseMovement * dt*m_movingSpeed;
			move.z = int(m_MovingAxis.z) * reverseMovement * dt*m_movingSpeed;
			vec+=move;
			m_Body->setPosition(vec);	
			m_transform->Position=Vector3ToVec3(vec);
			UpdateAttachedObjectsPosition(move);
		}
	}

	void AutoMovingComponent::Update(const float & dt)
	{
		
		if (TIMEMANAGER->GetTimeState() != TIME_NORMAL ) 
			return;
		UpdateRotation(dt);
		UpdateMovement(dt);

	}


	void AutoMovingComponent::UpdateRotation(const float & dt)
	{
		if (m_autoCanRotate && m_allowRotate)
		{
			D3DXQUATERNION q=calculateQuaterion(m_rotatingSpeed*dt,m_rotationAxis*(float)reverseRotation);//rotation axis
			m_transform->QUATERNION*=q;
			m_Body->setOrientation(m_transform->QUATERNION);
			UpdateAttachedObjectsRotation(q);
		}
	}


	void AutoMovingComponent::UpdateAttachedObjectsPosition(const Vector3 & move)
	{

		for(unsigned int i=0;i<objAttachedToThis.size();++i)
		{
			//update position here, remember they may not have a body
			RigidBody *body= NULL;
			body= objAttachedToThis[i]->has(RigidBody);
			Vector3 vec = objAttachedToThis[i]->has(Transform)->Position;
			vec += move;
			objAttachedToThis[i]->has(Transform)->Position = Vector3ToVec3(vec);
			if(body != NULL)
				body->setPosition(vec);	
		}
	}

	void AutoMovingComponent::UpdateAttachedObjectsRotation(const D3DXQUATERNION &q )
	{
		for(unsigned int i=0;i<objAttachedToThis.size();++i)
		{
			RigidBody *body= NULL;
			Transform *transform = objAttachedToThis[i]->has(Transform);
			body= objAttachedToThis[i]->has(RigidBody);

			transform->QUATERNION*=q;
			if(body != NULL)
				body->setOrientation(transform->QUATERNION);

			Quaternion localQ=Quaternion(q.w,q.x,q.y,q.z);
			Vector3 a=m_Body->getPosition();
			Vec3 temp= transform->Position - Vector3ToVec3(a);
			Vector3 dir=temp;
			Vector3 newPos=((localQ*dir)*dir.magnitude())+a;
			transform->Position=Vector3ToVec3(newPos);
			if(body)
				body->setPosition(newPos);
		}
	}
}