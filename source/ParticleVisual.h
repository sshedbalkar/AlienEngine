#ifndef PARTICLEVISUAL_H
#define PARTICLEVISUAL_H

#include "Transform.h"

namespace Framework
{

class ParticleVisual{
public:
	ParticleVisual();
	virtual ~ParticleVisual(){}
	ParticleVisual( const Vec4& _color, const Vector3& _position, const Vec2& _scale = Vec2(0.1f, 0.1f), const Vec3& _velocity = Vec3(0,0,0), const Vec3& _acceleration = Vec3(0,0,0), bool _collideDelete = false);
	virtual void Initialize(){}
	virtual bool Update(float);

	Transform transform;
	Vec4 color;
	Vec3 velocity;
	Vec3 acceleration;

	bool bCollideDelete;
	//need for every particle for time mechanic
	//do not update if duration < 0, but cannot delete it because time may go forward
	float duration;
};

class ParticleTimeLimit : public ParticleVisual{
public:
	ParticleTimeLimit();
	virtual ~ParticleTimeLimit(){}
	ParticleTimeLimit( const Vec4& _color, const Vector3& _position, const Vec2& _scale = Vec2(0.1f, 0.1f), const Vec3& _velocity = Vec3(0,0,0), const Vec3& _acceleration = Vec3(0,0,0), float _duration_max = 0.0f, bool _collideDelete = false, const Vec3& _sizeDecreasingRate = Vec3(0,0,0) );
	virtual void Initialize(){}
	virtual bool Update(float);
	
	float duration_max;

	Vec3 sizeDecreasingRate;
};

}
#endif
