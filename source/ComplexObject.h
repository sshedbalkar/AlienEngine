#include "PhysicsMath.h"
#include "Composition.h"
#include "Engine.h"
#include "Collisions.h"
#include <bitset>

#ifndef COMPLEXOBJECT_H
#define COMPLEXOBJECT_H


namespace Framework
{

class singleObjectProperties
{
public:
	real mass;
	Vector3 local_position;
	Quaternion local_orientation;
	Vector3 posToCOM;
	Vector3 halfSize;
	std::string graphicsType;
	CollisionPrimitive *shape;
	int index;
	int connectedTo[16] ;
};

class ComplexObject : public GameComponent
{
public:
	ComplexObject();
	~ComplexObject();

	virtual void Initialize();
	virtual void Serialize(ISerializer& stream);
	virtual void SendMessage(Message *m);
	void DrawObject();
	void calculateInternals();
	void BuildData();
	void updateGraphicsObjects();
	int getAlive(const int& i){return alivebitset[i];}
	ObjectLinkList<CollisionPrimitive> collisionPrimitives;
	void UpdateTheShape();
	void UpdateTheShapeWhenTimePaused();
	//destroy code
	void DestroyPointedObject();
	//void BuildCurrentState();
	//void BuildAliveArrayFromCurrentState();
	//int * ReturnPointerToAliveArray(){return alive;};
	
	std::bitset<16> alivebitset;
	//long int currentState;
private:
	void FindCOM();
	Transform * transform;
	RigidBody * body;

	ComplexObject * Next;
	ComplexObject * Prev;

	Vector3 CenterOfMass;
	real totalMass;

	//int alive[16];

	std::list<singleObjectProperties> properties;
	typedef std::list<singleObjectProperties>::iterator propertiesIterator;

	typedef ObjectLinkList<CollisionPrimitive>::iterator CollisionPrimitiveIterator;
};

}

#endif