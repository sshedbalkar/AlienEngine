#include "Precompiled.h"
#include "Physics.h"
#include "ComponentCreator.h"
#include "Core.h"
#include "GameLogic.h"
#include "Graphics.h"
#include "LevelEditor.h"
#include "Message.h"
#include "TimeMechanicManager.h"

namespace Framework
{
	void Physics::makeReversePhysics()
	{
		game_Paused=true;
		StopMovingObjects();
	}
	
	void Physics::makePhysicsNormal()
	{
		game_Paused=false;
		LetMovableObjectsMove();
	}	

	void Physics::RunReversePhysics(float dt)
	{
		
		IntegrateBodiesReverse(dt);

		ResetCollisionData();

		GenerateContactsReverse(dt);

		resolver.setIterations(cData.contactCount*3);
		resolver.resolveContacts(cData.contactArray,cData.contactCount,dt);

		PublishResults();

		CheckEventCollision();
	}

	void Physics::StopMovingObjects()
	{
		for(RigidBodyIterator it=RigidBodies.begin();it!=RigidBodies.end();++it)
		{	
			if(it->GetOwner()->CompositionTypeId!=GOC_Type_Hero && it->GetOwner()!= POWERGUN->gravityGun.GetPickedItem() )
				it->makeObjectStatic();
		}
	}

	void Physics::LetMovableObjectsMove()
	{
		for(RigidBodyIterator it=RigidBodies.begin();it!=RigidBodies.end();++it)
		{	
			if(it->GetOwner()->CompositionTypeId!=GOC_Type_Hero && it->GetOwner()!= POWERGUN->gravityGun.GetPickedItem() )
				it->ResetOriginalData();
		}
	}

	void Physics::IntegrateBodiesReverse(float dt)
	{
		for(RigidBodyIterator it=RigidBodies.begin();it!=RigidBodies.end();++it)
		{	
			if(it->hasFiniteMass())
				it->integrate(dt);
			else
				it->calculateDerivedData();
			it->isColliding=false;
			if(it->GetOwner()->has(ComplexObject))
			{
				it->GetOwner()->has(ComplexObject)->calculateInternals();
				continue;
			}
			it->GetOwner()->has(CollisionPrimitive)->calculateInternals();
		}
		for(RigidBodyIterator it= LevelData.begin();it!= LevelData.end();++it)
		{
			it->calculateDerivedData();
			it->isColliding=false;
			it->GetOwner()->has(CollisionPrimitive)->calculateInternals();
		}
	}

	void Physics::GenerateContactsReverse(float dt)
	{
		Vector3 position(GRAPHICS->screen_width/2.0,GRAPHICS->screen_height/2.0,0),direction;
		GRAPHICS->ScreenToWorldSpaceRay(position,direction);
		real dist;
		HERO->heroPos=position;
		HERO->heroDir=direction;
		LOGIC->objectPointedAt=GetGOCFromIntersection(position,direction,dist,LOGIC->objectPointedpoint);
		LOGIC->objectPointedAtDistance=dist;


		RigidBody * heroBody=HERO->GetHero()->has(RigidBody);
		DoPlaneCollision(heroBody);

		GOC *objectPicked=LOGIC->m_PowerGun.gravityGun.GetPickedItem();
		//rigid bodies against other rigid bodies and the plane at the origin
		for(RigidBodyIterator it=RigidBodies.begin();it!=RigidBodies.end();++it)
		{
			DetectCollision(heroBody,it);
			if(objectPicked)
				DetectCollision(objectPicked->has(RigidBody),it);
		}
		//rigid bdies against the level data i.e mass=0
		for(RigidBodyIterator it = LevelData.begin();it!=LevelData.end();++it)
		{
			if(objectPicked)
				DetectCollision(objectPicked->has(RigidBody),it);
			DetectCollision(heroBody,it);
		}
	}
}