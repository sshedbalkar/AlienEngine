#include "Level_4.h"
#include "Core.h"
#include "DebugTools.h"
#include "GSM.h"
#include "Factory.h"
//
namespace Framework
{
	//LoadLevelClass* ll;					//#### added temporary line
	Level_4::Level_4()
	{
		//
	}
	Level_4::~Level_4()
	{
		delete m_loader;						//#### added temporary line
	}
	int Level_4::load(void)
	{
		std::cout<<"Level_4: Load\n";
		CORE->gsm()->deletePrevLevel();
		m_loader = new LoadLevelClass();		//#### added temporary line
		m_loader->Initialize();
		return 1;
	}
	int Level_4::initialize(void)
	{
		std::cout<<"Level_4: init\n";
		m_loadingEffectTimer=0.0f;
		m_loadingEffectDone=false;
		//
		m_loader->LoadLevelFile( LEVEL_4_FILE );
		return 1;
	}
	int Level_4::update(float dt)
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
	int Level_4::draw(void)
	{
		return 1;
	}
	int Level_4::free(void)
	{
		std::cout<<"Level 4: Free\n";
		m_loader->freeLogicData();
		FACTORY->DestroyAllObjects();
		GRAPHICS->Unload();
		return 1;
	}
	int Level_4::unload(void)
	{
		std::cout<<"Level 4: Unload\n";
		return 1;
	}
	int Level_4::id(void)
	{
		return 2;
	}
	int Level_4::restart(void)
	{
		std::cout<<"Level 4: Restart\n";
		CORE->gsm()->next(GSM::LEVEL_RESTART);
		return 1;
	}
}