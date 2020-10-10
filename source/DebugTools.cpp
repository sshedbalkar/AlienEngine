#include "DebugTools.h"
//
namespace Debug
{
	DebugTools* DebugTools::_instance = 0;
	//
	DebugTools::DebugTools():
		_dStream(NULL)
	{
		init();
	}
	DebugTools::~DebugTools()
	{
		delete _dStream;
		_dStream = DEBUG_STREAM = NULL;
	}
	DebugTools::DebugTools(const DebugTools& orig){}
	DebugTools& DebugTools::operator=(const DebugTools& rhs){return *this;}
	//
	void DebugTools::init()
	{
		_dStream = new DebugStream();
		DEBUG_STREAM = _dStream;
	}
	DebugStream* DebugTools::getDebugStream()
	{
		return _dStream;
	}
}