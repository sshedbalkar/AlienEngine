#include "Contacts.h"
#include <queue>

#ifndef COLLISION_H
#define COLLISION_H


namespace Framework
{


class IntersectionTests;
class CollisionDetector;


class AABB
{
public:
	AABB(RigidBody * b):m_body(b){}
	Vector3 center;
	Vector3 halfSize;
	real min[2];//stores the min point on the selected axis, min[0] for X, //min[1] for Z
	real max[2];//stores the max point on the selected axis, max[0] for X, //max[1] for Z
	RigidBody* m_body;
};

class Ray
{
public:
	Ray(Vector3 pos,Vector3 dir):position(pos),direction(dir){ direction.normalise(); }
	Vector3 position;
	Vector3 direction;

	void normalizeDirection()
	{
		direction.normalise();
	}
};

class CollisionPrimitive : public GameComponent
{
public:
    friend IntersectionTests;
    friend CollisionDetector;

	enum ShapeId
	{
		Sphere,
		Box,
		Cylinder,
		Ellipsoid,
		Cone,
		NumberOfShapes
	};
	ShapeId Id;
	CollisionPrimitive():aabb(0){}
	CollisionPrimitive(ShapeId id) : Id(id),aabb(0){}
	~CollisionPrimitive(){ body=NULL; t=NULL; }

	RigidBody *body;
	Transform *t;
	
	//The offset of this primitive from the given rigid body. ??? neva used
	Matrix4 offset;
	Quaternion orientation;

	virtual void Initialize();
	virtual void Serialize(ISerializer& stream);
	virtual void SendMessage(Message *m);
	virtual void RemoveComponentFromGame();
	virtual void RestoreComponentToGame();

	virtual Vector3 Support(const Vector3 & dir){return Vector3();}
	virtual real getVolume(){return 0;}
	virtual Matrix3 getInettiaTensor(const real &mass){return Matrix3();}
	virtual real getBoundingRadius(){return 0;}
	virtual unsigned rayCast(const Ray &ray,Vector3 &pointOfIntersection){return 0;}

	CollisionPrimitive * Next;
	CollisionPrimitive * Prev;
	void calculateInternals();
	void calculateInternalsinEditor();
	
	Vector3 getAxis(unsigned index) const
    {
        return transform.getAxisVector(index);
    }

	const Matrix4& getTransform() const
    {
        return transform;
    }
	AABB *aabb;
protected:
	Matrix4 transform;
	

};

class CollisionSphere : public CollisionPrimitive
{
public:
	CollisionSphere() : CollisionPrimitive(Sphere){}
	virtual void Serialize(ISerializer& stream);
	virtual Vector3 Support(const Vector3 & dir);
	virtual real getVolume();
	virtual Matrix3 getInettiaTensor(const real &mass);
	virtual real getBoundingRadius(){return radius;}
	virtual unsigned rayCast(const Ray &ray,Vector3 &pointOfIntersection);
    real radius;
};

class CollisionBox : public CollisionPrimitive
{
public:
	CollisionBox():CollisionPrimitive(Box){}
	virtual void Serialize(ISerializer& stream);
	virtual Vector3 Support(const Vector3 & dir);
	virtual real getVolume();
	virtual Matrix3 getInettiaTensor(const real &mass);
	virtual real getBoundingRadius();
	virtual unsigned rayCast(const Ray &ray,Vector3 &pointOfIntersection);
    Vector3 halfSize;
};


class CollisionCylinder : public CollisionPrimitive
{
public:
	CollisionCylinder():CollisionPrimitive(Cylinder){}
	virtual void Serialize(ISerializer& stream);
	virtual Vector3 Support(const Vector3 & dir);
	virtual real getVolume();
	virtual Matrix3 getInettiaTensor(const real &mass);
	virtual real getBoundingRadius();
	virtual unsigned rayCast(const Ray &ray,Vector3 &pointOfIntersection);
    real radius;
	real height;
};

class CollisionCone : public CollisionPrimitive
{
public:
	CollisionCone():CollisionPrimitive(Cone){}
	virtual void Serialize(ISerializer& stream);
	virtual Vector3 Support(const Vector3 & dir);
	virtual real getVolume();
	virtual Matrix3 getInettiaTensor(const real &mass);
	virtual real getBoundingRadius();
	virtual unsigned rayCast(const Ray &ray,Vector3 &pointOfIntersection);
	void CalculateSinTheta();
    real radius;
	real height;
private:
	real sinTheta;
};

class CollisionEllipsoid : public CollisionPrimitive
{
public:
	CollisionEllipsoid():CollisionPrimitive(Ellipsoid){}
	virtual void Serialize(ISerializer& stream);
	virtual Vector3 Support(const Vector3 & dir);
	virtual real getVolume();
	virtual Matrix3 getInettiaTensor(const real &mass);
	virtual real getBoundingRadius();
	virtual unsigned rayCast(const Ray &ray,Vector3 &pointOfIntersection);
    Vector3 halfSize;
};

class CollisionPlane
{
public:
    Vector3 direction;
	//offset from the origin
	real offset;
};




class IntersectionTests
{
public:

