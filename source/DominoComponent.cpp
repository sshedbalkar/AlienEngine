#include "Precompiled.h"
#include "DominoComponent.h"
#include "Hero.h"
#include "GameLogic.h"
#include "ModelComponent.h"
#include "ConstraintsComponent.h"
#include "TimeMechanicManager.h"

namespace Framework
{

	Domino::Domino()
	{
		m_timer=0.0f;
		m_active=false;
		m_Finished=false;
		firstCollide=false;
		m_Body=0;
		onX=false;
		leftToRight=false;
		m_stoptimer=0.0f;
		stopped=false;
	}

	void Domino::Initialize()
	{//when we change or restart level
		m_Body = this->GetOwner()->has(RigidBody);
		m_initialPos=m_Body->getPosition();
		m_inverseInitialQuat= m_Body->getOrientation().getConjugate();
	}

	void Domino::RemoveComponentFromGame()
	{
	}

	

	void Domino::SendMessage(Message *m)
	{

		switch( m->MessageId )
		{

		case Mid::Collide:
		{
			MessageCollide * msg = (MessageCollide*)m;
			if(msg->CollidedWith)
			{
				if(msg->CollidedWith->CompositionTypeId==GOC_Type_Domino && msg->CollidedWith->GlobalId > GetOwner()->GlobalId)
				{
					//stopped=true;
					m_Body->setPosition(m_Body->prev_position);
					m_Body->setOrientation(m_Body->prev_orientation);

					GetOwner()->has(Transform)->Position=Vector3ToVec3(m_Body->getPosition());
					GetOwner()->has(Transform)->QUATERNION=D3DXQUATERNION(m_Body->getOrientation().i,m_Body->getOrientation().j,m_Body->getOrientation().k,m_Body->getOrientation().r);
				}
			}
			break;
		}				

		}
	}

	


	void Domino::Serialize(ISerializer& stream)
	{

	}

	void Domino::MarkAsFinished(bool t)
	{
		if(m_Finished!=t)
		{
			if(t)
			{
				//m_Body->makeObjectStatic();
			}
			else
			{
			//	m_Body->ResetOriginalDataTEMP();
			}
			m_Finished=t;
		}
		
	}

	void Domino::Update(const float & dt)
	{
		if (TIMEMANAGER->GetTimeState() != TimeState::TIME_NORMAL ) 
			return;
		
		//if(stopped)
		//{
		//	m_Body->setAcceleration(0,-10,0);
		//	/*m_Body->setPosition(m_Body->prev_position);
		//	m_Body->setOrientation(m_Body->prev_orientation);

		//	GetOwner()->has(Transform)->Position=Vector3ToVec3(m_Body->getPosition());
		//	GetOwner()->has(Transform)->QUATERNION=D3DXQUATERNION(m_Body->getOrientation().i,m_Body->getOrientation().j,m_Body->getOrientation().k,m_Body->getOrientation().r);*/

		//	m_stoptimer+=dt;
		//	if(m_stoptimer > 0.1f)
		//	{
		//		stopped=false;
		//		m_stoptimer=0.0f;
		//		m_Body->setAcceleration(0,-20,0);
		//	}

		//}

		

		if(m_Body->getAwake() && !m_Finished)
		{
			//m_timer+=dt;
			//
			//if((acos(m_Body->getOrientation().r)*2)>1.36135682     )
			//{
			//	//m_initialPos  HARSHA
			//	m_Body->canCollide=false;
			
			//	m_Body->setVelocity(0,0,0);
			//	m_Body->setRotation(0,0,0);
			//	m_Body->setAcceleration(0,0,0);
			//	m_Finished=true;
			//}

			Quaternion q= m_Body->getOrientation()*m_inverseInitialQuat;
			real angle = acos(q.r)*2.0;
			angle*= (180.0/3.14159);
		
		/*	if(this->GetOwner()->has(ConstraintComponent)->GetId()==30 && abs(angle)>70)
			{
				m_Finished=true;
				m_Body->setAwake(false);
			}*/
			if(abs(angle)>77)
			{
				m_Body->makeObjectStatic();
				MarkAsFinished(true);
				m_Body->setAwake(false);
			//	m_Body->setIsStatic(true);
			}
			
		}
		else
		{
			m_Body->setAwake(false);
		}
	}

}