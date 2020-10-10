#pragma once //Makes sure this header is only included once
#include "Composition.h"
#include "Message.h"
#include <iostream>
#include "Physics.h"

namespace Framework
{
	class AutoMovingComponent: public GameComponent
	{
		friend class TweakBarManager;
		friend class LoadLevelClass;
		friend class SaveLevelClass;
		friend class ActivationProperties;
	public:
		AutoMovingComponent();
		~AutoMovingComponent(){}
		virtual void SendMessage(Message *);
		virtual void Initialize();
		virtual void RemoveComponentFromGame();
		virtual void Serialize(ISerializer& stream);
		void Update(const float & );	
		void UpdateMovement(const float & );	
		void UpdateRotation(const float & );	
		void StartRotation();
		void StopRotation();
		void SetReverseRotation(bool t);
		void StartMovement();
		void StopMovement();
		void SetReverseMovement(bool t);
		bool GetCanMove(){return m_autoCanMove;}
		bool GetCanRotate(){return m_autoCanRotate;}
		Vec3 GetRotationAxis(){return m_rotationAxis;}
		float getRotatingSpeed(){return m_rotatingSpeed;}
		std::vector<GOC*> objAttachedToThis;
		void UpdateAttachedObjectsPosition(const Vector3 &);
		void UpdateAttachedObjectsRotation(const D3DXQUATERNION &);
	private:
		Vec3 m_rotationAxis;
		Vec3 m_MovingAxis;
		Vec3 m_RotationPoint;
		float m_timer;
		float m_rotatingSpeed;
		float m_movingSpeed;
		Vector3 m_initialPos;
		bool m_autoCanMove;
		bool m_autoCanRotate;
		bool m_allowRotate;
		bool m_allowMove;
		RigidBody * m_Body;
		Transform * m_transform;
		int reverseRotation;
		int reverseMovement;
		
		
	};
}