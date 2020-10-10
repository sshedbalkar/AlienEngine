#ifndef LEVEL_5_H
#define LEVEL_5_H
//
#include "ILevel.h"
#include "LoadLevelClass.h"
//
#define LEVEL_5_FILE "Levels\\Level_4.xml"
namespace Framework
{
	class Level_5: public ILevel
	{
	public:
		Level_5();
		~Level_5();
		//
		int load(void);
		int initialize(void);
		int update(float dt);
		int draw(void);
		int free(void);
		int unload(void);
		int id(void);
		int restart(void);
	private:
		float m_loadingEffectTimer;
		bool m_loadingEffectDone;
		LoadLevelClass *m_loader;
	};
}
//
#endif