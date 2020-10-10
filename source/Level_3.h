#ifndef LEVEL_3_H
#define LEVEL_3_H
//
#include "ILevel.h"
#include "LoadLevelClass.h"
//
#define LEVEL_3_FILE "Levels\\Level_2.xml"
namespace Framework
{
	class Level_3: public ILevel
	{
	public:
		Level_3();
		~Level_3();
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