    static bool sphereAndHalfSpace(const CollisionSphere &sphere, const CollisionPlane &plane);
	static bool sphereAndSphere(const CollisionSphere &one, const CollisionSphere &two);
	static bool sphereAndBox(const CollisionSphere &one, const CollisionBox &two);
	static bool boxAndBox(const CollisionBox &one, const CollisionBox &two);
	//box and plane that is only one sided 
    static bool boxAndHalfSpace(const CollisionBox &box, const CollisionPlane &plane);
    static bool boxAndPoint(const CollisionBox &box,const Vector3 &point);
};


//structure used to build the contact data
struct CollisionData
{
    Contact *contactArray;

    /** Holds the contact array to write into. */
    Contact *contacts;

    int contactsLeft;
    unsigned contactCount;

	//update these two to use the material properties later
    real friction;
    real restitution;

    /**
    * Holds the collision tolerance, even uncolliding objects this
    * close should have collisions generated.
    */
    real tolerance;

    bool hasMoreContacts()
    {
        return contactsLeft > 0;
    }

    void reset(unsigned maxContacts)
    {
        contactsLeft = maxContacts;
        contactCount = 0;
        contacts = contactArray;
    }

    void addContacts(unsigned count)
    {
        // Reduce the number of contacts remaining, add number used
        contactsLeft -= count;
        contactCount += count;

        // Move the array forward
        contacts += count;
    }
};

class Triangle
{
public:

	bool ConstructTriangle(Vector3 &A,Vector3 &B,Vector3 &C,
						   Vector3 &AS1,Vector3 &BS1,Vector3 &CS1,
						   Vector3 &AS2,Vector3 &BS2,Vector3 &CS2);

	Vector3 A,B,C;
	Vector3 v;//closest point to the origin
	real L1,L2,L3; // v = A*L1+BL2+C*L3
	real vSquaredDistance;
	Vector3 SupportA1,SupportB1,SupportC1;
	Vector3 SupportA2,SupportB2,SupportC2;
};


class TriangleComparision
{
public:
  bool operator() (const Triangle& lhs, const Triangle&rhs) const
  {
	  return (lhs.vSquaredDistance>rhs.vSquaredDistance);
  }
};


class CollisionDetector
{
public:

    static unsigned sphereAndHalfSpace(const CollisionSphere &sphere,const CollisionPlane &plane,CollisionData *data);
	static unsigned sphereAndTruePlane(const CollisionSphere &sphere,const CollisionPlane &plane,CollisionData *data);
    static unsigned boxAndHalfSpace(const CollisionBox &box,const CollisionPlane &plane,CollisionData *data);

    static unsigned sphereAndSphere(const CollisionSphere &one,const CollisionSphere &two,CollisionData *data);
    static unsigned boxAndBox(const CollisionBox &one,const CollisionBox &two,CollisionData *data);
    static unsigned boxAndPoint(const CollisionBox &box,const Vector3 &point,CollisionData *data);
    static unsigned boxAndSphere(const CollisionBox &box,const CollisionSphere &sphere,CollisionData *data);

	static unsigned rayCastSphere(const CollisionSphere &sphere,const Ray &ray,Vector3 &pointOfIntersection);
	static unsigned rayCastBox(const CollisionBox &box,const Ray &ray,Vector3 &pointOfIntersection);
	
	unsigned GJKTest(CollisionPrimitive *b1,CollisionPrimitive *b2,CollisionData *data);
	unsigned MPRTest(CollisionPrimitive *b1,CollisionPrimitive *b2,CollisionData *data);

private:

	std::vector<Vector3> simplex;
	std::vector<Vector3> SupportA;
	std::vector<Vector3> SupportB;

	std::priority_queue<Triangle,std::vector<Triangle>,TriangleComparision> triangles;

	Vector3 dir;

	bool updateSimplex();
	void constructTetrahedron();
	unsigned EPA(CollisionPrimitive *b1,CollisionPrimitive *b2,CollisionData *data);
	void Expand(CollisionPrimitive *b1,CollisionPrimitive *b2,Triangle &,Vector3 &,Vector3 &,Vector3 &);
};


}

#endif