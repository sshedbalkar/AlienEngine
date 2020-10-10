#include "precompiled.h"
#include "Physics.h"
#include "Graphics.h"

namespace Framework
{

	
void CollisionBox::Serialize(ISerializer& stream)
{
	ISerializer* str = stream.clone();
	if(str->isGood())
	{
		float arr[3]={0.0,0.0,0.0};
		str->readValue("HalfSize", arr, 3);
		halfSize=Vector3(arr[0],arr[1],arr[2]);
	}
	delete str;
}

Vector3 CollisionBox::Support(const Vector3 & dir)
{
	Vector3 dirNormal = dir;
	//dirNormal.normalise();
	dirNormal=transform.transformInverseDirection(dirNormal);
	Vector3 supportInLocalSpace = halfSize;
	if(dirNormal.x<0) supportInLocalSpace.x*=-1;
	if(dirNormal.y<0) supportInLocalSpace.y*=-1;
	if(dirNormal.z<0) supportInLocalSpace.z*=-1;
	return transform.transform(supportInLocalSpace);
}

real CollisionBox::getVolume()
{
	return (halfSize.x*halfSize.y*halfSize.z);
}

Matrix3 CollisionBox::getInettiaTensor(const real &mass)
{
	Matrix3 t;
	Vector3 squares = halfSize.componentProduct(halfSize);
    t.setInertiaTensorCoeffs(0.33f*mass*(squares.y + squares.z), 0.33f*mass*(squares.x + squares.z), 0.33f*mass*(squares.x + squares.y));
	return t;
}

real CollisionBox::getBoundingRadius()
{
	return sqrt((halfSize.x*2.0) * (halfSize.x*2.0) + (halfSize.y*2.0) * (halfSize.y*2.0) + (halfSize.z*2.0) * (halfSize.z*2.0)) * 0.5;
}


unsigned CollisionBox::rayCast(const Ray &ray,Vector3 &pointOfIntersection)
{
	Vector3 rayPos, rayDir;
	rayPos = transform.transformInverse(ray.position);
	rayDir = transform.transformInverseDirection(ray.direction);

	Vector3 min,max;
	min = halfSize*-1;
	max = halfSize;

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
	pointOfIntersection = transform.transform(point);
	return 1;
}


}