#ifndef LEVEL_6_H
#define LEVEL_6_H
//
#include "ILevel.h"
#include "LoadLevelClass.h"
//
#define LEVEL_6_FILE "Levels\\Level_5.xml"
namespace Framework
{
	class Level_6: public ILevel
	{
	public:
		Level_6();
		~Level_6();
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