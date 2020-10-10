#include "Precompiled.h"
#include "PowerGun.h"
#include "WindowsSystem.h"
#include "GameLogic.h"
#include "Physics.h"
#include "LightComponent.h"

#define BULLET_SPEED 10.0f
#define FREE_BULLET_DISTANCE 50.0f
namespace Framework
{

	PowerGun* POWERGUN = 0;

	PowerGun::PowerGun()
	{
		
		ErrorIf(POWERGUN!=NULL,"HERO already initialized");
			POWERGUN = this;
	}

	void PowerGun::SendMessage(Message *m)
	{
		if(m_state==powerMagnet)
			magnet.SendMessageA(m);
		//else if(m_state==gravity)
		//	gravityGun.SendMessageA(m);
		else if(m_state==powerConnect)
			connector.SendMessageA(m);

		switch( m->MessageId )
		{

		case Mid::Collide:
		{
			break;
		}				

		case Mid::CharacterKey:
			{
				MessageCharacterKey * key = (MessageCharacterKey*)m;
				if(key->character=='1')
				{
					m_state=powerConnect;
				}
				else if(key->character=='2')
				{
					m_state=powerMagnet;
				}
				else if(key->character=='3')
				{
					
				}
				else if(key->character=='e')
				{
					m_state=gravity;
					gravityGun.SendMessageA(m);
				}
				else if(key->character=='f')
				{
					CreateBullet();
				}
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
				break;
			}
		case Mid::RMouseButton:
			{
				RMouseButton * rmouse = (RMouseButton*)m;
				if(rmouse->ButtonIsPressed)
				{
					
				}
				else
				{
					
				}
				break;
			}
		case Mid::LMouseButton:
			{
				LMouseButton * lmouse = (LMouseButton*)m;
				if(lmouse->ButtonIsPressed)
				{
					if(LOGIC->objectPointedAt )
					{
						if (LOGIC->objectPointedAt->CompositionTypeId==GOC_Type_LightBulbWithMass || LOGIC->objectPointedAt->CompositionTypeId==GOC_Type_LightBulb)
						{
							LOGIC->objectPointedAt->has(LightComponent)->Color=Vec3(Rand(0.0,1.0),Rand(0.0,1.0),Rand(0.0,1.0));
							GRAPHICS->DeferredLights[LOGIC->objectPointedAt->has(LightComponent)->GetIndex()].Position = LOGIC->objectPointedAt->has(Transform)->Position;
							GRAPHICS->DeferredLights[LOGIC->objectPointedAt->has(LightComponent)->GetIndex()].Color = LOGIC->objectPointedAt->has(LightComponent)->Color;
							GRAPHICS->CreateLightBuffer();
						}
					}
				}
				break;
			}
		}
	}

	void PowerGun::CreateBullet()
	{
		GOC *g;
		g = FACTORY->BuildFromArchetype("GOC_Type_Bullet");
		Vector3 pos;
		Quaternion q = HERO->GetHero()->has(Transform)->QUATERNION;
		Vector3 p = HERO->heroPos;
		Matrix4 t;
		t.setOrientationAndPos(q,p);
		pos=t.transform(Vector3(0.3,-0.3,0.3));
		pos=Vector3ToVec3(pos + HERO->heroDir*1.4);
		g->has(Transform)->Position=Vector3ToVec3(pos);
		g->has(Transform)->Scale=Vec3(0.2f,0.2f,0.2f);
		static_cast<CollisionSphere*>(g->has(CollisionPrimitive))->radius=g->has(Transform)->Scale.x*0.5f;
		g->Initialize();
		Bullet b(g);
		b.endObject=LOGIC->objectPointedAt;
		if(b.endObject)
		{
			b.endPoint=LOGIC->objectPointedpoint;
			b.direction = (LOGIC->objectPointedpoint-pos);
			b.direction.normalise();
		}else
		{
			b.direction = HERO->heroDir;
			b.endPoint = pos+ (HERO->heroDir*FREE_BULLET_DISTANCE);
		}
		bullets.push_back(b);
	}

	void PowerGun::Initialize()
	{//when we change or restart level
		connector.Initialize ();
		magnet.Initialize ();
		bombs.Initialize ();
		gravityGun.Initialize();
		IsActivated=true;
	}


	void PowerGun::Update(float dt)
	{
		//updating bullets
		bulletIterator it=bullets.begin();
		for(;it!=bullets.end();++it)
		{
			Transform *t=it->owner->has(Transform);
			Vector3 pos=t->Position;
			pos+= it->direction*BULLET_SPEED*dt;
			t->Position=Vector3ToVec3(pos);
		}

		//check if collide
		it=bullets.begin();
		while(it!=bullets.end() && bullets.size()>0)
		{
			real dist= (it->endPoint - it->owner->has(Transform)->Position).magnitude();
			print(dist);
			if(dist < (BULLET_SPEED*dt+0.05))
			{
				//collision detected....
				//write ur code here


				//destroy the bullet
				it->owner->Destroy();
				bullets.erase(it++);
			}else
			{
				++it;
			}
		}
		

		magnet.Update();
		bombs.Update ();
		connector.Update();
		gravityGun.Update(dt);
	}

}