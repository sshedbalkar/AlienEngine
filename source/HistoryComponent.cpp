#include "Precompiled.h"
#include "HistoryComponent.h"
#include "GameLogic.h"
#include "Physics.h"
#include "Core.h"
#include "ActivationProperties.h"
#include "TimeMechanicManager.h"
#include "PowerGun.h"
#include "Dying.h"
#include "ModelComponent.h"
#include "AutoMovingComponent.h"
#include "DominoComponent.h"

namespace Framework
{

	HistoryComponent::HistoryComponent()
	{
	}

	HistoryComponent::~HistoryComponent()
	{
		if(!this->GetOwner()->OnRemovedList)
		{
			LOGIC->HistoryList.erase(this);
		}
	}

	

	void HistoryComponent::Initialize()
	{ 
		LOGIC->HistoryList.push_back( this );
		updateInitialHistoryData();
		m_firstTime=true;
		m_previousActivatedState=false;
		m_ActivateFlag=false;
		//m_isStartOfLevel=true;
		InitialData=0;
		m_previousTimer=0.0f;
		
	}

	void HistoryComponent::Serialize(ISerializer& stream)
	{
	}

	void HistoryComponent::SendMessage(Message *m)
	{
	}

	void HistoryComponent::RemoveComponentFromGame()
	{
		LOGIC->HistoryList.erase(this);
	}

	void HistoryComponent::RestoreComponentToGame()
	{
		LOGIC->HistoryList.push_back( this );
	}

	

	void HistoryComponent::SaveWhenDestroyed()
	{
		Update(0.0);
	}

	void HistoryComponent::updateInitialHistoryData()
	{
		if(!GetOwner()->has(RigidBody)  )
		{
			HistoryData* hd= CreateHistoryData(NORIGID_TYPE);
			TIMEMANAGER->AddTolist(hd);
			InitialData=hd;
		}
		else if(GetOwner()->has(RigidBody)->getIsStatic())
		{
			if(this->GetOwner()->has(AutoMovingComponent ) || this->GetOwner()->CompositionTypeId==GOC_Type_Door1 || this->GetOwner()->CompositionTypeId==GOC_Type_Button3|| this->GetOwner()->CompositionTypeId==GOC_Type_Laser||  this->GetOwner()->CompositionTypeId==GOC_Type_Sphere_Thrower )
			{
				HistoryData* hd= CreateHistoryData(STATIC_TYPE);
				TIMEMANAGER->AddTolist(hd);
				InitialData=hd;
			}
			else if(this->GetOwner()->has(ActivationProperties))
			{
				HistoryData* hd= CreateHistoryData(ACTIVATION_TYPE);
				TIMEMANAGER->AddTolist(hd);
				InitialData=hd;
			}
		
		}
		else
		{	
			if(this->GetOwner()->has(Domino)) 
			{
				HistoryData* hd= CreateHistoryData(DOMINO_TYPE);
				TIMEMANAGER->AddTolist(hd);
				InitialData=hd;
			}
			else if (this->GetOwner()->has(ComplexObject))
			{
				HistoryData* hd= CreateHistoryData(COMPLEX_TYPE);
				TIMEMANAGER->AddTolist(hd);
				InitialData=hd;
			}
			else
			{
				HistoryData* hd= CreateHistoryData(RIGID_TYPE);
				TIMEMANAGER->AddTolist(hd);
				InitialData=hd;
			
			}

		}
	}

	

