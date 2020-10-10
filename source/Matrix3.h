#include <cmath>
#include <iostream>
#include "Precision.h"
#include "Vector3.h"
#include "Quaternion.h"

#ifndef MATRIX3_H
#define MATRIX3_H

namespace Framework
{

//3x3 matrix
class Matrix3
{
public:
	real data[9];

	Matrix3()
	{
		data[0] = data[1] = data[2] = data[3] = data[4] = data[5] = data[6] = data[7] = data[8] = 0;
	}

	//set the components as the coloumnof the matrix
	Matrix3(const Vector3 &compOne, const Vector3 &compTwo,const Vector3 &compThree)
    {
        setComponents(compOne, compTwo, compThree);
    }

	
    Matrix3(real c0, real c1, real c2, real c3, real c4, real c5, real c6, real c7, real c8)
    {
        data[0] = c0; data[1] = c1; data[2] = c2;
        data[3] = c3; data[4] = c4; data[5] = c5;
        data[6] = c6; data[7] = c7; data[8] = c8;
    }

	//make a diagonal matrix
    void setDiagonal(real a, real b, real c)
    {
        setInertiaTensorCoeffs(a, b, c);
    }

    //Sets the value of the matrix from inertia tensor values.
    void setInertiaTensorCoeffs(real ix, real iy, real iz, real ixy=0, real ixz=0, real iyz=0)
    {
        data[0] = ix;
        data[1] = data[3] = -ixy;
        data[2] = data[6] = -ixz;
        data[4] = iy;
        data[5] = data[7] = -iyz;
        data[8] = iz;
    }

	//inertia tensor for a block   1/12*m*(dy^2 + dz^2) 1/12*m*(dx^2 + dz^2) 1/12*m*(dx^2 + dy^2)
	void setBlockInertiaTensor(const Vector3 &halfSizes, real mass)
    {
        Vector3 squares = halfSizes.componentProduct(halfSizes);
        setInertiaTensorCoeffs(0.33f*mass*(squares.y + squares.z), 0.33f*mass*(squares.x + squares.z), 0.33f*mass*(squares.x + squares.y));
    }

	void setSphereInertiaTensor(const real &radius, real mass)
	{
		real temp=0.4*mass*radius*radius;
		setInertiaTensorCoeffs(temp,temp,temp);
	}
    /**
    * Sets the matrix to be a skew symmetric matrix based on
    * the given vector. The skew symmetric matrix is the equivalent
    * of the vector product. So if a,b are vectors. a x b = A_s b
    * where A_s is the skew symmetric form of a.
    */
    void setSkewSymmetric(const Vector3 vector)
    {
        data[0] = data[4] = data[8] = 0;
        data[1] = -vector.z;
        data[2] = vector.y;
        data[3] = vector.z;
        data[5] = -vector.x;
        data[6] = -vector.y;
        data[7] = vector.x;
    }


	void setComponents(const Vector3 &compOne,const Vector3 &compTwo,const Vector3 &compThree)
	{
		data[0] = compOne.x;
        data[1] = compTwo.x;
        data[2] = compThree.x;
        data[3] = compOne.y;
        data[4] = compTwo.y;
        data[5] = compThree.y;
        data[6] = compOne.z;
        data[7] = compTwo.z;
        data[8] = compThree.z;
	}

	Vector3 operator*(const Vector3 &vector) const
    {
        return Vector3(
            vector.x * data[0] + vector.y * data[1] + vector.z * data[2],
            vector.x * data[3] + vector.y * data[4] + vector.z * data[5],
            vector.x * data[6] + vector.y * data[7] + vector.z * data[8]
        );
    }


	Vector3 transform(const Vector3 &vector) const
    {
        return (*this) * vector;
    }


	Vector3 transformTranspose(const Vector3 &vector) const
    {
        return Vector3(
            vector.x * data[0] + vector.y * data[3] + vector.z * data[6],
            vector.x * data[1] + vector.y * data[4] + vector.z * data[7],
            vector.x * data[2] + vector.y * data[5] + vector.z * data[8]
        );
    }

	Vector3 getRowVector(int i) const
    {
		if(i>=3 || i<0)
			return Vector3(0,0,0);
        return Vector3(data[i*3], data[i*3+1], data[i*3+2]);
    }

	//axis = column
	Vector3 getAxisVector(int i) const
    {
        return Vector3(data[i], data[i+3], data[i+6]);
    }

	void setInverse(const Matrix3 &m)
    {
        real t4 = m.data[0]*m.data[4];
        real t6 = m.data[0]*m.data[5];
        real t8 = m.data[1]*m.data[3];
        real t10 = m.data[2]*m.data[3];
        real t12 = m.data[1]*m.data[6];
        real t14 = m.data[2]*m.data[6];

        // Calculate the determinant
        real t16 = (t4*m.data[8] - t6*m.data[7] - t8*m.data[8]+
                    t10*m.data[7] + t12*m.data[5] - t14*m.data[4]);

        // Make sure the determinant is non-zero.
        if (t16 == (real)0.0f)
		{
			Matrix3 temp;
			*this=temp;
			return;
		}
        real t17 = 1/t16;

        data[0] = (m.data[4]*m.data[8]-m.data[5]*m.data[7])*t17;
        data[1] = -(m.data[1]*m.data[8]-m.data[2]*m.data[7])*t17;
        data[2] = (m.data[1]*m.data[5]-m.data[2]*m.data[4])*t17;
        data[3] = -(m.data[3]*m.data[8]-m.data[5]*m.data[6])*t17;
        data[4] = (m.data[0]*m.data[8]-t14)*t17;
        data[5] = -(t6-t10)*t17;
        data[6] = (m.data[3]*m.data[7]-m.data[4]*m.data[6])*t17;
        data[7] = -(m.data[0]*m.data[7]-t12)*t17;
        data[8] = (t4-t8)*t17;
    }

