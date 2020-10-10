////////////////////////////////////////////////////////////////
//
//	GameData.h
//	Parses an external game settings xml file and holds the data for later usage by other parts of the game.
//
//	Author: Santosh Shedbalkar
//	Copyright 2011, Digipen Institute of Technology
//
////////////////////////////////////////////////////////////////
#ifndef GAMEDATA_H
#define GAMEDATA_H
//#pragma message("Compiling: "__FILE__)
//
//#include <vector>
//#include "System.h"
#include "XMLSerialization.h"
#include "Archetype.h"
//#include "VMath.h"
//
namespace Framework
{
	class GameData
	{
//#pragma message("Including Class: GameData")
	public:
		GameData();
		~GameData();
		void initialize();
		void loadLevelData(const char* file);
		inline ArchetypeCollection* archetypes()
		{
			return _archetypes;
		}
		inline const ISerializer& stream()
		{
			return *_stream;
		}
		//
		template <typename T>
		inline bool readSetting(const char* prop, T& data)
		{
			return _gameSettings->readProperty(prop, data);
		}
		void LoadArchetypeData(const char* file);
		//
	private:
		typedef std::vector< XMLReader* > ReaderList;
		ReaderList _readers;
		XMLReader* _reader;
		ISerializer* _stream;
		ArchetypeCollection* _archetypes;
		ISerializer* _gameSettings;
	};
	//
	extern GameData* GAMEDATA;
}
//
#endif