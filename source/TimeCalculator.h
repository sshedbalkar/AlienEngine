#pragma once
//
#include "Singleton.h"
#include <stack>
#include <Windows.h>
#include <ostream>
//
#define TIMEKEEPER_NAME_LEN		64
//
namespace utility
{
	typedef unsigned int		Timer_Id;
	//
	class TimeKeeper
	{
	public:
								TimeKeeper();
								TimeKeeper::TimeKeeper( LARGE_INTEGER& time, Timer_Id& id, const char* name );
								~TimeKeeper();
		//
	public:
		LARGE_INTEGER			m_begin;
		char					m_name[TIMEKEEPER_NAME_LEN];
		Timer_Id				m_id;
	};
	//
	class TimeCalculator: public Singleton< TimeCalculator >
	{
	public:
								TimeCalculator();
								~TimeCalculator();
		//
	public:
		Timer_Id				Start();
		Timer_Id				Start( const char* name );
		double					End();
		Timer_Id				LastID();
		const char*				LastName();
		int						EndAndPrint( std::ostream &os = std::cout );
		double					ElapsedTime();
		//
	private:
		typedef std::stack< TimeKeeper*, std::deque<TimeKeeper*> > ListType;
		ListType				m_list;
		LARGE_INTEGER			m_freq;
	};
}
////////////////////////////////////////////////////////////
/// Example Usage:
/*
#include "Global.h"
#include "TimeCalculator.h"
//
g_timeCalculator->Start( "Test1" );	//
g_timeCalculator->Start( "Test2" );	// Start() calls can be nested
//
//Random time consuming code:
unsigned int var1 = 0;
for( unsigned int i = 0; i < 500; ++i )
{
	for( unsigned j = 0; j < 10000; ++j )
	{
		var1 += i+j;
	}
}

g_timeCalculator->EndAndPrint();		//Use this to print the time elapsed to the ostream with the identifier name
*/
