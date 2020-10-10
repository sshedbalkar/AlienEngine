#include "Precompiled.h"
#include "ActivationProperties.h"
#include <algorithm>
#include <vector>
#include "GameLogic.h"
#include "Audio.h"
#include "AutoMovingComponent.h"
#include "Dying.h"
#include "ModelComponent.h"
#include "Laser.h"
#include "LoadLevelClass.h"
#include "ModelComponent.h"

namespace Framework
{
	ActivationProperties::ActivationProperties()
	{
		m_ResetOnLoad=0;
		m_IsActivated=false;
		m_denyActivation=false;
		velocity=3;
		obj=NULL;
		m_timerLimit=0.0f;
		m_timer=0.0f;
		m_attachedObj=0;
		m_attachedToId=0;
		m_neverActivate=false;
		m_restrictedById=0;
		m_oneTimeActivated=false;
		m_initialSpeed=0.0f;
		m_ActivatedByVec=Vec4(0,0,0,0);
		m_pointing=Vec3(0,0,0);
		m_timeToStayActivated=0.0f;
		m_activatedTimer=0.0f;
		m_PreviousActivationState=false;
		throwerBindedObjId=-1;
		m_flagForHistoryComponent=true;
		m_shootingSpeed=0.0f;
		m_movedThisFrame=false;
	}

	ActivationProperties::~ActivationProperties()
	{
	}


	void ActivationProperties::Initialize()
	{ 
		m_owner = this->GetOwner();
		m_Transform = m_owner->has(Transform);
		if(this->GetOwner()->has(RigidBody)){
			m_Body = this->GetOwner()->has(RigidBody);
			m_InitialPos = m_Body->getPosition();
			m_inverseInitialQuat= m_Body->getOrientation().getConjugate();
			m_originalTransform.setOrientationAndPos(m_inverseInitialQuat.getConjugate(),m_InitialPos);
			m_prevPos=m_InitialPos;
		}
		else
		{
			m_Body=0;
			m_InitialPos =  m_Transform->Position;
			m_inverseInitialQuat = m_Transform->QUATERNION;
		}
		

		if(this->GetOwner()->CompositionTypeId==GOC_Type_Cube_Thrower_Periodic )
		{
			m_timerLimit=10.0f;
			m_timer=(rand() % int(m_timerLimit)*100 + 100)/100.0f;//from 0.1 to timer limit
		}
		if(this->GetOwner()->CompositionTypeId==GOC_Type_Sphere_Thrower )
		{
			//m_initialSpeed=15.7f;
		}

		if(this->GetOwner()->CompositionTypeId==GOC_Type_Cube_Thrower )
		{
			//m_initialSpeed=5.7f;
		}
				
	}

	void ActivationProperties::RemoveComponentFromGame()
	{
	}

	



	void ActivationProperties::Activate(GOC* g)//g can be used with differenct logic for every GOC_TYPE or even not at all
	{
		if (m_owner->CompositionTypeId ==GOC_Type_Teleporter )
		{
			if(m_attachedToId >0)
			{
				m_attachedObj=LOGIC->ActivatableObjectsList[m_attachedToId];
				if (g->has(RigidBody))
				{
					Vec3 TeleportTo = m_attachedObj->has(Transform)->Position + Vec3(0.0f,g->has(Transform)->Scale.y*0.5f,0.0f);
					if (g->CompositionTypeId != GOC_Type_Hero ) 
						g->has(RigidBody)->setPosition(TeleportTo);
					else
						HERO->TeleportHero(TeleportTo);
				}
			}
		}
		if (m_owner->CompositionTypeId ==GOC_Type_Button1|| m_owner->CompositionTypeId ==GOC_Type_Button2 ||m_owner->CompositionTypeId ==GOC_Type_Button3 || m_owner->CompositionTypeId ==GOC_Type_VerticalSlider  || m_owner->CompositionTypeId ==GOC_Type_RotatingSlider )
		{
			if(obj)
			{
				obj->has(RigidBody)->allowSleep=true;
			}
			obj=g;
			if(obj)
				obj->has(RigidBody)->allowSleep=false;
		}
	}

