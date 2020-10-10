#include "Precompiled.h"
#include "Level_2.h"
#include "Core.h"
#include "DebugTools.h"
#include "GSM.h"
#include "Factory.h"

//
namespace Framework
{
	//LoadLevelClass* ll;					//#### added temporary line
	Level_2::Level_2()
	{
		//
	}
	Level_2::~Level_2()
	{
		delete m_loader;						//#### added temporary line
	}
	int Level_2::load(void)
	{
		std::cout<<"Level_2: Load\n";
		CORE->gsm()->deletePrevLevel();
		m_loader = new LoadLevelClass();		//#### added temporary line
		m_loader->Initialize();
		return 1;
	}
	int Level_2::initialize(void)
	{
		
		std::cout<<"Level_2: init\n";
		m_loadingEffectTimer=0.0f;
		m_loadingEffectDone=false;
		//
		m_loader->LoadLevelFile( LEVEL_2_FILE );
		return 1;
	}
	int Level_2::update(float dt)
	{
		if(!m_loadingEffectDone){
			m_loadingEffectTimer+=dt;
			if (m_loadingEffectTimer>1.0f){
				m_loadingEffectDone=true;
			}
		}
		
		CORE->updateSystems(dt);
		return 1;
	}
	int Level_2::draw(void)
	{
		return 1;
	}
	int Level_2::free(void)
	{
		std::cout<<"Level 2: Free\n";
		m_loader->freeLogicData();
		FACTORY->DestroyAllObjects();
		GRAPHICS->Unload();
		return 1;
	}
	int Level_2::unload(void)
	{
		std::cout<<"Level 2: Unload\n";
		return 1;
	}
	int Level_2::id(void)
	{
		return 2;
	}
	int Level_2::restart(void)
	{
		std::cout<<"Level 2: Restart\n";
		CORE->gsm()->next(GSM::LEVEL_RESTART);
		return 1;
	}
}