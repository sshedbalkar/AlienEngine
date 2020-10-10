#include "precompiled.h"
#include "Physics.h"
#include "ModelComponent.h"
#include "Graphics.h"

namespace Framework
{
#define Max_GJK_Iterations 10
#define Max_EPA_Iterations 15
#define	EPSILONABS 0.0000001

unsigned CollisionDetector::GJKTest(CollisionPrimitive *b1,CollisionPrimitive *b2,CollisionData *data)
{
    if (data->contactsLeft <= 0) return 0;

	simplex.clear();
	SupportA.clear();
	SupportB.clear();
	dir= b2->getAxis(3)-b1->getAxis(3);
	dir.normalise();
	if( abs(dir.magnitude()-0) <=EPSILONABS ) 
	{
		dir.x=rand();
		dir.normalise();
	}
	SupportA.push_back(b1->Support(dir));
	SupportB.push_back(b2->Support(dir*-1));
	Vector3 s = SupportA.back() - SupportB.back();
	simplex.push_back(s);
	dir= s*-1;
	for (int i = 0; i < Max_GJK_Iterations; i++) 
    {
		dir.normalise();
		SupportA.push_back(b1->Support(dir));
		SupportB.push_back(b2->Support(dir*-1));
		s = SupportA.back() - SupportB.back();
		if(s*dir <0) return 0;
		simplex.push_back(s);
		if(updateSimplex())
		{
			//collision found
			return 1;
			return EPA(b1,b2,data);
			//return 1;
		}
	}
	return 0;
}

bool CollisionDetector::updateSimplex()
{
	if(simplex.size()==2)
	{
		Vector3 A = simplex[1]; 
		Vector3 B = simplex[0]; 
		Vector3 AB = B - A; 
		Vector3 AO = A*-1;
		dir= AB % AO % AB;		
	}else if(simplex.size()==3)
	{
		Vector3 A = simplex[2]; 
        Vector3 B = simplex[1]; 
        Vector3 C = simplex[0]; 
		Vector3 AO = A*-1;
		
		Vector3 AB = B - A; 
        Vector3 AC = C - A; 
        Vector3 ABC = AB % AC; 

		if( (ABC % AC)*AO > 0 ) 
        { 
			simplex.erase(simplex.begin()+1); 
			SupportA.erase(SupportA.begin()+1);
			SupportB.erase(SupportB.begin()+1);
			dir = AC % AO % AC; 			
        } 
        else if( (AB % ABC)*AO > 0) 
        { 
            simplex.erase(simplex.begin()); 
			SupportA.erase(SupportA.begin());
			SupportB.erase(SupportB.begin());
			dir= AB % AO % AB;
        }else if (ABC*AO > 0) 
        { 
            dir = ABC; 
        }else 
        { 
            {
				Vector3 temp= SupportA[0];
				SupportA[0]=SupportA[1];
				SupportA[1]=temp;
			}			
			{
				Vector3 temp= SupportB[0];
				SupportB[0]=SupportB[1];
				SupportB[1]=temp;
			}
			{
				Vector3 temp= simplex[0];
				simplex[0]=simplex[1];
				simplex[1]=temp;
			}
            dir = ABC * -1; 
        } 
    }else
	{

		Vector3 A = simplex[3]; 
		Vector3 B = simplex[2]; 
		Vector3 C = simplex[1]; 
		Vector3 D = simplex[0]; 

		Vector3 AO = A*-1; 
        Vector3 AB = B - A; 
        Vector3 AC = C - A; 
        Vector3 AD = D - A; 
        Vector3 ABC = AB % AC; 
        Vector3 ACD = AC % AD; 
        Vector3 ADB = AD % AB;

		if ((ABC*AO)>0) 
        { 
            simplex.erase(simplex.begin());
			SupportA.erase(SupportA.begin());
			SupportB.erase(SupportB.begin());
        }else if((ACD*AO)>0)
		{
			simplex.erase(simplex.begin()+2);
			SupportA.erase(SupportA.begin()+2);
			SupportB.erase(SupportB.begin()+2);
		}else if((ADB*AO)>0)
		{
			simplex.erase(simplex.begin()+1);
			SupportA.erase(SupportA.begin()+1);
			SupportB.erase(SupportB.begin()+1);
		}else
		{
			return true;
		}
		return updateSimplex();
	}
	//remove this line ??
	if( abs(dir.magnitude()-0) <=EPSILONABS ) 
		return true;
	return false;
}



void CollisionDetector::constructTetrahedron()
{
	while (!triangles.empty())
		triangles.pop();//clear the queue

	Vector3 A = simplex[3]; 
	Vector3 B = simplex[2]; 
	Vector3 C = simplex[1]; 
	Vector3 D = simplex[0]; 

	Triangle t1,t2,t3,t4;
	if(t1.ConstructTriangle(A,B,C, SupportA[3] ,SupportA[2],SupportA[1],  SupportB[3] ,SupportB[2],SupportB[1])) 
		triangles.push(t1);
	if(t2.ConstructTriangle(A,D,B, SupportA[3] ,SupportA[0],SupportA[2],  SupportB[3] ,SupportB[0],SupportB[2])) 
		triangles.push(t2);
	if(t3.ConstructTriangle(A,C,D, SupportA[3] ,SupportA[1],SupportA[0],  SupportB[3] ,SupportB[1],SupportB[0])) 
		triangles.push(t3);
	if(t4.ConstructTriangle(B,D,C, SupportA[2] ,SupportA[0],SupportA[1],  SupportB[2] ,SupportB[0],SupportB[1])) 
		triangles.push(t4);
}


bool Triangle::ConstructTriangle(Vector3 &a,Vector3 &b,Vector3 &c,
						   Vector3 &AS1,Vector3 &BS1,Vector3 &CS1,
						   Vector3 &AS2,Vector3 &BS2,Vector3 &CS2)
{
	A=a;
	B=b;
	C=c;
	SupportA1=AS1;
	SupportA2=AS2;
	SupportB1=BS1;
	SupportB2=BS2;
	SupportC1=CS1;
	SupportC2=CS2;

	Vector3 AB = B - A; 
    Vector3 AC = C - A; 
    Vector3 normal = AB % AC; //ABC
	if(normal*(A*-1)>0) 
	{
		int fukdup=1;
		B=c;
		C=b;
		SupportB1=BS2;
		SupportB2=BS1;
		SupportC1=CS2;
		SupportC2=CS1;
		Vector3 AB1 = B - A; 
		Vector3 AC1 = C - A; 
		normal = AB1 % AC1;
	}
	normal*=-1;

	L1 = (normal % C) * B; 
	if(L1<0.0) return false;
	L2 = (normal % A) * C; 
	if(L2<0.0) return false;
	L3 = (normal % B) * A; 
	if(L3<0.0) return false;

	if(abs(L1)<EPSILON && abs(L2)<EPSILON && abs(L3)<EPSILON) 
		return false;

	real denominator = 1.0/(L1 + L2 + L3);
	L1*=denominator;
	L2*=denominator;
	L3*=denominator;

	v= A*L1 + B*L2 + C*L3 ;
	vSquaredDistance = v.squareMagnitude();
	return true;
}



unsigned CollisionDetector::EPA(CollisionPrimitive *b1,CollisionPrimitive *b2,CollisionData *data)
{
	constructTetrahedron();
	Triangle bestifFail;
	bestifFail.vSquaredDistance=10000;
	for (int i = 0; i < Max_EPA_Iterations; i++) 
	{
		if(!triangles.empty())
		{
			Triangle t = triangles.top();
			triangles.pop();

			if(t.vSquaredDistance<bestifFail.vSquaredDistance) bestifFail=t;

			Vector3 direction = t.v;
			direction.normalise();
			Vector3 supA = b1->Support(direction);
			Vector3 supB = b2->Support(direction*-1);
			Vector3 w = supA - supB;

			if( abs( t.v*w - t.vSquaredDistance) <= 0.00001 )
			{
				//collision details found 
				/*Contact* contact = data->contacts;
				contact->penetration = t.v.magnitude();

				Vector3 supporta=t.SupportA1*t.L1 + t.SupportB1*t.L2 + t.SupportC1*t.L3;
				Vector3 supportb=t.SupportA2*t.L1 + t.SupportB2*t.L2 + t.SupportC2*t.L3;
				contact->contactPoint = (supporta+supportb)*0.5;

				contact->contactNormal = t.v;
				contact->contactNormal.normalise();
				contact->contactNormal *=-1;
				
				contact->setBodyData(b1->body, b2->body, data->friction, data->restitution);
				
				data->addContacts(1);*/

				
				t.v.normalise();
				Vector3 supporta=t.SupportA1*t.L1 + t.SupportB1*t.L2 + t.SupportC1*t.L3;
				Vector3 supportb=t.SupportA2*t.L1 + t.SupportB2*t.L2 + t.SupportC2*t.L3;
				D3DXVECTOR4 color(1,0,0,1),white(1,1,1,1);
				GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vector3ToVec3((supporta+supportb)*0.5),color));
				GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vector3ToVec3((supporta+supportb)*0.5+t.v),white));

