#include "Precompiled.h"
#include "GravityGun.h"
#include "Physics.h"
#include "Hero.h"
#include "cameraclass.h"
#include "WindowsSystem.h"
#include "GameLogic.h"
#include "ModelComponent.h"
#include "TimeMechanicManager.h"
#include "ActivationProperties.h"

#include "ParticleComponent.h"

namespace Framework
{

	void GravityGun::SendMessage(Message *m)
	{
		switch( m->MessageId )
		{

		case Mid::Collide:
		{
			break;
		}				

		case Mid::CharacterKey:
			{
				MessageCharacterKey * key = (MessageCharacterKey*)m;
				
				if(key->character=='e')
				{
					
					if(LOGIC->objectPointedAt )
					{
						if (LOGIC->objectPointedAt->CompositionTypeId==GOC_Type_Cube || LOGIC->objectPointedAt->CompositionTypeId==GOC_Type_Sphere || LOGIC->objectPointedAt->CompositionTypeId==GOC_Type_LightBulbWithMass)
						{
							if (!m_objectPicked)
							{
								Vec3 b = LOGIC->objectPointedAt->has(Transform)->Position - HERO->GetHero()->has(Transform)->Position;
								//if( lengthSquared(b) < 15.0f )
								{
								
									AddPickedItem(LOGIC->objectPointedAt);
								}
							}
							else
							{
								RemovePickedItem();
							}
							
						}
						else if (LOGIC->objectPointedAt->CompositionTypeId==GOC_Type_Button2)
						{
							Vec3 b = LOGIC->objectPointedAt->has(Transform)->Position - HERO->GetHero()->has(Transform)->Position;
							if( lengthSquared(b) < 15.0f )
							{
								
								LOGIC->objectPointedAt->has(ActivationProperties)->MarkAsActivated(true);
							}
							
						}
					}
				}
				break;
			}
		case Mid::VirtualKey:
			{
				break;
			}
		case Mid::MouseMove:
			{
				break;
			}
		case Mid::MouseWheel :
			{
				break;
			}
		case Mid::RMouseButton:
			{
				
				break;
			}
		case Mid::LMouseButton:
			{
				LMouseButton * lmouse = (LMouseButton*)m;
				if(lmouse->ButtonIsPressed)
				{
				}
				else
				{
					
				}
				break;
			}
		}
	}


	void GravityGun::Initialize()
	{//when we change or restart level
		m_PickedItem=0;
		m_PreviousPickedItem=0;
		m_objectPicked=false;
	}

	void GravityGun::AddPickedItem(GOC* p)
	{
		m_PickedItem = p;
		m_PickedItem->has(RigidBody)->setAwake(true);
		m_PickedItem->has(RigidBody)->ResetOriginalData();
		p->has(RigidBody)->setAcceleration(0.0, 0.0, 0.0f);
		p->has(RigidBody)->canRotate=false;
		m_PickedItem->has(RigidBody)->setCanSleep(false);
		orientation = p->has(RigidBody)->getOrientation();
		dirAtPickTime = CAMERA->movingDirection;
		InitialMass=m_PickedItem->has(RigidBody)->getMass();
		m_PickedItem->has(RigidBody)->setMass(0.1);
		m_PickedItem->has(ModelComponent)->SwapTexture();
		m_objectPicked=true;
		m_PreviousPickedItem=0;
	}

	void GravityGun::RemovePickedItem()
	{
		if(m_PickedItem)
		{
			m_PickedItem->has(RigidBody)->setAcceleration(m_PickedItem->has(RigidBody)->initialAcceleration);
			m_PickedItem->has(RigidBody)->canRotate=true;
			m_PickedItem->has(RigidBody)->clearAccumulators();
			m_PickedItem->has(RigidBody)->setVelocity(0,0,0);
			m_PickedItem->has(RigidBody)->setRotation(0,0,0);
			m_PickedItem->has(RigidBody)->setCanSleep();
			m_PickedItem->has(RigidBody)->setAwake(true);
			D3DXQUATERNION q=m_PickedItem->has(Transform)->QUATERNION;
			m_PickedItem->has(RigidBody)->setOrientation(q.w,q.x,q.y,q.z);
			m_PickedItem->has(RigidBody)->setMass(InitialMass);
			m_PickedItem->has(ModelComponent)->SwapTexture();
			if(TIMEMANAGER->GetTimeState()!=TIME_NORMAL)
				m_PickedItem->has(RigidBody)->makeObjectStatic();
			m_PreviousPickedItem=m_PickedItem;
			m_PickedItem = 0;
			m_objectPicked=false;

		}
	}		

	void GravityGun::ActionIfPickedItemDestroyed(GOC *g)
	{
		if (m_PickedItem ==g)
		{
			RemovePickedItem();
		}
		if (m_PreviousPickedItem ==g)
		{
			m_PreviousPickedItem=0;
		}
	}

	void GravityGun::Update(float dt)
	{
		if (m_PickedItem)
		{
			if(m_PickedItem->has(LightComponent))
			{
				GRAPHICS->DeferredLights[m_PickedItem->has(LightComponent)->GetIndex()].Position = m_PickedItem->has(Transform)->Position;
				GRAPHICS->CreateLightBuffer();
			}
			
			Vector3 a = HERO->heroPos + HERO->heroDir*3.0,b;
			b=a-m_PickedItem->has(RigidBody)->getPosition();
			real magnitude = b.magnitude();
			b.normalise();
			m_PickedItem->has(RigidBody)->setVelocity(0,0,0);
			//if(magnitude>0.01)
				m_PickedItem->has(RigidBody)->addForce(b*magnitude*(1.0/dt));
			//else
				//m_PickedItem->has(RigidBody)->setVelocity(HERO->m_hero->has(RigidBody)->getVelocity());


			//direction calculation
			if(dirAtPickTime != CAMERA->movingDirection)
			{
				real theta = dirAtPickTime*CAMERA->movingDirection;
				theta = acos(theta);
				Vector3 a= dirAtPickTime%Vector3(CAMERA->movingDirection);
				D3DXQUATERNION q; 
				if(a.y<0)
					q =calculateQuaterion(-(float)theta,Vec3(0,1,0));
				else
					q =calculateQuaterion((float)theta,Vec3(0,1,0));
				m_PickedItem->has(Transform)->QUATERNION*=q; 
				//m_PickedItem->has(RigidBody)->setOrientation(q);
			}

			//particle
			Vector3 GunPoint = Vector3( HERO->GetHero()->has(Transform)->Position.x, HERO->GetHero()->has(Transform)->Position.y, HERO->GetHero()->has(Transform)->Position.z );
			GunPoint += 0.7f * CAMERA->GetHeroUp();
			GunPoint += 1.5f * CAMERA->Getlookat();
			GunPoint += 0.275f * CAMERA->GetSideDirection();
			HERO->GetHero()->has(ParticleComponent)->CreateParticlesPointVector(dt,  GunPoint, -CAMERA->Getlookat(), CAMERA->GetSideDirection(), CAMERA->GetHeroUp(), HERO->GetHero()->has(RigidBody)->getVelocity() + HERO->GetVelocitySide()*0.5 + HERO->GetVelocityMove()*0.5 );
		}
	}
	
}