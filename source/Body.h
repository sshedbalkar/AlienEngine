///////////////////////////////////////////////////////////////////////////////////////
///
///	\file Body.h  Define Body GameComponent
///	
///	Authors: 
///	Copyright 2010, Digipen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////////////
#pragma once //Makes sure this header is only included once
#include "Composition.h"
#include "VMath.h"
#include "Collision.h"

namespace Framework
{
	///Body Component provides basic point physics dynamics including mass, 
	///velocity, forces, acceleration, and collision resolution.
	///Component will modify transform component attributes every frame.
	class Body : public GameComponent
	{
	public:
		Body();
		~Body();

		void AddForce(Vec2 force);
		void Integrate(float dt);
		void SetPosition(Vec2);
		void SetVelocity(Vec2);
		void PublishResults();

		virtual void Initialize();
		virtual void Serialize(ISerializer& stream);
		virtual void RemoveComponentFromGame();

		Body * Next;
		Body * Prev;

		Vec2 Position;
		Vec2 InitialPosition;
		Vec2 PrevPosition;
		Vec2 Velocity;
		Vec2 Acceleration;
		Vec2 BodyScale;
		float Mass;
		float InvMass;
		float Restitution;
		float Friction;
		float Damping;
		Vec2 AccumulatedForce;
		bool isCollidingUp;
		bool isCollidingLeft;
		bool isCollidingDown;
		bool isCollidingRight;

		bool isPreviousCollidingUp;
		bool isPreviousCollidingLeft;
		bool isPreviousCollidingDown;
		bool isPreviousCollidingRight;


		int movingDirection;
		virtual void SendMessage(Message *m);
		//Transform for this body
		Transform * tx;
		//Shape used for collision with this body
		Shape * BodyShape;
		//Static object are immovable fixed objects
		bool IsStatic;
		bool isCollidable;
	};
}