				return 1;
			}
			Expand(b1,b2,t,w,supA,supB);
		}
	}
	/*Triangle t=bestifFail;
	Contact* contact = data->contacts;
	contact->penetration = t.v.magnitude();
	contact->contactNormal = t.v;
	contact->contactNormal.normalise();
	contact->contactPoint = t.SupportA1*t.L1 + t.SupportB1*t.L2 + t.SupportC1*t.L3;
	contact->setBodyData(b1->body, b2->body, data->friction, data->restitution);
	data->addContacts(1);*/
	return 0;
}

static inline Vector3 _closestPointFromOrigin(Vector3 &a,Vector3 &b)
{
	Vector3 ab = b-a;
	real t= (a*-1*ab) / (ab*ab);
	if(t<0) 
		t=0.0;
	if(t>1.0)
		t=1.0;
	return a+(ab*t);
}


void CollisionDetector::Expand(CollisionPrimitive *b1,CollisionPrimitive *b2,Triangle &t,Vector3 &w,Vector3 &sA,Vector3 &sB)
{
	//check AB
	{
		Vector3 Ve = _closestPointFromOrigin(t.A,t.B);
		Vector3 direction = Ve;
		direction.normalise();

		Vector3 SupA,SupB;
		SupA = b1->Support(direction);
		SupB = b2->Support(direction*-1);

		Vector3 We = SupA - SupB;
		if( abs(Ve*We - Ve.squareMagnitude())<EPSILON )
		{
			//dont split
			Triangle tLocal;
			if(tLocal.ConstructTriangle(t.A,t.B,w, t.SupportA1,t.SupportB1,sA,  t.SupportA2,t.SupportB2,sB)) triangles.push(tLocal);
		}else
		{
			//split into two	
			Triangle tLocal1,tLocal2;
			if(tLocal1.ConstructTriangle(t.A,We,w, t.SupportA1,SupA,sA, t.SupportA2,SupB,sB)) triangles.push(tLocal1);
			if(tLocal2.ConstructTriangle(We,t.B,w, SupA,t.SupportB1,sA, SupB,t.SupportB2,sB)) triangles.push(tLocal2);
		}
	}


	//check BC
	{
		Vector3 Ve = _closestPointFromOrigin(t.B,t.C);
		Vector3 direction = Ve;
		direction.normalise();

		Vector3 SupA,SupB;
		SupA = b1->Support(direction);
		SupB = b2->Support(direction*-1);

		Vector3 We = SupA - SupB;
		if( abs(Ve*We - Ve.squareMagnitude())<EPSILON )
		{
			//dont split
			Triangle tLocal;
			if(tLocal.ConstructTriangle(t.B,t.C,w, t.SupportB1,t.SupportC1,sA,  t.SupportB2,t.SupportC2,sB)) triangles.push(tLocal);
		}else
		{
			//split into two	
			Triangle tLocal1,tLocal2;
			if(tLocal1.ConstructTriangle(t.B,We,w, t.SupportB1,SupA,sA, t.SupportB2,SupB,sB)) triangles.push(tLocal1);
			if(tLocal2.ConstructTriangle(We,t.C,w, SupA,t.SupportC1,sA, SupB,t.SupportC2,sB)) triangles.push(tLocal2);
		}
	}


	//check CA
	{
		Vector3 Ve = _closestPointFromOrigin(t.C,t.A);
		Vector3 direction = Ve;
		direction.normalise();

		Vector3 SupA,SupB;
		SupA = b1->Support(direction);
		SupB = b2->Support(direction*-1);

		Vector3 We = SupA - SupB;
		if( abs(Ve*We - Ve.squareMagnitude())<EPSILON )
		{
			//dont split
			Triangle tLocal;
			if(tLocal.ConstructTriangle(t.C,t.A,w, t.SupportC1,t.SupportA1,sA,  t.SupportC2,t.SupportA2,sB)) triangles.push(tLocal);
		}else
		{
			//split into two	
			Triangle tLocal1,tLocal2;
			if(tLocal1.ConstructTriangle(t.C,We,w, t.SupportC1,SupA,sA, t.SupportC2,SupB,sB)) triangles.push(tLocal1);
			if(tLocal2.ConstructTriangle(We,t.A,w, SupA,t.SupportA1,sA, SupB,t.SupportA2,sB)) triangles.push(tLocal2);
		}
	}
}




}