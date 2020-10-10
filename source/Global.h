#pragma once
//
namespace utility
{
	class TimeCalculator;
}
//
namespace Memory
{
	class MemAllocatorGen;
	//
	void PrintLeaks();
	void PrintStats();
}
//
namespace Framework
{
	class TextDisplay;
	class Menu;
	class TextureDraw;
	class MainMenu;
	class GameHistory;
	//
	extern utility::TimeCalculator*		g_timeCalculator;
	extern TextDisplay*					g_textDisplay;
	extern Menu*						g_menu;
	extern TextureDraw*					g_textureDraw;
	extern MainMenu*					g_mainMenu;
	extern GameHistory*					g_gameHistory;
	extern Memory::MemAllocatorGen*		g_memAllocator;
	//
	int									GlobalInit();
	int									GlobalFree();
	
}