#include "Precompiled.h"
#include "ObjectActivator.h"
#include "GameLogic.h"
#include "Physics.h"
#include "ActivationProperties.h"
#include "AutoMovingComponent.h"
#include "Audio.h"
#include "TimeMechanicManager.h"
#include "ParticleComponent.h"

namespace Framework
{


	void ObjectActivator::UpdateButtons(const float& dt)
	{

		std::map<int,GOC*>::iterator  it;
		for(it=LOGIC->ActivatableObjectsList.begin();it!=LOGIC->ActivatableObjectsList.end();++it)// FOR BUTTONS
		{
		/*	if(it->second->has(ParticleComponent))
			{
				Vec3 h=it->second->has(Transform)->Position;
				h.y+=3;
				it->second->has(ParticleComponent)->CreateParticlesLine(dt, it->second->has(Transform)->Position, h, 3);
			}*/
			if(it->second->has(ActivationProperties)->DoNotActivate())
				continue;

			//for history component 
	/*		if(it->second->has(ActivationProperties)->IsActivated() && it->second->has(ActivationProperties)->GetCurrentActivationTime()>0  && it->second->has(ActivationProperties)->GetActivationTimer()>0)
				it->second->has(ActivationProperties)->SetFlagHistoryComponent(false);
			else
				it->second->has(ActivationProperties)->SetFlagHistoryComponent(true);*/
			//
				
			if(!it->second->has(ActivationProperties)->IsOneTimeActivated())//objects that are activated only once should keep their previous value
			{
				
				if(it->second->has(ActivationProperties)->CheckIfActivationTimerIsOver())
					it->second->has(ActivationProperties)->MarkAsActivated(false);//dont initialize if it has an activation timer
				/*else
				{
					it->second->has(ActivationProperties)->Update (dt);
					continue;
				}*/
			}
			else
			{
				if(it->second->has(ActivationProperties)->IsActivated ())//if it's alread activated, and has OneTimeActivatedProperty
					continue;
			}
			
			bool activated=false;
			if(it->second->CompositionTypeId == GOC_Type_Button1 || it->second->CompositionTypeId == GOC_Type_Button2 || it->second->CompositionTypeId == GOC_Type_Button3)
			{

				if(it->second->has(RigidBody)->isColliding)
					activated=true;
				else
					it->second->has(ActivationProperties)->Activate(0);
				if(!activated)
				{
					if(it->second->has(ActivationProperties)->CheckIfActivationTimerIsOver())
					{
						it->second->has(ActivationProperties)->MarkAsActivated(activated);//dont deactivate yet if it has an activation timer
					}
				}
				else
				{
					it->second->has(ActivationProperties)->MarkAsActivated(activated);
					/*if(activated!=it->second->has(ActivationProperties)->GetPrevActivationState())
						AUDIO->Play("button");*/
				}
				it->second->has(ActivationProperties)->Update (dt);

			}
		}
	}

	void ObjectActivator::UpdateOtherObjects(const float& dt)
	{
		std::map<int,GOC*>::iterator  it;
		Vec4 temp;
		for(it=LOGIC->ActivatableObjectsList.begin();it!=LOGIC->ActivatableObjectsList.end();++it)
		{
			
			if(it->second->CompositionTypeId == GOC_Type_Button1 || it->second->CompositionTypeId == GOC_Type_Button2 || it->second->CompositionTypeId == GOC_Type_Button3)
				continue;
			if(it->second->has(ActivationProperties)->DoNotActivate())
				continue;

			if(it->second->has(ActivationProperties)->GetActivationId() ==0)//special case
			{
				
				//do nothing
				continue;
			}
			bool activated=false;

			temp=it->second->has(ActivationProperties)->GetObjectActivatedByVec();
			if((int)temp.x==0 || LOGIC->ActivatableObjectsList[(int)temp.x]->has(ActivationProperties)->IsActivated())
				if((int)temp.y==0 || LOGIC->ActivatableObjectsList[(int)temp.y]->has(ActivationProperties)->IsActivated())
					if((int)temp.z==0 || LOGIC->ActivatableObjectsList[(int)temp.z]->has(ActivationProperties)->IsActivated())
						if((int)temp.w==0 || LOGIC->ActivatableObjectsList[(int)temp.w]->has(ActivationProperties)->IsActivated())
							activated=true;
			
			if(TIMEMANAGER->GetTimeState() != TimeState::TIME_NORMAL)
			{
				it->second->has(ActivationProperties)->MarkAsActivatedForHistory(activated,0.0f);	
			}
			else
			{
				it->second->has(ActivationProperties)->MarkAsActivated(activated);	
				it->second->has(ActivationProperties)->Update (dt);
			}
			
		}
	}



	void ObjectActivator::Update(const float& dt)
	{
		
		if (TIMEMANAGER->GetTimeState() != TimeState::TIME_NORMAL )
		{
			UpdateAttachedObjects(dt);
			UpdateOtherObjects(dt);
			return;
		}
		UpdateButtons(dt);
		UpdateOtherObjects(dt);
		
	}


	void ObjectActivator::UpdateAttachedObjects(const float& dt)
	{
		std::map<int,GOC*>::iterator  it;
		for(it=LOGIC->ActivatableObjectsList.begin();it!=LOGIC->ActivatableObjectsList.end();++it)
		{
			if(it->second->has(ActivationProperties)->GetAttachedToId() >0)
			{
				it->second->has(ActivationProperties)->AttachedObjAction(dt);
			}
		}
	}

	void ObjectActivator::InitializeAttachedObjects()
	{
		std::map<int,GOC*>::iterator  it;
		for(it=LOGIC->ActivatableObjectsList.begin();it!=LOGIC->ActivatableObjectsList.end();++it)
		{
			int attachedToId=it->second->has(ActivationProperties)->GetAttachedToId();
			if( attachedToId>0)
			{
				GOC * g=LOGIC->ActivatableObjectsList[attachedToId];
				g->has(AutoMovingComponent)->objAttachedToThis.push_back(it->second);
			}
		}
	}

}