	void ActivationProperties::ButtonAction()
	{

		/*if(!(m_Body->isColliding))
			{
				if(abs(m_Body->getPosition().y-m_Body->getInitialPosition.y)>EPSILON)
				{
					m_Body->Position.y=m_Body->InitialPosition.y-(abs(m_Body->Position.y-m_Body->InitialPosition.y)*0.8f);
					
				}
		}*/
	}
	void ActivationProperties::DoorAction(const float & dt)
	{
		if (m_IsActivated)
		{
			Vector3 vec=m_Body->getPosition ();
			vec.y+=dt*velocity;
			if  (vec.y > m_InitialPos.y + m_Transform->Scale.y)
			{
				vec.y=m_InitialPos.y + m_Transform->Scale.y;
				m_Body->setPosition(vec);
			}
			else
				m_Body->setPosition(vec);
		}
		else
		{
			Vector3 vec=m_Body->getPosition ();
			vec.y-=dt*velocity;
			if (vec.y<=m_InitialPos.y)
			{
				m_Body->setPosition(m_InitialPos);
			}
			else
				m_Body->setPosition(vec);
		}
	}

	void ActivationProperties::ThrowerActionPeriodic(const float & dt)
	{
		if(m_timer>m_timerLimit)
		{
			GOC *g=FACTORY->BuildFromArchetype("GOC_Type_Cube");
			g->has(Transform)->Position=Vec3(m_Transform->Position );
			g->has(Transform)->QUATERNION=D3DXQUATERNION (0,0,0,1);
			g->Initialize();

			m_timer=0.0f;
		}
		else
		{
			m_timer+=dt;
		}
	}

	void ActivationProperties::ThrowerAction(const float & dt)
	{
		if (m_IsActivated && !m_denyActivation){
			if(throwerBindedObjId>-1)
			{
				GOC* g=FACTORY->GetObjectWithId(throwerBindedObjId);
				if (g && g->has(Dying)) 
					g->has(Dying)->MarkAsDying();
				throwerBindedObjId=-1;
			}
			GOC *g;
			if(this->GetOwner()->CompositionTypeId==GOC_Type_Cube_Thrower)
			{
				g = FACTORY->BuildFromArchetype("GOC_Type_Cube");
				g->has(RigidBody)->setRotation(0.0,3.0,0.0);
			}
			else if(this->GetOwner()->CompositionTypeId==GOC_Type_Sphere_Thrower)
			{
				g = FACTORY->BuildFromArchetype("GOC_Type_CanonBall");
				g->has(Transform)->Scale=Vec3(0.5,0.5,0.5);
				static_cast<CollisionSphere*>(g->has(CollisionPrimitive))->radius=g->has(Transform)->Scale.x*0.5f;
			}
			
			g->has(Transform)->Position=Vec3(m_Transform->Position );
			g->has(Transform)->QUATERNION=D3DXQUATERNION (0,0,0,1);


			Transform *t =  this->GetOwner()->has(Transform);
			Vector3 pos= t->Position;
			Quaternion q= t->QUATERNION;
			Matrix4 m;
			m.setOrientationAndPos(q,pos);
			Vector3 dir= m.transformDirection(Vector3(0,-1,0));

			g->has(RigidBody)->setVelocity(dir*m_shootingSpeed);
			g->Initialize();
			if(g->has(LightComponent)  )
			{
				LevelClass->CreateLight(g);
				GRAPHICS->CreateLightBuffer();
				LOGIC->BulbList.push_back(g);

			}
				//	prints("CREATION FRAME",g->FrameCreationId);

			throwerBindedObjId=g->GetId();
			m_denyActivation=true;
		}
		else if(!m_IsActivated)
		{
			m_denyActivation=false;
		}
	}

	void ActivationProperties::AttachedObjAction(const float & dt)
	{
	}



	bool StaticRectToStaticRect(Vec3 &Rect0, float & Width0, float & Height0, Vec3 &Rect1, float & Width1, float & Height1)
	{
		float rightA,topA, leftA, bottomA;
		float rightB,topB, leftB, bottomB;

		leftA=Rect0.x-Width0;
		rightA=Rect0.x+Width0;
		topA=Rect0.y+Height0;
		bottomA=Rect0.y-Height0;

		leftB=Rect1.x-Width1;
		rightB=Rect1.x+Width1;
		topB=Rect1.y+Height1;
		bottomB=Rect1.y-Height1;


		if (leftA>rightB ||leftB>rightA||topA<bottomB || topB<bottomA){
			return false;
		}
		else
			return true;
	}


