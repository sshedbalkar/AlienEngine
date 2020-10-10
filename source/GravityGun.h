#pragma once //Makes sure this header is only included once
#include "PhysicsMath.h"
#include <vector>
#include "Composition.h"

namespace Framework
{
	class GravityGun
	{
		//friend class LogicCollisionResolve;
		//friend class Physics;
		//friend class Laser;
	public:
		GravityGun(){m_PickedItem=NULL;m_objectPicked=false;}
		~GravityGun(){}
		void Initialize();
		void Update(float dt);		
		void AddPickedItem(GOC* i);	
		void RemovePickedItem();
		void SendMessage(Message *);
		void ActionIfPickedItemDestroyed(GOC *);
		GOC * GetPickedItem(){return m_PickedItem;}
		GOC * GetPreviouslyPickedItem(){return m_PreviousPickedItem;}
		void SetPreviouslyPickedItem(GOC * g){ m_PreviousPickedItem=g;}
	private:
		GOC * m_PickedItem;
		GOC * m_PreviousPickedItem;
		Quaternion orientation;
		Vector3 dirAtPickTime;
		real InitialMass;
		bool m_objectPicked;
	};
}
