#pragma once //Makes sure this header is only included once
#include "Composition.h"
#include "Message.h"
#include <iostream>

namespace Framework
{
	class ConnectorComponent: public GameComponent
	{
		friend class TweakBarManager;
		friend class LoadLevelClass;
		friend class SaveLevelClass;
		friend class ActivationProperties;
	public:
		ConnectorComponent();
		~ConnectorComponent(){}
		virtual void SendMessage(Message *);
		virtual void Initialize();
		virtual void Serialize(ISerializer& stream);
		void Update(const float & );	
		bool AllowStatic(){return m_canCollideWithStatic;}
		void RemoveComponentFromGame();
	private:
		bool m_isVertical;
		bool m_canCollideWithStatic;
	};
}