	void ActivationProperties::VerticalSliderAction(const float & dt)
	{	
		if (!m_owner->has(AutoMovingComponent)->GetCanMove()) return;
		if (m_IsActivated)
		{
			//bool isCollidingWithBridge=false;
			if (m_restrictedById  >0)
					obj=LOGIC->ActivatableObjectsList[m_restrictedById];
			if(!obj) return;

		/*	if(POWERGUN->connector.IsStaticCollisionAllowed())
			{
				GOC * bridge=POWERGUN->connector.GetBridgeObj();
				if(bridge)
					isCollidingWithBridge=PHYSICS->checkCollisionBetweenBoxes(this->GetOwner(),bridge);
			}*/
		
			Vector3 vec1 = m_Body->getPosition ();
			Vector3 vec2 = obj->has(RigidBody)->getPosition();

			real topA, bottomA, topB,bottomB ;

			topA=vec1.y + m_Transform->Scale.y*0.5f;
			bottomA=vec1.y - m_Transform->Scale.y*0.5f;

			topB=vec2.y + obj->has(Transform)->Scale.y*0.5f;
			bottomB=vec2.y - obj->has(Transform)->Scale.y*0.5f;
		

			if ((topA<bottomB || topB<bottomA) )//&& !isCollidingWithBridge)
			{
				this->GetOwner()->has(AutoMovingComponent)->StartMovement();
				this->GetOwner()->has(AutoMovingComponent)->SetReverseMovement(false);
			}
			else
			{
				this->GetOwner()->has(AutoMovingComponent)->StopMovement();
			}
		}

		else
		{
			Vector3 vec1 = m_Body->getPosition ();
			if ((this->GetOwner()->has(AutoMovingComponent)->m_MovingAxis.y<0.0f && vec1.y>=m_InitialPos.y) || (this->GetOwner()->has(AutoMovingComponent)->m_MovingAxis.y>0.0f && vec1.y<=m_InitialPos.y))
			{
				//m_Body->setPosition(m_InitialPos);
				this->GetOwner()->has(AutoMovingComponent)->StopMovement();
			}
			else
			{
				this->GetOwner()->has(AutoMovingComponent)->StartMovement();
				this->GetOwner()->has(AutoMovingComponent)->SetReverseMovement(true);
			}
		}
		
	}


	void ActivationProperties::RotatingObjectAction(const float & dt)
	{	
		if (!m_owner->has(AutoMovingComponent)->GetCanRotate()) return;
		if (m_IsActivated)
		{
			Quaternion q= m_Body->getOrientation()*m_inverseInitialQuat;
			real angle = acos(q.r)*2.0;
			angle*= (180.0/3.14159);

			if(this->GetOwner()->CompositionTypeId==GOC_Type_RotatingSlider1)
			{		
				this->GetOwner()->has(AutoMovingComponent)->StartRotation();
				return;
			}
			if(abs(angle)<90 )
			{
				this->GetOwner()->has(AutoMovingComponent)->StartRotation();
				this->GetOwner()->has(AutoMovingComponent)->SetReverseRotation(false);
			}
			else
			{
				this->GetOwner()->has(AutoMovingComponent)->StopRotation();
			}
		}
		else
		{
			
			Quaternion q= m_Body->getOrientation()*m_inverseInitialQuat;
			real angle = acos(q.r)*2.0;
			angle*= (180.0/3.14159);

			if(abs(angle)<2)
			{
				//m_Body->setPosition(m_InitialPos);
				this->GetOwner()->has(AutoMovingComponent)->StopRotation();
			}
			else
			{
				this->GetOwner()->has(AutoMovingComponent)->StartRotation();
				this->GetOwner()->has(AutoMovingComponent)->SetReverseRotation(true);
			}
		}
		
	}

	void ActivationProperties::TeleporterAction(const float & dt)
	{
		D3DXQUATERNION q=calculateQuaterion(dt,Vec3(0,1,0));
		GetOwner()->has(Transform)->QUATERNION*=q;
		GetOwner()->has(RigidBody)->setOrientation(GetOwner()->has(Transform)->QUATERNION);
	}

	void ActivationProperties::DestroyerAction(const float & dt)
	{
		
	}

	void ActivationProperties::LaserAction(const float & dt)
	{
	/*	if(m_IsActivated)
		{
			this->GetOwner()->has(Laser)->Update(dt);
		}*/
	}
	void ActivationProperties::MarkAsActivatedForHistory(bool b,float time)
	{	
		if(this->GetOwner()->CompositionTypeId==GOC_Type_Cube_Thrower || this->GetOwner()->CompositionTypeId==GOC_Type_Sphere_Thrower)
		{
			if(b)
				m_denyActivation=true;
			else
				m_denyActivation=false;
		}
		ActivateGlowingCylinders(b);

		m_IsActivated=b; 

		m_activatedTimer=time;
			
	}

