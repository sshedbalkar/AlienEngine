///////////////////////////////////////////////////////////////////////////////////////
///
///	\file Physics.h
///	Basic 2D iterative impulse physics engine.
///	
///	Authors: 
///	Copyright 2010, Digipen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////////////
#pragma once //Makes sure this header is only included once

#include "VMath.h"
#include "Engine.h"
#include "Collisions.h"
#include "ComplexObject.h"
#include "RigidBody.h"
#include "EventTrigger.h"
#include <iostream>
#include "Joints.h"

namespace Framework
{

	///Message sent when there is a Collsion between two
	///Body Components.

	class AABB;

	struct BodiesPairCollided
	{
		RigidBody * a;
		RigidBody * b;
	};

	class MessageCollide : public Message
	{
	public:
		MessageCollide() : Message(Mid::Collide) {};
		MessageCollide(const MessageCollide& rhs)
			:Message(rhs),
			ContactNormal(rhs.ContactNormal),
			CollidedWith(rhs.CollidedWith),
			CollideObject(rhs.CollideObject)
		{
			//
		}
		MessageCollide* clone()
		{
			return new MessageCollide(*this);
		}
		Vector3 ContactNormal;
		GOC * CollidedWith;
		GOC * CollideObject;
	};

	class EventCollide : public Message
	{
	public:
		EventCollide() : Message(Mid::EventTrigger) {};
		EventCollide(const EventCollide& rhs)
			:Message(rhs),
			CollidedWith(rhs.CollidedWith),
			CollideObject(rhs.CollideObject)
		{
			//
		}
		EventCollide* clone()
		{
			return new EventCollide(*this);
		}
		GOC * CollidedWith;
		GOC * CollideObject;
	};

	class Physics: public ISystem
	{
	public:
		Physics();
		virtual void Update(float dt);
		virtual std::string GetName(){return "Physics";}
		void SendMessage(Message * m );
		void Initialize();
		void Pause(bool t){_paused=t;}
		void makeReversePhysics();
		void makePhysicsNormal();
		void SetDebugDraw(bool b){DebugDraw =b;}
		bool GetIsDebugMode(){return DebugDraw;}
	private:
		void DetectCollision(RigidBody *a,RigidBody *b);
		void DetectCollision(CollisionPrimitive *a,CollisionPrimitive *b);
		void DoPlaneCollision(RigidBody *a);
		void IntegrateBodies(float dt);
		void GenerateContacts(float dt);
		void PublishResults();
		void ResetCollisionData();
		void CheckEventCollision();
		void Step(float dt);
		GOC* GetGOCFromIntersection(Vector3 & position,Vector3 & direction,real & distance,Vector3 &collisionPoint);
		bool _paused;
		bool game_Paused;
		IntersectionTests collisiontest;
		void DrawDebug();
		bool DebugDraw;
		void DebugDrawHelper(Vector3& a,Vector3& b,Vector3& c,Vector3& s, Vec4& color,Matrix4 & transform);
		void DebugDrawHelper(Vector3& a,Vector3& b,Vector3& s, Vec4& color,Matrix4 & transform);

		//reverse physics code
		void RunReversePhysics(float dt);
		void StopMovingObjects();
		void LetMovableObjectsMove();
		void IntegrateBodiesReverse(float dt);
		void GenerateContactsReverse(float dt);

		//broad phase
		//int cmpAABBs(AABB*,AABB*);
		void SweepPruneAABBArray();
		
		//std::vector<AABB*> m_SortedAABBArrayZ;
		//std::list<BodiesPairCollided> m_collidedPairs;
		bool TestAABBOverlap(AABB*,AABB*, int sortAxis);
		 
	public:
		std::vector<AABB*> SortedAABBArrayX;
		CollisionPrimitive *ObjectLookingAt;
		//typedef ObjectLinkList<Body>::iterator BodyIterator;
		//ObjectLinkList<Body> Bodies;
		typedef ObjectLinkList<RigidBody>::iterator RigidBodyIterator;
		ObjectLinkList<RigidBody> RigidBodies;
		ObjectLinkList<RigidBody> LevelData;

		typedef ObjectLinkList<EventTrigger>::iterator EventsIterator;
		ObjectLinkList<EventTrigger> Events;

		const static unsigned maxContacts = 1024;
		Contact contacts[maxContacts];

		CollisionData cData;
		ContactResolver resolver;
		IntersectionTests intersectionTest;
		CollisionDetector detector;

		std::list<Joint> joints;
		std::list<Constraint> constraints;

		bool checkCollisionBetweenBoxes(GOC *a,GOC *b);

		//old code
		GOC * TestPoint(Vec2 testPosition);
		//Gravity of the world
		Vec2 Gravity;
		//Max velocity for a physics body
		float MaxVelocity;
		float MaxVelocitySq;
		//See Resolution.cpp for use
		//Position correction tolerance
		float PenetrationEpsilon;
		//Position correction resolve percentage
		float PenetrationResolvePercentage;
		void ChangeCollisionProperties();
		

	};

	//A global pointer to the Physics system, used to access it globally.
	extern Physics* PHYSICS;
}


//TODO
//blocks stack destruction
//add ragdoll
//put level data into another list for easy access and put them as body1 during contact generation
//see calculate desired velocity function if objects bouncing  page no.394