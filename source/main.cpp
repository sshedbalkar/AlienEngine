///////////////////////////////////////////////////////////////////////////////////////
//
//	WinMain
//	The main entry point for the game--everything starts here.
//	
//	Authors:  , 
//	Copyright 2010, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////
#include "MemAllocatorGen.h"
#include "Precompiled.h"
#include "Core.h"
#include "WindowsSystem.h"
#include "Graphics.h"
#include "Physics.h"
#include "GameLogic.h"
#include "Audio.h"
#include "DebugTools.h"
#include <ctime>
#include <crtdbg.h>
#include "LevelEditor.h"
#include "Global.h"

using namespace Framework;

//The title the window will have at the top
const char windowTitle[] = "AlienEngine";
const int ClientWidth = 1280;
const int ClientHeight = 720;
const bool FullScreen = false;

void EnableMemoryLeakChecking(int breakAlloc=-1)
{
	//Set the leak checking flag
	int tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(tmpDbgFlag);

	//If a valid break alloc provided set the breakAlloc
	 if(breakAlloc!=-1) _CrtSetBreakAlloc( breakAlloc );
}

//The entry point for the application--called automatically when the game starts.
//The first parameter is a handle to the application instance.
//The second parameter is the previous app instance which you can use to prevent being launched multiple times
//The third parameter is the command line string, but use GetCommandLine() instead.
//The last parameter is the manner in which the application's window is to be displayed (not needed).
#ifdef _DEBUG
int main()
#endif
#ifndef _DEBUG
INT WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
#endif
{
//	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	srand(unsigned(time(NULL)));
//#ifdef _DEBUG
//	Debug::DebugTools::getInstance()->enable(true);
//	EnableMemoryLeakChecking();
//#else
////#define std::cout(...) ((void)0)
//#endif
	WNDCLASSEX wc;

	#ifdef _DEBUG
	EnableMemoryLeakChecking();
	#endif
	
	std::cout<<"Alien Main...\n";
	//Create the core engine which manages all the systems that make up the game
	CoreEngine* engine = new CoreEngine();

	//Initialize the game by creating the various systems needed and adding them to the core engine
	WindowsSystem* windows = new WindowsSystem(windowTitle, ClientWidth, ClientHeight, FullScreen);

	wc.hIcon= LoadIcon(windows->hInstance , "Assets/textures/Icon.ico");
	//
	GlobalInit();
	Graphics * graphics = new Graphics();
	graphics->SetWindowProperties(windows->hWnd, ClientWidth, ClientHeight, FullScreen);

	engine->AddSystem(windows);
	engine->AddSystem(new GameObjectFactory());
	engine->AddSystem(new Physics());
	engine->AddSystem(new GameLogic());
	engine->AddSystem(graphics);
	engine->AddSystem(new Audio());
	engine->AddSystem(new LevelEditor());
	//TODO: Add additional systems, such as audio, and possibly xinput, lua, etc.
	engine->Initialize();
	
	//Everything is set up, so activate the window
	windows->ActivateWindow();

	//inform the application that the game is about to begin
	engine->FirstFrameRun();

	//Run the game
	engine->GameLoop();

	//Delete all the game objects
	FACTORY->DestroyAllObjects();

	//Delete all the systems
	engine->DestroySystems();

	//Delete the engine itself
	delete engine;
	//
	GlobalFree();
	//
	#ifdef _DEBUG
	::_CrtDumpMemoryLeaks();
	#endif
	Memory::PrintLeaks();
	Memory::PrintStats();
	//Game over, application will now close
	return 0;
}

void DebugPrintHandler( const char * msg , ... )
{	
	const int BufferSize = 1024;
	char FinalMessage[BufferSize];
	va_list args;
	va_start(args, msg);
	vsnprintf_s(FinalMessage , BufferSize , _TRUNCATE , msg, args);
	va_end(args);

	OutputDebugString(FinalMessage);
	OutputDebugString("\n");
}


//A basic error output function
bool SignalErrorHandler(const char * exp, const char * file, int line, const char * msg , ...)
{
	const int BufferSize = 1024;
	char FinalMessage[BufferSize];

	//Print out the file and line in visual studio format so the error can be
	//double clicked in the output window file(line) : error
	int offset = sprintf_s(FinalMessage,"%s(%d) : ", file , line );	
	if (msg != NULL)
	{	
		va_list args;
		va_start(args, msg);
		vsnprintf_s(FinalMessage + offset, BufferSize - offset, _TRUNCATE , msg, args);
		va_end(args);
	}
	else
	{
		strcpy_s(FinalMessage + offset, BufferSize - offset, "No Error Message");
	}

	//Print to visual studio output window
	OutputDebugString(FinalMessage);
	OutputDebugString("\n");

	//Display a message box to the user
	MessageBoxA(NULL, FinalMessage, "Error", 0);
	
	//Do not debug break
	return true;
}
