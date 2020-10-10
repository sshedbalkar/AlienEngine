#pragma once
//
//#include <string>
//
#include "MemDefines.h"
//#include <ostream>
//
namespace Memory
{
	struct ModuleInfo
	{
		ModuleInfo()
			:file(),
			line( 0 )
		{
			//
		}
		//
		~ModuleInfo(){}
		//
		char		file[MAX_FILE_NAME_LENGTH];
		unsigned	line;
	};
	//
	void		Debugprintf( const char *str, ... );
	void		PrintAddress( Address address );
	Address		GetCallingFunction( unsigned skipDepth );
	Address		GetNearestModule( unsigned skipDepth );
	Cstr		MemSizeToText( size_t size );
	unsigned	UpperPowerOfTwo( unsigned v );
	bool		InitDebug();
	bool		GetModuleInfoFromAddr( Address address, ModuleInfo& info );
	unsigned	Round( float num );
	//
	extern Pointer		hProcess;
	extern Pointer		hThread;
}