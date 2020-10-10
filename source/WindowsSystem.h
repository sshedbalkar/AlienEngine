///////////////////////////////////////////////////////////////////////////////////////
///
///	\file WindowsSystem.h
///  This is a system that wraps everything related to the Windows OS. If you wished,
///  you could have a different system for each OS your game supported.
///	
///	Authors:  , 
///	Copyright 2010, Digipen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////////////
#pragma once //Makes sure this header is only included once

#include "Engine.h"
//
namespace Framework
{
	class Message;
	///Basic manager for windows. Implements the windows message pump and
	///broadcasts user input messages to all the systems.
	class WindowsSystem : public ISystem
	{
	private:
		bool _paused;
	public:
		WindowsSystem(const char* windowTitle, int ClientWidth, int ClientHeight, bool FullScreen);	//The constructor
		~WindowsSystem();															//The destructor

		void ActivateWindow();								//Activate the game window so it is actually visible
		virtual void Update(float dt);						//Update the system every frame
		virtual std::string GetName() {return "Windows";}	//Get the string name of the system
		HWND currenthwnd;
		HWND hWnd;											//The handle to the game window
		HINSTANCE hInstance;								//The handle to the instance
		POINTS MousePosition;
		virtual void SendMessage(Message *message);
	};

	///Message signaling that a key is pressed.
	class MessageCharacterKey : public Message
	{
	public:
		MessageCharacterKey() : Message(Mid::CharacterKey) {};	
		int character;
	};
	
	class MessageVirtualKey : public Message
	{
	public:
		MessageVirtualKey() : Message(Mid::VirtualKey) {};	
		int character;
	};

	class MessageCharacterKeyRelease : public Message
	{
	public:
		MessageCharacterKeyRelease() : Message(Mid::CharacterKeyRelease) {};	
		int character;
	};

	///Message signaling that a mouse button state has changed.
	class LMouseButton: public Message
	{
	public:
		enum MouseButtonIndexId
		{
			LeftMouse,
			RightMouse
		};
		LMouseButton(MouseButtonIndexId button,bool state,Vec2 position) 
			: Message(Mid::LMouseButton) ,  MouseButtonIndex(button) , ButtonIsPressed(state), MousePosition(position) {};

		MouseButtonIndexId MouseButtonIndex;
		bool ButtonIsPressed;
		Vec2 MousePosition;
	};
	
	class RMouseButton: public Message
	{
	public:
		enum MouseButtonIndexId
		{
			LeftMouse,
			RightMouse
		};
		RMouseButton(MouseButtonIndexId button,bool state,Vec2 position) 
			: Message(Mid::RMouseButton) ,  MouseButtonIndex(button) , ButtonIsPressed(state), MousePosition(position) {};

		MouseButtonIndexId MouseButtonIndex;
		bool ButtonIsPressed;
		Vec2 MousePosition;
	};

	///Message signaling that the mouse has moved.
	class MouseMove: public Message
	{
	public:
		MouseMove(Vec2 position) : Message(Mid::MouseMove) , MousePosition(position) {};	
		Vec2 MousePosition;
	};

	///Message signaling that a file was dropped onto the window.
	class FileDrop: public Message
	{
	public:
		FileDrop(std::string filename) : Message(Mid::FileDrop) , FileName(filename) {};	
		std::string FileName;
	};
	//
	class MouseWheel: public Message
	{
	public:
		MouseWheel(int key, int delta): Message(Mid::MouseWheel), Key(key), Delta(delta) {};
		int Key;
		int Delta;
	};
	//
	bool CheckIfCharPressed(int);
	Vec2 GetMousePosition();
	bool IsWindowActive();
	bool IsSpaceHeld();
	bool IsShiftHeld();
	bool IsCtrlHeld();
	bool IsAltHeld();
	bool IsUpHeld();
	bool IsDownHeld();
	bool IsLeftHeld();
	bool IsRightHeld();
	bool IsWHeld();
	bool IsAHeld();
	bool IsSHeld();
	bool IsDHeld();
	extern WindowsSystem* WINDOWSSYSTEM;

	inline void ShowCursorTil(bool _b){
		int DisplayCount;
		do{
			DisplayCount = ShowCursor(_b);
		}while( DisplayCount < 0 );
	}
	inline void HideCursorTil(bool _b){
		int DisplayCount;
		do{
			DisplayCount = ShowCursor(_b);
		}while( DisplayCount >= 0 );
	}
}