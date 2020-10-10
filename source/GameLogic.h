///////////////////////////////////////////////////////////////////////////////////////
///
///	\file GameLogic.h
///	Sample game logic for demo.
///	
///	Authors: , 
///	Copyright 2010, Digipen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////////////

#pragma once //Makes sure this header is only included once
#include "Engine.h"
#include "GameData.h"
#include "VMath.h"
#include "PowerGun.h"
#include <iostream>
#include <vector>
#include "Hero.h"
#include "ObjectActivator.h"
#include "LogicCollisionResolve.h"
#include "SaveLevelClass.h"
#include "TimeMechanicManager.h"
#include "TextDisplayWrapper.h"
#include "LogicComponentManager.h"
#include "ILevel.h"
//
namespace Framework
{
	class MsgRecvrComponent;
	//
	class GameLogic : public ISystem
	{
		friend class Level_1;
		friend class ObjectActivator;
		friend class LoadLevelClass;
		friend class RigidBody;
		friend class LevelEditor;
		friend class Physics;
		friend class ActivationProperties;
		friend class LogicComponentManager;
	public:	
		GameLogic();
		~GameLogic();
		void Initialize();
		void Update(float timeslice);
		virtual std::string GetName(){return "GameLogic";}
		virtual void SendMessage(Message *);
		
		//object pointed at data
		GOC* objectPointedAt;
		real objectPointedAtDistance;
		Vector3 objectPointedpoint;
		GOC* objectPointedAtMagnet;
		real objectPointedAtDistanceMagnet;
		Vector3 objectPointedpointMagnet;

		int LoadNextLevel();
		int LoadPrevLevel();
		int LoadLevel( unsigned id );
		DisplayOnScreen textOnScreen;//to display text on screen

		int ResetLevel();
		bool IsPaused(){return _paused;}
		int AddMsgRecvr( MsgRecvrComponent* recvr );
		int RemoveMsgRecvr( MsgRecvrComponent* recvr );
		void Pause( bool val );
		//test object remove later
		GOC *testObj,*testObj1;
		bool temp123;
		//temp code end --harsha

		ObjectLinkList<HistoryComponent> HistoryList;
		ObjectLinkList<Dying> DyingComponentList;
		std::map<int,GOC*> ActivatableObjectsList;
		std::map<int,GOC*> RopePointsList;
		std::vector<GOC*> LaserObjectsList;
		std::vector<GOC*> ObjThatCanDieList;
		std::vector<GOC*> DominosList;
		std::vector<GOC*> AutoMovingObjects;
		std::vector<GOC*> PickedHolderList;
		std::vector<GOC*> RopePartsList;
		std::vector<GOC*> RolesList;
		std::vector<GOC*> BulbList;
		std::vector<GOC*> LevelBoxes;

		
		void OnFirstFrame( void* data );
		int GetLevelId(){return m_levelId;}

		ILevel* level;	
	//
	private:
		TimeMechanicManager m_timeMechanism;
		Hero m_Hero;
		PowerGun m_PowerGun;
		LogicCollisionResolve m_logicCollisions;
		ObjectActivator  m_ObjectActivator;
		LogicComponentManager m_componentManager;
		enum m_powerState{power1,power2,power3,noOfPower};
		void loadGameSettings(const std::string& file);
		bool _paused;
		
		int	m_levelId;
		SaveLevelClass *m_saver;
		void MenuCallback( const char* fid, const std::vector<std::string>* option );
		std::vector<MsgRecvrComponent*> m_msgRecvrs;
	};

	//A global pointer to our game so that it can be accessed from anywhere.
	extern GameLogic* LOGIC;
	#define print(...) LOGIC->textOnScreen.AddToPrint(__VA_ARGS__)
	#define prints(...) LOGIC->textOnScreen.AddToPrints(__VA_ARGS__)

}


