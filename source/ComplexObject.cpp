#include "precompiled.h"
#include "Physics.h"
#include "Graphics.h"
#include "GameLogic.h"

namespace Framework
{

ComplexObject::ComplexObject()
{
	/*for(int i=0;i<16;i++)
		alive[i]=0;*/
}

ComplexObject::~ComplexObject()
{

}


void ComplexObject::Initialize()
{
	transform = GetOwner()->has(Transform);
	body = GetOwner()->has(RigidBody);
	FindCOM();
	propertiesIterator it=properties.begin();
	for(;it!=properties.end();++it)
	{
		GOC * newObject = FACTORY->BuildFromArchetype(it->graphicsType.c_str());
		newObject->CompositionTypeId=newObject->GetIdFromName(const_cast<char*>(it->graphicsType.c_str()));
		CollisionBox *box=new CollisionBox();
		box->body= body;
		box->t=newObject->has(Transform);
		box->t->Scale=Vector3ToVec3(it->halfSize*2);
		box->halfSize=it->halfSize;
		box->orientation=it->local_orientation;
		Matrix4 offset;
		offset.setOrientationAndPos(it->local_orientation,it->posToCOM);
		box->offset=offset;
		collisionPrimitives.push_back(box);
		it->shape = box;
		newObject->Initialize();
	}

	Matrix3 tensor;
	it=properties.begin();
	for(;it!=properties.end();++it)
	{	
		Matrix3 temp_tensor;
		temp_tensor.setBlockInertiaTensor(it->halfSize,it->mass);
		temp_tensor.moveFromCenterOfMass(it->posToCOM,it->mass);
		tensor+= temp_tensor;
	}

	body->setInertiaTensor(tensor);
	body->setMass(totalMass);
}

void ComplexObject::FindCOM()
{
	propertiesIterator it=properties.begin();
	totalMass=0;
	Vector3 temp(0,0,0);
	for(;it!=properties.end();++it)
	{
		if(!alivebitset[it->index]) continue;
		totalMass+=it->mass;
		temp+= (it->local_position*it->mass) ;
	}
	CenterOfMass = temp*(1.0/totalMass);
	
	it=properties.begin();
	for(;it!=properties.end();++it)
	{
		if(!alivebitset[it->index]) continue;
		it->posToCOM= it->local_position - CenterOfMass;
	}
}

void ComplexObject::DrawObject()
{
	for(CollisionPrimitiveIterator it=collisionPrimitives.begin();it!=collisionPrimitives.end();++it)
	{
		if(it->Id==CollisionPrimitive::ShapeId::Box)
		{
			Vector3 s,a,b,c;
			Matrix4 transform = it->getTransform();
			Vector3 halfsize= static_cast<CollisionBox*>(&(*it))->halfSize;
				
			s = a = b = c = halfsize;
				
			a-=Vector3(halfsize.x*2,0,0);
			b-=Vector3(0,halfsize.y*2,0);
			c-=Vector3(0,0,halfsize.z*2);
			s=transform.transform(s);
			a=transform.transform(a);
			b=transform.transform(b);
			c=transform.transform(c);

			Vec4 color(0,1,0,1);
			GRAPHICS->AddTwoPointsToLineDrawer(s,a,color);
			GRAPHICS->AddTwoPointsToLineDrawer(s,b,color);
			GRAPHICS->AddTwoPointsToLineDrawer(s,c,color);

			s = a = b = c = (halfsize*(-1));
			a+=Vector3(halfsize.x*2,0,0);
			b+=Vector3(0,halfsize.y*2,0);
			c+=Vector3(0,0,halfsize.z*2);
			s=transform.transform(s);
			a=transform.transform(a);
			b=transform.transform(b);
			c=transform.transform(c);

			GRAPHICS->AddTwoPointsToLineDrawer(s,a,color);
			GRAPHICS->AddTwoPointsToLineDrawer(s,b,color);
			GRAPHICS->AddTwoPointsToLineDrawer(s,c,color);

			s = a = b = Vector3(-halfsize.x,halfsize.y,-halfsize.z);
			a+=Vector3(0,0,halfsize.z*2);
			b+=Vector3(halfsize.x*2,0,0);
			s=transform.transform(s);
			a=transform.transform(a);
			b=transform.transform(b);

			GRAPHICS->AddTwoPointsToLineDrawer(s,a,color);
			GRAPHICS->AddTwoPointsToLineDrawer(s,b,color);

			s = a = b = Vector3(halfsize.x,-halfsize.y,-halfsize.z);
			a+=Vector3(0,0,halfsize.z*2);
			b+=Vector3(0,halfsize.y*2,0);
			s=transform.transform(s);
			a=transform.transform(a);
			b=transform.transform(b);

			GRAPHICS->AddTwoPointsToLineDrawer(s,a,color);
			GRAPHICS->AddTwoPointsToLineDrawer(s,b,color);

			s = a = b = Vector3(-halfsize.x,-halfsize.y,halfsize.z);
			a+=Vector3(halfsize.x*2,0,0);
			b+=Vector3(0,halfsize.y*2,0);
			s=transform.transform(s);
			a=transform.transform(a);
			b=transform.transform(b);

			GRAPHICS->AddTwoPointsToLineDrawer(s,a,color);
			GRAPHICS->AddTwoPointsToLineDrawer(s,b,color);
		}
	}
}

void ComplexObject::Serialize(ISerializer& stream)
{
	ISerializer* str = stream.clone();
	if(str->isGood())
	{
		std::vector<ISerializer*> objects;
		if(str->readProperty("PaPaJoe", objects))
		{
			for(std::vector<ISerializer*>::iterator it1 = objects.begin(), it2 = objects.end(); it1 != it2; ++it1)
			{
				singleObjectProperties a;
				float mass;
				(*it1)->readValue("mass", mass);
				a.mass= mass;

				int tempint;
				(*it1)->readValue("index",tempint);
				 a.index = tempint;

				float arr[16];
				(*it1)->readValue("connectedTo", arr,16);
				for (int i=0;i<16;++i)
				{
					a.connectedTo[i]=(int)arr[i];
				}
			

				float tempVec[3]={0.0,0.0,0.0};
				(*it1)->readValue("Position",tempVec,3);
				a.local_position=Vector3(tempVec[0],tempVec[1],tempVec[2]);

				float tempVec1[4]={0.0,0.0,0.0,0.0};
				(*it1)->readValue("Orientation",tempVec1,4);
				a.local_orientation=Quaternion(tempVec1[0],tempVec1[1],tempVec1[2],tempVec1[3]);

				(*it1)->readValue("halfSize",tempVec,3);
				a.halfSize=Vector3(tempVec[0],tempVec[1],tempVec[2]);

				(*it1)->readValue("GraphicsType", a.graphicsType);

				alivebitset[tempint] = 1;
				properties.push_back(a);
			}
		}
	}
	delete str;
}

void ComplexObject::SendMessage(Message *m)
{
	
}

void ComplexObject::calculateInternals()
{
	for(CollisionPrimitiveIterator it=collisionPrimitives.begin();it!=collisionPrimitives.end();++it)
	{
		it->calculateInternals();
	}
}

void ComplexObject::updateGraphicsObjects()
{
	int i=0;
	for(CollisionPrimitiveIterator it=collisionPrimitives.begin();it!=collisionPrimitives.end();++it,++i)
	{
		if(!alivebitset[i])
		{
			it->t->Position=Vec3(0,0,0);
		}else if(it->t)
		{
			it->t->Position=Vector3ToVec3(it->getAxis(3));
			Quaternion q=it->body->getOrientation()*it->orientation;
			it->t->QUATERNION=D3DXQUATERNION(q.i,q.j,q.k,q.r);
		}
	}
}

void ComplexObject::DestroyPointedObject()
{
	propertiesIterator it=properties.begin();
	Vector3 temp(0,0,0);
	for(int i=0;it!=properties.end();++it,++i)
	{
		if(it->shape==PHYSICS->ObjectLookingAt)
		{
			alivebitset[i]=0;
			break;
		}
	}
	PHYSICS->ObjectLookingAt=NULL;
	
	//bill code here

	UpdateTheShape();

	//dont remove
	/*if(properties.size()==0)
	{
		this->GetOwner()->Destroy();
		return;
	}*/
}
void ComplexObject::UpdateTheShapeWhenTimePaused()
{
	Vector3 OldCOM=CenterOfMass;
	FindCOM();

	body->setPosition( body->getTransform().transform( CenterOfMass - OldCOM) );

	propertiesIterator it=properties.begin();
	for(;it!=properties.end();++it)
	{
		Matrix4 offset;
		offset.setOrientationAndPos(it->local_orientation,it->posToCOM);
		it->shape->offset=offset;
	}
}

void ComplexObject::UpdateTheShape()
{
	Vector3 OldCOM=CenterOfMass;
	FindCOM();

	body->setPosition( body->getTransform().transform( CenterOfMass - OldCOM) );

	propertiesIterator it=properties.begin();
	for(;it!=properties.end();++it)
	{
		Matrix4 offset;
		offset.setOrientationAndPos(it->local_orientation,it->posToCOM);
		it->shape->offset=offset;
	}
	
	Matrix3 tensor;
	propertiesIterator it1=properties.begin();
	for(;it1!=properties.end();++it1)
	{	
		if(!alivebitset[it1->index]) 
			continue;
		Matrix3 temp_tensor;
		temp_tensor.setBlockInertiaTensor(it1->halfSize,it1->mass);
		temp_tensor.moveFromCenterOfMass(it1->posToCOM,it1->mass);
		tensor+= temp_tensor;
	}
	
	body->setInertiaTensor(tensor);
	body->setMass(totalMass);
	body->calculateDerivedData();
	body->setAwake();
}


//inline int power(const int &i)
//{
//	int result =1;
//	for(int j=0;j<i;j++)
//	{
//		result*=2;
//	}
//	return result;
//}
//
//void ComplexObject::BuildCurrentState()
//{
//	currentState = 0;
//	for(int i=0;i<16;++i)
//	{
//		if(alive[i]>0)
//		{
//			currentState|= power(i);
//		}
//	}
//}
//
//void ComplexObject::BuildAliveArrayFromCurrentState()
//{
//	for(int i=0;i<16;++i)
//	{
//		/*alive[i]=0;
//		alive[i]|=
//		if(alive[i])
//		{
//			currentState|= power(i);
//		}*/
//	}
//}





}
