#include "BasicLevels.h"
//
namespace Framework
{
	Level_Restart::Level_Restart()
	{
		//
	}
	Level_Restart::~Level_Restart()
	{
		//
	}
	int Level_Restart::load(void)
	{
		return 1;
	}
	int Level_Restart::initialize(void)
	{
		return 1;
	}
	int Level_Restart::update(float dt)
	{
		static_cast<void>(dt);
		return 1;
	}
	int Level_Restart::draw(void)
	{
		return 1;
	}
	int Level_Restart::free(void)
	{
		return 1;
	}
	int Level_Restart::unload(void)
	{
		return 1;
	}
	int Level_Restart::id(void)
	{
		return -1;
	}
	int Level_Restart::restart(void)
	{
		return 1;
	}
	//////////////////////////////////////////////////
	Level_Quit::Level_Quit()
	{
		//
	}
	Level_Quit::~Level_Quit()
	{
		//
	}
	int Level_Quit::load(void)
	{
		return 1;
	}
	int Level_Quit::initialize(void)
	{
		return 1;
	}
	int Level_Quit::update(float dt)
	{
		static_cast<void>(dt);
		return 1;
	}
	int Level_Quit::draw(void)
	{
		return 1;
	}
	int Level_Quit::free(void)
	{
		return 1;
	}
	int Level_Quit::unload(void)
	{
		return 1;
	}
	int Level_Quit::id(void)
	{
		return -2;
	}
	int Level_Quit::restart(void)
	{
		return 1;
	}
}