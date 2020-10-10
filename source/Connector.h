#pragma once //Makes sure this header is only included once
#include <vector>
#include "Composition.h"

namespace Framework
{
	class Connector
	{
		//friend class GameLogic;
	public:
		Connector();
		~Connector(){}
		void Initialize();
		void Update();		
		void AddBody1(GOC* );
		void AddBody2(GOC* );
		void AddBody(GOC* );
		void ConnectObjects();
		void ConnectObjectsVertical(real &distance,Vector3 &a,Vector3 &b);
		bool CheckIfCanConnect();
		void SendMessage(Message *);
		void Reset();
		bool IsStaticCollisionAllowed(){return m_StaticCollisionAllowed;}
		GOC * GetBridgeObj(){return Bridge;}
	
	private:
		GOC *ObjectA,*ObjectB, *Bridge;
		bool m_StaticCollisionAllowed;
	};
}