#include "BoomBoom.h"
#include "Physics.h"

namespace Framework
{
	void BoomBoom::Update()
	{
		for(unsigned int i=0;i<bomb_list.size();++i)
		{
			for(ObjectLinkList<RigidBody>::iterator it=PHYSICS->RigidBodies.begin();it!=PHYSICS->RigidBodies.end();++it)
			{
				real minSqrdDist = bomb_list[i].distance*bomb_list[i].distance;
				real sqrdDist = squaredDistance(bomb_list[i].pos,it->getPosition());
				if(sqrdDist<minSqrdDist)
				{
					Vector3 dir=it->getPosition()-bomb_list[i].pos;
					real dist=dir.magnitude();
					real forceMag;
					if(dist<1)
						forceMag=1;
					else
						forceMag=1/dist;
					forceMag*=bomb_list[i].force;
					
					dir=dir*forceMag;
					it->addForce(dir);
				}
			}
		}
		bomb_list.clear();
	}

	void BoomBoom::Initialize()
	{//when we change or restart level
	}

	void BoomBoom::AddBomb(Bomb bomb) 
	{
		bomb_list.push_back(bomb);
	}
}