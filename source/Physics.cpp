#include "Precompiled.h"
#include "Physics.h"
#include "ComponentCreator.h"
#include "Core.h"
#include "GameLogic.h"
#include "Graphics.h"
#include "LevelEditor.h"
#include "Message.h"
#include "TimeMechanicManager.h"
#include "ModelComponent.h"
//
namespace Framework
{
	Physics * PHYSICS = NULL;

	Physics::Physics():
		_paused(true),resolver(maxContacts*8)
	{
		ErrorIf(PHYSICS!=NULL,"Physics already initialized");
		PHYSICS = this;
		game_Paused = false;
		cData.contactArray = contacts;
		DebugDraw=false;
	}

	void Physics::Initialize()
	{
		RegisterComponent(RigidBody);
		RegisterComponent(EventTrigger);
//		RegisterComponent(Body);
		RegisterComponent(CollisionPrimitive);		
		RegisterComponent(ComplexObject);		
		RegisterComponent(CollisionSphere);		
		RegisterComponent(CollisionBox);		
		RegisterComponent(CollisionCylinder);		
		RegisterComponent(CollisionCone);		
		RegisterComponent(CollisionEllipsoid);		
	}

	void Physics::IntegrateBodies(float dt)
	{
		for(RigidBodyIterator it=RigidBodies.begin();it!=RigidBodies.end();++it)
		{	
			it->prev_position = it->getPosition();
			it->integrate(dt);
			it->isColliding=false;
			if(it->GetOwner()->has(ComplexObject))
			{
				it->GetOwner()->has(ComplexObject)->calculateInternals();
				//it->GetOwner()->has(ComplexObject)->DrawObject();
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

	GOC* Physics::GetGOCFromIntersection(Vector3 & position,Vector3 & direction,real & distance,Vector3 &collisionPoint)
	{
		ObjectLookingAt = NULL;
		GOC * Object=NULL;
		Vector3 tempposition;
		Ray ray(position,direction);
		CollisionDetector detect;
		distance=REAL_MAX;
		for(RigidBodyIterator it=RigidBodies.begin();it!=RigidBodies.end();++it)
		{
			if(it->GetOwner()->has(ComplexObject))
			{
				ComplexObject *object = it->GetOwner()->has(ComplexObject);
				ObjectLinkList<CollisionPrimitive>::iterator it1;
				it1= object->collisionPrimitives.begin();
				int index=0;
				for(;it1!=object->collisionPrimitives.end();++it1,++index)
				{
					if(!it->GetOwner()->has(ComplexObject)->getAlive(index)) continue;
					if(it1->rayCast(ray,tempposition))
					{
						real a=squaredDistance(position,tempposition);
						if(a<distance)
						{
							collisionPoint=tempposition;
							distance=a;
							Object=object->GetOwner();
							ObjectLookingAt=it1;
						}
					}
				}
			}else if(it->GetOwner()->CompositionTypeId != GOC_Type_Hero && it->canCollide)
			{
				if(it->GetOwner()->has(CollisionPrimitive)->rayCast(ray,tempposition))
				{
					real a=squaredDistance(position,tempposition);
					if(a<distance)
					{
						collisionPoint=tempposition;
						distance=a;
						Object=it->GetOwner();
					}
				}
			}
		}

		for(RigidBodyIterator it1=LevelData.begin();it1!=LevelData.end();++it1)
		{
			if(it1->GetOwner()->has(CollisionPrimitive)->rayCast(ray,tempposition))
			{
				real a=squaredDistance(position,tempposition);
				if(a<distance)
				{
					collisionPoint=tempposition;
					distance=a;
					Object=it1->GetOwner();
				}
			}
		}
		return Object;
	}

	void Physics::DetectCollision(CollisionPrimitive *a,CollisionPrimitive *b)
	{
		if((a->Id == CollisionPrimitive::ShapeId::Box || a->Id == CollisionPrimitive::ShapeId::Sphere) && (b->Id == CollisionPrimitive::ShapeId::Box || b->Id == CollisionPrimitive::ShapeId::Sphere))
		{
			if(a->Id==CollisionPrimitive::ShapeId::Box)
			{
				if(b->Id==CollisionPrimitive::ShapeId::Box)
					detector.boxAndBox(*(static_cast<CollisionBox*>(a)),*(static_cast<CollisionBox*>(b)),&cData);
				else
					detector.boxAndSphere(*(static_cast<CollisionBox*>(a)),*(static_cast<CollisionSphere*>(b)),&cData);
			}else
			{
				if(b->Id==CollisionPrimitive::ShapeId::Box)
					detector.boxAndSphere(*(static_cast<CollisionBox*>(b)),*(static_cast<CollisionSphere*>(a)),&cData);
				else
					detector.sphereAndSphere(*(static_cast<CollisionSphere*>(a)),*(static_cast<CollisionSphere*>(b)),&cData);
			}
		}else
		{
			detector.MPRTest(a,b,&cData);
		}
	}

	void Physics::DetectCollision(RigidBody *a,RigidBody *b)
	{
		int type=0;
		GOC *A,*B;
		A=a->GetOwner();
		B=b->GetOwner();
		if(a==NULL|| b==NULL || a==b) return;
		if(A->has(CollisionPrimitive) &&  B->has(CollisionPrimitive)) type=1;
		else if(A->has(ComplexObject) && B->has(ComplexObject)) type=2;
		else if(A->has(ComplexObject) && B->has(CollisionPrimitive)) type=3;
		else if(B->has(ComplexObject) && A->has(CollisionPrimitive))
		{
			GOC *temp=A;
			A=B;
			B=temp;
			type=3;
		}
		
		switch(type)
		{
			case 1:
			{
				DetectCollision(A->has(CollisionPrimitive),B->has(CollisionPrimitive));
				break;
			}
			case 2:
			{
				//fix this
				ObjectLinkList<CollisionPrimitive>::iterator it=A->has(ComplexObject)->collisionPrimitives.begin();
				ObjectLinkList<CollisionPrimitive>::iterator it1=B->has(ComplexObject)->collisionPrimitives.begin();
				for(;it!=A->has(ComplexObject)->collisionPrimitives.end();++it)
					for(;it1!=B->has(ComplexObject)->collisionPrimitives.end();++it1)
						DetectCollision(it,it1);
				break;
			}
			case 3:
			{
				ObjectLinkList<CollisionPrimitive>::iterator it=A->has(ComplexObject)->collisionPrimitives.begin();
				for(int index=0;it!=A->has(ComplexObject)->collisionPrimitives.end();++it,++index)
				{
					if(A->has(ComplexObject)->getAlive(index))
					DetectCollision(it,B->has(CollisionPrimitive));
				}
				break;
			}
			default:
				prints("Physics Collision ERROR");
				break;	
		}
	}

	void Physics::DoPlaneCollision(RigidBody *a)
	{
		CollisionPlane plane;
		plane.direction=Vector3(0,1,0);
		plane.offset=0;

		if(a->GetOwner()->has(ComplexObject))
		{
			ObjectLinkList<CollisionPrimitive>::iterator it=a->GetOwner()->has(ComplexObject)->collisionPrimitives.begin();
			int index=0;
			for(;it!=a->GetOwner()->has(ComplexObject)->collisionPrimitives.end();++it,++index)
			{
				if(a->GetOwner()->has(ComplexObject)->getAlive(index))
					detector.boxAndHalfSpace(*(static_cast<CollisionBox*>(&(*it))),plane,&cData);
			}
		}else
		{
			if(a->GetOwner()->has(CollisionPrimitive)->Id==CollisionPrimitive::ShapeId::Box)
				detector.boxAndHalfSpace(*(static_cast<CollisionBox*>(a->GetOwner()->has(CollisionPrimitive))),plane,&cData);
			else if(a->GetOwner()->has(CollisionPrimitive)->Id==CollisionPrimitive::ShapeId::Sphere)
				detector.sphereAndHalfSpace(*(static_cast<CollisionSphere*>(a->GetOwner()->has(CollisionPrimitive))),plane,&cData);
		}
	}


	void Physics::GenerateContacts(float dt)
	{
		Vector3 position(GRAPHICS->screen_width/2.0,GRAPHICS->screen_height/2.0,0),direction;
		GRAPHICS->ScreenToWorldSpaceRay(position,direction);
		real dist;
		HERO->heroPos=position;
		HERO->heroDir=direction;
		LOGIC->objectPointedAt=GetGOCFromIntersection(position,direction,dist,LOGIC->objectPointedpoint);
		LOGIC->objectPointedAtDistance=dist;
		
		//rigid bodies against other rigid bodies and the plane at the origin
		//SweepPruneAABBArray();
		for(RigidBodyIterator it=RigidBodies.begin();it!=RigidBodies.end();++it)
		{
			DoPlaneCollision(it);
			RigidBodyIterator it1 = it;
			++it1;
			for(;it1!=RigidBodies.end();++it1)
			{

				if(!it->canCollide || !it1->canCollide)//if object can't collide (dying etc)
					continue;

				//HERO should not collide with picked object
				if(LOGIC->m_PowerGun.gravityGun.GetPickedItem() && it1->GetOwner()->CompositionTypeId==GOC_Type_Hero && it->GetOwner()==LOGIC->m_PowerGun.gravityGun.GetPickedItem()) 
					continue;

				if(it->getAwake() || it1->getAwake())
				{
					real t = it->getBoundingVolumeRadius()  + it1->getBoundingVolumeRadius();
					if(t*t < (it1->getPosition()-it->getPosition()).squareMagnitude() )
						continue;
					DetectCollision(it,it1);
				}
			}
		}


		//gjk test
		/*for(RigidBodyIterator it=RigidBodies.begin();it!=RigidBodies.end();++it)
		{
			if(it->GetOwner()->CompositionTypeId!=GOC_Type_CubeTest) continue;//gjk test code
			detector.MPRTest(it->GetOwner()->has(CollisionPrimitive),HERO->GetHero()->has(CollisionPrimitive),&cData);
			RigidBodyIterator it1 = it;
			++it1;
			for(;it1!=RigidBodies.end();++it1)
			{
				if(it1->GetOwner()->CompositionTypeId!=GOC_Type_CubeTest) continue;//gjk test code
				if(detector.MPRTest(it->GetOwner()->has(CollisionPrimitive),it1->GetOwner()->has(CollisionPrimitive),&cData)==1)
				{
					if(it->GetOwner()->has(ModelComponent)->TextureName =="red")
						it->GetOwner()->has(ModelComponent)->SwapTexture();
					if(it1->GetOwner()->has(ModelComponent)->TextureName =="red")
						it1->GetOwner()->has(ModelComponent)->SwapTexture();
				}else
				{
					if(it->GetOwner()->has(ModelComponent)->TextureName =="dark_blue_trans")
						it->GetOwner()->has(ModelComponent)->SwapTexture();
					if(it1->GetOwner()->has(ModelComponent)->TextureName =="dark_blue_trans")
						it1->GetOwner()->has(ModelComponent)->SwapTexture();
				}
			}
		}*/

		//rigid bdies against the level data i.e mass=0
		for(RigidBodyIterator it=RigidBodies.begin();it!=RigidBodies.end();++it)
		{
			if(it->getAwake())
			{
				for(RigidBodyIterator it1 = LevelData.begin();it1!=LevelData.end();++it1)
				{
					DetectCollision(it,it1);
				}
			}
		}

		std::list<Joint>::iterator joint= joints.begin();
		for(;joint!=joints.end();++joint)
		{
			if (!cData.hasMoreContacts()) return;
			unsigned added = joint->addContact(cData.contacts);
			cData.addContacts(added);
		}

		std::list<Constraint>::iterator constraint= constraints.begin();
		for(;constraint!=constraints.end();++constraint)
		{
			if (!cData.hasMoreContacts()) return;
			unsigned added = constraint->addContact(cData.contacts);
			cData.addContacts(added);
		}

	}


	void Physics::PublishResults()
	{
		//Commit all physics updates
		for(RigidBodyIterator it=RigidBodies.begin();it!=RigidBodies.end();++it)
		{
			it->setAcceleration(it->initialAcceleration);
			(it)->PublishResults();
		}
		for(RigidBodyIterator it=LevelData.begin();it!=LevelData.end();++it)
		{
			it->setAcceleration(it->initialAcceleration);
			(it)->PublishResults();
		}
		//Broadcast physics collision messages AFTER physics
		//has update the bodies
		Contact* lastContact = cData.contactArray+cData.contactCount;
		for (Contact* contact= cData.contactArray; contact < lastContact; contact++)
		{
			MessageCollide messageCollide;
			messageCollide.ContactNormal = contact->contactNormal;
			if(contact->body[1] != NULL)
				messageCollide.CollidedWith = contact->body[1]->GetOwner();
			else
				messageCollide.CollidedWith = NULL;
			messageCollide.CollideObject = contact->body[0]->GetOwner();
			if(!contact->body[0]->canRotate && (messageCollide.ContactNormal*Vector3(0,1,0))>=(0.5*messageCollide.ContactNormal.magnitude()))
			{
				contact->body[0]->setAcceleration(0,0,0);
				contact->body[0]->setVelocity(0,0,0);
			}
			contact->body[0]->GetOwner()->SendMessage( &messageCollide );		
			if( contact->body[1] != NULL )
			{
				messageCollide.ContactNormal = contact->contactNormal*-1;
				messageCollide.CollidedWith = contact->body[0]->GetOwner();
				messageCollide.CollideObject = contact->body[1]->GetOwner();
				if(!contact->body[1]->canRotate && (messageCollide.ContactNormal*Vector3(0,1,0))>=(0.5*messageCollide.ContactNormal.magnitude()))
				{
					contact->body[1]->setAcceleration(0,0,0);
					contact->body[1]->setVelocity(0,0,0);
				}
				contact->body[1]->GetOwner()->SendMessage( &messageCollide );				
			}
		}
	}

	void Physics::ResetCollisionData()
	{
		cData.reset(maxContacts);
		cData.friction=(real)0.9;
		cData.restitution=(real)0.1;
		cData.tolerance=(real)0.1;
	}

	void Physics::CheckEventCollision()
	{
		EventsIterator it= Events.begin();
		for(;it!=Events.end();++it)
		{
			it->GetOwner()->has(CollisionPrimitive)->calculateInternalsinEditor();
			if( intersectionTest.sphereAndBox(  *(static_cast<CollisionSphere*>(HERO->GetHero()->has(CollisionPrimitive))) ,*(static_cast<CollisionBox*>(it->GetOwner()->has(CollisionPrimitive)))))
			{
				EventCollide ec;
				ec.CollideObject=it->GetOwner();
				ec.CollidedWith= HERO->GetHero();
				it->SendMessageA(&ec);
				return;
			}
		}
	}

	void Physics::Step(float dt)
	{
		
		IntegrateBodies(dt);

		ResetCollisionData();

		GenerateContacts(dt);

		resolver.setIterations(cData.contactCount*3);
		resolver.resolveContacts(cData.contactArray,cData.contactCount,dt);

		PublishResults();

		CheckEventCollision();
	}

	void Physics::Update(float dt)
	{
		print("rigidbodies",RigidBodies.size());
		if(_paused) return;
		if(DebugDraw)
			DrawDebug();
		if(EDITOR && EDITOR->inEditor)return;
		if(game_Paused)
		{
			RunReversePhysics(dt);
		}
		else
		{
			Step(dt);
		}
	}


	void Physics::SendMessage(Message *msg)
	{
		if (msg->MessageId == Mid::SystemPause)
		{
			const SystemPause *m = static_cast<const SystemPause*>(msg);
			_paused = m->Pause;
		}
	}

	bool Physics::checkCollisionBetweenBoxes(GOC *a,GOC *b)
	{
		if(a->has(CollisionPrimitive)->Id!=CollisionPrimitive::ShapeId::Box  || b->has(CollisionPrimitive)->Id!=CollisionPrimitive::ShapeId::Box)
			return false;

		return collisiontest.boxAndBox(  *(static_cast<CollisionBox*>(a->has(CollisionPrimitive))) , *(static_cast<CollisionBox*>(b->has(CollisionPrimitive))) );
	}



	void Physics::DebugDrawHelper(Vector3& a,Vector3& b,Vector3& c,Vector3& s, Vec4& color, Matrix4 & transform)
	{
		s=transform.transform(s);
		a=transform.transform(a);
		b=transform.transform(b);
		c=transform.transform(c);
		GRAPHICS->AddTwoPointsToLineDrawer(s,a,color);
		GRAPHICS->AddTwoPointsToLineDrawer(s,b,color);
		GRAPHICS->AddTwoPointsToLineDrawer(s,c,color);
	}
	void Physics::DebugDrawHelper(Vector3& a,Vector3& b,Vector3& s, Vec4& color, Matrix4 & transform)
	{
		s=transform.transform(s);
		a=transform.transform(a);
		b=transform.transform(b);
		GRAPHICS->AddTwoPointsToLineDrawer(s,a,color);
		GRAPHICS->AddTwoPointsToLineDrawer(s,b,color);
	}


	void Physics::DrawDebug()
	{
		for(RigidBodyIterator it=RigidBodies.begin();it!=RigidBodies.end();++it)
		{
			Vec4 color(1,1,1,1);
			if(it->GetOwner()->has(ComplexObject))
			{
				//it->GetOwner()->has(ComplexObject)->DrawObject();
				continue;
			}
			if(it->GetOwner()->has(CollisionPrimitive)->Id==CollisionPrimitive::ShapeId::Box)
			{
				Vector3 halfsize= static_cast<CollisionBox*>(it->GetOwner()->has(CollisionPrimitive))->halfSize;
				Matrix4 transform = it->getTransform();
				Vector3 s,a,b,c;
				AABB* aabb=it->GetOwner()->has(CollisionPrimitive)->aabb;

				if (!aabb) continue;
				aabb->center = it->getPosition();//it->getTransform().transform();
				aabb->halfSize= it->getTransform().transformOOBBtoAABB(halfsize);
				
				
				
				s = a = b = c = aabb->halfSize;
				
				a-=Vector3(aabb->halfSize.x*2,0,0);
				b-=Vector3(0,aabb->halfSize.y*2,0);
				c-=Vector3(0,0,aabb->halfSize.z*2);
				s+=aabb->center;
				a+=aabb->center;
				b+=aabb->center;
				c+=aabb->center;
				color=Vec4(1,0,0,1);
				GRAPHICS->AddTwoPointsToLineDrawer(s,a,color);
				GRAPHICS->AddTwoPointsToLineDrawer(s,b,color);
				GRAPHICS->AddTwoPointsToLineDrawer(s,c,color);

				s = a = b = c = (aabb->halfSize*(-1));
				a+=Vector3(aabb->halfSize.x*2,0,0);
				b+=Vector3(0,aabb->halfSize.y*2,0);
				c+=Vector3(0,0,aabb->halfSize.z*2);
				s+=aabb->center;
				a+=aabb->center;
				b+=aabb->center;
				c+=aabb->center;
				GRAPHICS->AddTwoPointsToLineDrawer(s,a,color);
				GRAPHICS->AddTwoPointsToLineDrawer(s,b,color);
				GRAPHICS->AddTwoPointsToLineDrawer(s,c,color);

				s = a = b = Vector3(-aabb->halfSize.x,aabb->halfSize.y,-aabb->halfSize.z);
				a+=Vector3(0,0,aabb->halfSize.z*2);
				b+=Vector3(aabb->halfSize.x*2,0,0);
				s+=aabb->center;
				a+=aabb->center;
				b+=aabb->center;
				GRAPHICS->AddTwoPointsToLineDrawer(s,a,color);
				GRAPHICS->AddTwoPointsToLineDrawer(s,b,color);

				s = a = b = Vector3(aabb->halfSize.x,-aabb->halfSize.y,-aabb->halfSize.z);
				a+=Vector3(0,0,aabb->halfSize.z*2);
				b+=Vector3(0,aabb->halfSize.y*2,0);
				s+=aabb->center;
				a+=aabb->center;
				b+=aabb->center;
				GRAPHICS->AddTwoPointsToLineDrawer(s,a,color);
				GRAPHICS->AddTwoPointsToLineDrawer(s,b,color);

				s = a = b = Vector3(-aabb->halfSize.x,-aabb->halfSize.y,aabb->halfSize.z);
				a+=Vector3(aabb->halfSize.x*2,0,0);
				b+=Vector3(0,aabb->halfSize.y*2,0);
				s+=aabb->center;
				a+=aabb->center;
				b+=aabb->center;
				GRAPHICS->AddTwoPointsToLineDrawer(s,a,color);
				GRAPHICS->AddTwoPointsToLineDrawer(s,b,color);


				//Vector3 s,a,b,c;
			
				color=Vec4(1,1,1,1);
				s = a = b = c = halfsize;
				a-=Vector3(halfsize.x*2,0,0);
				b-=Vector3(0,halfsize.y*2,0);
				c-=Vector3(0,0,halfsize.z*2);
				DebugDrawHelper(a,b,c,s,color,transform);

				s = a = b = c = (halfsize*(-1));
				a+=Vector3(halfsize.x*2,0,0);
				b+=Vector3(0,halfsize.y*2,0);
				c+=Vector3(0,0,halfsize.z*2);
				DebugDrawHelper(a,b,c,s,color,transform);

				s = a = b = Vector3(-halfsize.x,halfsize.y,-halfsize.z);
				a+=Vector3(0,0,halfsize.z*2);
				b+=Vector3(halfsize.x*2,0,0);
				DebugDrawHelper(a,b,s,color,transform);

				s = a = b = Vector3(halfsize.x,-halfsize.y,-halfsize.z);
				a+=Vector3(0,0,halfsize.z*2);
				b+=Vector3(0,halfsize.y*2,0);
				DebugDrawHelper(a,b,s,color,transform);

				s = a = b = Vector3(-halfsize.x,-halfsize.y,halfsize.z);
				a+=Vector3(halfsize.x*2,0,0);
				b+=Vector3(0,halfsize.y*2,0);
				DebugDrawHelper(a,b,s,color,transform);


			}
			if(it->GetOwner()->has(CollisionPrimitive)->Id==CollisionPrimitive::ShapeId::Sphere)
			{
				if(it->GetOwner()->CompositionTypeId==GOC_Type_Hero)
					continue;
				Vector3 s,a,b,c;
				Matrix4 transform = it->getTransform();
				real radius= static_cast<CollisionSphere*>(it->GetOwner()->has(CollisionPrimitive))->radius;
				real angleT=R_PI/16,theta=0;
				real angleF=R_PI/8,fi=0;
				//a=Vector3(0,radius + it->getPosition().y,0);
				//a=transform.transform(a);
				Vec4 color(1,1,1,1);
				Vector3 center=it->getPosition();
				for(int i=0;i<16;++i)
				{
					theta=0;
					for(int j=0;j<32;++j)
					{

						b=Vector3(radius*cos(theta)*sin(fi),radius*sin(theta)*sin(fi),radius*cos(fi));
						c=Vector3(radius*cos(theta+angleT)*sin(fi),radius*sin(theta+angleT)*sin(fi),radius*cos(fi));
						GRAPHICS->AddTwoPointsToLineDrawer(transform.transform(b),transform.transform(c),color);
						theta+=angleT;
					}

					fi+=angleF;


					
				}

				for(int j=0;j<32;++j)
				{
					fi=0;
					for(int i=0;i<16;++i)
					{

						b=Vector3(radius*cos(theta)*sin(fi),radius*sin(theta)*sin(fi),radius*cos(fi));
						c=Vector3(radius*cos(theta)*sin(fi+angleF),radius*sin(theta)*sin(fi+angleF),radius*cos(fi+angleF));
						GRAPHICS->AddTwoPointsToLineDrawer(transform.transform(b),transform.transform(c),color);
						fi+=angleF;
					}
					theta+=angleT;

				}
			}
		}
	

		for(RigidBodyIterator it=LevelData.begin();it!=LevelData.end();++it)
		{
			Vec4 color(1,1,1,1);
			
			if(it->GetOwner()->has(CollisionPrimitive)->Id==CollisionPrimitive::ShapeId::Box)
			{
				Vector3 s,a,b,c;
				Matrix4 transform = it->getTransform();
				Vector3 halfsize= static_cast<CollisionBox*>(it->GetOwner()->has(CollisionPrimitive))->halfSize;
				s = a = b = c = halfsize;
				a-=Vector3(halfsize.x*2,0,0);
				b-=Vector3(0,halfsize.y*2,0);
				c-=Vector3(0,0,halfsize.z*2);
				DebugDrawHelper(a,b,c,s,color,transform);

				s = a = b = c = (halfsize*(-1));
				a+=Vector3(halfsize.x*2,0,0);
				b+=Vector3(0,halfsize.y*2,0);
				c+=Vector3(0,0,halfsize.z*2);
				DebugDrawHelper(a,b,c,s,color,transform);

				s = a = b = Vector3(-halfsize.x,halfsize.y,-halfsize.z);
				a+=Vector3(0,0,halfsize.z*2);
				b+=Vector3(halfsize.x*2,0,0);
				DebugDrawHelper(a,b,s,color,transform);

				s = a = b = Vector3(halfsize.x,-halfsize.y,-halfsize.z);
				a+=Vector3(0,0,halfsize.z*2);
				b+=Vector3(0,halfsize.y*2,0);
				DebugDrawHelper(a,b,s,color,transform);

				s = a = b = Vector3(-halfsize.x,-halfsize.y,halfsize.z);
				a+=Vector3(halfsize.x*2,0,0);
				b+=Vector3(0,halfsize.y*2,0);
				DebugDrawHelper(a,b,s,color,transform);

			}

			
		}
	
	}


}



