#include "precompiled.h"
#include "Physics.h"
#include "ModelComponent.h"
#include "Graphics.h"

namespace Framework
{

#define	EPSILONABS 0.0000001
#define Max_MPR_Iterations 34
#define	CollideEpsilon 0.00001

template<typename T>
inline void SWAP(T &a,T &b)
{
	T temp;
	temp=a;
	a=b;
	b=temp;
}

unsigned CollisionDetector::MPRTest(CollisionPrimitive *b1,CollisionPrimitive *b2,CollisionData *data)
{
	Vector3 v01, v02, v0;
    Vector3 v11, v12, v1;
    Vector3 v21, v22, v2;
    Vector3 v31, v32, v3;
    Vector3 v41, v42, v4;
	Vector3 mn,normal;

	v01 = b1->getAxis(3);
	v02 = b2->getAxis(3);
	v0 = v02 - v01;

	if( abs(v0.magnitude()-0) <=EPSILONABS ) 
	{
		v0= Vector3(0.00001, 0, 0);
	}

	mn = v0;
    normal = v0*-1;
	mn.normalise();
	normal.normalise();
	v11=b1->Support(mn);
	v12=b2->Support(normal);
	v1 = v12 - v11;
	if(v1*normal<=0.0) return 0;

	normal = v1 % v0;
	if( abs(normal.magnitude()-0) <=EPSILONABS ) 
	{
		normal = v1-v0;
		normal.normalise();
		Contact* contact = data->contacts;
		contact->contactNormal = normal;
		contact->contactPoint = (v11+v12)*0.5;
		Vector3 temp;
		temp = v12-v11;
		contact->penetration = temp*normal;
		contact->setBodyData(b1->body, b2->body, data->friction, data->restitution);
		data->addContacts(1);
		return 1;
	}

	normal.normalise();
	mn = normal*-1;
	v21 = b1->Support(mn);
	v22 = b2->Support(normal);
	v2 = v22-v21;
	if(v2*normal<=0.0) return 0;

	Vector3 temp1,temp2;
	temp1 = v1-v0;
	temp2 = v2-v0;
	normal = temp1 % temp2;

	float dist = normal*v0;
	if(dist>0)
	{
		SWAP(v1,v2);
		SWAP(v11,v21);
		SWAP(v12,v22);
		normal = normal*-1;
	}

	int phase2 = 0;
    int phase1 = 0;
    bool hit = false;

	//phase 1 find the first portal
	while(true)
	{
		if (phase1 > Max_MPR_Iterations) return 0;
		phase1++;

		normal.normalise();
		mn = normal*-1;
		v31 = b1->Support(mn);
		v32 = b2->Support(normal);
		v3 = v32-v31;
		if(v3*normal<=0.0) return 0;


		// If origin is outside (v1,v0,v3), then eliminate v2 and loop
		temp1 = v1 % v3;
        if (temp1*v0 < 0.0f)
        {
            v2 = v3;
            v21 = v31;
            v22 = v32;
			temp1 = v1-v0;
			temp2 = v3-v0;
			normal = temp1 % temp2;
            continue;
        }


		// If origin is outside (v3,v0,v2), then eliminate v1 and loop
		temp1 = v3 % v2;
        if (temp1*v0 < 0.0f)
        {
            v1 = v3;
            v11 = v31;
            v12 = v32;
			temp1 = v3-v0;
			temp2 = v2-v0;
			normal = temp1 % temp2;
            continue;
        }

		//phase 2 refine the portal and get the contact data
		while(true)
		{
			phase2++;


			temp1 = v2-v1;
			temp2 = v3-v1;
			normal = temp1 % temp2;
			if( abs(normal.magnitude()-0) <=EPSILONABS )  
				return 0;//support plane close to portal return miss; step frm the book

			normal.normalise();
			real d = normal*v1;

			 // origin is inside the portal
            if (d >= 0 && !hit)	hit = true;

			mn = normal*-1;
			v41 = b1->Support(mn);
			v42 = b2->Support(normal);
			v4 = v42-v41;

			temp1 = v4-v3;
			float delta = temp1*normal;
			float penetration = v4*normal;

			if (delta <= CollideEpsilon || penetration <= 0.0f || phase2 > Max_MPR_Iterations)
            {
				if(hit)
				{
					//barycentric coordinates in the tetrahederon
                    float L0 = (v1%v2)*v3;
                    float L1 = (v3%v2)*v0;
                    float L2 = (v0%v1)*v3;
                    float L3 = (v2%v1)*v0;
                    float sum = L0 + L1 + L2 + L3;

					if (sum <= 0)
                    {
                        L0 = 0;
						L1 = (v2%v3)*normal;
						L2 = (v3%v1)*normal;
						L3 = (v1%v2)*normal;
                        sum = L1 + L2 + L3;
                    }

					float inv = 0.5f / sum;
					Vector3 point1 = v01*L0 + v11*L1 + v21*L2 + v31*L3;
					Vector3 point2 = v02*L0 + v12*L1 + v22*L2 + v32*L3;

					Contact* contact = data->contacts;
					contact->contactNormal = normal;
					contact->contactPoint = (point1+point2)*inv;
					contact->penetration = penetration;
					contact->setBodyData(b1->body, b2->body, data->friction, data->restitution);
					data->addContacts(1);

					/*D3DXVECTOR4 color(1,0,0,1),white(1,1,1,1);
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vector3ToVec3(contact->contactPoint),color));
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vector3ToVec3(contact->contactPoint+normal),white));*/

					return 1;
				}
				return 0;
			}

			temp1 = v4 % v0;
            float dot = temp1*v1;

			if (dot >= 0.0f)
            {
				dot = temp1*v2;
                if (dot >= 0.0f)
                {
                    v1 = v4;
                    v11 = v41;
                    v12 = v42;
                }
                else
                {
                    v3 = v4;
                    v31 = v41;
                    v32 = v42;
                }
			}
			else{
				dot = temp1*v3;
                if (dot >= 0.0f)
                {
                    v2 = v4;
                    v21 = v41;
                    v22 = v42;
                }
                else
                {
                    v1 = v4;
                    v11 = v41;
                    v12 = v42;
                }
			}
		}
	}
}




}