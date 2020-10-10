#pragma once //Makes sure this header is only included once
#include "Composition.h"
#include "Message.h"
#include <iostream>
#include "Physics.h"

namespace Framework
{
	class Domino: public GameComponent
	{
		friend class TweakBarManager;
		friend class LoadLevelClass;
		friend class SaveLevelClass;
	public:
		Domino();
		~Domino(){}
		virtual void SendMessage(Message *);
		virtual void Initialize();
		virtual void Serialize(ISerializer& stream);
		virtual void RemoveComponentFromGame();
		void MarkAsActive(){m_active=true;}
		void MarkAsFinished(bool t);
		bool GetIsFinished(){return m_Finished ;}
		void Update(const float &);	
		Quaternion GetInitialOrientation(){return m_inverseInitialQuat;}	
		

	private:
		bool m_Finished;
		float m_timer;
		float m_stoptimer;
		Vector3 m_initialPos;
		bool m_active;
		bool firstCollide;
		RigidBody * m_Body;
		Quaternion m_inverseInitialQuat;
		bool onX;
		bool leftToRight;
		bool stopped;
	};
}