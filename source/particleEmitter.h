#pragma once

#include "Component.h"
#include "VMath.h"
#include <D3D10.h>

namespace Framework
{

enum ParticleType
{
	PT_LAUNCHER = 1,
	PT_SHRINKING = 2,
	PT_ROTATING = 4,
	PT_RANDOMROTATION = 8,
};
enum BlendMethod
{
	BLEND_NO_BLEND = 0,
	BLEND_ADD_DIFFUSE = 1,
	BLEND_SUB_DIFFUSE = 2
};

class Transform;
class Texture;
class ParticleSystem;
class Emitter;

class ParticleEmitter : public GameComponent
{
public:
	Transform* transform;
private:
	Emitter** emitters;
	int emitter_count;
	
public:
	ParticleEmitter();
	~ParticleEmitter();
	
	virtual void Serialize(ISerializer& stream );

	virtual void Initialize();
	virtual void Load(){};
	virtual void Unload(){};
	virtual void Free();

//	void Activate( bool enable = true );
	
	virtual void SendMessage(Message *){};
	Emitter* GetEmitter( const char* id );

private:
	void SerializeIndividual( ISerializer& stream );
};

class Emitter
{
public:
	ParticleSystem* psystem;

	Vec3 offset_bottomleft;
	Vec3 offset_topright;
	Vec3 initial_velocity;
	Vec3 velocity_deviation;
	Vec3 acceleration;
	Vec4 color;
	float life_span;
	float life_span_deviation;
	float particles_per_millisecond;
	float interval;
	float radius;
	Texture* texture;
	int flag;
	unsigned int type;
	int blend_method;
	int max_particles;
	std::string id;

	float timer;
	ID3D10Buffer* vb_launcher;
	ID3D10Buffer* vb[2];
	int cur_target_buffer;

	bool initializing;
	bool activated;
	bool deactivating;

	Vec3 global_rotation;
	Vec3 global_rotation_velocity;

	ParticleEmitter* parent;
	Emitter* Next;
	Emitter* Prev;
		
public:
	Emitter();
	~Emitter();

	void Serialize( ISerializer& stream );
	void GetWorldTransformation( Mat4& mat_world );
	void Activate( bool enable = true );
	void Free();
	void Update( float dt );

};

}