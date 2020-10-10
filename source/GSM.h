#ifndef GSM_H
#define GSM_H
//
#include "ILevel.h"
#include "Core.h"
//
namespace Framework
{
	class GSM
	{
	private:
		GSM();
		~GSM();
		//
		int levelLoad(void);
		int levelInitialize(void);
		int levelUpdate(float dt);
		int levelDraw(void);
		int levelFree(void);
		int levelUnload(void);
		//
	public:
		int initialize(ILevel* level);
		int update();
		//
		ILevel* next(void);
		ILevel* next(ILevel* level);
		ILevel* prev(void);
		ILevel* prev(ILevel* level);
		ILevel* current(void);
		ILevel* current(ILevel* level);
		//
		void deletePrevLevel();
		//
		friend class CoreEngine;
		//
	public:
		static ILevel* LEVEL_RESTART;
		static ILevel* LEVEL_QUIT;
		//
	private:
		ILevel* _next;
		ILevel* _prev;
		ILevel* _current;
	};
}
//
#endif GSM_H