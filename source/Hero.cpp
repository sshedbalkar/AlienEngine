#include "Precompiled.h"
#include "Hero.h"
#include "Physics.h"
#include "ModelComponent.h"
#include "PhysicsMath.h"
#include "VMath.h"
#include "WindowsSystem.h"
#include "cameraclass.h"
#include "Graphics.h"
#include "GameLogic.h"


namespace Framework
{
	Hero* HERO = NULL;

	Hero::Hero()
	{
		m_hero=NULL;
		m_gun=NULL;
		m_fowardSpeed = 12.0f;
		m_backwardSpeed = 12.0f;
		m_SideMoveSpeed = 10.0f;
		m_jumpSpeed = 4.5f;
		m_allowSpace=true;
		m_pickDistanceFromSurface=3.0f;
		m_pickDistanceFromCenter=3.0f;
		ErrorIf(HERO!=NULL,"HERO already initialized");
		HERO = this;
		m_CheatMode=false;
	}
	void Hero::Initialize()
	{//when we change or restart level
		if(m_hero)
		{
			m_transform=m_hero->has(Transform);
			CAMERA->position = m_hero->has(Transform)->Position;
			CAMERA->position.y += (m_hero->has(Transform)->Scale.y/2.0f);
		}
		CAMERA->ResetCameraDirection();
	}

	void Hero::TeleportHero(Vec3 & ToPosition)
	{
		m_hero->has(RigidBody)->setPosition(ToPosition);
		m_hero->has(Transform)->Position=ToPosition;
		if (POWERGUN->gravityGun.GetPickedItem())
		{
			POWERGUN->gravityGun.GetPickedItem()->has(RigidBody)->setPosition(ToPosition);
		}
	}


	void Hero::SendMessage(Message *m)
	{
		if(!m_hero)
			return;
		switch( m->MessageId )
		{

		case Mid::CharacterKey:
			{
				MessageCharacterKey * key = (MessageCharacterKey*)m;
				/*if(key->character==VK_SPACE)
				{
					if(m_hero->has(RigidBody)->getAcceleration().y==0.0 && m_allowSpace)
					{
						Vector3 temp=m_hero->has(RigidBody)->getVelocity();
						temp.y = m_jumpSpeed;
						m_hero->has(RigidBody)->setVelocity (temp);
						m_allowSpace=false;
					}
				}*/
				
				break;
			}
		case Mid::VirtualKey:
			{
				MessageVirtualKey* key = (MessageVirtualKey*)m;
				if(key->character==VK_F11)
				{
					m_CheatMode=!m_CheatMode;
					CheatMode(m_CheatMode);
				}
				break;
			}
		case Mid::CharacterKeyRelease:
			{/*
				MessageCharacterKeyRelease* key = (MessageCharacterKeyRelease*)m;
				if(key->character==VK_SPACE)
				{
					m_allowSpace=true;
				}*/
				m_allowSpace=true;
				break;
			}
		case Mid::MouseMove:
			{
				break;
			}
		case Mid::MouseWheel :
			{
				break;
			}
		case Mid::RMouseButton:
			{
				RMouseButton * rmouse = (RMouseButton*)m;
				if(rmouse->ButtonIsPressed)
				{
				}
				break;
			}
		case Mid::LMouseButton:
			{
				break;
			}
		}
	}


	void Hero::CheatMode(bool b)
	{
		if(b)
		{
			m_hero->has(RigidBody)->setAcceleration(m_hero->has(RigidBody)->getAcceleration().x,0,m_hero->has(RigidBody)->getAcceleration().z);
		}
		else
		{
		}
	}

