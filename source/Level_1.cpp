#include "Precompiled.h"
#include "Level_1.h"
#include "Core.h"
#include "DebugTools.h"
#include "GSM.h"
#include "Factory.h"
#include "GameLogic.h"
#include "TextDisplay.h"
#include "Global.h"
#include "GameLogic.h"
#include "ModelComponent.h"
#include "ActivationProperties.h"
#include "WindowsSystem.h"
//#include "Graphics.h"

namespace Framework
{
	//LoadLevelClass* ll;					//#### added temporary line
	Level_1::Level_1()
	{
		::strcpy_s( m_file, LEVEL_1_FILE );
	}
	Level_1::~Level_1()
	{
		delete m_loader;						//#### added temporary line
	}
	int Level_1::load(void)
	{
		std::cout<<"Load Level: "<<m_file<<"\n";
		m_loader = new LoadLevelClass();		//#### added temporary line
		
		m_loader->Initialize();
		return 1;
	}
	int Level_1::initialize(void)
	{
		
		GUN=0;
		std::cout<<"Level_1: init\n";
		m_loadingEffectTimer=0.0f;
		m_loadingEffectDone=false;
		//
		m_loader->LoadLevelFile( LEVEL_1_FILE );
		HERO->GetHero()->has(ModelComponent)->SwapTexture();
		POWERGUN->IsActivated=false;
		TIMEMANAGER->Disabled=true;
		LOGIC->ActivatableObjectsList[1]->has(ActivationProperties)->MarkAsActivated(true);
		return 1;
	}
	int Level_1::update(float dt)
	{
		if(!m_loadingEffectDone){
			m_loadingEffectTimer+=dt;
			if (m_loadingEffectTimer>1.0f){
				m_loadingEffectDone=true;
			}
		}

		Transform * m_transform= HERO->GetHero()->has(Transform);
		float posX,posZ;
		int tutorialBoxId=-1;
		posX=m_transform->Position.x;
		posZ=m_transform->Position.z;
		for (unsigned int i=0;i<TurorialBoxes.size();i++)
		{
			GOC *goc=TurorialBoxes[i];
			float minX,maxX,minZ,maxZ;
			minX=goc->has(Transform)->Position.x-goc->has(Transform)->Scale.x*0.5f;
			maxX=goc->has(Transform)->Position.x+goc->has(Transform)->Scale.x*0.5f;
			minZ=goc->has(Transform)->Position.z-goc->has(Transform)->Scale.z*0.5f;
			maxZ=goc->has(Transform)->Position.z+goc->has(Transform)->Scale.z*0.5f;
			if(posX>minX && posX<maxX && posZ>minZ && posZ<maxZ)
			{
				tutorialBoxId = goc->LevelBoxId;
				break;
			}
		}


		print(tutorialBoxId);
		for(int i=1;i<9;++i)
		{
			if (i==tutorialBoxId)
			{
				LOGIC->ActivatableObjectsList[i]->has(ActivationProperties)->MarkAsActivated(true);
			}
			else
			{
				LOGIC->ActivatableObjectsList[i]->has(ActivationProperties)->MarkAsActivated(false);
			}
		}


		if(GUN)
		{
			if(POWERGUN->IsActivated)
			{
				GUN->Destroy();
				GUN=0;
			}
		}

		if(POWERGUN->gravityGun.GetPickedItem())
		{
			LOGIC->ActivatableObjectsList[1]->has(ActivationProperties)->MarkAsActivated(false);
		}
		
		CORE->updateSystems(dt);
		return 1;
	}
	int Level_1::draw(void)
	{
		return 1;
	}
	int Level_1::free(void)
	{
		std::cout<<"Free Level: "<<m_file<<"\n";
		m_loader->freeLogicData();
		FACTORY->DestroyAllObjects();
		GRAPHICS->Unload();
		return 1;
	}
	int Level_1::unload(void)
	{
		std::cout<<"Unload Level: "<<m_file<<"\n";
		return 1;
	}
	int Level_1::id(void)
	{
		return 1;
	}
	int Level_1::restart(void)
	{
		std::cout<<"Restart Level: "<<m_file<<"\n";
		CORE->gsm()->next(GSM::LEVEL_RESTART);
		return 1;
	}
	int val1 = 1;
	char str[32];
	const char* Level_1::UpdText( DispTextData* d )
	{
		::sprintf_s( str, 32, "Hello: %d", val1 );
		++val1;
		return str;
	}
	const char* Level_1::UpdText2( DispTextData* d )
	{
		::sprintf_s( str, 32, "Hey: %d", val1 );
		++val1;
		return str;
	}
}