#pragma once //Makes sure this header is only included once

#include "GameData.h"
#include <vector>
#include <map>
#include "Composition.h"
#include "Engine.h"
//
namespace Framework
{
		
	class SaveLevelClass
	{
	public:	
		SaveLevelClass();
		~SaveLevelClass();
		void Initialize();
		void SaveLevel(const std::string& file);
		void SaveLevel(int);
		void Save(const std::string& file);
		bool ValidateConstraint(GOC*);
	private:
		std::vector<GOC*> ConstraintsObjects;
		std::map<int,GOC *> ObjectsThatHaveConstraints;
		GameData* _gameData;
	};

	//A global pointer to our game so that it can be accessed from anywhere.
	extern SaveLevelClass* SAVECLASS;
}