	void HistoryComponent::Update(const float & dt)
	{
		if (TIMEMANAGER->GetTimeState() == TimeState::TIME_BACK || TIMEMANAGER->GetTimeState() == TimeState::TIME_FORWARD ) 
		{
			if(this->GetOwner()->has(RigidBody))
				this->GetOwner()->has(RigidBody)->setAwake(false);
			if (TIMEMANAGER->GetTimeState() == TimeState::TIME_BACK && m_ActivateFlag &&  this->GetOwner()->has(ActivationProperties))
			{
				if(this->GetOwner()->GlobalId==5)
					int o=0;
				this->GetOwner()->has(ActivationProperties)->MarkAsActivatedForHistory(m_previousActivatedState,m_previousTimer);
			}
				
		
		}
		if (TIMEMANAGER->GetTimeState() != TimeState::TIME_NORMAL)
			return;

		if(TIMEMANAGER->GetFrameId()==1 )
		{
			if(InitialData)
				TIMEMANAGER->AddTolist(InitialData);
			return;
		}

		if(!GetOwner()->has(RigidBody) )
		{
			if(this->GetOwner()->has(ActivationProperties))
			{
				if(this->GetOwner()->has(ActivationProperties)->GetMovedThisFrame())
				{
					HistoryData* hd= CreateHistoryData(NORIGID_TYPE);
					TIMEMANAGER->AddTolist(hd);
					InitialData=hd;
				}
				//else if (( this->GetOwner()->has(ActivationProperties)->m_IsActivated!=this->GetOwner()->has(ActivationProperties)->GetPrevActivationState())
				//	|| (this->GetOwner()->has(ActivationProperties)->m_IsActivated && !this->GetOwner()->has(ActivationProperties)->CheckIfActivationTimerIsOver())  )
				//{
				//	/*if(this->GetOwner()->GlobalId==5)
				//	{
				//		if(this->GetOwner()->has(ActivationProperties)->m_IsActivated)
				//			print("SAVING 5 TRUE");
				//		else
				//			print("SAVING 5 FALSE");

				//	}*/
				//	HistoryData* hd= CreateHistoryData(ACTIVATION_TYPE);
				//	TIMEMANAGER->AddTolist(hd);
				//	m_ActivateFlag=false;
				//}
				
			}
			
		}
		else if(GetOwner()->has(RigidBody)->getIsStatic())
		{
			if((this->GetOwner()->has(RigidBody)->position!=this->GetOwner()->has(RigidBody)->prev_position ||this->GetOwner()->has(RigidBody)->orientation!=this->GetOwner()->has(RigidBody)->prev_orientation  )&& 
				(this->GetOwner()->has(AutoMovingComponent ) || this->GetOwner()->CompositionTypeId==GOC_Type_Door1) || this->GetOwner()->CompositionTypeId==GOC_Type_Button3 || this->GetOwner()->CompositionTypeId==GOC_Type_Laser)
			{
				HistoryData* hd= CreateHistoryData(STATIC_TYPE);
				TIMEMANAGER->AddTolist(hd);
			}
			else if(this->GetOwner()->has(ActivationProperties)  && !this->GetOwner()->has(AutoMovingComponent))
			{
				//this->GetOwner()->has(ActivationProperties)->m_flagForHistoryComponent &&
				if (( this->GetOwner()->has(ActivationProperties)->m_IsActivated!=this->GetOwner()->has(ActivationProperties)->GetPrevActivationState())
					|| (this->GetOwner()->has(ActivationProperties)->m_IsActivated && !this->GetOwner()->has(ActivationProperties)->CheckIfActivationTimerIsOver())  )
				{
					
						
					HistoryData* hd= CreateHistoryData(ACTIVATION_TYPE);
					TIMEMANAGER->AddTolist(hd);
					m_ActivateFlag=false;
				}
			}
		
		}
		else
		{	

			if(this->GetOwner()->has(Domino) && this->GetOwner()->has(RigidBody)->isAwake) 
			{
				HistoryData* hd= CreateHistoryData(DOMINO_TYPE);
				TIMEMANAGER->AddTolist(hd);
				if (m_firstTime)
					m_firstTime=false;
			}
			else if (this->GetOwner()->has(ComplexObject) && this->GetOwner()->has(RigidBody)->isAwake )
			{
				HistoryData* hd= CreateHistoryData(COMPLEX_TYPE);
				TIMEMANAGER->AddTolist(hd);
			}
			else if (this->GetOwner()->has(RigidBody)->isAwake )
			{
				HistoryData* hd= CreateHistoryData(RIGID_TYPE);
				TIMEMANAGER->AddTolist(hd);
			}

		}

	}

