#include "GameData.h"
//#include "Archetype.h"
//#include "Precompiled.h"
#include "DebugTools.h"
#include "Utility.h"
//
namespace Framework
{
	GameData* GAMEDATA = NULL;
	//
	GameData::GameData()
		:_reader(NULL),
		_archetypes(NULL),
		_stream(NULL)
	{
		//ErrorIf(GAMEDATA!=NULL,"Gamedata already initialized\n");
		GAMEDATA = this;
	}
	GameData::~GameData()
	{
		utility::safeDelete(_archetypes);
		utility::safeDelete(_gameSettings);
		utility::safeDelete(_stream);
		utility::safeDelete(_reader);
		//
		for each( XMLReader* r in _readers )
		{
			utility::safeDelete( r );
		}
	}
	void GameData::initialize()
	{
		_reader = new XMLReader();
		_archetypes = new ArchetypeCollection();
	}
	void GameData::loadLevelData(const char* file)
	{
		//std::cout<<"Loading level data file: "<<file<<"\n";
		if(_reader->open(file))
		{
			_stream = _reader->data().clone();
			//
			_gameSettings = _stream->clone();
			_gameSettings->readProperty("Settings", *_gameSettings);
			//
			//_archetypes->createCollectionFrom(*_stream);
		}
	}
	void GameData::LoadArchetypeData(const char* file)
	{
		XMLReader* reader = new XMLReader();
		if(reader->open(file))
		{
			ISerializer* stream = reader->data().clone();
			//
			_archetypes->createCollectionFrom( *stream );
			_readers.push_back( reader );
		}
	}
}