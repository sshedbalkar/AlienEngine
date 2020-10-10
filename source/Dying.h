#pragma once //Makes sure this header is only included once
#include "Composition.h"
#include "Message.h"
#include <iostream>

namespace Framework
{
	class Dying: public GameComponent
	{
		//friend class RigidBody;
	public:
		Dying();
		~Dying();
		virtual void SendMessage(Message *);
		virtual void Initialize();
		virtual void Serialize(ISerializer& stream);
		virtual void RemoveComponentFromGame();
		virtual void RestoreComponentToGame();
		void Update(const float &);	
		void SetIsDying(bool t);//used for timemechanic
		void SetTransparrency(float t);//used for timemechanic
		void MarkAsDying();
		bool GetIsDying(){return m_isDying;}

		Dying * Next;
		Dying * Prev;
	private:
		bool m_isDying;
		float m_dyingRate;
		
	};
}