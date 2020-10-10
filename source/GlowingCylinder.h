#pragma once //Makes sure this header is only included once
#include "Composition.h"
#include "Message.h"

namespace Framework
{
	class GlowingCylinders: public GameComponent
	{
		friend class TweakBarManager;
		friend class LoadLevelClass;
		friend class SaveLevelClass;
	public:
		GlowingCylinders();
		~GlowingCylinders(){}
		virtual void SendMessage(Message *);
		virtual void Initialize();
		virtual void Serialize(ISerializer& stream);
		virtual void RemoveComponentFromGame();
		int GetParentActivationId(){return m_parentActivationId;}
		void UpdateGlowingCylinders();
	private:
		int m_parentActivationId;

	};
}