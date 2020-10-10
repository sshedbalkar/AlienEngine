///////////////////////////////////////////////////////////////////////////////////////
///
///	\file Core.h
///	Defines the CoreEngine.
///	Authors:  
///	Copyright 2010, Digipen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////////////
#pragma once //Makes sure this header is only included once
//
#include "System.h"
#include <vector>
#include "FunctionBind.h"
//
namespace Framework
{
	///The core manages all the systems in the game. Updating them, routing messages, and
	///destroying them when the game ends.
	class WindowsSystem;
	class GSM;
	class DispTextData;
	//
	class CoreEngine
	{
	public:
		CoreEngine();
		~CoreEngine();
		///Update all the systems until the game is no longer active.
		void GameLoop();
		///Destroy all systems in reverse order that they were added.
		void DestroySystems();
		///Broadcasts a message to all systems.
		void BroadcastMessage(Message* m);
		///Adds a new system to the game.
		void AddSystem(ISystem* system);
		///Initializes all systems in the game.
		void Initialize();
		//
		float fps();
		void fps(int f);
		void updateSystems(float dt);
		float frameTime();
		GSM* gsm();
		bool pauseSystems(bool pause);
		bool togglePauseSystems();
		bool isPaused();
		int FirstFrameRun();
		//
		template< typename ClassType > 
		inline int AddFirstFrameCallback( void(ClassType::*f)(utility::Object), ClassType* obj, utility::Object data )
		{
			utility::FunctionObject* fo = utility::Bind( f );
			AddFuncIntern( fo, obj, data );
			//
			return RET_SUCCESS;
		}
		unsigned GetFrameCounter(){return _frame;}
		//
		unsigned FrameId();
		//
	private:
		const char* TextUpdate( DispTextData* data );
		void		AddFuncIntern( utility::FunctionObject* fo, utility::Object obj, utility::Object data );
		//
	private:
		//Tracks all the systems the game uses
		std::vector<ISystem*> Systems;
		//
		//The last time the game was updated
		unsigned _lastTime;
		//Is the game running (true) or being shut down (false)?
		bool _gameActive;
		//
		int _fps;
		float _dt;
		bool _frameMode;
		unsigned _advanceFrame;
		WindowsSystem* _winSys;
		unsigned _frame;
		bool _paused;
		bool _showTime;
		//
		GSM* _gsm;
	};

	///Message to tell the game to quit
	class MessageQuit : public Message
	{
	public:
		MessageQuit() : Message(Mid::Quit) {};
	};
	//
	class ToggleFrameByFrameMode: public Message
	{
	public:
		ToggleFrameByFrameMode(): Message(Mid::FrameByFrame){};
	};
	//
	class AdvanceFrame: public Message
	{
	public:
		unsigned count;
		AdvanceFrame(unsigned frames): Message(Mid::AdvanceFrame), count(frames){};
	};
	///Signals all systems to activate or deactivate the display of debug data.
	class ToggleDebugDisplay : public Message
	{
	public:
		bool DebugActive;
		ToggleDebugDisplay(bool debugActive)
			: Message(Mid::ToggleDebugInfo), DebugActive(debugActive) {};
	};
	//
	class SystemPause: public Message
	{
	public:
		bool Pause;
		friend class CoreEngine;
	//private:
		SystemPause(bool pause):
			Message(Mid::SystemPause),
			Pause(pause)
		{
			//
		};
		~SystemPause(){};
	};
	//
	class ToggleTimeDisplay: public Message
	{
	public:
		ToggleTimeDisplay()
			:Message(Mid::ToggleTime){};
	};
	//A global pointer to the instance of the core
	extern CoreEngine* CORE;
}