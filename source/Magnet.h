#pragma once //Makes sure this header is only included once
#include "PhysicsMath.h"
#include <vector>
#include "Composition.h"

namespace Framework
{
	class Magnet
	{
		friend class PowerGun;
	public:
		Magnet(){ObjectA=NULL;ObjectB=NULL;}
		~Magnet(){}
		void Initialize();
		void Update();		
		void AddBody1(GOC* );	
		void AddBody2(GOC* );		
		void SendMessage(Message *);
		void Reset();
		void AddBody(GOC* );
	private:
		GOC *ObjectA,*ObjectB;
		real InitialMassA;
		real InitialMassB;
	};
}