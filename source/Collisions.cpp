#include "precompiled.h"
#include "Collisions.h"
#include "Physics.h"

namespace Framework
{

bool IntersectionTests::sphereAndHalfSpace(const CollisionSphere &sphere,const CollisionPlane &plane)
{
    // Find the distance from the origin
    real ballDistance = plane.direction * sphere.getAxis(3) - sphere.radius;
	// Check for the intersection
    return (ballDistance <= plane.offset);
}

bool IntersectionTests::sphereAndSphere(const CollisionSphere &one, const CollisionSphere &two)
{
    Vector3 midline = one.getAxis(3) - two.getAxis(3);
    return ( midline.squareMagnitude() < (one.radius+two.radius)*(one.radius+two.radius) );
}

bool IntersectionTests::boxAndPoint(const CollisionBox &box,const Vector3 &point)
{
	 // Transform the point into box coordinates
    Vector3 relPt = box.transform.transformInverse(point);

    // Check each axis, looking for the axis on which the
    // penetration is least deep.
    real min_depth = box.halfSize.x - real_abs(relPt.x);
    if (min_depth < 0) return false;

    real depth = box.halfSize.y - real_abs(relPt.y);
    if (depth < 0) return false;

    depth = box.halfSize.z - real_abs(relPt.z);
    if (depth < 0) return false;

	return true;
}

bool IntersectionTests::sphereAndBox(const CollisionSphere &sphere, const CollisionBox &box)
{
    // Transform the centre of the sphere into box coordinates
    Vector3 centre = sphere.t->Position;
    Vector3 relCentre = box.transform.transformInverse(centre);

    // Early out check to see if we can exclude the contact
    if (real_abs(relCentre.x) - sphere.radius > box.halfSize.x ||
        real_abs(relCentre.y) - sphere.radius > box.halfSize.y ||
        real_abs(relCentre.z) - sphere.radius > box.halfSize.z)
    {
        return false;
    }

    Vector3 closestPt(0,0,0);
    real dist;

    // Clamp each coordinate to the box.
    dist = relCentre.x;
    if (dist > box.halfSize.x) dist = box.halfSize.x;
    if (dist < -box.halfSize.x) dist = -box.halfSize.x;
    closestPt.x = dist;

    dist = relCentre.y;
    if (dist > box.halfSize.y) dist = box.halfSize.y;
    if (dist < -box.halfSize.y) dist = -box.halfSize.y;
    closestPt.y = dist;

    dist = relCentre.z;
    if (dist > box.halfSize.z) dist = box.halfSize.z;
    if (dist < -box.halfSize.z) dist = -box.halfSize.z;
    closestPt.z = dist;

    // Check we're in contact
    dist = (closestPt - relCentre).squareMagnitude();
    if (dist > sphere.radius * sphere.radius) return false;

	return true;
}
//calculate the half-length of the box on the given axis
static inline real transformToAxis(const CollisionBox &box, const Vector3 &axis)
{
    return
        box.halfSize.x * real_abs(axis * box.getAxis(0)) +
        box.halfSize.y * real_abs(axis * box.getAxis(1)) +
        box.halfSize.z * real_abs(axis * box.getAxis(2));
}

//check if the boxes overlap on a given axis
//toCentre id the vector from box a to b
static inline bool overlapOnAxis(const CollisionBox &one,const CollisionBox &two,Vector3 &axis,const Vector3 &toCentre)
{
	if (axis.squareMagnitude() < 0.0001) return true;
    axis.normalise();

    // Project the half-size of one onto axis
    real oneProject = transformToAxis(one, axis);
    real twoProject = transformToAxis(two, axis);

    // Project this onto the axis
    real distance = real_abs(toCentre * axis);

    // Check for overlap
    return (distance < oneProject + twoProject);
}

// This preprocessor definition is only used as a convenience
// in the boxAndBox intersection  method.
#define TEST_OVERLAP(axis) overlapOnAxis(one, two, (axis), toCentre)
	bool IntersectionTests::boxAndBox(const CollisionBox &one, const CollisionBox &two)
	{
		// Find the vector between the two centres
		Vector3 toCentre = two.getAxis(3) - one.getAxis(3);

		return (
			// Check on box one's axes first
			TEST_OVERLAP(one.getAxis(0)) &&
			TEST_OVERLAP(one.getAxis(1)) &&
			TEST_OVERLAP(one.getAxis(2)) &&

			// And on two's
			TEST_OVERLAP(two.getAxis(0)) &&
			TEST_OVERLAP(two.getAxis(1)) &&
			TEST_OVERLAP(two.getAxis(2)) &&

			// Now on the cross products
			TEST_OVERLAP(one.getAxis(0) % two.getAxis(0)) &&
			TEST_OVERLAP(one.getAxis(0) % two.getAxis(1)) &&
			TEST_OVERLAP(one.getAxis(0) % two.getAxis(2)) &&
			TEST_OVERLAP(one.getAxis(1) % two.getAxis(0)) &&
			TEST_OVERLAP(one.getAxis(1) % two.getAxis(1)) &&
			TEST_OVERLAP(one.getAxis(1) % two.getAxis(2)) &&
			TEST_OVERLAP(one.getAxis(2) % two.getAxis(0)) &&
			TEST_OVERLAP(one.getAxis(2) % two.getAxis(1)) &&
			TEST_OVERLAP(one.getAxis(2) % two.getAxis(2))
		);
	}
#undef TEST_OVERLAP


bool IntersectionTests::boxAndHalfSpace(const CollisionBox &box, const CollisionPlane &plane)
{
    // Work out the projected radius of the box onto the plane direction
    real projectedRadius = transformToAxis(box, plane.direction);
    real boxDistance = plane.direction * box.getAxis(3) - projectedRadius;
	return boxDistance <= plane.offset;
}


//collision detector functions

unsigned CollisionDetector::sphereAndTruePlane(const CollisionSphere &sphere, const CollisionPlane &plane, CollisionData *data)
{
    // Make sure we have contacts
    if (data->contactsLeft <= 0) return 0;

    // sphere position
    Vector3 position = sphere.getAxis(3);

    // Find the distance from the plane
    real centreDistance = plane.direction * position - plane.offset;

    // Check if we're within radius
    if (centreDistance*centreDistance > sphere.radius*sphere.radius)
    {
        return 0;
    }

    // Check which side of the plane we're on
    Vector3 normal = plane.direction;
    real penetration = -centreDistance;
    if (centreDistance < 0)
    {
        normal *= -1;
        penetration = -penetration;
    }
    penetration += sphere.radius;

    // Create the contact - it has a normal in the plane direction.
    Contact* contact = data->contacts;
    contact->contactNormal = normal;
    contact->penetration = penetration;
    contact->contactPoint = position - plane.direction * centreDistance;
    contact->setBodyData(sphere.body, NULL, data->friction, data->restitution);

    data->addContacts(1);
    return 1;
}


unsigned CollisionDetector::sphereAndHalfSpace(const CollisionSphere &sphere, const CollisionPlane &plane, CollisionData *data)
{
    // Make sure we have contacts
    if (data->contactsLeft <= 0) return 0;

    // sphere position
    Vector3 position = sphere.getAxis(3);

    // penetration
    real penetration = plane.direction * position -sphere.radius - plane.offset;

    if (penetration >= 0) return 0;

    // Create the contact - it has a normal in the plane direction.
    Contact* contact = data->contacts;
    contact->contactNormal = plane.direction;
    contact->penetration = -penetration;
    contact->contactPoint = position - plane.direction * (penetration + sphere.radius);
    contact->setBodyData(sphere.body, NULL, data->friction, data->restitution);

    data->addContacts(1);
    return 1;
}


unsigned CollisionDetector::sphereAndSphere(const CollisionSphere &one, const CollisionSphere &two, CollisionData *data)
{
    // Make sure we have contacts
    if (data->contactsLeft <= 0) return 0;

    // sphere positions
    Vector3 positionOne = one.getAxis(3);
    Vector3 positionTwo = two.getAxis(3);

    // Find the vector between the objects
    Vector3 midline = positionOne - positionTwo;
    real size = midline.magnitude();

    // See if it is large enough.
    if (size <= 0.0f || size >= one.radius+two.radius)
		return 0;
    
	// calculate the normal from 2 to 1
    Vector3 normal = midline * (((real)1.0)/size);

    Contact* contact = data->contacts;
    contact->contactNormal = normal;
    contact->contactPoint = positionOne + midline * (real)0.5;
    contact->penetration = (one.radius+two.radius - size);
    //if(two.body->hasFiniteMass() && one.body->hasFiniteMass())
	contact->setBodyData(one.body, two.body, data->friction, data->restitution);
	/*else
	{
		if(!two.body->hasFiniteMass())
			contact->setBodyData(one.body, NULL, data->friction, data->restitution);
		else
			contact->setBodyData(NULL, two.body, data->friction, data->restitution);
	}*/
	//contact->setBodyData(one.body, two.body, data->friction, data->restitution);

    data->addContacts(1);
    return 1;
}

//returns the ammount of overlap on the given axis
static inline real penetrationOnAxis(const CollisionBox &one, const CollisionBox &two, const Vector3 &axis, const Vector3 &toCentre)
{
    real oneProject = transformToAxis(one, axis);
    real twoProject = transformToAxis(two, axis);
    
	real distance = real_abs(toCentre * axis);

    // Return the overlap (i.e. positive indicates overlap, negative indicates separation).
    return (oneProject + twoProject - distance);
}

//smallest penetration and smallest case need to be updated if they overlap on the given axis
static inline bool tryAxis(const CollisionBox &one, const CollisionBox &two, Vector3& axis, const Vector3& toCentre, unsigned index, real& smallestPenetration, unsigned &smallestCase)
{
    // Make sure we have a normalized axis, and don't check almost parallel axes
    if (axis.squareMagnitude() < 0.0001) return true;
    axis.normalise();

    real penetration = penetrationOnAxis(one, two, axis, toCentre);

    if (penetration < 0) return false;
    if (penetration < smallestPenetration) 
	{
        smallestPenetration = penetration;
        smallestCase = index;
    }
    return true;
}

// This method is called when we know that a vertex from
// box two is in contact with box one.
void fillPointFaceBoxBox(const CollisionBox &one, const CollisionBox &two, const Vector3 &toCentre, CollisionData *data, unsigned best, real pen)
{
    Contact* contact = data->contacts;

    // We know which axis the collision is on (i.e. best),
    // but we need to work out which of the two faces on
    // this axis.
    Vector3 normal = one.getAxis(best);
    if (one.getAxis(best) * toCentre > 0)
    {
        normal = normal * -1.0f;
    }

    // Work out which vertex of box two we're colliding with.
    // Using toCentre doesn't work!
    Vector3 vertex = two.halfSize;
    if (two.getAxis(0) * normal < 0) vertex.x = -vertex.x;
    if (two.getAxis(1) * normal < 0) vertex.y = -vertex.y;
    if (two.getAxis(2) * normal < 0) vertex.z = -vertex.z;

    // Create the contact data
    contact->contactNormal = normal;
    contact->penetration = pen;
    contact->contactPoint = two.getTransform() * vertex;
	//if(two.body->hasFiniteMass() && one.body->hasFiniteMass())
	contact->setBodyData(one.body, two.body, data->friction, data->restitution);
	/*else
	{
		if(!two.body->hasFiniteMass())
			contact->setBodyData(one.body, NULL, data->friction, data->restitution);
		else
			contact->setBodyData(NULL, two.body, data->friction, data->restitution);
	}*/
    //contact->setBodyData(one.body, two.body, data->friction, data->restitution);
}


static inline Vector3 contactPoint(const Vector3 &pOne, const Vector3 &dOne, real oneSize, const Vector3 &pTwo, const Vector3 &dTwo, real twoSize,
    // If this is true, and the contact point is outside
    // the edge (in the case of an edge-face contact) then
    // we use one's midpoint, otherwise we use two's.
    bool useOne)
{
    Vector3 toSt, cOne, cTwo;
    real dpStaOne, dpStaTwo, dpOneTwo, smOne, smTwo;
    real denom, mua, mub;

    smOne = dOne.squareMagnitude();
    smTwo = dTwo.squareMagnitude();
    dpOneTwo = dTwo * dOne;

    toSt = pOne - pTwo;
    dpStaOne = dOne * toSt;
    dpStaTwo = dTwo * toSt;

    denom = smOne * smTwo - dpOneTwo * dpOneTwo;

    // Zero denominator indicates parrallel lines
    if (real_abs(denom) < 0.0001f) 
	{
        return useOne?pOne:pTwo;
    }

    mua = (dpOneTwo * dpStaTwo - smTwo * dpStaOne) / denom;
    mub = (smOne * dpStaTwo - dpOneTwo * dpStaOne) / denom;

    // If either of the edges has the nearest point out
    // of bounds, then the edges aren't crossed, we have
    // an edge-face contact. Our point is on the edge, which
    // we know from the useOne parameter.
    if (mua > oneSize ||
        mua < -oneSize ||
        mub > twoSize ||
        mub < -twoSize)
    {
        return useOne?pOne:pTwo;
    }
    else
    {
        cOne = pOne + dOne * mua;
        cTwo = pTwo + dTwo * mub;

        return cOne * 0.5 + cTwo * 0.5;
    }
}



// This preprocessor definition is only used as a convenience
// in the boxAndBox contact generation method.
#define CHECK_OVERLAP(axis, index) \
    if (!tryAxis(one, two, (axis), toCentre, (index), pen, best)) return 0;

unsigned CollisionDetector::boxAndBox(const CollisionBox &one, const CollisionBox &two, CollisionData *data)
{
    if (data->contactsLeft <= 0) return 0;

    // Find the vector between the two centres
    Vector3 toCentre = two.getAxis(3) - one.getAxis(3);

    // We start assuming there is no contact
    real pen = REAL_MAX;
    unsigned best = 0xffffff;

    // Now we check each axes, returning if it gives us
    // a separating axis, and keeping track of the axis with
    // the smallest penetration otherwise.
    CHECK_OVERLAP(one.getAxis(0), 0);
    CHECK_OVERLAP(one.getAxis(1), 1);
    CHECK_OVERLAP(one.getAxis(2), 2);

    CHECK_OVERLAP(two.getAxis(0), 3);
    CHECK_OVERLAP(two.getAxis(1), 4);
    CHECK_OVERLAP(two.getAxis(2), 5);

    // Store the best axis-major, in case we run into almost
    // parallel edge collisions later
    unsigned bestSingleAxis = best;

    CHECK_OVERLAP(one.getAxis(0) % two.getAxis(0), 6);
    CHECK_OVERLAP(one.getAxis(0) % two.getAxis(1), 7);
    CHECK_OVERLAP(one.getAxis(0) % two.getAxis(2), 8);
    CHECK_OVERLAP(one.getAxis(1) % two.getAxis(0), 9);
    CHECK_OVERLAP(one.getAxis(1) % two.getAxis(1), 10);
    CHECK_OVERLAP(one.getAxis(1) % two.getAxis(2), 11);
    CHECK_OVERLAP(one.getAxis(2) % two.getAxis(0), 12);
    CHECK_OVERLAP(one.getAxis(2) % two.getAxis(1), 13);
    CHECK_OVERLAP(one.getAxis(2) % two.getAxis(2), 14);

    //best<6 = face point collision
    if (best < 3)
    {
        // We've got a vertex of box two on a face of box one.
        fillPointFaceBoxBox(one, two, toCentre, data, best, pen);
        data->addContacts(1);
        return 1;
    }
    else if (best < 6)
    {
        // We've got a vertex of box one on a face of box two.
        fillPointFaceBoxBox(two, one, toCentre*-1.0f, data, best-3, pen);
        data->addContacts(1);
        return 1;
    }
    else
    {
        // We've got an edge-edge contact. Find out which axes
        best -= 6;
        unsigned oneAxisIndex = best / 3;
        unsigned twoAxisIndex = best % 3;
        Vector3 oneAxis = one.getAxis(oneAxisIndex);
        Vector3 twoAxis = two.getAxis(twoAxisIndex);
        Vector3 axis = oneAxis % twoAxis;
        axis.normalise();

        // The axis should point from box one to box two.
        if (axis * toCentre > 0) axis = axis * -1.0f;

        // We have the axes, but not the edges: each axis has 4 edges parallel
        // to it, we need to find which of the 4 for each object. We do
        // that by finding the point in the centre of the edge. We know
        // its component in the direction of the box's collision axis is zero
        // (its a mid-point) and we determine which of the extremes in each
        // of the other axes is closest.
        Vector3 ptOnOneEdge = one.halfSize;
        Vector3 ptOnTwoEdge = two.halfSize;
        for (unsigned i = 0; i < 3; i++)
        {
            if (i == oneAxisIndex) 
				ptOnOneEdge[i] = 0;
            else if (one.getAxis(i) * axis > 0) 
				ptOnOneEdge[i] = -ptOnOneEdge[i];

            if (i == twoAxisIndex) 
				ptOnTwoEdge[i] = 0;
            else if (two.getAxis(i) * axis < 0) 
				ptOnTwoEdge[i] = -ptOnTwoEdge[i];
        }

        // Move them into world coordinates (they are already oriented
        // correctly, since they have been derived from the axes).
        ptOnOneEdge = one.transform * ptOnOneEdge;
        ptOnTwoEdge = two.transform * ptOnTwoEdge;

        // So we have a point and a direction for the colliding edges.
        // We need to find out point of closest approach of the two
        // line-segments.
        Vector3 vertex = contactPoint(
            ptOnOneEdge, oneAxis, one.halfSize[oneAxisIndex],
            ptOnTwoEdge, twoAxis, two.halfSize[twoAxisIndex],
            bestSingleAxis > 2
            );

        // We can fill the contact.
        Contact* contact = data->contacts;

        contact->penetration = pen;
        contact->contactNormal = axis;
        contact->contactPoint = vertex;
		//if(two.body->hasFiniteMass() && one.body->hasFiniteMass())
			contact->setBodyData(one.body, two.body, data->friction, data->restitution);
		/*else
		{
			if(!two.body->hasFiniteMass())
				contact->setBodyData(one.body, NULL, data->friction, data->restitution);
			else
				contact->setBodyData(NULL, two.body, data->friction, data->restitution);
		}*/
        data->addContacts(1);
        return 1;
    }
    return 0;
}
#undef CHECK_OVERLAP


unsigned CollisionDetector::boxAndPoint(const CollisionBox &box, const Vector3 &point, CollisionData *data)
{
    if (data->contactsLeft <= 0) return 0;

    // Transform the point into box coordinates
    Vector3 relPt = box.transform.transformInverse(point);

    Vector3 normal;

    // Check each axis, looking for the axis on which the
    // penetration is least deep.
    real min_depth = box.halfSize.x - real_abs(relPt.x);
    if (min_depth < 0) return 0;
    normal = box.getAxis(0) * ((relPt.x < 0)?-1:1);

    real depth = box.halfSize.y - real_abs(relPt.y);
    if (depth < 0) return 0;
    else if (depth < min_depth)
    {
        min_depth = depth;
        normal = box.getAxis(1) * ((relPt.y < 0)?-1:1);
    }

    depth = box.halfSize.z - real_abs(relPt.z);
    if (depth < 0) return 0;
    else if (depth < min_depth)
    {
        min_depth = depth;
        normal = box.getAxis(2) * ((relPt.z < 0)?-1:1);
    }

    // Compile the contact
    Contact* contact = data->contacts;
    contact->contactNormal = normal;
    contact->contactPoint = point;
    contact->penetration = min_depth;

    // Note that we don't know what rigid body the point
    // belongs to, so we just use NULL. Where this is called
    // this value can be left, or filled in.
    contact->setBodyData(box.body, NULL, data->friction, data->restitution);

    data->addContacts(1);
    return 1;
}


unsigned CollisionDetector::boxAndSphere(const CollisionBox &box, const CollisionSphere &sphere, CollisionData *data)
{
    if (data->contactsLeft <= 0) return 0;

    // Transform the centre of the sphere into box coordinates
    Vector3 centre = sphere.getAxis(3);
    Vector3 relCentre = box.transform.transformInverse(centre);

    // Early out check to see if we can exclude the contact
    if (real_abs(relCentre.x) - sphere.radius > box.halfSize.x ||
        real_abs(relCentre.y) - sphere.radius > box.halfSize.y ||
        real_abs(relCentre.z) - sphere.radius > box.halfSize.z)
    {
        return 0;
    }

    Vector3 closestPt(0,0,0);
    real dist;

    // Clamp each coordinate to the box.
    dist = relCentre.x;
    if (dist > box.halfSize.x) dist = box.halfSize.x;
    if (dist < -box.halfSize.x) dist = -box.halfSize.x;
    closestPt.x = dist;

    dist = relCentre.y;
    if (dist > box.halfSize.y) dist = box.halfSize.y;
    if (dist < -box.halfSize.y) dist = -box.halfSize.y;
    closestPt.y = dist;

    dist = relCentre.z;
    if (dist > box.halfSize.z) dist = box.halfSize.z;
    if (dist < -box.halfSize.z) dist = -box.halfSize.z;
    closestPt.z = dist;

    // Check we're in contact
    dist = (closestPt - relCentre).squareMagnitude();
    if (dist > sphere.radius * sphere.radius) return 0;

    // Compile the contact
    Vector3 closestPtWorld = box.transform.transform(closestPt);

    Contact* contact = data->contacts;
    contact->contactNormal = (closestPtWorld - centre);
    contact->contactNormal.normalise();
    contact->contactPoint = closestPtWorld;
    contact->penetration = sphere.radius - real_sqrt(dist);

    /*if(box.body->hasFiniteMass() && sphere.body->hasFiniteMass())
		contact->setBodyData(box.body, sphere.body, data->friction, data->restitution);
	else
	{
		if(!sphere.body->hasFiniteMass())
			contact->setBodyData(box.body, NULL, data->friction, data->restitution);
		else
			contact->setBodyData(NULL, sphere.body, data->friction, data->restitution);
	}*/
	contact->setBodyData(box.body, sphere.body, data->friction, data->restitution);

    data->addContacts(1);
    return 1;
}


unsigned CollisionDetector::boxAndHalfSpace(const CollisionBox &box, const CollisionPlane &plane, CollisionData *data)
{
    if (data->contactsLeft <= 0) return 0;

    if (!IntersectionTests::boxAndHalfSpace(box, plane))
    {
        return 0;
    }

    // We have an intersection, so find the intersection points. We can make
    // do with only checking vertices. If the box is resting on a plane
    // or on an edge, it will be reported as four or two contact points.

    // Go through each combination of + and - for each half-size
    static real mults[8][3] = {{1,1,1},{-1,1,1},{1,-1,1},{-1,-1,1},
                               {1,1,-1},{-1,1,-1},{1,-1,-1},{-1,-1,-1}};

    Contact* contact = data->contacts;
    unsigned contactsUsed = 0;
    for (unsigned i = 0; i < 8; i++) {

        // Calculate the position of each vertex
        Vector3 vertexPos(mults[i][0], mults[i][1], mults[i][2]);
        vertexPos.componentProductUpdate(box.halfSize);
        vertexPos = box.transform.transform(vertexPos);

        // Calculate the distance from the plane
        real vertexDistance = vertexPos * plane.direction;

        // Compare this to the plane's distance
        if (vertexDistance <= plane.offset)
        {
            // Create the contact data.

            // The contact point is halfway between the vertex and the
            // plane - we multiply the direction by half the separation
            // distance and add the vertex location.
            contact->contactPoint = plane.direction;
            contact->contactPoint *= (vertexDistance-plane.offset);
            contact->contactPoint = vertexPos;
            contact->contactNormal = plane.direction;
            contact->penetration = plane.offset - vertexDistance;

            // Write the appropriate data
            contact->setBodyData(box.body, NULL, data->friction, data->restitution);

            // Move onto the next contact
            contact++;
            contactsUsed++;
            if (contactsUsed == data->contactsLeft) return contactsUsed;
        }
    }

    data->addContacts(contactsUsed);
    return contactsUsed;
}


unsigned CollisionDetector::rayCastSphere(const CollisionSphere &sphere,const Ray &ray,Vector3 &pointOfIntersection)
{
	Vector3 centre = sphere.getAxis(3);
	Vector3 centretoRay = ray.position-centre;

	real v = centretoRay*ray.direction;
	real c = (centretoRay*centretoRay) - (sphere.radius*sphere.radius); 
	if(c>0.0f && v>0.0f) return 0;

	real distance = v*v -c;
	if(distance<0.0f)
		return 0;
	else
	{
		real d=-v-sqrt(distance);
		if(d<0.0f)d=0.0;
		pointOfIntersection = ray.position + ray.direction*d;
		return 1;
	}
}


unsigned CollisionDetector::rayCastBox(const CollisionBox &box,const Ray &ray,Vector3 &pointOfIntersection)
{
	Vector3 rayPos, rayDir;
	rayPos = box.transform.transformInverse(ray.position);
	rayDir = box.transform.transformInverseDirection(ray.direction);

	Vector3 min,max;
	min = box.halfSize*-1;
	max = box.halfSize;

	real t_min = 0.0;
	real t_max = REAL_MAX;

	for(int i=0;i<3;++i)
	{
		if(abs(rayDir[i])<EPSILON)
		{
			// Ray is parallel to slab. No hit if origin not within slab
			if(rayPos[i]<min[i] || rayPos[i]>max[i])
				return 0;
		}else
		{
			real d = 1.0/rayDir[i];
			real t1 = (min[i] - rayPos[i]) * d;
			real t2 = (max[i] - rayPos[i]) * d;

			if(t1>t2)
			{
				real temp = t1;
				t1 = t2;
				t2 = temp;
			}

			if(t1>t_min) t_min = t1;
			if(t2<t_max) t_max = t2;

			if(t_min>t_max)
				return 0;
		}
	}

	Vector3 point = rayPos + (rayDir*(t_min+EPSILON));
	pointOfIntersection = point;
	/*if(point.x-min.x<0 || point.y-min.y<0 || point.z-min.z<0  || point.x-max.x>0 || point.y-max.y>0 || point.z-max.z>0)
		return 0;*/
	pointOfIntersection = box.transform.transform(point);
	return 1;
}


}