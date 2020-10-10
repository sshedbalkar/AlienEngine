#ifndef Torus_h
#define Torus_h

#include "polynomial.h"
#include "Vector3.h"
#include "Collisions.h"

namespace Framework
{

class Torus
{
private:
        Vector3 location;
        double bigRadius;
        double smallRadius;
		Matrix4 globalToLocal;
public:
        Torus(const Vector3 &p, const double &r1, const double &r2, const Quaternion &q);
        bool intersection(Ray& ray) const;
};


}
#endif