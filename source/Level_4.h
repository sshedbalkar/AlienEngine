#ifndef LEVEL_4_H
#define LEVEL_4_H
//
#include "ILevel.h"
#include "LoadLevelClass.h"
//
#define LEVEL_4_FILE "Levels\\Level_3.xml"
namespace Framework
{
	class Level_4: public ILevel
	{
	public:
		Level_4();
		~Level_4();
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