	void HistoryComponent::Update(HistoryData* hd, bool isBackwards)
	{
		if((TIMEMANAGER->GetImmuneObject() && this->GetOwner()==TIMEMANAGER->GetImmuneObject()) || this->GetOwner()==POWERGUN->gravityGun.GetPickedItem())//dont update immune object
			return;
		if(hd->dataType == HistoryDataType:: RIGID_TYPE)
		{	
			RigidBodyData * data = static_cast<RigidBodyData*>(hd->data);
			this->GetOwner()->has(RigidBody)->setAwake(data->isawake);
			this->GetOwner()->has(RigidBody)->orientation = data->orientation;
			this->GetOwner()->has(RigidBody)->position = data->position;
			this->GetOwner()->has(RigidBody)->rotation = data->rotation;
			this->GetOwner()->has(RigidBody)->velocity = data->velocity;
			if(this->GetOwner()->has(Dying))
			{
				this->GetOwner()->has(Dying)->SetIsDying (data->dying);
				this->GetOwner()->has(Dying)->SetTransparrency (data->transparrency);
			}
		}
		else if(hd->dataType == HistoryDataType:: COMPLEX_TYPE)
		{	
			RigidBodyComplexData * data = static_cast<RigidBodyComplexData*>(hd->data);
			this->GetOwner()->has(RigidBody)->setAwake(data->isawake);
			this->GetOwner()->has(RigidBody)->orientation = data->orientation;
			this->GetOwner()->has(RigidBody)->position = data->position;
			this->GetOwner()->has(RigidBody)->rotation = data->rotation;
			this->GetOwner()->has(RigidBody)->velocity = data->velocity;
			if(this->GetOwner()->has(Dying))
			{
				this->GetOwner()->has(Dying)->SetIsDying (data->dying);
				this->GetOwner()->has(Dying)->SetTransparrency (data->transparrency);
			}
			/*int * temp=this->GetOwner()->has(ComplexObject)->ReturnPointerToAliveArray();
			bool changed=false;
			for (int i=0;i<16;++i)
			{
				
				if(temp[i]!=data->connected[i])
				{
					temp[i]=data->connected[i];
					changed=true;
				}
			}*/
			//this->GetOwner()->has(ComplexObject)->BouldCurrentState();
			//long int tempPrev=this->GetOwner()->has(ComplexObject)->currentState;
			if(this->GetOwner()->has(ComplexObject)->alivebitset!=data->alivebitset)
			{
				this->GetOwner()->has(ComplexObject)->alivebitset=data->alivebitset;
				this->GetOwner()->has(ComplexObject)->UpdateTheShapeWhenTimePaused();
			}
			

		}
		else if((hd->dataType == HistoryDataType:: STATIC_TYPE))
		{
			StaticBodyData * data = static_cast<StaticBodyData*>(hd->data);
			this->GetOwner()->has(RigidBody)->orientation = data->orientation;
			this->GetOwner()->has(RigidBody)->position = data->position;
			//if(isBackwards)
			//	this->GetOwner()->has(ActivationProperties)->MarkAsActivated(!data->activated );
			//else
		
			this->GetOwner()->has(ActivationProperties)->MarkAsActivated(data->activated );
		}
		else if((hd->dataType == HistoryDataType:: ACTIVATION_TYPE))
		{
			
			ActivationData * data = static_cast<ActivationData*>(hd->data);
			if(isBackwards)
			{
				//
				if(m_ActivateFlag)
				{	
					if(data->activated)//if it is activated again that mean we are in this case: 001110000 where previous data was from index 4 (starting with 0), so m_previousActivatedState we stored as false
					{
						//in this case we dont deactivate, instead we activate
						this->GetOwner()->has(ActivationProperties)->MarkAsActivatedForHistory(!m_previousActivatedState,m_previousTimer);
						
					}
					else//else we are in this case 000010000 where previous data was from index 4 (starting with 0), so m_previousActivatedState we stored as false
					{
						//in this case we want to deactivate, this case is handles outside this function
					}
				}

				if(data->activated)
				{
					m_previousTimer=data->timer;
					m_previousActivatedState=false;
					m_ActivateFlag=true;
				}
				/*else
				{
						if(this->GetOwner()->GlobalId==5)
							int l=1;
					m_previousTimer=data->timer;
					m_previousActivatedState=true;
					m_ActivateFlag=true;
				}*/
			}
			else
			{
				this->GetOwner()->has(ActivationProperties)->MarkAsActivatedForHistory(data->activated,data->timer );
				m_ActivateFlag=false;
			}
		}
		else if((hd->dataType == HistoryDataType:: NORIGID_TYPE))
		{
			NoRigid * data = static_cast<NoRigid*>(hd->data);
			this->GetOwner()->has(Transform)->Position=data->position;
			this->GetOwner()->has(Transform)->QUATERNION=data->orientation;
		}
		else if((hd->dataType == HistoryDataType:: DOMINO_TYPE))
		{
			DominoData * data = static_cast<DominoData*>(hd->data);
			this->GetOwner()->has(RigidBody)->setAwake(data->sleeping);
			this->GetOwner()->has(RigidBody)->orientation = data->orientation;
			this->GetOwner()->has(RigidBody)->position = data->position;
			this->GetOwner()->has(RigidBody)->rotation = data->rotation;
			this->GetOwner()->has(RigidBody)->velocity = data->velocity;
			this->GetOwner()->has(Domino)->MarkAsFinished(data->finished);
			m_firstTime = data->isInitial;
			if(m_firstTime)
			{
				this->GetOwner()->has(RigidBody)->setOrientation(this->GetOwner()->has(Domino)->GetInitialOrientation().getConjugate());
			}

		}

	}

