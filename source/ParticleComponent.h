#pragma once //Makes sure this header is only included once
#include "Composition.h"
#include "Message.h"
#include <iostream>
#include <deque>

namespace Framework
{
	inline float rand01(){ return (float)(rand() )/(float)(RAND_MAX); }
	inline float rand11(){ return -1.0f + 2.0f * (float)(rand() )/(float)(RAND_MAX); }
	inline double gaussrand(void){
		static unsigned long seed = 61829450;
		double sum = 0;
		for(int i=0; i<3; i++)
		{   //Uses an xorshift PRNG
			unsigned long hold = seed;
			seed^=seed<<13;
			seed^=seed>>17;
			seed^=seed<<5;
			long r = hold+seed;
			sum += (double)r * (1.0/0x7FFFFFFF);
		}
		//Returns [-3.0,3.0] (66.7%?5.8%?00%)
		return sum;
	}
	
	enum ParticleEmitterTypes{
		layserHit = 0, 
		GravityGunVacuumSuck = 1, 
		PET_Fire = 2, 
		num_ParticleEmitterTypes
	};

	class ParticleComponent;
	class VisualParticleEmitter{
		friend class ParticleComponent;

		float t_emitParticle;
		float t_emitPeriod;

		Vec4 colorBase, colorRange;
		Vec2 size;

		float velocityBase;
		float duration_max;
		Vec3 sizeDecreasingRate;

		ParticleEmitterTypes particleEmitterTypes;
	public:
		VisualParticleEmitter();
		int UpdateEmitter(float dt);
		Vec4 ParticleColorRange();
	};

	class ParticleComponent: public GameComponent
	{
		friend class LoadLevelClass;

		std::deque<VisualParticleEmitter> m_emitters;
		std::deque<Vec3> emitterPositions_fireLava;
	public:
		ParticleComponent();
		virtual ~ParticleComponent(){}
		virtual void SendMessage(Message *);
		virtual void Initialize();
		virtual void Serialize(ISerializer& stream);
		virtual void RemoveComponentFromGame();

		void CreateParticles(float dt, const Vector3& endPoint, const Vector3& dir);
		void CreateParticlesLine(float dt, const Vec3& endPoint, const Vec3& startPosition, real mag);
		void CreateParticlesPointVector(float dt, const Vector3& positionOri, const Vec3& _vector, const Vec3& _sidedirection, const Vec3& _heroUp, Vector3& _heroVelocity);
		void CreateParticlesLava(float dt);
		void CreateParticlesLavaSurfaceUp(float dt);
	};
}