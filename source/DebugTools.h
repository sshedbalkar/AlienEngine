////////////////////////////////////////////////////////////////
//
//	DebugTools.h
//	Collection of all the available debug tools. Although there aren't many tools in this class at the moment(only debug stream). Planning on expanding in the future.
//
//	Author: Santosh Shedbalkar
//	Copyright 2011, Digipen Institute of Technology
//
////////////////////////////////////////////////////////////////
#ifndef DEBUG_TOOLS_H
#define DEBUG_TOOLS_H
//#pragma message("Compiling: "__FILE__)
//
#include "DebugPrint.h"
//
namespace Debug
{
	class DebugTools
	{
//#pragma message("Including Class: DebugTools")
	private:
		DebugTools();
		~DebugTools();
		DebugTools(const DebugTools& orig);
		DebugTools& operator=(const DebugTools& rhs);
		//
		void init();
		//
		static DebugTools* _instance;
		DebugStream* _dStream;
		//
	public:
		static DebugTools* getInstance()
		{
			if(!_instance)
			{
				_instance = new DebugTools();
			}
			return _instance;
		}
		inline void logLevel(int i)
		{
			_dStream->logLevel(i);
		}
		inline int logLevel()
		{
			return _dStream->logLevel();
		}
		inline void threshold(int i)
		{
			_dStream->threshold(i);
		}
		inline int threshold()
		{
			return _dStream->threshold();
		}
		inline void enable(bool b)
		{
			_dStream->enable(b);
		}
		inline int enable()
		{
			return _dStream->enable();
		}
		DebugStream* getDebugStream();
	};
}
//
#endif
