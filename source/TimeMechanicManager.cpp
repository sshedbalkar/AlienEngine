#include "Precompiled.h"
#include "TimeMechanicManager.h"
#include "WindowsSystem.h"
#include "DebugDiagnostic.h"
#include "Global.h"
#include "GameHistory.h"
#include "HistoryComponent.h"
#include "TimeCalculator.h"
#include "GameLogic.h" //REMOVE LATER

namespace Framework
{

	HistoryDataListWrapper::~HistoryDataListWrapper()
	{
		for(auto it = m_historyDataList.begin(); it != m_historyDataList.end(); it++)
		{
			utility::safeDelete(*it);
		}
	}

	int HistoryDataListWrapper::GetBytesUsed()
	{
		int mem=0;
		mem += sizeof(HistoryDataListWrapper);
		for(auto it = m_historyDataList.begin(); it != m_historyDataList.end(); it++)
		{
			mem += (*it)->GetBytesUsed();					 
		}
		return mem;	
	}
	////////////////////////////////////////////////////////////////
		
	
	
	TimeMechanicManager* TIMEMANAGER = 0;
	
	TimeMechanicManager::TimeMechanicManager()
	{
		m_frameCounter=0;
		ErrorIf(TIMEMANAGER!=NULL,"TIMEMANAGER already initialized");
			TIMEMANAGER = this;
	}




	void TimeMechanicManager::SendMessage(Message *m)
	{
		switch( m->MessageId )
		{

		case Mid::Collide:
		{
			break;
		}	
		case Mid::LMouseButton:
			{
				TimeState t;
				LMouseButton * lmouse = (LMouseButton*)m;
				if(lmouse->ButtonIsPressed)
				{
					
					if(m_timeState!=TimeState::TIME_NORMAL )
					{
						m_timerStarted=false;
						t = TimeState::TIME_NORMAL;
						SetTimeState(t);
					}
				}
				break;
			}
		case Mid::RMouseButton:
			{
				TimeState t;
				RMouseButton * rmouse = (RMouseButton*)m;
				if(rmouse->ButtonIsPressed)
				{
					if(!m_timerStarted)
					{
						m_timerStarted=true;
						timecounter=0;
					}
					if(m_timeState==TimeState::TIME_PAUSED )
						t=TimeState::TIME_BACK;
					else
						t = TimeState::TIME_PAUSED;
					SetTimeState(t);
				}
				if(!rmouse->ButtonIsPressed)//unclicked
				{
					m_timerStarted=false;
					if(m_timeState==TimeState::TIME_BACK )
					{
						t = TimeState::TIME_PAUSED;
						SetTimeState(t);
					}
				}
				break;
			}

		case Mid::CharacterKey:
			{
				//MessageCharacterKey * key = (MessageCharacterKey*)m;
			
				break;
			}
		case Mid::TimeStateChange:
			{
				MessageTimeStateChange * msg = (MessageTimeStateChange*)m;
				m_nextFrameState=msg->t;
				//SetTimeState(msg->t);
				break;
			}
		case Mid::VirtualKey:
			{
				break;
			}
		case Mid::MouseMove:
			{
				break;
			}
		case Mid::MouseWheel :
			{
				MouseWheel * wmouse = (MouseWheel*)m;
				if(wmouse->Delta >0)
				{
					if(m_xSpeed<8)
					{
						if (m_xSpeed<0)
						{
							if(m_xSpeed%2==0)
								m_xSpeed/=2;
							else
								m_xSpeed=1;
						}
						else
							m_xSpeed*=2;
					}
						
				}
				else if(wmouse->Delta <0)
					if(m_xSpeed>-8)
					{
						if (m_xSpeed>0)
						{
							if(m_xSpeed%2==0)
								m_xSpeed/=2;
							else
								m_xSpeed=-1;
						}
						else
							m_xSpeed*=2;
					}
				break;
			}
		}


	}

	void TimeMechanicManager::SetTimeState(TimeState t)
	{
		if(Disabled)
			return;
		m_timeState = m_nextFrameState=  t;
					
		if(m_timeState != TimeState::TIME_NORMAL) 
		{
			PHYSICS->makeReversePhysics();
		}
		else
		{
			g_gameHistory->DeleteSucceedingFrameData();
			PHYSICS->makePhysicsNormal();
		}
	}

	void TimeMechanicManager::AddTolist(HistoryData * hd)
	{
		if(hd)
		{
			if(!m_container)
				m_container = new HistoryDataListWrapper;
			m_container->m_historyDataList.push_back(hd); 
		}
	}

	void TimeMechanicManager::Initialize()
	{//when we change or restart level
		//PHYSICS->makePhysicsNormal();
		framePos=FRM_BEGIN;
		m_timerStarted=false;
		m_container=NULL;
		m_frameCounter=1;
		m_bTimeListEnd = false;
		m_timeState = m_nextFrameState =  TimeState::TIME_NORMAL;
		m_immuneObject=0;
		m_xSpeed=1;
		Disabled=false;
	}