	Matrix3 inverse() const
    {
        Matrix3 result;
        result.setInverse(*this);
        return result;
    }

	void invert()
    {
        setInverse(*this);
    }

	void setTranspose(const Matrix3 &m)
    {
        data[0] = m.data[0];
        data[1] = m.data[3];
        data[2] = m.data[6];
        data[3] = m.data[1];
        data[4] = m.data[4];
        data[5] = m.data[7];
        data[6] = m.data[2];
        data[7] = m.data[5];
        data[8] = m.data[8];
    }

    Matrix3 transpose() const
    {
        Matrix3 result;
        result.setTranspose(*this);
        return result;
    }

	Matrix3 operator*(const Matrix3 &o) const
    {
        return Matrix3(
            data[0]*o.data[0] + data[1]*o.data[3] + data[2]*o.data[6],
            data[0]*o.data[1] + data[1]*o.data[4] + data[2]*o.data[7],
            data[0]*o.data[2] + data[1]*o.data[5] + data[2]*o.data[8],

            data[3]*o.data[0] + data[4]*o.data[3] + data[5]*o.data[6],
            data[3]*o.data[1] + data[4]*o.data[4] + data[5]*o.data[7],
            data[3]*o.data[2] + data[4]*o.data[5] + data[5]*o.data[8],

            data[6]*o.data[0] + data[7]*o.data[3] + data[8]*o.data[6],
            data[6]*o.data[1] + data[7]*o.data[4] + data[8]*o.data[7],
            data[6]*o.data[2] + data[7]*o.data[5] + data[8]*o.data[8]
            );
    }

	void operator*=(const Matrix3 &o)
    {
        real t1;
        real t2;
        real t3;

        t1 = data[0]*o.data[0] + data[1]*o.data[3] + data[2]*o.data[6];
        t2 = data[0]*o.data[1] + data[1]*o.data[4] + data[2]*o.data[7];
        t3 = data[0]*o.data[2] + data[1]*o.data[5] + data[2]*o.data[8];
        data[0] = t1;
        data[1] = t2;
        data[2] = t3;

        t1 = data[3]*o.data[0] + data[4]*o.data[3] + data[5]*o.data[6];
        t2 = data[3]*o.data[1] + data[4]*o.data[4] + data[5]*o.data[7];
        t3 = data[3]*o.data[2] + data[4]*o.data[5] + data[5]*o.data[8];
        data[3] = t1;
        data[4] = t2;
        data[5] = t3;

        t1 = data[6]*o.data[0] + data[7]*o.data[3] + data[8]*o.data[6];
        t2 = data[6]*o.data[1] + data[7]*o.data[4] + data[8]*o.data[7];
        t3 = data[6]*o.data[2] + data[7]*o.data[5] + data[8]*o.data[8];
        data[6] = t1;
        data[7] = t2;
        data[8] = t3;
    }


	void operator*=(const real scalar)
    {
        data[0] *= scalar; data[1] *= scalar; data[2] *= scalar;
        data[3] *= scalar; data[4] *= scalar; data[5] *= scalar;
        data[6] *= scalar; data[7] *= scalar; data[8] *= scalar;
    }

	void operator+=(const Matrix3 &o)
    {
        data[0] += o.data[0]; data[1] += o.data[1]; data[2] += o.data[2];
        data[3] += o.data[3]; data[4] += o.data[4]; data[5] += o.data[5];
        data[6] += o.data[6]; data[7] += o.data[7]; data[8] += o.data[8];
    }

	//quaternion to matrix
	void setOrientation(const Quaternion &q)
    {
        data[0] = 1 - (2*q.j*q.j + 2*q.k*q.k);
        data[1] = 2*q.i*q.j + 2*q.k*q.r;
        data[2] = 2*q.i*q.k - 2*q.j*q.r;
        data[3] = 2*q.i*q.j - 2*q.k*q.r;
        data[4] = 1 - (2*q.i*q.i  + 2*q.k*q.k);
        data[5] = 2*q.j*q.k + 2*q.i*q.r;
        data[6] = 2*q.i*q.k + 2*q.j*q.r;
        data[7] = 2*q.j*q.k - 2*q.i*q.r;
        data[8] = 1 - (2*q.i*q.i  + 2*q.j*q.j);
    }

	void moveFromCenterOfMass(Vector3 &arr,real &mass)
	{
		real mag=arr*arr;
		real x,y,z;
		x=arr.x; y=arr.y; z=arr.z;
		data[0]= data[0]+ (mass*(mag - x*x));
		data[1]= data[1]+ (mass*(0 - x*y));
		data[2]= data[2]+ (mass*(0 - x*z));

		data[3]= data[3]+ (mass*(0 - y*x));
		data[4]= data[4]+ (mass*(mag - y*y));
		data[5]= data[5]+ (mass*(0 - y*z));

		data[6]= data[6]+ (mass*(0 - z*x));
		data[7]= data[7]+ (mass*(0 - z*y));
		data[8]= data[8]+ (mass*(mag - z*z));
	}

	static Matrix3 linearInterpolate(const Matrix3& a, const Matrix3& b, real prop);

};

std::ostream& operator<<(std::ostream &out, const Matrix3 &m);

}

#endif