#include "PhysicsMath.h"
#include "Composition.h"
#include "Engine.h"

#ifndef BODY_H
#define BODY_H


namespace Framework
{

#define sleepEpsilon 0.3f


class RigidBody : public GameComponent
{
	friend class TweakBarManager;
	friend class HistoryComponent;
public:
	Vector3 prev_position;
    Quaternion prev_orientation;
protected:
	real inverseMass;
	real initialMass;
	real density;

	//given in the body space instead of the world space coordiantes
    Matrix3 inverseInertiaTensor;
    
	//damping variables
	real linearDamping;
	real angularDamping;

	//position and orientation
    Vector3 position;
    Vector3 initial_position;
    Quaternion orientation;

	
	real boundingVolumeRadius;

	//linear and angular velocities
    Vector3 velocity;
    Vector3 rotation;

	//inverse inertia tensor in world coordinates
    Matrix3 inverseInertiaTensorWorld;
	Matrix3 initialTensor;

	//transform matrix used to convert from local to world space and vice versa
	Matrix4 transformMatrix;

	//force accumulators
    Vector3 forceAccum;
    Vector3 torqueAccum;

	//acceleration that is constant on the body ex. gravity
    Vector3 acceleration;
	//the last frame acceleration that is used to update the body
    Vector3 lastFrameAcceleration;

	//kinetic energy
	real motion;

    bool isAwake;
	bool canSleep;
	bool isStatic;
	bool hasContraint;
	
public:
	bool allowSleep;
	bool canCollide;
	bool isColliding;//temp code
	bool canRotate;
	Vector3 initialAcceleration;

		RigidBody();
		~RigidBody();
		bool getIsStatic(){return isStatic;}
		void integrate(real dt);
		void calculateDerivedData();
		void RemoveComponentFromGame();
		void RestoreComponentToGame();

		real getBoundingVolumeRadius() const;

	//mass setter and getter functions
		void setIsStatic(bool t){isStatic=t;}
		void setMass(const real mass);
		real getMass() const;
		real getInitialMass() const {return initialMass;}
		bool getHasConstraint(){return hasContraint;}
		void setHasConstraint(bool t){hasContraint = t;}
		void setInverseMass(const real inverseMass);
		real getInverseMass() const;

		bool hasFiniteMass() const;

	//inertia tensor setter and getter functions
		void setInertiaTensor(const Matrix3 &inertiaTensor);
		void getInertiaTensor(Matrix3 *inertiaTensor) const;
		Matrix3 getInertiaTensor() const;
		Matrix3 getInitialInertiaTensor() const{return initialTensor;}
		
		void getInertiaTensorWorld(Matrix3 *inertiaTensor) const;
		Matrix3 getInertiaTensorWorld() const;

	//inverse inertia tensor setter and getter functions
		void setInverseInertiaTensor(const Matrix3 &inverseInertiaTensor);
		void getInverseInertiaTensor(Matrix3 *inverseInertiaTensor) const;
		Matrix3 getInverseInertiaTensor() const;

		void getInverseInertiaTensorWorld(Matrix3 *inverseInertiaTensor) const;
		Matrix3 getInverseInertiaTensorWorld() const;

	//damping setter and getter functions
		void setDamping(const real linearDamping, const real angularDamping);
        
		void setLinearDamping(const real linearDamping);
		real getLinearDamping() const;

		void setAngularDamping(const real angularDamping);
		real getAngularDamping() const;

	//position getter and setter functions
		void setPosition(const Vector3 &position);
		void setPosition(const real x, const real y, const real z);
		void getPosition(Vector3 *position) const;
		Vector3 getPosition() const;

	//orientation setter adn getter functions
		void setOrientation(const Quaternion &orientation);
		void setOrientation(const real r, const real i,const real j, const real k);
		void getOrientation(Quaternion *orientation) const;
		Quaternion getOrientation() const;
		void getOrientation(Matrix3 *matrix) const;
		void getOrientation(real matrix[9]) const;
	
	//transform setter adn getter functions
		void getTransform(Matrix4 *transform) const;
		void getTransform(real matrix[16]) const;
		void getGLTransform(float matrix[16]) const;
		Matrix4 getTransform() const;

		Vector3 getPointInLocalSpace(const Vector3 &point) const;
		Vector3 getPointInWorldSpace(const Vector3 &point) const;
		
		Vector3 getDirectionInLocalSpace(const Vector3 &direction) const;
		Vector3 getDirectionInWorldSpace(const Vector3 &direction) const;

	//velocity setter and getter fns.
        void setVelocity(const Vector3 &velocity);
        void setVelocity(const real x, const real y, const real z);
        void getVelocity(Vector3 *velocity) const;
        Vector3 getVelocity() const;
        void addVelocity(const Vector3 &deltaVelocity);

	//rotation setter and getter fns.
        void setRotation(const Vector3 &rotation);
        void setRotation(const real x, const real y, const real z);
        void getRotation(Vector3 *rotation) const;
        Vector3 getRotation() const;
        void addRotation(const Vector3 &deltaRotation);

		bool getAwake() const { return isAwake; }
        void setAwake(const bool awake=true);

		bool getCanSleep() const  { return canSleep;  }
        void setCanSleep(const bool canSleep=true);
	
	//dynamic quantities 
        void getLastFrameAcceleration(Vector3 *linearAcceleration) const;
        Vector3 getLastFrameAcceleration() const;

        void clearAccumulators();

        void addForce(const Vector3 &force);
		//in world space
        void addForceAtPoint(const Vector3 &force, const Vector3 &point);
        //in the body space
		void addForceAtBodyPoint(const Vector3 &force, const Vector3 &point);
		void addTorque(const Vector3 &torque);

	//acceleration i.e. the constant acceleration
		void setAcceleration(const Vector3 &acceleration);
        void setAcceleration(const real x, const real y, const real z);

		void setDensity(const real a){density=a;}

        void getAcceleration(Vector3 *acceleration) const;
        Vector3 getAcceleration() const;

public:

	virtual void Initialize();
	virtual void Serialize(ISerializer& stream);
	virtual void SendMessage(Message *m);
	
	void PublishResults();
	void editorInitialize();

	Transform * transform;

	RigidBody * Next;
	RigidBody * Prev;

	template<typename T>
	void swap(T&a,T&b)
	{
		T temp;
		temp=a;
		a=b;
		b=temp;
	}

	void makeObjectStatic();
	void ResetOriginalData();
};

}

#endif