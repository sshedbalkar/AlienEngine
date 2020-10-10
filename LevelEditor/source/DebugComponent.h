#pragma once //Makes sure this header is only included once
#include "Composition.h"
#include "Message.h"
#include <iostream>
#include "Physics.h"

namespace Framework
{
	class DebugComponent: public GameComponent
	{
		//friend class RigidBody;
	public:
		DebugComponent();
		~DebugComponent(){}
		virtual void SendMessage(Message *);
		virtual void Initialize();
		virtual void Serialize(ISerializer& stream);
		void Update(float &);	
		void SetIsCube(bool t){m_IsCube=t;}
		bool IsCube(){return m_IsCube;}
		void Init();
	private:
		bool m_IsCube;
		std::string ModelName;
	};
}