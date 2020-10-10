#pragma once //Makes sure this header is only included once
#include "Composition.h"
#include "HistoryComponent.h"
#include "Dying.h"

namespace Framework
{
	
	class LogicComponentManager
	{
		//friend class LoadLevelClass;
	public:
		LogicComponentManager()
		{
		
		}
		~LogicComponentManager(){};

		void AddGOCtoLogicContainer(GOC*);
		void RegisterComponents();
		void Update(const float&);
		void Initialize();


		std::map<int,GOC*> ActivatableObjectsList;
		std::map<int,GOC*> RopePointsList;
		std::vector<GOC*> LaserObjectsList;
		std::vector<GOC*> ObjThatCanDieList;
		std::vector<GOC*> DominosList;
		std::vector<GOC*> AutoMovingObjects;

	private:
		typedef ObjectLinkList<HistoryComponent>::iterator HistoryComponentIterator;
		typedef ObjectLinkList<Dying>::iterator DyingObjectsComponentIterator;
	};
}