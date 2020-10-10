#pragma once //Makes sure this header is only included once
#include "Composition.h"
#include "Message.h"
#include <iostream>

namespace Framework
{
	class LightComponent: public GameComponent
	{
		friend class TweakBarManager;
	public:
		LightComponent();
		~LightComponent(){}
		virtual void SendMessage(Message *);
		virtual void Initialize();
		virtual void Serialize(ISerializer& stream);
		virtual void RemoveComponentFromGame();
		void Update(float &);	
		bool UpdatePrevPos();
		int GetIndex(){return m_index;}
		void SetIndex(int k){m_index=k;}
		Vec3 Color;
		float fallout;
		Vec3 prevPos;
		bool CanMove;

	private:
		int m_index;
		

	};
}