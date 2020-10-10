#pragma once //Makes sure this header is only included once
#include <vector>
#include <list>
#include "Message.h"
#include "BoomBoom.h"
#include "Magnet.h"
#include "Connector.h"
#include "GravityGun.h"

namespace Framework
{
	class Bullet
	{
	public:
		Bullet(GOC *a):owner(a){endObject=NULL;}
		~Bullet(){}
		GOC* owner;
		GOC * endObject;
		Vector3 endPoint;
		Vector3 direction;
	};

	class PowerGun
	{
		//friend class RigidBody;
	public:
		PowerGun();
		~PowerGun(){}
		void Initialize();
		void Update(float dt);	
	    void SendMessage(Message *);
		BoomBoom bombs;	
		Magnet magnet;
		Connector connector;
		GravityGun gravityGun;
		void CreateBullet();
		bool IsActivated;
	private:
		typedef std::list<Bullet>::iterator bulletIterator;
		std::list<Bullet> bullets;

		enum powerState{powerConnect,powerBomb,powerMagnet,gravity, noOfPower} m_state;
		
	};
	extern PowerGun* POWERGUN;
}