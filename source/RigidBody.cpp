#include "precompiled.h"
#include "RigidBody.h"
#include "Physics.h"
#include "GameLogic.h"
#include "BoomBoom.h"

namespace Framework
{
//helper functions

//this basically does the change of basis and calculates the inverse inertia tensor in the world space
//i.e it does the change of basis
//this is an optimized code
//the actual formula is  [transform * inverse inertia * transform^(-1)]
static inline void _transformInertiaTensor(Matrix3 &iitWorld, const Quaternion &q, const Matrix3 &iitBody, const Matrix4 &rotmat)
{
    real t4 = rotmat.data[0]*iitBody.data[0]+
        rotmat.data[1]*iitBody.data[3]+
        rotmat.data[2]*iitBody.data[6];
    real t9 = rotmat.data[0]*iitBody.data[1]+
        rotmat.data[1]*iitBody.data[4]+
        rotmat.data[2]*iitBody.data[7];
    real t14 = rotmat.data[0]*iitBody.data[2]+
        rotmat.data[1]*iitBody.data[5]+
        rotmat.data[2]*iitBody.data[8];
    real t28 = rotmat.data[4]*iitBody.data[0]+
        rotmat.data[5]*iitBody.data[3]+
        rotmat.data[6]*iitBody.data[6];
    real t33 = rotmat.data[4]*iitBody.data[1]+
        rotmat.data[5]*iitBody.data[4]+
        rotmat.data[6]*iitBody.data[7];
    real t38 = rotmat.data[4]*iitBody.data[2]+
        rotmat.data[5]*iitBody.data[5]+
        rotmat.data[6]*iitBody.data[8];
    real t52 = rotmat.data[8]*iitBody.data[0]+
        rotmat.data[9]*iitBody.data[3]+
        rotmat.data[10]*iitBody.data[6];
    real t57 = rotmat.data[8]*iitBody.data[1]+
        rotmat.data[9]*iitBody.data[4]+
        rotmat.data[10]*iitBody.data[7];
    real t62 = rotmat.data[8]*iitBody.data[2]+
        rotmat.data[9]*iitBody.data[5]+
        rotmat.data[10]*iitBody.data[8];

    iitWorld.data[0] = t4*rotmat.data[0]+
        t9*rotmat.data[1]+
        t14*rotmat.data[2];
    iitWorld.data[1] = t4*rotmat.data[4]+
        t9*rotmat.data[5]+
        t14*rotmat.data[6];
    iitWorld.data[2] = t4*rotmat.data[8]+
        t9*rotmat.data[9]+
        t14*rotmat.data[10];
    iitWorld.data[3] = t28*rotmat.data[0]+
        t33*rotmat.data[1]+
        t38*rotmat.data[2];
    iitWorld.data[4] = t28*rotmat.data[4]+
        t33*rotmat.data[5]+
        t38*rotmat.data[6];
    iitWorld.data[5] = t28*rotmat.data[8]+
        t33*rotmat.data[9]+
        t38*rotmat.data[10];
    iitWorld.data[6] = t52*rotmat.data[0]+
        t57*rotmat.data[1]+
        t62*rotmat.data[2];
    iitWorld.data[7] = t52*rotmat.data[4]+
        t57*rotmat.data[5]+
        t62*rotmat.data[6];
    iitWorld.data[8] = t52*rotmat.data[8]+
        t57*rotmat.data[9]+
        t62*rotmat.data[10];
}

//calculates the transform matrix from the orientation and position
static inline void _calculateTransformMatrix(Matrix4 &transformMatrix,const Vector3 &position,const Quaternion &orientation)
{
    transformMatrix.data[0] = 1-2*orientation.j*orientation.j-
        2*orientation.k*orientation.k;
    transformMatrix.data[1] = 2*orientation.i*orientation.j -
        2*orientation.r*orientation.k;
    transformMatrix.data[2] = 2*orientation.i*orientation.k +
        2*orientation.r*orientation.j;
    transformMatrix.data[3] = position.x;

    transformMatrix.data[4] = 2*orientation.i*orientation.j +
        2*orientation.r*orientation.k;
    transformMatrix.data[5] = 1-2*orientation.i*orientation.i-
        2*orientation.k*orientation.k;
    transformMatrix.data[6] = 2*orientation.j*orientation.k -
        2*orientation.r*orientation.i;
    transformMatrix.data[7] = position.y;

    transformMatrix.data[8] = 2*orientation.i*orientation.k -
        2*orientation.r*orientation.j;
    transformMatrix.data[9] = 2*orientation.j*orientation.k +
        2*orientation.r*orientation.i;
    transformMatrix.data[10] = 1-2*orientation.i*orientation.i-
        2*orientation.j*orientation.j;
    transformMatrix.data[11] = position.z;
}


//Rigid Body functions

void RigidBody::calculateDerivedData()
{
    orientation.normalise();

    // Calculate the transform matrix for the body.
    _calculateTransformMatrix(transformMatrix, position, orientation);

    // Calculate the inertiaTensor in world space.
    _transformInertiaTensor(inverseInertiaTensorWorld, orientation, inverseInertiaTensor, transformMatrix);
}

//integrate for the rigid body
void RigidBody::integrate(real duration)
{
	if (!isAwake || isStatic) return;
	prev_position=position;
	prev_orientation=orientation;

    // Calculate linear acceleration from force inputs.
    lastFrameAcceleration = acceleration;
    lastFrameAcceleration.addScaledVector(forceAccum, inverseMass);

    // Calculate angular acceleration from torque inputs.
	//w = I^(-1) * T
    Vector3 angularAcceleration = inverseInertiaTensorWorld.transform(torqueAccum);

    // v=u+at
    velocity.addScaledVector(lastFrameAcceleration, duration);
	rotation.addScaledVector(angularAcceleration, duration);

    // Impose drag.
    velocity *= real_pow(linearDamping, duration);
    rotation *= real_pow(angularDamping, duration);

    // S=s + vt (+1/2at^2)
    position.addScaledVector(velocity, duration);
	if(canRotate)
		orientation.addScaledVector(rotation, duration);

    //update matrices with new position and orientation
	calculateDerivedData();

    // Clear accumulators.
    clearAccumulators();

    // Update the kinetic energy store, and possibly put the body to
    // sleep.
    if (canSleep) 
	{
		//kinetic energy hack
		real currentMotion = velocity.dotProduct(velocity) + rotation.dotProduct(rotation);
        real bias = real_pow(0.5, duration);
        motion = bias*motion + (1-bias)*currentMotion;

        if (motion < sleepEpsilon) 
			setAwake(false);
        else if (motion > 10 * sleepEpsilon) 
			motion = 10 * sleepEpsilon;
	
    }
}


void RigidBody::setMass(const real mass)
{
    if(mass == 0)
		inverseMass=0.0f;
	else
		inverseMass = ((real)1.0)/mass;
}

real RigidBody::getMass() const
{
    if (inverseMass == 0) 
        return inverseMass;
    else
        return ((real)1.0)/inverseMass;
}

void RigidBody::setInverseMass(const real inverseMass)
{
    RigidBody::inverseMass = inverseMass;
}

real RigidBody::getInverseMass() const
{
    return inverseMass;
}

bool RigidBody::hasFiniteMass() const
{
    return inverseMass > 0.0f;
}

void RigidBody::setInertiaTensor(const Matrix3 &inertiaTensor)
{
    inverseInertiaTensor.setInverse(inertiaTensor);
}

void RigidBody::getInertiaTensor(Matrix3 *inertiaTensor) const
{
    inertiaTensor->setInverse(inverseInertiaTensor);
}

Matrix3 RigidBody::getInertiaTensor() const
{
    Matrix3 it;
    getInertiaTensor(&it);
    return it;
}

void RigidBody::getInertiaTensorWorld(Matrix3 *inertiaTensor) const
{
    inertiaTensor->setInverse(inverseInertiaTensorWorld);
}

Matrix3 RigidBody::getInertiaTensorWorld() const
{
    Matrix3 it;
    getInertiaTensorWorld(&it);
    return it;
}

void RigidBody::setInverseInertiaTensor(const Matrix3 &inverseInertiaTensor)
{
    RigidBody::inverseInertiaTensor = inverseInertiaTensor;
}

void RigidBody::getInverseInertiaTensor(Matrix3 *inverseInertiaTensor) const
{
    *inverseInertiaTensor = RigidBody::inverseInertiaTensor;
}

Matrix3 RigidBody::getInverseInertiaTensor() const
{
    return inverseInertiaTensor;
}

void RigidBody::getInverseInertiaTensorWorld(Matrix3 *inverseInertiaTensor) const
{
    *inverseInertiaTensor = inverseInertiaTensorWorld;
}

Matrix3 RigidBody::getInverseInertiaTensorWorld() const
{
    return inverseInertiaTensorWorld;
}

void RigidBody::setDamping(const real linearDamping, const real angularDamping)
{
    RigidBody::linearDamping = linearDamping;
    RigidBody::angularDamping = angularDamping;
}

void RigidBody::setLinearDamping(const real linearDamping)
{
    RigidBody::linearDamping = linearDamping;
}

real RigidBody::getLinearDamping() const
{
    return linearDamping;
}

void RigidBody::setAngularDamping(const real angularDamping)
{
    RigidBody::angularDamping = angularDamping;
}

real RigidBody::getAngularDamping() const
{
    return angularDamping;
}

void RigidBody::setPosition(const Vector3 &position)
{
    RigidBody::position = position;
}

void RigidBody::setPosition(const real x, const real y, const real z)
{
    position.x = x;
    position.y = y;
    position.z = z;
}

void RigidBody::getPosition(Vector3 *position) const
{
    *position = RigidBody::position;
}

Vector3 RigidBody::getPosition() const
{
    return position;
}

void RigidBody::setOrientation(const Quaternion &orientation)
{
    RigidBody::orientation = orientation;
    RigidBody::orientation.normalise();
}

void RigidBody::setOrientation(const real r, const real i,
                   const real j, const real k)
{
    orientation.r = r;
    orientation.i = i;
    orientation.j = j;
    orientation.k = k;
    orientation.normalise();
}

void RigidBody::getOrientation(Quaternion *orientation) const
{
    *orientation = RigidBody::orientation;
}

Quaternion RigidBody::getOrientation() const
{
    return orientation;
}

void RigidBody::getOrientation(Matrix3 *matrix) const
{
    getOrientation(matrix->data);
}

void RigidBody::getOrientation(real matrix[9]) const
{
    matrix[0] = transformMatrix.data[0];
    matrix[1] = transformMatrix.data[1];
    matrix[2] = transformMatrix.data[2];

    matrix[3] = transformMatrix.data[4];
    matrix[4] = transformMatrix.data[5];
    matrix[5] = transformMatrix.data[6];

    matrix[6] = transformMatrix.data[8];
    matrix[7] = transformMatrix.data[9];
    matrix[8] = transformMatrix.data[10];
}

void RigidBody::getTransform(Matrix4 *transform) const
{
    memcpy(transform, &transformMatrix.data, sizeof(Matrix4));
}

void RigidBody::getTransform(real matrix[16]) const
{
    memcpy(matrix, transformMatrix.data, sizeof(real)*12);
    matrix[12] = matrix[13] = matrix[14] = 0;
    matrix[15] = 1;
}

void RigidBody::getGLTransform(float matrix[16]) const
{
    matrix[0] = (float)transformMatrix.data[0];
    matrix[1] = (float)transformMatrix.data[4];
    matrix[2] = (float)transformMatrix.data[8];
    matrix[3] = 0;

    matrix[4] = (float)transformMatrix.data[1];
    matrix[5] = (float)transformMatrix.data[5];
    matrix[6] = (float)transformMatrix.data[9];
    matrix[7] = 0;

    matrix[8] = (float)transformMatrix.data[2];
    matrix[9] = (float)transformMatrix.data[6];
    matrix[10] = (float)transformMatrix.data[10];
    matrix[11] = 0;

    matrix[12] = (float)transformMatrix.data[3];
    matrix[13] = (float)transformMatrix.data[7];
    matrix[14] = (float)transformMatrix.data[11];
    matrix[15] = 1;
}

Matrix4 RigidBody::getTransform() const
{
    return transformMatrix;
}

Vector3 RigidBody::getPointInLocalSpace(const Vector3 &point) const
{
    return transformMatrix.transformInverse(point);
}

Vector3 RigidBody::getPointInWorldSpace(const Vector3 &point) const
{
    return transformMatrix.transform(point);
}

Vector3 RigidBody::getDirectionInLocalSpace(const Vector3 &direction) const
{
    return transformMatrix.transformInverseDirection(direction);
}

Vector3 RigidBody::getDirectionInWorldSpace(const Vector3 &direction) const
{
    return transformMatrix.transformDirection(direction);
}

void RigidBody::setVelocity(const Vector3 &velocity)
{
    RigidBody::velocity = velocity;
}

void RigidBody::setVelocity(const real x, const real y, const real z)
{
    velocity.x = x;
    velocity.y = y;
    velocity.z = z;
}

void RigidBody::getVelocity(Vector3 *velocity) const
{
    *velocity = RigidBody::velocity;
}

Vector3 RigidBody::getVelocity() const
{
    return velocity;
}

void RigidBody::addVelocity(const Vector3 &deltaVelocity)
{
    velocity += deltaVelocity;
}

void RigidBody::setRotation(const Vector3 &rotation)
{
    RigidBody::rotation = rotation;
}

void RigidBody::setRotation(const real x, const real y, const real z)
{
    rotation.x = x;
    rotation.y = y;
    rotation.z = z;
}

void RigidBody::getRotation(Vector3 *rotation) const
{
    *rotation = RigidBody::rotation;
}

Vector3 RigidBody::getRotation() const
{
    return rotation;
}

void RigidBody::addRotation(const Vector3 &deltaRotation)
{
    rotation += deltaRotation;
}


void RigidBody::setAwake(const bool awake)
{
	if(!allowSleep) return;
	if (awake) {
        isAwake= true;

        // Add a bit of motion to avoid it falling asleep immediately.
        motion = sleepEpsilon*2.0f;
    } else {
        isAwake = false;
        velocity.clear();
        rotation.clear();
    }
}

void RigidBody::setCanSleep(const bool canSleep)
{
    RigidBody::canSleep = canSleep;

    if (!canSleep && !isAwake) setAwake();
}

void RigidBody::getLastFrameAcceleration(Vector3 *acceleration) const
{
    *acceleration = lastFrameAcceleration;
}

Vector3 RigidBody::getLastFrameAcceleration() const
{
    return lastFrameAcceleration;
}

void RigidBody::clearAccumulators()
{
    forceAccum.clear();
    torqueAccum.clear();
}

void RigidBody::addForce(const Vector3 &force)
{
    forceAccum += force;
    isAwake = true;
}

void RigidBody::addForceAtBodyPoint(const Vector3 &force, const Vector3 &point)
{
    // Convert to coordinates relative to center of mass.
    Vector3 pt = getPointInWorldSpace(point);
    addForceAtPoint(force, pt);

    isAwake = true;
}

void RigidBody::addForceAtPoint(const Vector3 &force, const Vector3 &point)
{
    // Convert to coordinates relative to center of mass.
    Vector3 pt = point;
    pt -= position;

    forceAccum += force;
    torqueAccum += pt % force;//cross product

    isAwake = true;
}

void RigidBody::addTorque(const Vector3 &torque)
{
    torqueAccum += torque;
    isAwake = true;
}

void RigidBody::setAcceleration(const Vector3 &acceleration)
{
    RigidBody::acceleration = acceleration;
}

void RigidBody::setAcceleration(const real x, const real y, const real z)
{
    acceleration.x = x;
    acceleration.y = y;
    acceleration.z = z;
}

void RigidBody::getAcceleration(Vector3 *acceleration) const
{
    *acceleration = RigidBody::acceleration;
}

Vector3 RigidBody::getAcceleration() const
{
    return acceleration;
}

real RigidBody::getBoundingVolumeRadius() const
{
	return boundingVolumeRadius;
}

RigidBody::RigidBody()
{
	isColliding=false;//temp code
	canCollide=true;
	allowSleep=true;
	hasContraint=false;
}


RigidBody::~RigidBody()
{
	if(!this->GetOwner()->OnRemovedList)
	{
		if(initialMass!=0.0)
			PHYSICS->RigidBodies.erase(this);
		else
			PHYSICS->LevelData.erase(this);

	}
	
}

void RigidBody::RemoveComponentFromGame()
{
	if(initialMass!=0.0)
		PHYSICS->RigidBodies.erase(this);
	else
		PHYSICS->LevelData.erase(this);
}

void RigidBody::RestoreComponentToGame()
{
	if(initialMass!=0.0)
	{
		PHYSICS->RigidBodies.push_back(this);
		makeObjectStatic();
	}
	else
		PHYSICS->LevelData.push_back(this);
}

void RigidBody::editorInitialize()
{
	position = Vector3(transform->Position.x,transform->Position.y,transform->Position.z);
	orientation = Quaternion(transform->QUATERNION.w,transform->QUATERNION.x,transform->QUATERNION.y,transform->QUATERNION.z);

	CollisionPrimitive* collisionprimitive = GetOwner()->has(CollisionPrimitive);
	real m = collisionprimitive->getVolume()*density;
	setMass(m);
	Matrix3 tensor = collisionprimitive->getInettiaTensor(m);
	setInertiaTensor(tensor);

	velocity=rotation=Vector3(0,0,0);
	clearAccumulators();
	calculateDerivedData();
}

void RigidBody::Initialize()
{
	
	transform = GetOwner()->has(Transform);
	position = Vector3(transform->Position.x,transform->Position.y,transform->Position.z);
	initial_position=position;
	orientation = Quaternion(transform->QUATERNION.w,transform->QUATERNION.x,transform->QUATERNION.y,transform->QUATERNION.z);
	if(GetOwner()->has(ComplexObject))
	{
		PHYSICS->RigidBodies.push_back(this);
		isStatic = false;
		initialMass=getMass();
		initialTensor=inverseInertiaTensor.inverse();
		return;
	}
	ErrorIf(!GetOwner()->has(CollisionPrimitive), "A Body without shape");

	CollisionPrimitive* collisionprimitive = GetOwner()->has(CollisionPrimitive);
	real m = collisionprimitive->getVolume()*density;
	setMass(m);
	initialMass=m;
	Matrix3 tensor = collisionprimitive->getInettiaTensor(m);
	setInertiaTensor(tensor);
	initialTensor=tensor;
	boundingVolumeRadius = collisionprimitive->getBoundingRadius();

	clearAccumulators();
	calculateDerivedData();

	if(hasFiniteMass())
	{
		PHYSICS->RigidBodies.push_back(this);
	}
	else
	{
		PHYSICS->LevelData.push_back(this);
	}

	if(hasFiniteMass())
	{	
		isStatic = false;
	}
	else
	{
		isStatic = true;
	}
}



void RigidBody::Serialize(ISerializer& stream)
{
	ISerializer* str = stream.clone();
	if(str->isGood())
	{
		float temp;
		float tempVec[3]={0.0,0.0,0.0};
		int tempint;

		str->readValue("density", temp);
			density=(real)temp;
		str->readValue("acceleration",tempVec,3);
			acceleration=Vector3(tempVec[0],tempVec[1],tempVec[2]);
		str->readValue("linearDamping",temp);
			setLinearDamping((real)temp);
		str->readValue("angularDamping",temp);
			setAngularDamping((real)temp);
		str->readValue("canSleep",tempint);
			if(tempint==1)
				setCanSleep(true);
			else
				setCanSleep(false);
		str->readValue("isSleeping",tempint);
			if(tempint==0)
				setAwake();
			else
				setAwake(false);
		if(str->readValue("canRotate",tempint))
			if(tempint==0)
				canRotate=false;
			else
				canRotate=true;
		else
			canRotate=true;
		initialAcceleration=acceleration;

	}
	delete str;
}


void RigidBody::PublishResults()
{
	transform->Position=Vec3(position.x,position.y,position.z);
	transform->QUATERNION=D3DXQUATERNION(orientation.i,orientation.j,orientation.k,orientation.r);

	ComplexObject* temp=GetOwner()->has(ComplexObject);
	if(temp)
	{
		temp->updateGraphicsObjects();
	}
}

void RigidBody::SendMessage(Message *m)
{

	switch( m->MessageId )
	{
		case Mid::Collide:
		{
			if(this->GetOwner()->GlobalId==5)
			MessageCollide * msg = (MessageCollide*)m;
			isColliding=true;
			LOGIC->SendMessageA(m);
			break;
		}				
	}
}


void RigidBody::makeObjectStatic()
{
	setMass(0);
	Matrix3 tensor;
	setInertiaTensor(tensor);
	clearAccumulators();
	calculateDerivedData();
	isStatic = true;
}

void RigidBody::ResetOriginalData()
{
	setMass(initialMass);
	setInertiaTensor(initialTensor);
	clearAccumulators();
	calculateDerivedData();
	isStatic = false;
}



}
