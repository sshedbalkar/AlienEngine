#include "Precompiled.h"
#include "ParticleVisual.h"

#include "Physics.h"

namespace Framework
{

ParticleVisual::ParticleVisual() :  
color( Vec4(1.0f,1.0f,1.0f, 1.0f) ), 
velocity( Vec3(0,0,0) ), 
acceleration( Vec3(0,0,0) ), 
duration(0.0f)
{
	transform.Scale = Vec3(0.1f, 0.1f, 0.1f );
};

ParticleVisual::ParticleVisual( const Vec4& _color, const Vector3& _position, const Vec2& _scale, const Vec3& _velocity, const Vec3& _acceleration, bool _collideDelete ) :  
color( _color ), 
velocity(_velocity), 
acceleration( _acceleration ), 
duration(0.0f), 
bCollideDelete(_collideDelete)
{
	transform.Position.x = (float)_position.x;
	transform.Position.y = (float)_position.y;
	transform.Position.z = (float)_position.z;
	transform.Scale.x = _scale.x;
	transform.Scale.y = _scale.y;
	transform.Scale.z = 0.1f;
};

bool ParticleVisual::Update(float dt){
	duration+=dt;
	if( duration < 0 ) return true;
	velocity += acceleration*dt;
	transform.Position += dt * velocity;

	if(bCollideDelete){
		for( Physics::RigidBodyIterator it = PHYSICS->LevelData.begin();it != PHYSICS->LevelData.end();++it){
			if(it->GetOwner()->has(CollisionPrimitive)->Id == CollisionPrimitive::ShapeId::Box){
				const CollisionPrimitive* pBox = it->GetOwner()->has(CollisionPrimitive);
				bool collisionResult = PHYSICS->intersectionTest.boxAndPoint( *static_cast<CollisionBox*>(it->GetOwner()->has(CollisionPrimitive)), transform.Position );
				if(collisionResult){
					return true;
				}
			}
		}
	}

	return false;
}

ParticleTimeLimit::ParticleTimeLimit() :  
duration_max(3.0f), 
sizeDecreasingRate( Vec3(0.05f, 0.05f, 0.05f) )
{
};

ParticleTimeLimit::ParticleTimeLimit( 
	const Vec4& _color, const Vector3& _position, const Vec2& _scale, 
	const Vec3& _velocity, const Vec3& _acceleration, 
	float _duration_max, bool _collideDelete, const Vec3& _sizeDecreasingRate ) :  
duration_max(_duration_max), 
ParticleVisual( _color, _position, _scale, _velocity, _acceleration, _collideDelete), 
sizeDecreasingRate( _sizeDecreasingRate )
{
}

bool ParticleTimeLimit::Update(float dt){
	if( ParticleVisual::Update(dt) ) return true;
	if(duration>=duration_max){
		return true;
	}
	transform.Scale -= sizeDecreasingRate * dt;

	return false;
}

}
