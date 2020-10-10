#include <cmath>
#include <iostream>
#include "Precision.h"
#include "Vector3.h"
#include <d3dx10.h>

#ifndef QUATERNION_H
#define QUATERNION_H

namespace Framework
{


class Quaternion
{
public:
	//Holds the real component of the quaternion.
    real r;

	//complex components
    real i;
	real j;
	real k;

	//The default constructor creates a quaternion representing zero rotation    
	Quaternion() : r(1), i(0), j(0), k(0) {}
	Quaternion(const real r, const real i, const real j, const real k) : r(r), i(i), j(j), k(k){}
	Quaternion(D3DXQUATERNION &q);

	D3DXQUATERNION QuaternionToD3DXQUATERNION();

	void normalise()
    {
        real d = r*r+i*i+j*j+k*k;

        if (d == 0) 
		{
            r = 1;
            return;
        }

        d = ((real)1.0)/real_sqrt(d);
        r *= d;
        i *= d;
        j *= d;
        k *= d;
    }
	
    bool operator==(const Quaternion& other) const
    {
        return r == other.r && i == other.i && j == other.j && k == other.k;
    }

    bool operator!=(const Quaternion& other) const
    {
        return !(*this == other);
    }

	void operator *=(const Quaternion &multiplier)
    {
        Quaternion q = *this;
        r = q.r*multiplier.r - q.i*multiplier.i -
            q.j*multiplier.j - q.k*multiplier.k;
        i = q.r*multiplier.i + q.i*multiplier.r +
            q.j*multiplier.k - q.k*multiplier.j;
        j = q.r*multiplier.j + q.j*multiplier.r +
            q.k*multiplier.i - q.i*multiplier.k;
        k = q.r*multiplier.k + q.k*multiplier.r +
            q.i*multiplier.j - q.j*multiplier.i;
    }

	Quaternion operator* (const Quaternion &multiplier) const
	{
        Quaternion q = *this;
		return Quaternion(q.r*multiplier.r - q.i*multiplier.i - q.j*multiplier.j - q.k*multiplier.k,q.r*multiplier.i + q.i*multiplier.r + q.j*multiplier.k - q.k*multiplier.j,
			q.r*multiplier.j + q.j*multiplier.r + q.k*multiplier.i - q.i*multiplier.k, q.r*multiplier.k + q.k*multiplier.r + q.i*multiplier.j - q.j*multiplier.i);
	}


	void addScaledVector(const Vector3& vector, real scale)
    {
        Quaternion q(0,
            vector.x * scale,
            vector.y * scale,
            vector.z * scale);
        q *= *this;
        r += q.r * ((real)0.5);
        i += q.i * ((real)0.5);
        j += q.j * ((real)0.5);
        k += q.k * ((real)0.5);
    }

    void rotateByVector(const Vector3& vector)
    {
        Quaternion q(0, vector.x, vector.y, vector.z);
        (*this) *= q;
    }

	Vector3 operator* (const Vector3 &vector) const
	{
		Vector3 vn(vector);
		vn.normalise(); 
		Quaternion resQuat, conjugate;
		Quaternion vecQuat(0, vn.x, vn.y, vn.z);
		conjugate = Quaternion(r, -i, -j, -k); 
		resQuat = vecQuat * conjugate;
		resQuat = *this * resQuat; 
		return (Vector3(resQuat.i, resQuat.j, resQuat.k));
	}

	Quaternion getConjugate()
	{
		return Quaternion(r, -i, -j, -k);
	}

	

};


std::ostream& operator<<(std::ostream &out, const Quaternion &m);

}

#endif