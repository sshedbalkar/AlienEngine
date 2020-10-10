#include "Assert.h"
//
namespace debug
{
	SignalErrorHandler	_errorHandler = 0;
	DebugPrintHandler	_printHandler = 0;
	//
	void debug::SetErrorHandler( debug::SignalErrorHandler handler )
	{
		_errorHandler = handler;
	}
	debug::SignalErrorHandler debug::GetErrorHandler()
	{
		return _errorHandler;
	}
	void debug::SetDebugPrintHandler( debug::DebugPrintHandler handler )
	{
		_printHandler = handler;
	}
	debug::DebugPrintHandler debug::GetDebugPrintHandler()
	{
		return _printHandler;
	}
}