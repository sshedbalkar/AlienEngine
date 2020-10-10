///////////////////////////////////////////////////////////////////////////////////////
//
//	WindowsSystem.cpp
//	
//	Authors:  , 
//	Copyright 2010, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////
#include "Precompiled.h"
#include "WindowsSystem.h"
#include "Core.h"
#include "Message.h"
#include "AntTweakBar.h"
#include <iostream>

namespace Framework
{
	//A global pointer to the windows system
	WindowsSystem* WINDOWSSYSTEM = NULL;

	//Used by windows to identify our window class type
	//(it's needed for registering/creating/unregistering the window)
	const char windowsClassName[] = "FrameworkEngineWindowClass";

	//Process any windows messages and run the game until we get a quit message
	//While we don't use the window handle, in other cases we might want to only process messages for this window
	void WindowsSystem::Update(float dt)
	{
		static_cast<void>(dt);
		//if(!_paused)
		{
			MSG msg;
			//Look for any pending windows messages, remove them, then handle them
			//The second parameter is the window handle--NULL just means get any message from the current thread
			//The third and forth parameters are the start and end message types to process
			//The last parameter determines whether or not the message is removed
			while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) //It is important to get all windows messages available not just one
			{
				TranslateMessage(&msg);	//Makes sure WM_CHAR and similar messages are generated
				DispatchMessage(&msg);	//Calls the message procedure (see below) with this message

				//If we get a quit message, broadcast it to all systems
				if (msg.message == WM_QUIT)
				{
					MessageQuit q;
					CORE->BroadcastMessage(&q);
				}
			}
		}
	}

	//The message handling procedure for the game
	LRESULT WINAPI MessageHandler(HWND hWnd,	 //The window the message is for (ours in this case)
								  UINT msg,		 //The message type
								  WPARAM wParam, //The primary data for the message
								  LPARAM lParam) //The secondary data for the message (if any)
	{
		int twhandled = TwEventWin( hWnd, msg, wParam, lParam ); // send event message to AntTweakBar
		if ( twhandled && msg != WM_INPUT ) return 0;
		
		
		WINDOWSSYSTEM->currenthwnd=GetForegroundWindow();
		if (WINDOWSSYSTEM->currenthwnd==hWnd)
		{
			//See what type of message it is
			switch(msg)
			{
			case WM_INPUT:
				{
					POINT pt;
					GetCursorPos(&pt);
					WINDOWSSYSTEM->MousePosition.x = static_cast<SHORT>(pt.x);
					WINDOWSSYSTEM->MousePosition.y = static_cast<SHORT>(pt.y);
					MouseMove m(Vec2(WINDOWSSYSTEM->MousePosition.x,WINDOWSSYSTEM->MousePosition.y));
					
					CORE->BroadcastMessage(&m);
					break;
				}
			case WM_CHAR: //A character key was pressed
				{
					if (!IsCtrlHeld() && !IsAltHeld()) //do not handle keys if ctr or alt is pressed
					{
						//Create a key message
						MessageCharacterKey key;
						//Set the character pressed (the wParam is the ascii value)
						key.character = wParam;
						//Broadcast the message to all systems
						CORE->BroadcastMessage(&key);
						break;
					}
				}
			case WM_LBUTTONDOWN:
				{
					LMouseButton m(LMouseButton::LeftMouse,true,Vec2(WINDOWSSYSTEM->MousePosition.x,WINDOWSSYSTEM->MousePosition.y));
					CORE->BroadcastMessage(&m);
					break;
				}
			case WM_RBUTTONDOWN:
				{
					RMouseButton m(RMouseButton::RightMouse,true,Vec2(WINDOWSSYSTEM->MousePosition.x,WINDOWSSYSTEM->MousePosition.y));
					CORE->BroadcastMessage(&m);
					break;
				}
			case WM_LBUTTONUP:
				{
					LMouseButton m(LMouseButton::LeftMouse,false,Vec2(WINDOWSSYSTEM->MousePosition.x,WINDOWSSYSTEM->MousePosition.y));
					CORE->BroadcastMessage(&m);
					break;
				}
			case WM_RBUTTONUP:
				{
					RMouseButton m(RMouseButton::RightMouse,false,Vec2(WINDOWSSYSTEM->MousePosition.x,WINDOWSSYSTEM->MousePosition.y));
					CORE->BroadcastMessage(&m);
					break;
				}
			case WM_KEYDOWN: //A key was pressed
				{
					if (!IsCtrlHeld() && !IsAltHeld()) 
					{
						if((wParam < 0x30 || wParam > 0x5A) && (wParam != VK_SPACE) && (wParam != VK_ESCAPE)  )//
						{
							//std::cout<<"Dispatching from WM_KEYDOWN: "<<wParam<<"\n";
							//TODO: Handle any key logic you might need for game controls
							//Use virtual key codes (VK_LEFT, VK_RIGHT, VK_UP, VK_DOWN, VK_SPACE, etc.)
							//to detect specific keys (the wParam is the key pressed) and then
							//broadcast whatever message you need
							MessageVirtualKey key;
							key.character = wParam;
							CORE -> BroadcastMessage(&key);
						}
					}
					else // if control or alt held then check
					{
						if((wParam != VK_SPACE) && (wParam != VK_ESCAPE)  )//
						{
							//std::cout<<"Dispatching from WM_KEYDOWN: "<<wParam<<"\n";
							//TODO: Handle any key logic you might need for game controls
							//Use virtual key codes (VK_LEFT, VK_RIGHT, VK_UP, VK_DOWN, VK_SPACE, etc.)
							//to detect specific keys (the wParam is the key pressed) and then
							//broadcast whatever message you need
							MessageVirtualKey key;
							key.character = wParam;
							CORE -> BroadcastMessage(&key);
						}
					}
					break;
				}
			case WM_KEYUP: //A key was released
				{
					//TODO: Handle any key logic you might need for game controls
					if((wParam < 0x30 || wParam > 0x5A)  && (wParam != VK_ESCAPE))//&& (wParam != VK_SPACE)
					{
						MessageCharacterKeyRelease key;
						key.character = wParam;
						CORE -> BroadcastMessage(&key);
					}
					break;
				}
			case WM_DESTROY: //A destroy message--time to kill the game
				//Make sure we shut everything down properly by telling Windows
				//to post a WM_QUIT message (the parameter is the exit code).
				PostQuitMessage(0);
				return 0;
			case WM_DROPFILES:
				{
					uint itemCount = DragQueryFile((HDROP)wParam, 0xFFFFFFFF,0,0);
					if (itemCount)
					{
						char  buffer[512] = {0};
						DragQueryFile((HDROP)wParam, 0, buffer, 512);
						DragFinish((HDROP)wParam);


						FileDrop drop(buffer);
						CORE->BroadcastMessage(&drop);
					}
					return 0;
				}
			case WM_SYSKEYDOWN:
				{
					//Eat the WM_SYSKEYDOWN message to prevent freezing the game when
					//the alt key is pressed
					switch( wParam )
					{
					case VK_LMENU:
					case VK_RMENU:
						return 0;//Stop Alt key from stopping the winProc
					case VK_F4:
						//Check for Alt F4
						DWORD dwAltKeyMask = ( 1 << 29 );
						if( ( lParam & dwAltKeyMask ) != 0 )
							PostQuitMessage(0);
						return 0;
					}
					return 0;//
				}
			case WM_MOUSEWHEEL:
				{
					MouseWheel m(GET_KEYSTATE_WPARAM(wParam), GET_WHEEL_DELTA_WPARAM(wParam)/WHEEL_DELTA);
					//std::cout<<"Mouse wheel: Key: " << GET_KEYSTATE_WPARAM(wParam) << ", Delta: " << GET_WHEEL_DELTA_WPARAM(wParam) << ", Dispatching: " << &m<<"\n";
					CORE->BroadcastMessage(&m);
					break;
				}
			}
			//TODO: Handle mouse messages and other windows messages
		}
		//We didn't completely handle the message, so pass it on for Windows to handle.
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	/*! Defines used for human interface devices */
	#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
	#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
	#define HID_USAGE_GENERIC_KEYBOARD     ((USHORT) 0x03)

	WindowsSystem::WindowsSystem(const char* windowTitle, int ClientWidth, int ClientHeight, bool FullScreen)
		:_paused(false)
	{
		//Check to make sure the windows system is created before the factory
		ErrorIf(FACTORY != NULL, "Windows System already created");

		//Set the global pointer to the windows system
		WINDOWSSYSTEM = this;

		//The size passed to CreateWindow is the full size including the windows border and caption 
		//AdjustWindowRect will adjust the provided rect so that the client size of the window is the desired size
//		RECT fullWinRect = {0, 0, ClientWidth, ClientHeight};
//		AdjustWindowRect(&fullWinRect,			//The rectangle for the full size of the window
//						WS_OVERLAPPEDWINDOW,	//The style of the window, which must match what is passed in to CreateWindow below
//						FALSE);					//Does this window have a menu?

		//Register the window class for the game.
		WNDCLASSEX wc = {sizeof(WNDCLASSEX),	//The size of this structure (passing the size allows Microsoft to update their interfaces and maintain backward compatibility)
			CS_CLASSDC,							//The style of the window class--this is the base type (one device context for all windows in the process)
			MessageHandler,						//The name of the message handling function
			0L, 0L,								//The amount of extra memory to allocate for this class and window
			GetModuleHandle(NULL),				//Handle to the instance that has the windows procedure--NULL means use this file.
			NULL,								//Add an Icon as a resource and add them here
			LoadCursor(NULL, IDC_ARROW),		//Use the default arrow cursor
			NULL, NULL,							//The background brush and menu--these can be NULL
			windowsClassName, NULL};			//The class name and the small icon (NULL just uses the default)

		RegisterClassEx(&wc);
		ShowCursor(false);

		//Store the handle to the instance
		hInstance = wc.hInstance;
		
		if ( FullScreen )
		{
			//DEVMODE screen_settings;
			//memset(&screen_settings, 0, sizeof(screen_settings));
			//screen_settings.dmSize       = sizeof(screen_settings);
			//screen_settings.dmPelsWidth  = ClientWidth;
			//screen_settings.dmPelsHeight = ClientHeight;
			//screen_settings.dmBitsPerPel = 32;			
			//screen_settings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
			//// Change the display settings to full screen.
			//ChangeDisplaySettings(&screen_settings, CDS_FULLSCREEN);
		}
		else
		{
		}

		
		//move console so that it is visible
		HWND hWnd1 = GetConsoleWindow();
		if( hWnd1 == NULL)
		{
			//std::cout << "Error getting console window handle\n";
		}
		else{
			MoveWindow(hWnd1,1280,400,1000,500,TRUE);
		}


		//Create the game's window			
		hWnd = CreateWindow(windowsClassName,	//The class name
			windowTitle,						//The name for the title bar
			WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,//The style of the window (WS_BORDER, WS_MINIMIZEBOX, WS_MAXIMIZE, etc.)
			0, 0,								//The x and y position of the window (screen coords for base windows, relative coords for child windows)
			ClientWidth,						//Width of the window, including borders
			ClientHeight,						//Height of the window, including borders and caption
			GetDesktopWindow(),					//The parent window
			NULL,								//The menu for the window
			hInstance,							//The handle to the instance of the window (ignored in NT/2K/XP)
			NULL);								//The lParam for the WM_CREATE message of this window

		DragAcceptFiles( hWnd, true );

		// Register mouse and keyboard raw input devices @@
		RAWINPUTDEVICE Rid[2];
		Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC; 
		Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE; 
		Rid[0].dwFlags = RIDEV_INPUTSINK;   
		Rid[0].hwndTarget = hWnd;
	
		Rid[1].usUsagePage = 1; 
		Rid[1].usUsage = 6; 
		Rid[1].dwFlags = 0;   
		Rid[1].hwndTarget = hWnd;
		RegisterRawInputDevices(Rid, 2, sizeof(Rid[0]));
	}

	WindowsSystem::~WindowsSystem()
	{
		//Unregister the window class
		UnregisterClass(windowsClassName, hInstance);
	}

	void WindowsSystem::ActivateWindow()
	{
		//Show the window (could also be SW_SHOWMINIMIZED, SW_SHOWMAXIMIZED, etc.)
		ShowWindow(hWnd, SW_SHOWDEFAULT);
		//Send a WM_PAINT message to the window
		UpdateWindow(hWnd);
	}
	//
	void WindowsSystem::SendMessage(Message *message)
	{
		if (message->MessageId == Mid::SystemPause)
		{
			const SystemPause *m = static_cast<const SystemPause*>(message);
			_paused = m->Pause;
		}
	}

	bool IsWindowActive()
	{
		HWND currenthwnd;
		currenthwnd=GetForegroundWindow();
		if (currenthwnd!=WINDOWSSYSTEM->hWnd )
			return false;
		else
			return true;
	}


	Vec2 GetMousePosition()
	{
		POINT pt;
		GetCursorPos(&pt);
		return Vec2(static_cast<SHORT>(pt.x),static_cast<SHORT>(pt.y));
	}

	//
	bool CheckIfCharPressed(int c)
	{
		HWND currenthwnd;
		currenthwnd=GetForegroundWindow();
		if (currenthwnd!=WINDOWSSYSTEM->hWnd )
			return false;
		else
		{

			return GetKeyState(c) < 0 ;
		}
	}

	bool IsSpaceHeld(){	return CheckIfCharPressed(VK_SPACE);}
	bool IsShiftHeld(){	return (CheckIfCharPressed(VK_LSHIFT) || CheckIfCharPressed(VK_RSHIFT));}
	bool IsCtrlHeld(){return (CheckIfCharPressed(VK_LCONTROL) || CheckIfCharPressed(VK_RCONTROL));}
	bool IsAltHeld(){return (CheckIfCharPressed(VK_LMENU) || CheckIfCharPressed(VK_RMENU));}
	bool IsUpHeld(){ return CheckIfCharPressed(VK_UP);}
	bool IsDownHeld(){ return CheckIfCharPressed(VK_DOWN); }
	bool IsLeftHeld(){ return CheckIfCharPressed(VK_LEFT); }
	bool IsRightHeld(){  return CheckIfCharPressed(VK_RIGHT ); }
	bool IsWHeld(){ return CheckIfCharPressed(0x57);  }
	bool IsSHeld(){ return CheckIfCharPressed(0x53); }
	bool IsAHeld(){ return CheckIfCharPressed(0x41); }
	bool IsDHeld(){ return CheckIfCharPressed(0x44);}
}
