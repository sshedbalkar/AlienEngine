#ifndef BASICLEVELS_H
#define BASICLEVELS_H
//
#include "ILevel.h"
#include "GSM.h"
//
namespace Framework
{
	class Level_Restart: public ILevel
	{
	private:
		Level_Restart();
		~Level_Restart();
	public:
		int load(void);
		int initialize(void);
		int update(float dt);
		int draw(void);
		int free(void);
		int unload(void);
		int id(void);
		int restart(void);
		//
		friend class GSM;
	};
	//
	class Level_Quit: public ILevel
	{
	private:
		Level_Quit();
		~Level_Quit();
	public:
		int load(void);
		int initialize(void);
		int update(float dt);
		int draw(void);
		int free(void);
		int unload(void);
		int id(void);
		int restart(void);
		//
		friend class GSM;
	};
}
//
#endif BASICLEVELS_H