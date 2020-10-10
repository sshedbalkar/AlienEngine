#include "GSM.h"
#include "BasicLevels.h"
//
namespace Framework
{
	ILevel* GSM::LEVEL_QUIT = new Level_Quit();
	ILevel* GSM::LEVEL_RESTART = new Level_Restart();
	//
	GSM::GSM()
		:_next(0),
		_prev(0),
		_current(0)
	{
		//
	}
	//
	GSM::~GSM()
	{
		delete LEVEL_QUIT;
		delete LEVEL_RESTART;
	}
	//
	int GSM::initialize(ILevel* level)
	{
		_next = _prev = _current = level;
		return 1;
	}
	//
	int GSM::update()
	{
		if(_current != 0 && _current != LEVEL_QUIT && _current != LEVEL_RESTART)
		{
			//
		}
		return 1;
	}
	int GSM::levelLoad(void)
	{
		return _current->load();
	}
	int GSM::levelInitialize(void)
	{
		return _current->initialize();
	}
	int GSM::levelUpdate(float dt)
	{
		return _current->update(dt);
	}
	int GSM::levelDraw(void)
	{
		return _current->draw();
	}
	int GSM::levelFree(void)
	{
		return _current->free();
	}
	int GSM::levelUnload(void)
	{
		return _current->unload();
	}
	//
	ILevel* GSM::next(void)
	{
		return _next;
	}
	ILevel* GSM::next(ILevel* level)
	{
		return _next = level;
	}
	ILevel* GSM::prev(void)
	{
		return _prev;
	}
	ILevel* GSM::prev(ILevel* level)
	{
		return _prev = level;
	}
	ILevel* GSM::current(void)
	{
		return _current;
	}
	ILevel* GSM::current(ILevel* level)
	{
		return _current = level;
	}
	void GSM::deletePrevLevel()
	{
		delete _prev;
		_prev = 0;
	}
}