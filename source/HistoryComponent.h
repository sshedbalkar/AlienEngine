#pragma once //Makes sure this header is only included once
#include "Composition.h"
#include <iostream>
#include "Physics.h"
#include "Transform.h"
#include "HistoryData.h"

namespace Framework
{
	class HistoryComponent : public GameComponent
	{
		
	public:
		HistoryComponent();
		~HistoryComponent();

		virtual void SendMessage(Message *);
		virtual void Initialize();
		virtual void Serialize(ISerializer& stream);
		virtual void RemoveComponentFromGame();
		virtual void RestoreComponentToGame();
		void SaveWhenDestroyed();
		void Update(const float &);
		void Update(HistoryData*,bool goingBack);
		HistoryData* CreateHistoryData(HistoryDataType type);
		void updateInitialHistoryData();

		HistoryComponent * Next;
		HistoryComponent * Prev;
	private:
		bool m_firstTime;
		bool m_previousActivatedState;
		float m_previousTimer;
		bool m_ActivateFlag;
		//bool m_isStartOfLevel;
		HistoryData * InitialData;
	};
}