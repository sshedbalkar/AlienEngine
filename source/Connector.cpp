#include "Precompiled.h"
#include "Connector.h"
#include "Physics.h"
#include "ModelComponent.h"
#include "PhysicsMath.h"
#include "VMath.h"
#include "WindowsSystem.h"
#include "GameLogic.h"
#include "ConnectorComponent.h"

namespace Framework
{

	Connector::Connector()
	{
		ObjectA=NULL;ObjectB=NULL;Bridge=NULL;
		m_StaticCollisionAllowed=false;
	}


	void Connector::SendMessage(Message *m)
	{
		switch( m->MessageId )
		{

		case Mid::Collide:
		{
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
					AddBody(LOGIC->objectPointedAt);
					/*if(!ObjectA)
						AddBody1(LOGIC->objectPointedAt);	
					else if (!ObjectB)
						AddBody2(LOGIC->objectPointedAt);	*/
					//else nothing
				}
				else
				{					
				}
				break;
			}
		}
	}


	void Connector::Initialize()
	{//when we change or restart level
		ObjectA=ObjectB=0;
		Bridge=0;
		m_StaticCollisionAllowed=false;
	}

	void Connector::Reset()
	{//when we change or restart level
		if(ObjectA)
			ObjectA->has(ModelComponent)->SwapTexture();
		if(ObjectB)
			ObjectB->has(ModelComponent)->SwapTexture();
		
		ObjectA=ObjectB=0;
		m_StaticCollisionAllowed=false;
	}

	void Connector::AddBody(GOC* g)
	{
		if(g==0) return;
		if(g->CompositionTypeId!=GOC_Type_Wall_Selectable ) return;

		if(ObjectA && ObjectB)
			Reset();

		if (ObjectA==0)
			AddBody1(g);
		else if (ObjectB==0)
			AddBody2(g);

	}

	void Connector::AddBody1(GOC* a)
	{
		if (ObjectA==a)
			return;

		if (ObjectA)//not null
		{
			ObjectA->has(ModelComponent)->SwapTexture();//restore previous model
			if(Bridge)
			{
				Bridge->Destroy();
				Bridge=NULL;
			}
		}
		
		if (a!=NULL && a->CompositionTypeId==GOC_Type_Wall_Selectable )
		{
			ObjectA=a;
			ObjectA->has(ModelComponent)->SwapTexture();//the new selected component should swap Model
			if(ObjectB && ObjectB==ObjectA)//if A=B
			{
				//replace ObjectB
				ObjectB->has(ModelComponent)->SwapTexture();
				ObjectB=0;
			}
			else if(ObjectB)//is A selected
			{
				/*if(CheckIfCanConnect())
					ConnectObjects();*/
			}
		}
		else
		{
			ObjectA=0;
		}
		
	}	

	void Connector::AddBody2(GOC* b)
	{
		if (ObjectB==b)
			return;

		if (ObjectB)//not null
		{
			ObjectB->has(ModelComponent)->SwapTexture();//restore previous model
			if(Bridge)
				{
					Bridge->Destroy();
					Bridge=NULL;
				}
		}

		if (b!=NULL && b->CompositionTypeId==GOC_Type_Wall_Selectable)
		{
			ObjectB=b;
			ObjectB->has(ModelComponent)->SwapTexture();//the new selected component should swap Model
			if(ObjectA && ObjectB==ObjectA)//if A=B
			{
				//replace ObjectA
				ObjectA->has(ModelComponent)->SwapTexture();
				ObjectA=0;
			}
			else if(ObjectA)//is A selected
			{
				/*if(CheckIfCanConnect())
					ConnectObjects();*/
			}
		}
		else
		{
			ObjectB=0;
		}
	}		


	bool Connector::CheckIfCanConnect()
	{
		if (abs(ObjectA->has(Transform)->Position.x - ObjectB->has(Transform)->Position.x)<EPSILON && ObjectA->has(Transform)->Position.z!=ObjectB->has(Transform)->Position.z)
		{
			return true;
		}
		else if (abs(ObjectA->has(Transform)->Position.z-ObjectB->has(Transform)->Position.z) < EPSILON && ObjectA->has(Transform)->Position.x!=ObjectB->has(Transform)->Position.x)
		{
			return true;
		}
		/*if (abs(ObjectA->has(Transform)->Position.x - ObjectB->has(Transform)->Position.x)<EPSILON  ||  
			abs(ObjectA->has(Transform)->Position.y - ObjectB->has(Transform)->Position.y)<EPSILON  ||
			abs(ObjectA->has(Transform)->Position.z - ObjectB->has(Transform)->Position.z)<EPSILON)
		{
			return true;
		}*/
		if(Bridge)
			Bridge->Destroy();
		return false;
	}

	void HelperFunc(Vector3 & v, Vector3 & w,Vector3 & closestA, float & distance)
	{
		float tempdist;
		tempdist=lengthSquared(Vector3ToVec3(v-w));
		if (tempdist<distance)
		{
			distance=tempdist;
			closestA=v;
		}
	}

	void Connector::ConnectObjectsVertical(real &distance,Vector3 &closestA,Vector3 &closestB)
	{
		//bool created=false;
		//if(!Bridge){
		//	Bridge= FACTORY->BuildFromArchetype("GOC_Type_Bridge");
		//	created=true;
		//}
		//Vector3 a= (ObjectA->has(RigidBody)->getPosition() + ObjectB->has(RigidBody)->getPosition())*0.5;
		////real distance= squaredDistance(ObjectA->has(RigidBody)->getPosition(),ObjectB->has(RigidBody)->getPosition());
		//real scale = (ObjectA->has(Transform)->Scale.y<ObjectB->has(Transform)->Scale.y)?ObjectA->has(Transform)->Scale.y:ObjectB->has(Transform)->Scale.y;
		//scale-=0.1;			
		//Bridge->has(Transform)->Position=Vector3ToVec3(a);
		//Bridge->has(RigidBody)->setPosition(a);
		//if((closestA.y < closestB.y))
		//{
		//	Vector3 temp=closestA;
		//	closestA=closestB;
		//	closestB=temp;
		//}
		//if (abs(ObjectB->has(Transform)->Position.x-ObjectA->has(Transform)->Position.x)<EPSILON)//then x is constant
		//{
		//			
		//	Vector3 angle = closestA - closestB;
		//	angle.normalise();
		//	Quaternion q; 
		//	if((closestA.z > closestB.z))
		//		angle*=-1;
		//	q.r=cos(angle.y/2.0);
		//	q.i=sin(angle.y/2.0);			
		//	Bridge->has(Transform)->QUATERNION = q.QuaternionToD3DXQUATERNION();
		//	Bridge->has(RigidBody)->setOrientation(q);
		//	real scalez=sqrt(distance)+ (scale-1)*cos(angle.x);
		//	Bridge->has(Transform)->Scale=Vec3(0.1,scale-1,scalez);

		//	static_cast<CollisionBox*>(Bridge->has(CollisionPrimitive))->halfSize = Vector3(0.05,(scale-1)/2,scalez/2.0);
		//}else
		//{

		//	Vector3 angle = closestA - closestB;
		//	angle.normalise();
		//	Quaternion q; 
		//	if((closestA.x < closestB.x))
		//		angle*=-1;
		//	q.r=cos(angle.y/2.0);
		//	q.k=sin(angle.y/2.0);	

		//	Bridge->has(Transform)->QUATERNION = q.QuaternionToD3DXQUATERNION();
		//	Bridge->has(RigidBody)->setOrientation(q);
		//	float scalex=(float)sqrt(distance)+ (scale-1.0f)*cos(angle.x);
		//	Bridge->has(Transform)->Scale=Vec3(scalex,scale-1,0.1);

		//	static_cast<CollisionBox*>(Bridge->has(CollisionPrimitive))->halfSize = Vector3(scalex/2.0,(scale-1)/2.0,0.05);
		//	
		//}
		//if (created)
		//	Bridge->Initialize();
		
	}

	void Connector::ConnectObjects()
	{
		//if(ObjectA->has(ConnectorComponent)->AllowStatic() && ObjectB->has(ConnectorComponent)->AllowStatic())
		//{
		//	m_StaticCollisionAllowed=true;
		//}
		//else
		//{
		//	m_StaticCollisionAllowed=false;
		//}

		//Vector3 k,v,w,closestA, closestB;
		//float distance=FLT_MAX;		

		////first take center of point B
		//w = ObjectB->has(Transform)->Position;
		//
		////check the distance from all top points of A and store the closestPoint============
		//k.x=(ObjectA->has(Transform)->Scale.x /2.0f);
		//k.y=(ObjectA->has(Transform)->Scale.y /2.0f);
		//k.z=0.0f;
		//v = ObjectA->has(RigidBody)->getTransform().transform(k);
		//
		//HelperFunc(v,w,closestA,distance);

		//k.x=-(ObjectA->has(Transform)->Scale.x /2.0f);
		//k.y=(ObjectA->has(Transform)->Scale.y /2.0f);
		//k.z=0.0f;
		//v = ObjectA->has(RigidBody)->getTransform().transform(k);
	
		//HelperFunc(v,w,closestA,distance);

		//k.x=0.0f;
		//k.y=(ObjectA->has(Transform)->Scale.y /2.0f);
		//k.z=(ObjectA->has(Transform)->Scale.z /2.0f);
		//v = ObjectA->has(RigidBody)->getTransform().transform(k);

		//HelperFunc(v,w,closestA,distance);

		//k.x=0.0f;
		//k.y=(ObjectA->has(Transform)->Scale.y /2.0f);
		//k.z=-(ObjectA->has(Transform)->Scale.z /2.0f);
		//v = ObjectA->has(RigidBody)->getTransform().transform(k);
	
		//HelperFunc(v,w,closestA,distance);
		////====================================================================================
		//
		//distance=FLT_MAX;
		//w=closestA;
		////check the distance from closestA to all top points of B and store the closest point==
		//k.x=(ObjectB->has(Transform)->Scale.x /2.0f);
		//k.y=(ObjectB->has(Transform)->Scale.y /2.0f);
		//k.z=0.0f;
		//v = ObjectB->has(RigidBody)->getTransform().transform(k);
	
		//HelperFunc(v,w,closestB,distance);

		//k.x=-(ObjectB->has(Transform)->Scale.x /2.0f);
		//k.y=(ObjectB->has(Transform)->Scale.y /2.0f);
		//k.z=0.0f;
		//v = ObjectB->has(RigidBody)->getTransform().transform(k);
		//
		//HelperFunc(v,w,closestB,distance);

		//k.x=0.0f;
		//k.y=(ObjectB->has(Transform)->Scale.y /2.0f);
		//k.z=(ObjectB->has(Transform)->Scale.z /2.0f);
		//v = ObjectB->has(RigidBody)->getTransform().transform(k);
	
		//HelperFunc(v,w,closestB,distance);

		//k.x=0.0f;
		//k.y=(ObjectB->has(Transform)->Scale.y /2.0f);
		//k.z=-(ObjectB->has(Transform)->Scale.z /2.0f);
		//v = ObjectB->has(RigidBody)->getTransform().transform(k);
	
		//HelperFunc(v,w,closestB,distance);
		////====================================================================================
		//if(ObjectA->has(Transform)->Scale.y >ObjectA->has(Transform)->Scale.x || ObjectA->has(Transform)->Scale.y>ObjectA->has(Transform)->Scale.z ||
		//   ObjectB->has(Transform)->Scale.y >ObjectB->has(Transform)->Scale.x || ObjectB->has(Transform)->Scale.y>ObjectB->has(Transform)->Scale.z)
		//{
		//	real a=distance;
		//	ConnectObjectsVertical(a,closestA,closestB);
		//	return;
		//}

		//if (abs(closestA.z-ObjectA->has(Transform)->Position.z)<EPSILON)//then z is constant, create object on y
		//{
		//	bool created=false;
		//	if(!Bridge){
		//		Bridge= FACTORY->BuildFromArchetype("GOC_Type_Bridge");
		//		created=true;
		//	}
		//	Vec3 newPos= Vec3((closestA.x + closestB.x)/2.0f,(closestA.y+closestB.y)/2.0,(closestA.z+closestB.z)/2.0f);
		//	newPos.y-=0.1;
		//	real scale = (ObjectA->has(Transform)->Scale.z<ObjectB->has(Transform)->Scale.z)?ObjectA->has(Transform)->Scale.z:ObjectB->has(Transform)->Scale.z;
		//	scale-=0.1;
		//	Bridge->has(Transform)->Scale=Vec3(sqrt(distance)+0.1,0.1,scale);
		//	Bridge->has(Transform)->Position=newPos;
		//	
		//	if((closestA.y < closestB.y))
		//	{
		//		Vector3 temp=closestA;
		//		closestA=closestB;
		//		closestB=temp;
		//	}
		//	
		//	Vector3 angle = closestA - closestB;
		//	angle.normalise();
		//	Quaternion q; 
		//	if((closestA.x < closestB.x))
		//		angle*=-1;
		//	q.r=cos(angle.y/2.0);
		//	q.k=sin(angle.y/2.0);			
		//	Bridge->has(Transform)->QUATERNION = D3DXQUATERNION(q.i,q.j,q.k,q.r);

		//	static_cast<CollisionBox*>(Bridge->has(CollisionPrimitive))->halfSize = Vector3(sqrt(distance)/2.0,0.05,scale/2.0);

		//	if (created)
		//		Bridge->Initialize();
		//}
		//else if (abs(closestA.x-ObjectA->has(Transform)->Position.x)<EPSILON)//then x is constant, create object on y
		//{
		//	bool created=false;
		//	if(!Bridge){
		//		Bridge= FACTORY->BuildFromArchetype("GOC_Type_Bridge");
		//		created=true;
		//	}
		//	Vec3 newPos= Vec3((closestA.x + closestB.x)/2.0f,(closestA.y+closestB.y)/2.0,(closestA.z+closestB.z)/2.0f);
		//	newPos.y-=0.1;
		//	float scale = (ObjectA->has(Transform)->Scale.x<ObjectB->has(Transform)->Scale.x)?ObjectA->has(Transform)->Scale.x:ObjectB->has(Transform)->Scale.x;
		//	scale-=0.1;
		//	Bridge->has(Transform)->Scale=Vec3(scale,0.1f,sqrt(distance)+0.1f);
		//	Bridge->has(Transform)->Position=newPos;
		//	
		//	if((closestA.y < closestB.y))
		//	{
		//		Vector3 temp=closestA;
		//		closestA=closestB;
		//		closestB=temp;
		//	}
		//	
		//	Vector3 angle = closestA - closestB;
		//	angle.normalise();
		//	Quaternion q; 
		//	if((closestA.z > closestB.z))
		//		angle*=-1;
		//	q.r=cos(angle.y/2.0);
		//	q.i=sin(angle.y/2.0);			
		//	Bridge->has(Transform)->QUATERNION = q.QuaternionToD3DXQUATERNION();

		//	static_cast<CollisionBox*>(Bridge->has(CollisionPrimitive))->halfSize = Vector3(scale/2.0,0.05,sqrt(distance)/2.0);
		//	if (created)
		//		Bridge->Initialize();
		//}		
	}
	
	

	void Connector::Update()
	{
		/*if(!ObjectA||!ObjectB)
		{
			if(Bridge)
			{
				Bridge->Destroy();
				Bridge=NULL;
			}
		}else if(ObjectA && ObjectB)
		{
			if(!CheckIfCanConnect())
			{
				if(Bridge)
				{
					Bridge->Destroy();
					Bridge=NULL;
				}
			}else
			{
				ConnectObjects();
			}
		}*/
	}
}