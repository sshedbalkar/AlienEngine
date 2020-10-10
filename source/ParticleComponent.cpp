#include "Precompiled.h"
#include "ParticleComponent.h"
#include "ParticleVisual.h"
#include "particlesystemclass.h"
#include "Graphics.h"
#include "Physics.h"
#include "TimeMechanicManager.h"

#include "GameLogic.h"

namespace Framework
{

VisualParticleEmitter::VisualParticleEmitter() : 
t_emitParticle( 0.0f ), 
t_emitPeriod( 0.1f ), 
colorBase( Vec4(0, 0, 0, 1) ), 
colorRange( Vec4(0, 0, 0, 0) ), 
size( Vec2(0.1f, 0.1f) ), 
particleEmitterTypes(layserHit), 
duration_max(3.0f), 
velocityBase(4.0f), 
sizeDecreasingRate( Vec3(0,0,0) )
{
}

ParticleComponent::ParticleComponent()
{
}
void ParticleComponent::Initialize()
{//when we change or restart level
	for(unsigned i = 0 ; i<m_emitters.size() ; ++i){
		m_emitters[i].t_emitParticle = 0.0f;
		if( m_emitters[i].particleEmitterTypes == PET_Fire ){
			emitterPositions_fireLava.clear();

			const Transform* const transform  = this->GetOwner()->has(Transform);
			Vec3 scale( transform->Scale.x /2, transform->Scale.y /2, transform->Scale.z /2 );
			Vec3 minPosition = transform->Position - scale;
			minPosition.y += transform->Scale.y;

			int NumberFire = 20;
			for(int i=0;i < NumberFire ; ++i){
				Vec3 FirePosition = minPosition + Vec3( transform->Scale.x * rand01(), 0, transform->Scale.z * rand01() );
				emitterPositions_fireLava.push_back( FirePosition );
			}
		}
	}
}

void ParticleComponent::SendMessage(Message *m)
{
	
		
}

void ParticleComponent::Serialize(ISerializer& stream)
{
	float tempVec[4]={0.0f, 0.0f, 0.0f, 0.0f};
	int tempint;
	ISerializer* str = stream.clone();
	if(str->isGood())
	{
		m_emitters.resize(1);

		str->readValue("ParticleEmitterTypes",tempint);
			m_emitters[0].particleEmitterTypes = (ParticleEmitterTypes)tempint;

		str->readValue("EmitterPeriod",m_emitters[0].t_emitPeriod);

		str->readValue("size", m_emitters[0].size, 2);
		//size
		str->readValue("duration", m_emitters[0].duration_max);

		str->readValue("velocityBase", m_emitters[0].velocityBase);

		str->readValue("colorBase",tempVec,4);
			m_emitters[0].colorBase = Vec4(tempVec[0],tempVec[1],tempVec[2], tempVec[3]);
		if( str->readValue("colorRange",tempVec,4) ){
			m_emitters[0].colorRange = Vec4(tempVec[0],tempVec[1],tempVec[2], tempVec[3]);
		}
		if( str->readValue("sizeDecreasingRate",tempVec,3) ){
			m_emitters[0].sizeDecreasingRate = Vec3(tempVec[0],tempVec[1],tempVec[2]);
		}
	}
	delete str;
}

void ParticleComponent::RemoveComponentFromGame()
{

}

//return value
//  1 emit a new forward particle
// -1 emit a new backward particle
//  0 no particle emitted for this frame
int VisualParticleEmitter::UpdateEmitter(float dt){
	if( GRAPHICS->IsDebugCameraON() ) return 0;

	switch( TIMEMANAGER->GetTimeState() ){
	case TIME_FORWARD:
		if(TIMEMANAGER->IsbTimeListEnd() ) return 0;
		else t_emitParticle += dt;
		if( t_emitParticle >= t_emitPeriod ){
			t_emitParticle = 0.0f;
			return 1;
		}
		break;
	case TIME_PAUSED:
		return 0;
	case TIME_BACK:
		if(TIMEMANAGER->IsbTimeListEnd() ) return 0;
		else t_emitParticle += dt;
		if( t_emitParticle >= t_emitPeriod ){
			t_emitParticle = 0.0f;
			return -1;
		}
		break;
	default:// normal
		t_emitParticle += dt;
		if( t_emitParticle >= t_emitPeriod ){
			t_emitParticle = 0.0f;
			return 1;
		}
	}
	return 0;
}

void ParticleComponent::CreateParticlesLine(float dt, const Vec3& endPoint, const Vec3& startPosition, real mag){
	if( m_emitters[0].UpdateEmitter(dt) ){
		Vec3 velo( rand11()*m_emitters[0].velocityBase, rand11()*m_emitters[0].velocityBase, rand11()*m_emitters[0].velocityBase );
		ParticleTimeLimit* newPart = new ParticleTimeLimit( m_emitters[0].ParticleColorRange(), endPoint, m_emitters[0].size, velo, Vec3(0, -4.0f, 0), rand01()*m_emitters[0].duration_max, false, m_emitters[0].sizeDecreasingRate );
		GRAPHICS->GetParticleSystemClass()->m_particleDQ.push_back( newPart );
	}
}

void ParticleComponent::CreateParticles(float dt, const Vector3& endPoint, const Vector3& dir){
	int dUpdateResult = m_emitters[0].UpdateEmitter(dt);
	if( dUpdateResult == 0 ) return;
	switch( m_emitters[0].particleEmitterTypes){
	case layserHit:
		for(int i=0;i<20;++i){
			Vec3 velo( rand11() * m_emitters[0].velocityBase, rand11()* m_emitters[0].velocityBase, rand11()* m_emitters[0].velocityBase );
			//velo = Vec3(4,4,4);
			while( dir.dotProduct( velo)  > 0 ){
				velo = Vec3( rand11()*4, rand11()*4, rand11()*4 );
			}
			Vector3 Position = endPoint;
			Vec3 accer = Vec3(0, -4.0f, 0);
			//if( dUpdateResult == -1 ) 
			ParticleTimeLimit* newPart = new ParticleTimeLimit( m_emitters[0].ParticleColorRange(), Position, m_emitters[0].size, velo, accer, rand01()*m_emitters[0].duration_max, false, m_emitters[0].sizeDecreasingRate );
			if( dUpdateResult == -1 ){
				newPart->duration = newPart->duration_max - FLT_EPSILON;
				newPart->velocity += newPart->acceleration * newPart->duration;
				newPart->transform.Position += velo * newPart->duration + 0.5f * accer * newPart->duration * newPart->duration;
				newPart->transform.Scale -= m_emitters[0].sizeDecreasingRate * newPart->duration;
			}
			GRAPHICS->GetParticleSystemClass()->m_particleDQ.push_back( newPart );
		}
		break;
	default:
		__debugbreak();
	}
}

void ParticleComponent::CreateParticlesLava(float dt){
	const Transform* const transform  = this->GetOwner()->has(Transform);

	if( transform->Scale.x * transform->Scale.z < 10.0f ) CreateParticlesLavaSurfaceUp(dt);
	else{
		//CreateParticlesLavaRandomFire
		int dUpdateResult = m_emitters[0].UpdateEmitter(dt);
		if( dUpdateResult == 0 ) return;
	
		const Transform* const transform  = this->GetOwner()->has(Transform);
		Vec3 scale( transform->Scale.x /2, transform->Scale.y /2, transform->Scale.z /2 );
		Vec3 minPosition = transform->Position - scale;
		minPosition.y += transform->Scale.y;
		
		for(unsigned int i=0;i < emitterPositions_fireLava.size() ; ++i){

			int NumberParticles = 1;
			for(int j=0;j < NumberParticles ; ++j){

				Vec3 velo( 0, rand01()* m_emitters[0].velocityBase + 1.0f, 0 );
				Vec3 accer = Vec3(0, 0.0f, 0);

				ParticleTimeLimit* newPart = new ParticleTimeLimit( m_emitters[0].ParticleColorRange(), emitterPositions_fireLava[i], m_emitters[0].size, velo, accer, rand01()*m_emitters[0].duration_max, false, m_emitters[0].sizeDecreasingRate );
				if( dUpdateResult == -1 ){
					newPart->duration = newPart->duration_max - FLT_EPSILON;
					newPart->velocity += newPart->acceleration * newPart->duration;
					newPart->transform.Position += velo * newPart->duration + 0.5f * accer * newPart->duration * newPart->duration;
					newPart->transform.Scale -= m_emitters[0].sizeDecreasingRate * newPart->duration;
				}
				GRAPHICS->GetParticleSystemClass()->m_particleDQ.push_back( newPart );
			}
		}
	}
}

void ParticleComponent::CreateParticlesLavaSurfaceUp(float dt){
	int dUpdateResult = m_emitters[0].UpdateEmitter(dt);
	if( dUpdateResult == 0 ) return;
	
	const Transform* const transform  = this->GetOwner()->has(Transform);
	Vec3 scale( transform->Scale.x /2, transform->Scale.y /2, transform->Scale.z /2 );
	Vec3 minPosition = transform->Position - scale;
	minPosition.y += transform->Scale.y;
	//Vec3 maxPosition = transform->Position + scale;

	int NumberParticles = scale.x * scale.z;
	NumberParticles = 2;

	for(int i=0;i < NumberParticles ; ++i){

		Vec3 velo( 0, rand01()* m_emitters[0].velocityBase + 1.0f, 0 );
		Vec3 accer = Vec3(0, 0.0f, 0);
		
		Vec3 Position = minPosition + Vec3( transform->Scale.x * rand01(), 0, transform->Scale.z * rand01() );

		ParticleTimeLimit* newPart = new ParticleTimeLimit( m_emitters[0].ParticleColorRange(), Position, m_emitters[0].size, velo, accer, rand01()*m_emitters[0].duration_max, false, m_emitters[0].sizeDecreasingRate );
		if( dUpdateResult == -1 ){
			newPart->duration = newPart->duration_max - FLT_EPSILON;
			newPart->velocity += newPart->acceleration * newPart->duration;
			newPart->transform.Position += velo * newPart->duration + 0.5f * accer * newPart->duration * newPart->duration;
			newPart->transform.Scale -= m_emitters[0].sizeDecreasingRate * newPart->duration;
		}
		GRAPHICS->GetParticleSystemClass()->m_particleDQ.push_back( newPart );
	}
}

void ParticleComponent::CreateParticlesPointVector(float dt, const Vector3& positionOri, const Vec3& _vector, const Vec3& _sidedirection, const Vec3& _heroUp, Vector3& _heroVelocity ){
	if( m_emitters[0].UpdateEmitter(dt) ){
		switch( m_emitters[0].particleEmitterTypes){
		case GravityGunVacuumSuck:{
			//Vector3 HeroDirection = (positionDest - positionOri).unit();
			//HeroDirection = Vector3( -HeroDirection.x, -HeroDirection.y, -HeroDirection.z );
			//Vec3 Direction( (float)HeroDirection.x, (float)HeroDirection.y + 0.21f, (float)HeroDirection.z );
			for(int i=0;i<10;++i){
				//positionOri - _sidedirection*0.1f*rand11() + _heroUp*0.08f*rand11()
				float theta = rand01() * 2.0f * PI;
				float brightness = 0.25f + 0.5f * rand01(); //0.25 ~ 0.75
				Vec4 color( brightness, brightness, 1.0f, rand01() );
				/*GRAPHICS->GetParticleSystemClass()->m_particleDQ.push_back( new ParticleTimeLimit( color, positionOri, size, _vector * 1.5f, _vector * 2.0f, 0.2f, Vec3(0,0,0) ) );

				GRAPHICS->GetParticleSystemClass()->m_particleDQ.push_back( new ParticleTimeLimit( color, positionOri + _sidedirection*0.1f, size, _vector * 1.5f, _vector * 2.0f, 0.15f, Vec3(0,0,0) ) );
				GRAPHICS->GetParticleSystemClass()->m_particleDQ.push_back( new ParticleTimeLimit( color, positionOri - _sidedirection*0.1f, size, _vector * 1.5f, _vector * 2.0f, 0.15f, Vec3(0,0,0) ) );
				
				GRAPHICS->GetParticleSystemClass()->m_particleDQ.push_back( new ParticleTimeLimit( color, positionOri + _heroUp*0.08f, size, _vector * 1.5f, _vector * 2.0f, 0.15f, Vec3(0,0,0) ) );
				GRAPHICS->GetParticleSystemClass()->m_particleDQ.push_back( new ParticleTimeLimit( color, positionOri - _heroUp*0.08f, size, _vector * 1.5f, _vector * 2.0f, 0.15f, Vec3(0,0,0) ) );*/
				//prints(_heroVelocity );
				//GRAPHICS->GetParticleSystemClass()->m_particleDQ.push_back( new ParticleTimeLimit( color, positionOri + 0.08f * rand11() * sin(theta) * _heroUp + 0.1f * rand11() * _sidedirection * cos(theta), size*rand01(), _vector*1.5f + Vector3ToVec3(_heroVelocity) , _vector * 2.0f, duration_max, false, sizeDecreasingRate ) );
			}
			break;}
		default:
			__debugbreak();
		}
	}
}

Vec4 VisualParticleEmitter::ParticleColorRange(){
	Vec4 randVec4_01( rand01(), rand01(), rand01(), rand01() );
	for( int i=0; i<4 ;++i ){
		randVec4_01[i] = randVec4_01[i] * ( colorRange[i]) + colorBase[i];
	}
	return randVec4_01;
}

}
