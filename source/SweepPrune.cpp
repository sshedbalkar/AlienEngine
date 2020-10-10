#include "Physics.h"
#include <algorithm>
#include "GameLogic.h"

namespace Framework
{
	struct myclass {
		bool operator() (AABB *a ,AABB* b) 
		{
			
			return (a->min[sortaxis] < b->min[sortaxis]);
		}
	public:
		int sortaxis;
	} myobject;

	//int Physics::cmpAABBs(AABB* a,AABB* b)
	//{
	//	float minA=a->min[m_sortAxis];
	//	float minB=b->min[m_sortAxis];
	//	if (minA<minB) return -1;
	//	else return 1;
	//}
	bool Physics::TestAABBOverlap(AABB* a,AABB* b,int sortAxis)
	{
		//sortAxis 0,1 for X, Z
		if(a->max[sortAxis] > b->min[sortAxis] && a->min[sortAxis] < b->max[sortAxis] )
			return true;
		return false;
	}

	void Physics::SweepPruneAABBArray()
	{
		print(SortedAABBArrayX.size());
		int sortAxis=1;
		myobject.sortaxis=sortAxis;
		std::sort(SortedAABBArrayX.begin(),SortedAABBArrayX.end(),myobject);//sort the AABBs
		//myobject.sortaxis=1;
		//std::sort(m_SortedAABBArrayX.begin(),m_SortedAABBArrayX.end(),myobject);//sort the AABBs
		
		std::vector<AABB*> ::iterator it = SortedAABBArrayX.begin();
		for (;it<SortedAABBArrayX.end();++it)
		{

			std::vector<AABB*> ::iterator it1=it;
			for (;it1 < SortedAABBArrayX.end(); ++it1)
			{
				if(it1==it)
				{	if(!(*it)->m_body->getIsStatic())
						DoPlaneCollision((*it)->m_body);
					continue;
				}
				

				if((*it)->max[sortAxis] < (*it1)->min[sortAxis])
					break;

				if((*it)->m_body->getIsStatic() && (*it1)->m_body->getIsStatic() )
					continue;
		
				if(!(*it)->m_body->canCollide && !(*it1)->m_body->getIsStatic())//if object can't collide (dying etc)
					continue;

				if((*it)->m_body->getIsStatic() && !(*it1)->m_body->getAwake() || (*it1)->m_body->getIsStatic() && !(*it)->m_body->getAwake())
					continue;
				
			

				//HERO should not collide with picked object
				if(LOGIC->m_PowerGun.gravityGun.GetPickedItem() && (*it1)->m_body->GetOwner()->CompositionTypeId==GOC_Type_Hero && (*it)->m_body->GetOwner()==LOGIC->m_PowerGun.gravityGun.GetPickedItem()) 
					continue;

				if((*it)->m_body->getAwake() || (*it1)->m_body->getAwake())
				{
					//else it overlaps with this object on this axis(X), do another check on the other axis(Z)
					int otherAxis;
					if(sortAxis==1) otherAxis=0;
					else if(sortAxis==0) otherAxis=1;
					if(TestAABBOverlap(*it,*it1,otherAxis))
						DetectCollision((*it)->m_body, (*it1)->m_body );
				}
			}
		}
	}


}