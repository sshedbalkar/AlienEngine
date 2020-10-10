#ifndef LEVEL_1_H
#define LEVEL_1_H
#pragma once //Makes sure this header is only included once
#include "ILevel.h"
#include "LoadLevelClass.h"
#include "SaveLevelClass.h"
#include "Composition.h"
#include <vector>

#define LEVEL_1_FILE "Levels\\Level_0.xml"
namespace Framework
{
	class DispTextData;
	class Level_1: public ILevel
	{
	public:
		Level_1();
		~Level_1();
		//
		int load(void);
		int initialize(void);
		int update(float dt);
		int draw(void);
		int free(void);
		int unload(void);
		int id(void);
		int restart(void);
		GOC * GUN;
		std::vector<GOC*> TurorialBoxes;

	private:
		float m_loadingEffectTimer;
		bool m_loadingEffectDone;
		LoadLevelClass *m_loader;
		
		char	m_file[256];
		const char* UpdText( DispTextData* d );
		const char* UpdText2( DispTextData* d );
	};
}
//
#endif