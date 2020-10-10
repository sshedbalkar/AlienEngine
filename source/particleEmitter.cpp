#include "particleEmitter.h"
#include "Composition.h"
#include "Graphics.h"
#include "particleSystem.h"

namespace Framework
{
/***********************
	emitter composition
***********************/
	
//=====================================================
ParticleEmitter::ParticleEmitter() : emitter_count(0)
{
}
//=====================================================
ParticleEmitter::~ParticleEmitter()
{
	Free();
}
//=====================================================
void ParticleEmitter::Initialize()
{
	transform = GetOwner()->has(Transform);
}
//=====================================================
void ParticleEmitter::Serialize( ISerializer& stream )
{
	std::vector<ISerializer*> stream_emitters;
	stream.readProperty("emitter", stream_emitters );
	emitter_count = stream_emitters.size();
	emitters = new Emitter*[emitter_count];
	
	for ( int i = 0; i < emitter_count; ++i ) 
	{
		emitters[i] = new Emitter;
		emitters[i]->Serialize( *stream_emitters[i] );
		emitters[i]->parent = this;
		delete stream_emitters[i];
	}
}
//=====================================================
void ParticleEmitter::Free()
{
	if ( emitters )
	{
		for ( int i = 0; i < emitter_count; ++i )
		{
			delete emitters[i];
		}
		delete emitters;
	}
}
//=====================================================
//void ParticleEmitter::Activate( bool enable )
//{
//	if ( emitters )
//	{
//		for ( int i =  0; i < emitter_count; ++i )
//		{
//			emitters[i]->Activate( enable );
//		}
//	}
//}
//=====================================================
Emitter* ParticleEmitter::GetEmitter( const char* id )
{
	for ( int i = 0; i < emitter_count; ++i )
	{
		if ( emitters[i]->id == id ) return emitters[i];
	}
	return NULL;
}

/***********************
	single emitter
***********************/
//=====================================================
Emitter::Emitter() : initializing(true), deactivating(false), activated(true), texture( NULL )
{
}
//=====================================================
Emitter::~Emitter()
{
	Free();
}
//=====================================================
void Emitter::Serialize( ISerializer& stream )
{	

	ISerializer* emitter = stream.clone();
	std::string str;
	int k;
	std::string psystem_id;
	/// if it's activated by default
	if ( !emitter->readValue( "Activated", k) )
		activated = true;
	else if (k==0) 
		activated = false;
	/// which particle system it applies
	if ( !emitter->readValue( "System", psystem_id ) )
		psystem_id = "";
	if ( !emitter->readValue( "id", id ) )
		id = "nameless";
	/// max particle count
	if ( !emitter->readValue( "MaxParticles", max_particles ) )
		max_particles = 1000;
	/// offsets from the center
	if ( !emitter->readValue( "BottomLeftFront", offset_bottomleft ) ) 
		offset_bottomleft = Vec3( 0.0f,0.0f,0.0f );
	if ( !emitter->readValue( "TopRightBack", offset_topright ) )
		offset_topright = Vec3( 0.0f, 0.0f, 0.0f );
	/// initial velocity
	if ( !emitter->readValue( "InitialVelocity", initial_velocity ) )
		initial_velocity = Vec3( 0.0f, 0.0f, 0.0f );
	/// randomness of velocity
	if ( !emitter->readValue( "VelocityDeviation", velocity_deviation ) )
		velocity_deviation = Vec3( 0.0f, 0.0f, 0.0f );
	/// acceleration
	if ( !emitter->readValue( "Acceleration", acceleration ) )
		acceleration = Vec3( 0.0f,0.0f,0.0f );
	/// rotation of the emitter
	if ( !emitter->readValue( "GlobalRotationVelocity", global_rotation_velocity ) )
		global_rotation_velocity = Vec3( 0.0f, 0.0f, 0.0f );
	/// life time of individual particle
	if ( !emitter->readValue( "LifeSpan", life_span ) )
		life_span = 1.0f;
	/// randomness of lifetime
	if ( !emitter->readValue( "LifeSpanDeviation", life_span_deviation ) )
		life_span_deviation = 0.0f;
	/// this decide how many particles will be generated
	if ( !emitter->readValue( "ParticlesPerMillisecond", particles_per_millisecond ) )
		particles_per_millisecond = 1.0f;
	/// interval between emits
	if ( !emitter->readValue( "Interval", interval ) )
		interval = 0.0f;
	/// size of individual particle
	if ( !emitter->readValue( "ParticleRadius", radius ) )
		radius = 0.1f;
	/// texture
	if ( emitter->readValue( "Texture", str ) )
		texture = GRAPHICS->GetTexture( str.c_str() );
	/// color
	if ( !emitter->readValue( "Color", color ) )
		color = Vec4( 1.0, 1.0, 1.0, 0.0 );
	/// blend method
	if ( emitter->readValue( "BlendMethod", str ) )
	{
		if ( str == "Add" ) blend_method = BLEND_ADD_DIFFUSE;
		else if ( str == "Inv" ) blend_method = BLEND_SUB_DIFFUSE;
		else blend_method = BLEND_NO_BLEND;
	}
	else blend_method = BLEND_NO_BLEND;

	type = 0;
	////// type
	// particles will start with a random rotation
	k = 0;
	emitter->readValue( "RandomRotation", k );
	if ( k == 1 ) type |= PT_RANDOMROTATION;
	// particle will shrink over time
	k = 0;
	emitter->readValue( "Shrinking", k );
	if ( k == 1 ) type |= PT_SHRINKING;
	// particle will constantly rotate
	k = 0;
	emitter->readValue( "Rotating", k );
	if ( k == 1 ) type |= PT_ROTATING;
	// filter flag
	if ( !emitter->readValue( "FilterFlag", flag ) )
		flag = 0;

	global_rotation = Vec3( 0.0f,0.0f,0.0f );

	psystem = GRAPHICS->GetParticleSystem(psystem_id.c_str());
	psystem->RegisterEmitter( this );

	delete emitter;

}
//=====================================================
void Emitter::Free()
{
	S_RELEASE(vb[0]);
	S_RELEASE(vb[1]);
	psystem->RemoveEmitter( this );
}
//=====================================================
void Emitter::Activate( bool enable )
{
	if ( enable )
	{
		if ( !activated || deactivating )
		{
			activated = true;
			deactivating = false;
		}
	}
	else
	{
		if ( activated )
		{
			deactivating = true;
			timer = life_span + life_span_deviation;
		}
	}
}
//=====================================================
void Emitter::Update( float dt )
{
	global_rotation += global_rotation_velocity * dt;
}
//=====================================================
void Emitter::GetWorldTransformation( Mat4& mat_world )
{
	parent->transform->BuildMatrix( mat_world );
	Mat4 mat_rotation;
	D3DXMatrixRotationYawPitchRoll( &mat_rotation, global_rotation.y, global_rotation.x, global_rotation.z );
	D3DXMatrixMultiply( &mat_world, &mat_rotation, &mat_world );
}

}