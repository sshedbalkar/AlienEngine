#pragma once
//
namespace debug
{
	//Define the debug break using the MS specific or general using assembly interrupt 3
	#ifdef _MSC_VER
		#define DEBUG_BREAK __debugbreak()
	#else
		#define DEBUG_BREAK do { __asm int 3 } while(0)
	#endif
	
	//By defining G_ENABLE_DEBUG_DIAGNOSTICS you can explicitly 
	//enable or disable debugging and diagnostic macros
	#if !defined(ENABLE_DEBUG_DIAGNOSTICS)
		#if defined(_DEBUG)
			#define ENABLE_DEBUG_DIAGNOSTICS 1
		#else
			#define ENABLE_DEBUG_DIAGNOSTICS 0
		#endif
	#endif
	
	//Implement these functions to control how errors and
	//debug printing are handled
	typedef bool ( *SignalErrorHandler )( const char *expression, const char *file, int line, const char *formatMessage, ...);
	typedef void ( *DebugPrintHandler )( const char *msg , ... );
	//
	extern SignalErrorHandler	_errorHandler;
	extern DebugPrintHandler	_printHandler;
	//
	void						SetErrorHandler			( debug::SignalErrorHandler handler );
	debug::SignalErrorHandler	GetErrorHandler			();
	void						SetDebugPrintHandler	( debug::DebugPrintHandler handler );
	debug::DebugPrintHandler	GetDebugPrintHandler	();
	//
	#if ENABLE_DEBUG_DIAGNOSTICS

		//If diagnostics are enabled use the debug functions
		#define MCR_DebugPrint(...) ( *debug::_printHandler )( __VA_ARGS__ );
	
		// The do/while blocks embedded here are ABSOLUTELY NECESSARY to prevent
		// bizzare compilation errors. 
		// Check out http://cnicholson.net/2009/02/stupid-c-tricks-adventures-in-assert/
		// for more details.
		//#pragma message("assert included")
		//#error C++ compiler required.
		#define MCR_ErrorIf( exp, ... ) \
			do { if( ( exp ) && ( *debug::_errorHandler )( #exp, __FILE__, __LINE__, __VA_ARGS__ ) ) \
			DEBUG_BREAK; } while( 0 )

		#define MCR_Assert( exp, ... ) \
			do { if( ( !( exp ) ) && (*debug::_errorHandler )( #exp, __FILE__, __LINE__, __VA_ARGS__ ) ) \
			DEBUG_BREAK; } while( 0 )

	#else

		//If the diagnostics are disabled all the debug functions
		//are no ops

		#define MCR_DebugPrint(...) ((void)0)
		#define MCR_ErrorIf(...) ((void)0)
		#define MCR_Assert(...) ((void)0)

	#endif
}
