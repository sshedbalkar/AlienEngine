#include "Level_3.h"
#include "Core.h"
#include "DebugTools.h"
#include "GSM.h"
#include "Factory.h"
//
namespace Framework
{
	//LoadLevelClass* ll;					//#### added temporary line
	Level_3::Level_3()
	{
		//
	}
	Level_3::~Level_3()
	{
		delete m_loader;						//#### added temporary line
	}
	int Level_3::load(void)
	{
		std::cout<<"Level_3: Load\n";
		CORE->gsm()->deletePrevLevel();
		m_loader = new LoadLevelClass();		//#### added temporary line
		m_loader->Initialize();
		return 1;
	}
	int Level_3::initialize(void)
	{
		std::cout<<"Level_3: init\n";
		m_loadingEffectTimer=0.0f;
		m_loadingEffectDone=false;
		//
		m_loader->LoadLevelFile( LEVEL_3_FILE );
		return 1;
	}
	int Level_3::update(float dt)
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
	int Level_3::draw(void)
	{
		return 1;
	}
	int Level_3::free(void)
	{
		std::cout<<"Level 3: Free\n";
		m_loader->freeLogicData();
		FACTORY->DestroyAllObjects();
		GRAPHICS->Unload();
		return 1;
	}
	int Level_3::unload(void)
	{
		std::cout<<"Level 3: Unload\n";
		return 1;
	}
	int Level_3::id(void)
	{
		return 2;
	}
	int Level_3::restart(void)
	{
		std::cout<<"Level 3: Restart\n";
		CORE->gsm()->next(GSM::LEVEL_RESTART);
		return 1;
	}
}