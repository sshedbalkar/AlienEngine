///////////////////////////////////////////////////////////////////////////////////////
//
//	Body.cpp
//	
//	Authors: 
//	Copyright 2010, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////
#include "Precompiled.h"
#include "Body.h"
#include "DebugDraw.h"
#include "Physics.h"
#include "DebugTools.h"
#include "Utility.h"
#include "GameLogic.h"


namespace Framework
{

	Body::Body()
	{
		Position = Vec2(0,0);
		PrevPosition = Vec2(0,0);
		Velocity = Vec2(0,0);
		Mass = 0.0f;
		InvMass = 0.0f;
		Damping = 0.9f;
		Acceleration = Vec2(0,0);
		BodyShape = NULL;
		Friction = 0.0f;
		Restitution = 0.0f;
		movingDirection=0;
		IsStatic = false;
		AccumulatedForce = Vec2(0,0);
		isCollidable=true;
		isCollidingUp=false;
		isCollidingDown=false;
		isCollidingLeft=false;
		isCollidingRight=false;
		isPreviousCollidingUp=false;
		isPreviousCollidingLeft=false;
		isPreviousCollidingDown=false;
		isPreviousCollidingRight=false;
	}

	Body::~Body()
	{
		delete BodyShape;
		PHYSICS->Bodies.erase(this);
	}

	void Body::Integrate(float dt)
	{
		isPreviousCollidingUp=isCollidingUp;
		isPreviousCollidingDown=isCollidingDown;
		isPreviousCollidingLeft=isCollidingLeft;
		isPreviousCollidingRight=isCollidingRight;
		isCollidingUp=false;
		isCollidingDown=false;
		isCollidingLeft=false;
		isCollidingRight=false;



		//Do not integrate static bodies
		if(IsStatic) return;

		//Store prev position
		PrevPosition = Position;

		//Integrate the position using Euler 
		Position = Position + Velocity * dt; //acceleration term is small

		//Determine the acceleration
		Acceleration = PHYSICS->Gravity;
		Vec2 newAcceleration = AccumulatedForce * InvMass + Acceleration;

		//Integrate the velocity
		Velocity = Velocity + newAcceleration * dt;

		//Dampen the velocity for numerical stability and soft drag
		Velocity *= std::pow(Damping, dt);

		//Clamp to velocity max for numerical stability
		if ( dot(Velocity, Velocity) > PHYSICS->MaxVelocitySq )
		{
			normalize(Velocity);
			Velocity = Velocity * PHYSICS->MaxVelocity;
		}

		//Clear the force
		AccumulatedForce = Vec2(0,0);
	}

	void Body::PublishResults()
	{
		tx->Position = Vec3(Position.x,Position.y,tx->Position.z );
	}

	

	void Body::Initialize()
	{ 
		//Get the transform to write results to
		tx = GetOwner()->has(Transform);

		//Get the starting position
//		Position = tx->Position;

		//Add this body to the body list
		PHYSICS->Bodies.push_back(this);

		//If mass is zero object is interpreted
		//to be static
		if( Mass > 0.0f )
		{			
			IsStatic = false;
			InvMass = 1.0f / Mass;
		}
		else
		{
			IsStatic = true;
			InvMass = 0.0f;
		}
		InitialPosition=Position;
		BodyShape->body = this;
	}

	void Body::RemoveComponentFromGame()
	{
	}

	void Body::Serialize(ISerializer& stream)
	{
		ISerializer* str = stream.clone();
		if(str->isGood())
		{
			str->readProperty("Mass", *str);
			str->readValue("value", Mass);
			//
			str->goToParent();
			str->readProperty("Restitution", *str);
			str->readValue("value", Restitution);
			//
			str->goToParent();
			str->readProperty("Friction", *str);
			str->readValue("value", Friction);
			//
			std::string s;
			str->goToParent();
			str->readProperty("Shape", *str);
			str->readValue("value", s);
			//
			float arr[2];
			if(!s.compare("Circle"))
			{
				ShapeCircle* shape = new ShapeCircle();
				str->goToParent();
				str->readProperty("DimensionHalf", *str);
				//Vec2 v;
				//str->readValue("value", v);
				//shape->Radius = v.x;
				str->readValue("value", arr, 2);
				shape->Radius = arr[0];
				this->BodyShape = shape;
				//std::cout<<"Shape: Circle, Radius: "<<v.x<<"\n";
			}
			else if(!s.compare("Box"))
			{
				ShapeAAB* shape = new ShapeAAB();
				str->goToParent();
				str->readProperty("DimensionHalf", *str);
				//str->readValue("value", shape->Extents);
				str->readValue("value", arr, 2);
				shape->Extents.x = arr[0];
				shape->Extents.y = arr[1];
				this->BodyShape = shape;
				//std::cout<<"Shape: Box, Extents: "<<shape->Extents<<"\n";
			}
		}
		//std::cout<<"Body: Mass:"<<Mass<<", Restitution: "<<Restitution<<", Friction: " <<Friction<<"\n";
		delete str;
	}

	void Body::AddForce(Vec2 force)
	{
		AccumulatedForce += force;
	}

	void Body::SetPosition(Vec2 p)
	{
		Position = p;
//		tx->Position = p;
	}

	void Body::SetVelocity(Vec2 v)
	{
		Velocity = v;
	}
	void Body::SendMessage(Message *m)
	{
		switch( m->MessageId )
		{
			case Mid::Collide:
			{
				
				break;
			}
		}
	}




}