	void Hero::Update(const float & dt)
	{
		CAMERA->UpdateMovingDirectionAndLookAt(dt);
		m_hero->LevelBoxId=-1;
		if(m_hero)
		{	
			//updateHero's LevelBoxId
			float posX,posZ;
			posX=m_transform->Position.x;
			posZ=m_transform->Position.z;
			for (unsigned int i=0;i<LOGIC->LevelBoxes.size();i++)
			{
				GOC *goc=LOGIC->LevelBoxes[i];
				float minX,maxX,minZ,maxZ;
				minX=goc->has(Transform)->Position.x-goc->has(Transform)->Scale.x*0.5f;
				maxX=goc->has(Transform)->Position.x+goc->has(Transform)->Scale.x*0.5f;
				minZ=goc->has(Transform)->Position.z-goc->has(Transform)->Scale.z*0.5f;
				maxZ=goc->has(Transform)->Position.z+goc->has(Transform)->Scale.z*0.5f;
				if(posX>minX && posX<maxX && posZ>minZ && posZ<maxZ)
				{
					m_hero->LevelBoxId = goc->LevelBoxId;
					break;
				}
			}


			//initialize every time
			m_velocityMove = Vector3 (0.0,0.0,0.0);
			m_velocitySide = Vector3 (0.0,0.0,0.0);
			Vector3 heroVelocity = m_hero->has(RigidBody)->getVelocity();

			if(heroVelocity.y>m_jumpSpeed)
			    heroVelocity.y=0;

			if (IsWHeld() )      m_velocityMove =  m_fowardSpeed * CAMERA->movingDirection;
			else if (IsSHeld() )	m_velocityMove = -m_fowardSpeed * CAMERA->movingDirection;
			
			if (IsAHeld() )		m_velocitySide= -m_SideMoveSpeed * CAMERA->sideDirection;
			else if (IsDHeld() )	m_velocitySide=  m_SideMoveSpeed * CAMERA->sideDirection;
			
			if ((IsWHeld() && (IsAHeld() || IsDHeld())) || (IsSHeld() && (IsAHeld() || IsDHeld())) )
			{
				Vector3 temp=(m_velocityMove + m_velocitySide)*0.5f;
				temp.y=heroVelocity.y;
				if(collisionNormal.y>=0.5 && m_hero->has(RigidBody)->isColliding)
				{
					real mag=temp.magnitude();
					temp.normalise();
					Vector3 a= temp%collisionNormal;
					a= collisionNormal%a;
					a.normalise();
					a=a*mag;
					temp=a;
				}else if(m_hero->has(RigidBody)->isColliding)
				{
					/*real mag=temp.magnitude();
					temp.normalise();
					Vector3 a= temp%collisionNormal;
					a= collisionNormal%a;
					a.normalise();
					real costheta=a*temp;
					temp*=(costheta*mag);
					temp.y=heroVelocity.y;*/
					temp.y=-3;
				}
				m_hero->has(RigidBody)->setVelocity (temp);
			}
			else if(IsWHeld() || IsAHeld() || IsDHeld() || IsSHeld())
			{
				Vector3 temp=(m_velocityMove + m_velocitySide)*0.5f;
				temp.y = heroVelocity.y;
				if(collisionNormal.y>=0.5 && m_hero->has(RigidBody)->isColliding)
				{
					real mag=temp.magnitude();
					temp.normalise();
					Vector3 a= temp%collisionNormal;
					a= collisionNormal%a;
					a.normalise();
					a=a*mag;
					temp=a;
				}else if(m_hero->has(RigidBody)->isColliding)
				{
					temp.y=-3;
				}
				m_hero->has(RigidBody)->setVelocity (temp);
			}else if(!IsWHeld() || !IsAHeld() || !IsDHeld() || !IsSHeld())
			{
				m_hero->has(RigidBody)->setVelocity (heroVelocity);
			}

			if (IsSpaceHeld() )
			{
				if(m_hero->has(RigidBody)->getAcceleration().y==0.0 && m_allowSpace)
				{
					Vector3 temp=m_hero->has(RigidBody)->getVelocity();
					temp.y = m_jumpSpeed;
					m_hero->has(RigidBody)->setVelocity (temp);
					m_allowSpace=false;
				}

			}
					

			CAMERA->position = m_hero->has(Transform)->Position;
			//get the eye position of hero
			Vector3 look=CAMERA->Getlookat();
			Vector3 heroUp= CAMERA->GetHeroUp();//look%CAMERA->sideDirection;
			heroUp.normalise();
			CAMERA->position += Vector3ToVec3(heroUp)*(m_hero->has(Transform)->Scale.y/2.0f);		

			CAMERA->Render(dt);
			D3DXMATRIX mat;
			CAMERA->GetViewMatrix(mat);
			float thetax=atan2(mat._32,mat._33);
			float thetay=atan2(-mat._31,sqrt((mat._33*mat._33)+(mat._32*mat._32)));
			float thetaz=atan2(mat._21,mat._11);
			Quaternion qx=calculateQuaterion(thetax,Vec3(1,0,0));
			Quaternion qy=calculateQuaterion(thetay,Vec3(0,1,0));
			Quaternion qz=calculateQuaterion(thetaz,Vec3(0,0,1));
			Quaternion q= qz*(qy*qx);
			m_hero->has(Transform)->QUATERNION= D3DXQUATERNION(q.i,q.j,q.k,q.r);

			if(m_gun)
			{
				m_gun->has(Transform)->QUATERNION=m_hero->has(Transform)->QUATERNION;
				Vector3 look=CAMERA->Getlookat();
				look.normalise();
				Vector3 side= look%Vector3(0,1,0);
				side.normalise();
				Vector3 pos = look*1.6 + CAMERA->position;
				pos-= side*0.8;
				m_gun->has(Transform)->Position=Vector3ToVec3(pos);
			}
		}
	}

	void Hero::ShowPositionInEditor(){
		m_hero->has(ModelComponent)->SwapTexture();
	}
	void Hero::RemoveShowPositionInEditor(){
		m_hero->has(ModelComponent)->SwapTexture();
	}
}