#include "Precompiled.h"
#include "LogicComponentManager.h"
#include "GameLogic.h"
#include "Laser.h"
#include "Dying.h"
#include "DominoComponent.h"
#include "ConstraintsComponent.h"
#include "AutoMovingComponent.h"
#include "RopeComponent.h"
#include "HistoryComponent.h"
#include "PickedObjectHolder.h"
#include "ActivationProperties.h"
#include "ComponentCreator.h"
#include "ConnectorComponent.h"
#include "MsgRecvrComponent.h"
#include "LightComponent.h"
#include "GlowingCylinder.h"

namespace Framework
{

	void LogicComponentManager::Initialize()
	{
		LOGIC->ActivatableObjectsList.clear();
		LOGIC->RopePartsList.clear();
		LOGIC->BulbList.clear();
		LOGIC->RolesList .clear();
		LOGIC->PickedHolderList.clear();
		LOGIC->LaserObjectsList.clear();
		LOGIC->DominosList.clear();
		LOGIC->AutoMovingObjects.clear();
		LOGIC->LevelBoxes.clear();
	}

	void LogicComponentManager::RegisterComponents()
	{
		RegisterComponent(ActivationProperties);
		RegisterComponent(Laser);
		RegisterComponent(Dying);
		RegisterComponent(Domino);
		RegisterComponent(ConstraintComponent);
		RegisterComponent(AutoMovingComponent);
		RegisterComponent(RopeComponent);
		RegisterComponent(MsgRecvrComponent);
		RegisterComponent(ConnectorComponent);
		RegisterComponent(HistoryComponent);
		RegisterComponent(PickedObjectHolder);
		RegisterComponent(GlowingCylinders);
	}


	void LogicComponentManager::AddGOCtoLogicContainer(GOC* g)
	{
		if(g->CompositionTypeId==GOC_Type_Hero  )
		{
			LOGIC->m_Hero.SetHero(g); 
			HERO->Initialize();
		}
		if(g->has(ActivationProperties))
			LOGIC->ActivatableObjectsList[g->has(ActivationProperties)->GetActivationId()]=g;
		if(g->has(Laser))
			LOGIC->LaserObjectsList.push_back(g);
		if(g->has(Domino))
			LOGIC->DominosList.push_back(g);
		if(g->has(RopeComponent))
			LOGIC->RopePointsList[g->has(RopeComponent)->GetId()]=g;
		if(g->has(PickedObjectHolder))
			LOGIC->PickedHolderList.push_back(g);
		if(g->has(AutoMovingComponent))
			LOGIC->AutoMovingObjects.push_back(g);
		if(g->CompositionTypeId==GOC_Type_LevelBox  )
			LOGIC->LevelBoxes.push_back(g);
		
	}



	void LogicComponentManager::Update(const float & dt)
	{
			// YOU CAN REMOVE MOST OF THIS CODE IF YOU MAKE SURE THAT YOU DO NOT DELETE THESE OBJECTS IN REAL TIME; AS LONG AS WE USE THE EDITOR KEEP THIS CODE
		//GameObjectIdMapType mapOfGOCS = FACTORY->GetAllObjects() ;
		//GameObjectIdMapType::iterator p;
		//LOGIC->ActivatableObjectsList.clear();
		//LOGIC->ObjThatCanDieList.clear();
		//LOGIC->LaserObjectsList.clear();
		//LOGIC->DominosList.clear();
		//LOGIC->AutoMovingObjects.clear();
		//for(p = mapOfGOCS.begin(); p != mapOfGOCS.end(); p++){
			
		//	if (p->second->has(HistoryComponent))//DYNAMIC
			//	p->second->has(HistoryComponent)->Update(dt);
			//if (p->second->has(PickedObjectHolder))//STATIC
			//	p->second->has(PickedObjectHolder)->Update(dt);
			//if (p->second->has(ActivationProperties))//STATIC
			//	LOGIC->ActivatableObjectsList[p->second->has(ActivationProperties)->GetActivationId()] = p->second;
			//if (p->second->has(Laser))//STATIC
			//	LOGIC->LaserObjectsList.push_back(p->second);
			//if (p->second->has(Domino))//STATIC
			//	LOGIC->DominosList.push_back(p->second);
			//if (p->second->has(AutoMovingComponent))//STATIC
			//	LOGIC->AutoMovingObjects.push_back(p->second);
		//	if (p->second->has(Dying))//DYNAMIC
		//		LOGIC->ObjThatCanDieList.push_back(p->second);
	
		//}
		//==========================================================================================================================================
		for(HistoryComponentIterator it=LOGIC->HistoryList.begin(); it != LOGIC->HistoryList.end();++it)
		{
			it->Update(dt);
		}

		for(DyingObjectsComponentIterator it=LOGIC->DyingComponentList.begin(); it != LOGIC->DyingComponentList.end();++it)
		{
			it->Update(dt);
		}
		
		LOGIC->m_PowerGun.Update(dt);
		LOGIC->m_Hero.Update(dt);
		LOGIC->m_ObjectActivator.Update(dt);
	
		//
		for (unsigned int i=0; i<LOGIC->LaserObjectsList.size();++i)
		{
			LOGIC->LaserObjectsList[i]->has(Laser)->Update(dt); 
		}
		for (unsigned int i=0; i<LOGIC->ObjThatCanDieList.size();++i)
		{
			LOGIC->ObjThatCanDieList[i]->has(Dying)->Update(dt);
		}
		for (unsigned int i=0; i<LOGIC->DominosList.size();++i)
		{
			LOGIC->DominosList[i]->has(Domino)->Update(dt);
		}
		for (unsigned int i=0; i<LOGIC->AutoMovingObjects.size();++i)
		{
			LOGIC->AutoMovingObjects[i]->has(AutoMovingComponent)->Update(dt);
		}
		for (unsigned int i=0; i<LOGIC->PickedHolderList.size();++i)
		{
			LOGIC->PickedHolderList[i]->has(PickedObjectHolder)->Update(dt);
		}
	
		LOGIC->m_timeMechanism.Update(dt);
	}

}
