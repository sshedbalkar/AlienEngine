#include "TimeCalculator.h"
#include "Utility.h"
#include "ReturnCodes.h"
#include <iomanip>
//
namespace utility
{
	TimeKeeper::TimeKeeper()
		:m_id( 0 )
	{
		//
	}
	//
	TimeKeeper::TimeKeeper( LARGE_INTEGER& time, Timer_Id& id, const char* name )
		:m_begin( time ),
		m_id( id )
	{
		::strcpy_s( m_name, name );
	}
	//
	TimeKeeper::~TimeKeeper()
	{
		//
	}
	////////////////////////////////////////////////////////////////////////
	TimeCalculator::TimeCalculator()
	{
		::QueryPerformanceFrequency( &m_freq );
	}
	//
	TimeCalculator::~TimeCalculator()
	{
		//
	}
	//
	Timer_Id TimeCalculator::Start( const char* name )
	{
		LARGE_INTEGER time;
		::QueryPerformanceCounter( &time );
		//
		Timer_Id id = utility::GetUniqueId();
		//
		TimeKeeper *t = new TimeKeeper( time, id, name );
		m_list.push( t );
		//
		return id;
	}
	//
	Timer_Id TimeCalculator::Start()
	{
		char name[TIMEKEEPER_NAME_LEN];
		::sprintf_s( name, TIMEKEEPER_NAME_LEN, "TIME" );
		//
		return Start( name );
	}
	//
	double TimeCalculator::End()
	{
		double time = ElapsedTime();
		//
		if( !m_list.empty() )
		{
			delete m_list.top();
			m_list.pop();
		}
		//
		return time;
	}
	//
	double TimeCalculator::ElapsedTime()
	{
		if( m_list.empty() ) return 0.0;
		//
		TimeKeeper *tk = m_list.top();
		//
		LARGE_INTEGER end;
		::QueryPerformanceCounter( &end );
		//
		double time = ( double(end.QuadPart - tk->m_begin.QuadPart) / m_freq.QuadPart );
		//
		return time;
	}
	//
	int TimeCalculator::EndAndPrint( std::ostream &os )
	{
		char name[TIMEKEEPER_NAME_LEN];
		::strcpy_s( name, LastName() );
		//
		os.setf( std::ios_base::left );
		os<<"["<<name<<"] clocked: "<<std::setprecision( 20 )<<End()<<" sec\n";
		os.unsetf( std::ios_base::left );
		//
		return RET_SUCCESS;
	}
	//
	Timer_Id TimeCalculator::LastID()
	{
		if( m_list.empty() ) return 0;
		return m_list.top()->m_id;
	}
	//
	const char* TimeCalculator::LastName()
	{
		if( m_list.empty() ) return GetRetCodeName( RET_DOESNT_EXIST );
		return m_list.top()->m_name;
	}
}