#include "Precompiled.h"
#include "Laser.h"
#include "GameLogic.h"
#include "Graphics.h"
#include "Physics.h"
#include "Dying.h"
#include "ParticleComponent.h"
#include "ActivationProperties.h"

namespace Framework
{

	Laser::Laser()
	{
		m_CollidedWith=0;
		m_dir=Vector3(1,0,0);
		
	}

	Laser::~Laser()
	{
	}


	void Laser::Initialize()
	{ 
		m_Transform = this->GetOwner()->has(Transform);
		Quaternion q= m_Transform->QUATERNION;
		m_dir= q*m_dir;

		laserLight= FACTORY->BuildFromArchetype("GOC_Type_LaserLight");
		laserLight->Initialize();
	}

	void Laser::intersect()
	{
		GOC * Object=NULL;
		Vector3 tempposition;
		Ray ray(m_position,m_dir);
		CollisionDetector detect;
		real distance=REAL_MAX;
		for(ObjectLinkList<RigidBody>::iterator it=PHYSICS->RigidBodies.begin();it!=PHYSICS->RigidBodies.end();++it)
		{
			if(it->GetOwner()->has(ComplexObject) ) continue;
			if(!GRAPHICS->DetermingIfInsideSameLevelBox(this->GetOwner()->LevelBoxId,it->GetOwner()->LevelBoxId))
				continue;			

			if(it->GetOwner() != this->GetOwner() && it->canCollide )
			{
				if(it->GetOwner()->has(CollisionPrimitive)->rayCast(ray,tempposition))
				{
					real a=squaredDistance(m_position,tempposition);
					if(a<distance)
					{
						distance=a;
						Object=it->GetOwner();
					}
				}
			}
		}

		for(ObjectLinkList<RigidBody>::iterator it1=PHYSICS->LevelData.begin();it1!=PHYSICS->LevelData.end();++it1)
		{
			if(it1->GetOwner() != this->GetOwner() && it1->GetOwner()->CompositionTypeId!=GOC_Type_TransparentWall && it1->GetOwner()->CompositionTypeId!=GOC_Type_LaserBase)
			{
				if(it1->GetOwner()->has(CollisionPrimitive)->rayCast(ray,tempposition))
				{
					real a=squaredDistance(m_position,tempposition);
					if(a<distance)
					{
						distance=a;
						Object=it1->GetOwner();
					}
				}
			}
		}


		ObjectLinkList<EventTrigger>::iterator it= PHYSICS->Events.begin();
		for(;it!=PHYSICS->Events.end();++it)
		{
			if(detect.rayCastBox(*(static_cast<CollisionBox*>(it->GetOwner()->has(CollisionPrimitive))),ray,tempposition))
			{
				real a=squaredDistance(m_position,tempposition);
				if(a<distance)
				{
					distance=a;
					Object=it->GetOwner();
				}
			}
		}

		m_endpoint=m_position+m_dir*sqrt(distance);
		m_CollidedWith=Object;
	}


	void Laser::Update(const float & dt)
	{
		if(this->GetOwner()->GlobalId==65)
			int o=1;
		if(!this->GetOwner()->has(ActivationProperties)->IsActivated())
			return;
		if(!GRAPHICS->DetermingIfInsideAdjacentToHeroLevelBox(this->GetOwner()->LevelBoxId))
			return;
		m_dir=Vector3(1,0,0);
		Quaternion q= m_Transform->QUATERNION;
		m_dir= q*m_dir;
		m_dir.normalise();
		m_position=m_Transform->Position;
		intersect();
		//Vec4 color(0,0.5,1,1);
		//GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(m_Transform->Position, color) );
		//GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vector3ToVec3(m_endpoint), color) );

		Vector3 pos= ( m_endpoint + m_Transform->Position)*0.5;
		laserLight->has(Transform)->Position=Vector3ToVec3(pos);
		Vector3 size= Vector3(m_Transform->Position) - m_endpoint;
		real mag=size.magnitude();
		laserLight->has(Transform)->Scale=Vec3(0.05f,(float)mag,0.05f);

		//Particle
		//laserLight->has(ParticleComponent)->CreateParticlesLine(dt, m_Transform->Position, Vector3ToVec3(m_endpoint), mag);
		//if(GRAPHICS->DetermingIfInsideAdjacentToHeroLevelBox(HERO->GetLevelBoxId(),this->GetOwner()->LevelBoxId))
		this->GetOwner()->has(ParticleComponent)->CreateParticles(dt, m_endpoint, m_dir);

		Vector3 v=size;
		Vector3 axis= Vector3(0,1,0)%size;
		axis.normalise();
		size.normalise();
		real angle=acos(size.y);

		laserLight->has(Transform)->QUATERNION=calculateQuaterion(angle,Vector3ToVec3(axis));



		if (m_CollidedWith)
		{
			if(m_CollidedWith->CompositionTypeId==GOC_Type_Hero )
				LOGIC->ResetLevel();//for now

			//if(m_CollidedWith->CompositionTypeId==GOC_Type_Cube  || m_CollidedWith->CompositionTypeId==GOC_Type_Sphere)
			//{
			if(m_CollidedWith->has(Dying))
			{
				POWERGUN->gravityGun.ActionIfPickedItemDestroyed(m_CollidedWith);
				m_CollidedWith->has(Dying)->MarkAsDying();
			}
			//else
			//m_CollidedWith->Destroy();
			//}
		}

	}

	void Laser::SendMessageA(Message *m)
	{
	}


	void Laser::Serialize(ISerializer& stream)
	{	

	}

	void Laser::RemoveComponentFromGame()
	{	

	}

}