	HistoryData* HistoryComponent::CreateHistoryData(HistoryDataType type)
	{

		HistoryData* hd=0;
		if(type==STATIC_TYPE)
		{
			StaticBodyData* data= new StaticBodyData;
			data->orientation = this->GetOwner()->has(RigidBody)->orientation;
			data->position = this->GetOwner()->has(RigidBody)->position;
			if(this->GetOwner()->has(ActivationProperties))
			{
				data->activated=this->GetOwner()->has(ActivationProperties)->IsActivated();
			}
			hd= new HistoryData;
			hd->dataType = type;
			hd->objectId = this->GetOwner()->GetId();
			hd->data = data;
		}
		
		else if(type==ACTIVATION_TYPE)
		{
			ActivationData* data= new ActivationData;
			data->activated=this->GetOwner()->has(ActivationProperties)->IsActivated();
			data->timer=this->GetOwner()->has(ActivationProperties)->GetCurrentActivationTime();
			hd= new HistoryData;
			hd->dataType =type ;
			hd->objectId = this->GetOwner()->GetId();
			hd->data = data;
		}

		else if (type==DOMINO_TYPE)
		{	
			DominoData* data= new DominoData;
			data->velocity = this->GetOwner()->has(RigidBody)->velocity;
			data->orientation = this->GetOwner()->has(RigidBody)->orientation;
			data->position = this->GetOwner()->has(RigidBody)->position;
			data->rotation = this->GetOwner()->has(RigidBody)->rotation;
			data->finished = this->GetOwner()->has(Domino)->GetIsFinished();
			data->sleeping= this->GetOwner()->has(RigidBody)->getAwake();
			data->isInitial=m_firstTime; 
				
			hd= new HistoryData;
			hd->dataType = type;
			hd->objectId = this->GetOwner()->GetId();
			hd->data = data;
			if (this->GetOwner()->MarkedAsDestroyed)
				hd->destroyed = true;
		}
		else if (type==NORIGID_TYPE)
		{	
			NoRigid* data= new NoRigid;
			data->orientation = this->GetOwner()->has(Transform)->QUATERNION;
			data->position = this->GetOwner()->has(Transform)->Position;
				
			hd= new HistoryData;
			hd->dataType = type;
			hd->objectId = this->GetOwner()->GetId();
			hd->data = data;
			if (this->GetOwner()->MarkedAsDestroyed)
				hd->destroyed = true;
		}
		else if(type==COMPLEX_TYPE)
		{
			RigidBodyComplexData* data= new RigidBodyComplexData;
			data->orientation = this->GetOwner()->has(RigidBody)->orientation;
			data->position = this->GetOwner()->has(RigidBody)->position;
			data->rotation = this->GetOwner()->has(RigidBody)->rotation;
			data->isawake = this->GetOwner()->has(RigidBody)->getAwake();
			
			//this->GetOwner()->has(ComplexObject)->BuildCurrentState();
			data->alivebitset=this->GetOwner()->has(ComplexObject)->alivebitset;
			//int * temp=this->GetOwner()->has(ComplexObject)->ReturnPointerToAliveArray();
				
			/*for (int i=0;i<16;++i)
			{
				data->connected[i]=temp[i];
			}*/
			if (this->GetOwner()->has(Dying))
			{
				data->dying = this->GetOwner()->has(Dying)->GetIsDying();
				data->transparrency = this->GetOwner()->has(ModelComponent)->Transparency;
			}
			hd= new HistoryData;
			hd->dataType = type;
			hd->objectId = this->GetOwner()->GetId();
			hd->data = data;
			if (this->GetOwner()->MarkedAsDestroyed)
				hd->destroyed = true;
		}
		else if (type==RIGID_TYPE )
		{
			RigidBodyData* data= new RigidBodyData;
			if (POWERGUN->gravityGun.GetPickedItem() && POWERGUN->gravityGun.GetPickedItem()== this->GetOwner())
			{
				data->velocity = Vector3();
			}
			else
			{
				data->velocity = this->GetOwner()->has(RigidBody)->velocity;
			}
				
			data->orientation = this->GetOwner()->has(RigidBody)->orientation;
			data->position = this->GetOwner()->has(RigidBody)->position;
			data->rotation = this->GetOwner()->has(RigidBody)->rotation;
			data->isawake = this->GetOwner()->has(RigidBody)->getAwake();

			if (this->GetOwner()->has(Dying))
			{
				data->dying = this->GetOwner()->has(Dying)->GetIsDying();
				data->transparrency = this->GetOwner()->has(ModelComponent)->Transparency;
			}
			hd= new HistoryData;
			hd->dataType = type;
			hd->objectId = this->GetOwner()->GetId();
			hd->data = data;
			if (this->GetOwner()->MarkedAsDestroyed)
				hd->destroyed = true;
		}
		return hd;
	}


}





							
						