	void ActivationProperties::MarkAsActivated(bool t)
	{	
		ActivateGlowingCylinders(t);

		m_IsActivated=t; 

		if(m_IsActivated)
		{
			m_activatedTimer=0.0f;//initialize to 0;
			
		}
	}

	void ActivationProperties::ActivateGlowingCylinders(bool t)
	{
		if(glowingCylList.size()>0)
		{
			//if(TIMEMANAGER->GetTimeState()!=TIME_NORMAL)
			//{
				for (unsigned int i=0;i<glowingCylList.size();++i)
				{
					if(t)
						glowingCylList[i]->has(ModelComponent)->ChangeToTexture(glowingCylList[i]->has(ModelComponent)->InitialSwapTextureName);
					else
						glowingCylList[i]->has(ModelComponent)->ChangeToTexture(glowingCylList[i]->has(ModelComponent)->InitialTextureName);
				}
			//}
			//else
			/*{
				if(m_IsActivated!=t)
				{
					for (unsigned int i=0;i<glowingCylList.size();++i)
					{
						glowingCylList[i]->has(ModelComponent)->SwapTexture();
					}
				}
			}*/
		}
		
		
		
	}

	bool ActivationProperties::CheckIfActivationTimerIsOver()
	{
		if(m_activatedTimer>=m_timeToStayActivated)
			return true;
		return false;
	}

	bool ActivationProperties::GetPrevActivationState()
	{
		if(m_activationStateChanged)
		{
			return !m_IsActivated;
		}
		else
			return m_IsActivated;

	}

	void ActivationProperties::Update(const float & dt)
	{
		if(m_Body){
			m_Body->prev_position = m_Body->getPosition();
			m_Body->prev_orientation=m_Body->getOrientation();
		}

		if(m_PreviousActivationState!=m_IsActivated)
		{
			m_activationStateChanged=true;
		}
		else
		{
			m_activationStateChanged=false;
		}
		m_PreviousActivationState=m_IsActivated;
		if(m_IsActivated)
			m_activatedTimer+=dt;//how long it is activated
		if(m_activatedTimer > FLT_MAX-10.0f)//just in case
			m_activatedTimer=FLT_MAX;

		if (m_owner->CompositionTypeId ==GOC_Type_Button1 || m_owner->CompositionTypeId ==GOC_Type_Button2 ||m_owner->CompositionTypeId == GOC_Type_Button3)
		{
			ButtonAction();
		}
		else if (m_owner->CompositionTypeId ==GOC_Type_Door1)
		{
			DoorAction(dt);
		}
		else if (m_owner->CompositionTypeId ==GOC_Type_Cube_Thrower || m_owner->CompositionTypeId ==GOC_Type_Sphere_Thrower )
		{
			ThrowerAction(dt);
		}
		else if (m_owner->CompositionTypeId ==GOC_Type_Cube_Thrower_Periodic )
		{
			ThrowerActionPeriodic(dt);
		}
		else if ( m_owner->has(AutoMovingComponent))
		{
			VerticalSliderAction(dt);
			RotatingObjectAction(dt);
		}
		//else if ( m_owner->CompositionTypeId == GOC_Type_RotatingSword || m_owner->CompositionTypeId == GOC_Type_RotatingSlider)
		//{
		//	
		//}
		else if( m_owner->CompositionTypeId == GOC_Type_Teleporter)
		{
			TeleporterAction(dt);
		}
		else if( m_owner->CompositionTypeId == GOC_Type_Destroyer)
		{
			//DestroyerAction(dt);
		}
		else if( m_owner->CompositionTypeId == GOC_Type_Laser)
		{
			LaserAction(dt);
		}

	
	}

	void ActivationProperties::SendMessageA(Message *m)
	{
		if(m->MessageId== Mid::Collide)
		{
		}
	}


	void ActivationProperties::Serialize(ISerializer& stream)
	{	
		ISerializer* str = stream.clone();
		if(str->isGood())
		{
			//tr->readValue("ObjectActivationId", m_ActivationId);
			str->readValue("ResetOnLoad", m_ResetOnLoad);
			//str->readValue("ActivatedBy", m_ActivatedByVec,4);
		}
		delete str;
	}

}
