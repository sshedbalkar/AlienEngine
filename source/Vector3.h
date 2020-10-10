#include <cmath>
#include <iostream>
#include "Precision.h"
#include <d3dx10.h>

#ifndef VECTOR3_H
#define VECTOR3_H

namespace Framework
{
	
	class Vector3
    {
    public:
        real x;
		real y;
		real z;

    private:
        real pad;
	public:
        //constructors
		Vector3() : x(0), y(0), z(0) {}
		Vector3(const D3DXVECTOR3 &a);
		Vector3(const real x, const real y, const real z)
            : x(x), y(y), z(z) {}

        real operator[](unsigned i) const
        {
            if (i == 0) return x;
            if (i == 1) return y;
            return z;
        }

        real& operator[](unsigned i)
        {
            if (i == 0) return x;
            if (i == 1) return y;
            return z;
        }

        void operator+=(const Vector3& v)
        {
            x += v.x;
            y += v.y;
            z += v.z;
        }

        Vector3 operator+(const Vector3& v) const
        {
            return Vector3(x+v.x, y+v.y, z+v.z);
        }

        void operator-=(const Vector3& v)
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
        }

        Vector3 operator-(const Vector3& v) const
        {
            return Vector3(x-v.x, y-v.y, z-v.z);
        }

        void operator*=(const real value)
        {
            x *= value;
            y *= value;
            z *= value;
        }

        Vector3 operator*(const real value) const
        {
            return Vector3(x*value, y*value, z*value);
        }

        Vector3 componentProduct(const Vector3 &vector) const
        {
            return Vector3(x * vector.x, y * vector.y, z * vector.z);
        }

        void componentProductUpdate(const Vector3 &vector)
        {
            x *= vector.x;
            y *= vector.y;
            z *= vector.z;
        }

        Vector3 crossProduct(const Vector3 &vector) const
        {
            return Vector3(y*vector.z-z*vector.y,
                           z*vector.x-x*vector.z,
                           x*vector.y-y*vector.x);
        }

        void operator %=(const Vector3 &vector)
        {
            *this = crossProduct(vector);
        }

        Vector3 operator%(const Vector3 &vector) const
        {
            return Vector3(y*vector.z-z*vector.y,
                           z*vector.x-x*vector.z,
                           x*vector.y-y*vector.x);
        }

        real dotProduct(const Vector3 &vector) const
        {
            return x*vector.x + y*vector.y + z*vector.z;
        }

        real operator *(const Vector3 &vector) const
        {
            return x*vector.x + y*vector.y + z*vector.z;
        }

        void addScaledVector(const Vector3& vector, real scale)
        {
            x += vector.x * scale;
            y += vector.y * scale;
            z += vector.z * scale;
        }

        real magnitude() const
        {
            return real_sqrt(x*x+y*y+z*z);
        }

        real squareMagnitude() const
        {
            return x*x+y*y+z*z;
        }

        void trim(real size)
        {
            if (squareMagnitude() > size*size)
            {
                normalise();
                x *= size;
                y *= size;
                z *= size;
            }
        }

        void normalise()
        {
            real l = magnitude();
            if (l > 0)
            {
                (*this) *= 1.0/l;
            }
        }

        Vector3 unit() const
        {
            Vector3 result = *this;
            result.normalise();
            return result;
        }

        bool operator==(const Vector3& other) const
        {
            return x == other.x && y == other.y && z == other.z;
        }

        bool operator!=(const Vector3& other) const
        {
            return !(*this == other);
        }

        bool operator<(const Vector3& other) const
        {
            return x < other.x && y < other.y && z < other.z;
        }

        bool operator>(const Vector3& other) const
        {
            return x > other.x && y > other.y && z > other.z;
        }

        bool operator<=(const Vector3& other) const
        {
            return x <= other.x && y <= other.y && z <= other.z;
        }

        bool operator>=(const Vector3& other) const
        {
            return x >= other.x && y >= other.y && z >= other.z;
        }

        void clear()
        {
            x = y = z = 0;
        }

        void invert()
        {
            x = -x;
            y = -y;
            x = -z;
        }

    };

	std::ostream& operator<<(std::ostream &out, const Vector3 &v);

	inline real squaredDistance(Vector3& a,Vector3& b)
	{
		return (((a.x-b.x)*(a.x-b.x))+((a.y-b.y)*(a.y-b.y))+((a.z-b.z)*(a.z-b.z)));
	}

}
#endif