#pragma once //Makes sure this header is only included once
#include "Composition.h"
#include "VMath.h"
#include <iostream>
#include "TweakBarManager.h"
#include "Physics.h"
#include "Transform.h"

namespace Framework
{

	class MessageActivateObj : public Message
	{
	public:
		MessageActivateObj(GOC* c) : Message(Mid::ActivateObj), WhoSentIt(c),wasActivatedAfterMsg(false) {};
		GOC * WhoSentIt;
		bool wasActivatedAfterMsg;
	};


	
	class ActivationProperties : public GameComponent
	{
		friend class HistoryComponent;
		friend class TweakBarManager;
		friend class LoadLevelClass;
		friend class SaveLevelClass;
	public:
		ActivationProperties();
		~ActivationProperties();
		virtual void RemoveComponentFromGame();
		virtual void SendMessage(Message *);
		virtual void Initialize();
		virtual void Serialize(ISerializer& stream);
		void MarkAsActivated(bool t);
		void MarkAsActivatedForHistory(bool t,float);
		void ActivateGlowingCylinders(bool t);
		bool  IsActivated(){return m_IsActivated;}
		void Update(const float &);
		void Activate(GOC *);
		void ButtonAction();
		void DoorAction(const float & );
		int GetActivationId(){return m_ActivationId;};
		void ThrowerAction(const float & dt);
		void ThrowerActionPeriodic(const float & dt);
		void VerticalSliderAction(const float & dt);
		void RotatingObjectAction(const float & dt);
		void AttachedObjAction(const float & dt);
		void TeleporterAction(const float & dt);
		void DestroyerAction(const float & dt);
		void LaserAction(const float & dt);
		Vec4 GetObjectActivatedByVec(){return m_ActivatedByVec;}
		int GetAttachedToId(){return m_attachedToId;}
		bool DoNotActivate(){return m_neverActivate;}
		bool IsOneTimeActivated(){return m_oneTimeActivated;}
		bool CheckIfActivationTimerIsOver();
		float & GetCurrentActivationTime(){	return m_activatedTimer;}
		float GetActivationTimer(){	return m_timeToStayActivated;}
		bool GetPrevActivationState();
		void SetFlagHistoryComponent(bool t){m_flagForHistoryComponent=t;}
		bool GetMovedThisFrame(){return m_movedThisFrame;}
		void SetMovedThisFrame(bool t){m_movedThisFrame=t;}
		std::vector<GOC*> glowingCylList;
	private:
		bool m_PreviousActivationState;
		Quaternion m_inverseInitialQuat;
		bool m_denyActivation;
		bool m_IsActivated;
		int m_ResetOnLoad;
		Vec4 m_ActivatedByVec;
		int m_ActivationId;
		GOC * m_owner;
		RigidBody * m_Body;
		Transform * m_Transform;
		Vector3 m_InitialPos;
		float velocity;
		GOC * obj ;
		int throwerBindedObjId; 
		float m_timer;
		float m_timerLimit;
		GOC * m_attachedObj;
		int m_attachedToId;
		int m_restrictedById;
		bool m_neverActivate;
		bool m_oneTimeActivated;
		float m_initialSpeed;
		float m_shootingSpeed;
		Vec3 m_pointing;
		float m_timeToStayActivated;
		float m_activatedTimer;
		bool m_activationStateChanged;
		bool m_flagForHistoryComponent;
		Vector3 m_prevPos;
		Matrix4 m_originalTransform;
		bool m_movedThisFrame;
		


	};
}