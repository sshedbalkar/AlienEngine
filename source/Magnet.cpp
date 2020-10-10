#include "Magnet.h"
#include "Physics.h"
#include "WindowsSystem.h"
#include "GameLogic.h"

namespace Framework
{

	void Magnet::SendMessage(Message *m)
	{
		switch( m->MessageId )
		{

		case Mid::Collide:
			{
				MessageCollide * msg = (MessageCollide*)m;

				if(ObjectA == msg->CollideObject && ObjectB == msg->CollidedWith )
				{
					if(ObjectA && ObjectB)
					{
						ObjectA->has(RigidBody)->setRotation(0,0,0);
						ObjectA->has(RigidBody)->setVelocity(ObjectA->has(RigidBody)->getVelocity()*0.1);
						if(!ObjectA->has(RigidBody)->getIsStatic () && !ObjectB->has(RigidBody)->getIsStatic ())
						{
							ObjectA->has(RigidBody)->setRotation(0,0,0);
							ObjectA->has(RigidBody)->setVelocity(ObjectA->has(RigidBody)->getVelocity()*0.1);
							Reset();
						}
					}
					/*magnet.ObjectB->has(RigidBody)->setVelocity(0,0,0);
					magnet.ObjectA->has(RigidBody)->setVelocity(0,0,0);*/
				
				}
				break;
			}				

		case Mid::CharacterKey:
			{
				break;
			}
		case Mid::VirtualKey:
			{
				break;
			}
		case Mid::MouseMove:
			{
				break;
			}
		case Mid::MouseWheel :
			{
				break;
			}
		case Mid::RMouseButton:
			{
				Reset();
				break;
			}
		case Mid::LMouseButton:
			{
				LMouseButton * lmouse = (LMouseButton*)m;
				if(lmouse->ButtonIsPressed)
				{
					AddBody(LOGIC->objectPointedAtMagnet );
					/*if(!ObjectA)
						AddBody1(LOGIC->objectPointedAt);	
					else if(!ObjectB)
						AddBody2(LOGIC->objectPointedAt);	*/
					//else nothing
				}
				break;
				
			}
		}

	}

	void Magnet::Initialize()
	{//when we change or restart level
		LOGIC->objectPointedAtMagnet=0;
		ObjectA=ObjectB=0;
	}

	void Magnet::Reset()
	{	if(ObjectA)
			ObjectA->has(RigidBody)->setMass(InitialMassA);	
		if(ObjectB)
			ObjectB->has(RigidBody)->setMass(InitialMassB);
		ObjectA=ObjectB=0;
	}

	void Magnet::AddBody(GOC* g)
	{
		if(g==0) return;
		if(g->CompositionTypeId==GOC_Type_HangingBall ||  g->CompositionTypeId==GOC_Type_MetalPlate) 
		{
			if(ObjectA && ObjectB)
				Reset();

			if (ObjectA==0)
				AddBody1(g);
			else if (ObjectB==0)
				AddBody2(g);
		}

	}


	void Magnet::AddBody1(GOC* g)
	{
		//if(g)
		//{
		InitialMassA=g->has(RigidBody)->getMass();
		if(!g->has(RigidBody)->getIsStatic ())
			g->has(RigidBody)->setMass(1);
		//}
		ObjectA=g;
	}		
	
	void Magnet::AddBody2(GOC* g)
	{
		//if(g)
		//{
		InitialMassB=g->has(RigidBody)->getMass();
		if(!g->has(RigidBody)->getIsStatic ())
			g->has(RigidBody)->setMass(1);
		//}
		ObjectB=g;
	}		



	void Magnet::Update()
	{
		if(ObjectA==NULL || ObjectB==NULL) return;
		Vector3 posa,posb;
		posa=ObjectA->has(Transform)->Position;
		posb=ObjectB->has(Transform)->Position;
		//if(squaredDistance(posa,posb)>200) return;
		Vector3 dir=posb-posa;
		dir.normalise();
		ObjectA->has(RigidBody)->addForce(dir*80);
		ObjectB->has(RigidBody)->addForce(dir*-80);
	}
}