	void TimeMechanicManager::SetImmuneObject(GOC *g)
	{
		if(m_immuneObject==g)
			m_immuneObject=0;
		else
		{
			m_immuneObject=g;
			g->has(RigidBody)->ResetOriginalData();
		}
	}

	GOC * TimeMechanicManager::GetImmuneObject()
	{
		return m_immuneObject;
	}

	void TimeMechanicManager::UpdateObjects(HistoryDataListWrapper * m_container,bool isBackwards)
	{
		if(m_container)
		{
			for(auto it = m_container->m_historyDataList.begin(); it != m_container->m_historyDataList.end(); it++)
			{
				GOC * g = FACTORY->GetObjectWithId((*it)->objectId);
				if(g) //if g exists in FACTORY
				{
					if(isBackwards && g->FrameCreationId == m_frameCounter-1  && m_frameCounter-1 > 1)
					{
						g->RemoveFromGame();
					}
					else if((*it)->destroyed) //if it was destroyed, remove from factory list, put to removed lis
					{
						g->RemoveFromGame();
					}
					else//else keep updating
					{
						g->has(HistoryComponent)->Update(*it,isBackwards); 
					}
				}
				else //else if doesnt exist
				{
					g = FACTORY->GetRemovedGOCById((*it)->objectId); //get it from the removed list
					if(g)
					{
						if(FACTORY->RestoreRemovedGOC(g)==0) //restore it back to factory list
							g->has(HistoryComponent)->Update(*it,isBackwards); //update it
					}
				}
								 
			}
		}
	}

	void PrintState(TimeState t)
	{
		if(t == TimeState::TIME_PAUSED)
			print("STATE: " , "PAUSED");
		if(t == TimeState::TIME_NORMAL)
			print("STATE: " , "NORMAL");
		if(t == TimeState::TIME_BACK)
			print("STATE: " , "BACK");
		if(t == TimeState::TIME_FORWARD)
			print("STATE: " , "FORWARD");
	}


	void TimeMechanicManager::Update(float dt)
	{
		if(Disabled)
			return;
		print("TIME_SPEED: x" , m_xSpeed);
		print("CURRENT FRAME",m_frameCounter);//REMOVE LATER
		timecounter++;
		//if(m_timeManagerBegin<200)//give it some time to start 
		//	return;
		PrintState(m_timeState);

		if(m_timerStarted)
		{
			if(timecounter>50)
			{
				if(m_timeState == TimeState::TIME_PAUSED)
				{
					SetTimeState(TimeState::TIME_BACK);
				}
			}

		}
		

		
		/*float mem=g_gameHistory->GetBytesUsed();
		mem /=1024;
		mem /=1024;
		std::cout<<"MEMORY USED OVERALL: ---  "<<mem<<" MB  ---  "<<std::endl;*/

		if(m_timeState == TimeState::TIME_NORMAL)
		{
			if(m_container && m_container->m_historyDataList.size()>0)
			{
				g_gameHistory->NewFrameData(m_frameCounter,m_container);
				
				
			}
			++m_frameCounter;
			m_bTimeListEnd = false;
			
		}
		else
		{	
			for (int i=0; i<m_xSpeed;++i)
			{
				if(m_timeState == TimeState::TIME_BACK)//BACK TO TIME
				{
					if(	m_frameCounter>1)
					{
						utility::Object o1 = g_gameHistory->GetPrevFrameData(m_frameCounter,framePos);
						m_container = static_cast<HistoryDataListWrapper*>(o1);
						if(framePos==FRM_BEGIN ||  framePos==FRM_END)
							m_bTimeListEnd = true;
						else
							m_bTimeListEnd=false;
						UpdateObjects(m_container,true);
						if(framePos!=FRM_BEGIN )
							--m_frameCounter;
					}
					
				}
				else if(IsShiftHeld())//FORWARD TO TIME
				{
					m_timeState = TimeState::TIME_FORWARD;
					utility::Object o1 = g_gameHistory->GetNextFrameData(m_frameCounter,framePos);
					m_container = static_cast<HistoryDataListWrapper*>(o1);
					if(framePos==FRM_BEGIN ||  framePos==FRM_END)
						m_bTimeListEnd = true;
					else
						m_bTimeListEnd=false;
					UpdateObjects(m_container,false);
					if(framePos!=FRM_END )
						++m_frameCounter;
				}
				else
				{
					m_timeState = TimeState::TIME_PAUSED;
				}
			}
			
		}
		m_container=0;
		//if(m_timeManagerBegin==200)
		//	SetTimeState(TIME_NORMAL);//will only happen once

		if(m_timeState == TIME_PAUSED &&  m_timeState != m_nextFrameState)
			SetTimeState(m_nextFrameState);
		
	}

}
