#pragma once //Makes sure this header is only included once
#include "Composition.h"
#include "Transform.h"
namespace Framework
{
	class Hero
	{
		friend class LoadLevelClass;
	public:
		Hero();
		~Hero(){}
		void Initialize();
		void Update(const float &);	
		void SendMessage(Message *);
		void ShowPositionInEditor();
		void RemoveShowPositionInEditor();
		Vector3 heroPos,heroDir,collisionNormal;
		GOC* GetHero(){return m_hero;}
		void TeleportHero(Vec3 & );
		Vector3 GetVelocityMove() const{ return m_velocityMove; }
		Vector3 GetVelocitySide() const{ return m_velocitySide; }
		void SetHero(GOC *g){m_hero=g;}
		int GetLevelBoxId(){return m_hero->LevelBoxId;}
	private:
		Vec3 prevlookat;
		GOC * m_hero;
		GOC * m_gun;
		Transform * m_transform;
		float m_pickDistanceFromSurface;
		float m_pickDistanceFromCenter;
		Vector3 m_velocityMove;
		Vector3 m_velocitySide;
		float m_fowardSpeed;
		float m_backwardSpeed;
		float m_SideMoveSpeed,m_jumpSpeed;
		bool m_allowSpace;
		bool m_CheatMode;
		void CheatMode(bool);
	};
	extern Hero* HERO;
}