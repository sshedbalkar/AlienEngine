#include "Precompiled.h"
#include "LogicCollisionResolve.h"
#include "Physics.h"
#include "Hero.h"
#include "GameLogic.h"
#include "ActivationProperties.h"
#include "DominoComponent.h"
#include "Dying.h"
#include "TimeMechanicManager.h"
#include "LightComponent.h"
#include "ModelComponent.h"

namespace Framework
{


	LogicCollisionResolve::LogicCollisionResolve()
	{
	}

	void LogicCollisionResolve::SendMessage(Message *m)
	{

		switch( m->MessageId )
		{

		case Mid::Collide:
		{
			MessageCollide * msg = (MessageCollide*)m;
			if(msg->CollideObject->CompositionTypeId==GOC_Type_Hero)
			{
				HERO->collisionNormal=msg->ContactNormal;
			}

			if (msg->CollidedWith && msg->CollidedWith ->CompositionTypeId==GOC_Type_Conv_Belt &&  msg->CollidedWith->has(ActivationProperties)->IsActivated())
			{
				if(msg->CollideObject->CompositionTypeId==GOC_Type_Hero)
					msg->CollideObject->has(RigidBody)->setVelocity(msg->CollideObject->has(RigidBody)->getVelocity() + Vector3(0,0,-2));
				else
					msg->CollideObject->has(RigidBody)->setVelocity(Vector3(0,0,-2));
			}

			if (msg->CollidedWith  && msg->CollidedWith ->CompositionTypeId==GOC_Type_Destroyer && msg->CollideObject ->has(Dying)  )
			{
				POWERGUN->gravityGun.ActionIfPickedItemDestroyed(msg->CollideObject);
				msg->CollideObject->has(Dying)->MarkAsDying();
			}
			if (msg->CollidedWith  && msg->CollidedWith ->CompositionTypeId==GOC_Type_Destroyer && msg->CollideObject ->CompositionTypeId==GOC_Type_Hero  )
			{
				LOGIC->ResetLevel();
			}
			if (msg->CollidedWith  && msg->CollidedWith ->CompositionTypeId==GOC_Type_Teleporter  )
			{
				msg->CollidedWith->has(ActivationProperties)->Activate(msg->CollideObject);
			}
			if (msg->CollideObject ->CompositionTypeId==GOC_Type_Button1 || msg->CollideObject->CompositionTypeId ==GOC_Type_Button2 || msg->CollideObject->CompositionTypeId ==GOC_Type_Button3 || msg->CollideObject ->CompositionTypeId==GOC_Type_VerticalSlider ||  msg->CollideObject ->CompositionTypeId==GOC_Type_RotatingSlider )
			{
				msg->CollideObject->has(ActivationProperties)->Activate(msg->CollidedWith);//do not allow sleeping
			}
			if (msg->CollideObject ->CompositionTypeId==GOC_Type_Button1 || msg->CollideObject->CompositionTypeId ==GOC_Type_Button2 ||msg->CollideObject->CompositionTypeId ==GOC_Type_Button3  )
			{
				if(msg->CollidedWith->has(Dying))
				{
					if(msg->CollidedWith->has(Dying)->GetIsDying())
					{
						msg->CollideObject->has(RigidBody)->isColliding=false; //if the cube is dying do not make button activate
					}
				}
				
			}
			if ( msg->CollideObject ->CompositionTypeId==GOC_Type_Domino)
			{
				//msg->CollideObject->has(Domino)->MarkAsActive();	
			}
			if ( msg->CollidedWith && msg->CollideObject ->CompositionTypeId==GOC_Type_BridgeRigid && msg->CollidedWith->CompositionTypeId == GOC_Type_VerticalSlider)
			{
				msg->CollideObject->has(RigidBody)->setAwake(false); //do not allow sleeping
			}
			if ( msg->CollidedWith && msg->CollideObject ->CompositionTypeId==GOC_Type_BridgeRigid && msg->CollidedWith->CompositionTypeId == GOC_Type_Rope)
			{
				msg->CollidedWith->has(Dying)->MarkAsDying();
			}
			if (  msg->CollideObject ->CompositionTypeId==GOC_Type_CanonBall && msg->CollidedWith && (msg->CollidedWith->CompositionTypeId == GOC_Type_Floor || msg->CollidedWith->CompositionTypeId == GOC_Type_Domino))
			{
				msg->CollideObject->has(Dying)->MarkAsDying();
			}
			if (  msg->CollideObject ->CompositionTypeId==GOC_Type_LightBulbWithMass && msg->CollidedWith)
			{
				GRAPHICS->DeferredLights[msg->CollideObject->has(LightComponent)->GetIndex()].Position = msg->CollideObject->has(Transform)->Position;
				GRAPHICS->CreateLightBuffer();
			}
			//if ( msg->CollideObject ->CompositionTypeId==GOC_Type_Hero && msg->CollidedWith && msg->CollidedWith->CompositionTypeId == GOC_Type_EventTriggerBox)
			//{
			//	//msg->CollidedWith->Destroy();io
			//	TimeState t =TimeState::TIME_NORMAL;
			//	MessageTimeStateChange* newMsg = new MessageTimeStateChange(t); 
			//	TIMEMANAGER->SendMessageA(newMsg);
			//}
			//if ( msg->CollideObject ->CompositionTypeId==GOC_Type_Cube && msg->CollidedWith && msg->CollidedWith->CompositionTypeId == GOC_Type_EventTriggerBox)
			//{
			//	msg->CollideObject->Destroy();
			//}







			//HACKS FOR TUTORIAL LEVEL 1
			if(LOGIC->GetLevelId()==0)
			{
				if (!POWERGUN->IsActivated && msg->CollidedWith && msg->CollideObject ->CompositionTypeId==GOC_Type_Hero &&  msg->CollidedWith->CompositionTypeId ==GOC_Type_PillarForGun)
				{
					msg->CollidedWith->has(ActivationProperties)->MarkAsActivated(true);
					HERO->GetHero()->has(ModelComponent)->SwapTexture();
					POWERGUN->IsActivated=true;
					//msg->CollidedWith->Destroy();
				}
				

			}

			
			break;
		}				

		}
	}

	void LogicCollisionResolve::Initialize()
	{//when we change or restart level
	}


	void LogicCollisionResolve::Update(float & dt)
	{
	}

}