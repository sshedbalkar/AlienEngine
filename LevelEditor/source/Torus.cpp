#include "Torus.h"
#include <iostream>
using namespace std;

namespace Framework
{

Torus::Torus(const Vector3 &p, const double &r1, const double &r2, const Quaternion &q)
{
        location=p;
        bigRadius=r1;
        smallRadius=r2;
		globalToLocal.setOrientationAndPos(q,p);
}


bool Torus::intersection(Ray& ray) const
{
    Vector3 O = globalToLocal.transformInverse(ray.position);
    Vector3 D = globalToLocal.transformInverseDirection(ray.direction);

    double a, b, c, d, e, t;
    Polynomial solver;
    double* roots;
    int nbRoots;
        
    double tmpA = D.x * D.x + D.y * D.y + D.z * D.z;
    double tmpB = 2.0 * (D.x * O.x + D.y * O.y + D.z * O.z);
    double tmpC = O.x * O.x + O.y * O.y + O.z * O.z + bigRadius * bigRadius - smallRadius * smallRadius;
        
    double tmp = 4.0 * bigRadius * bigRadius;
    double tmpD = tmp * (D.x * D.x + D.z * D.z);
    double tmpE = tmp * 2.0 * (D.x * O.x + D.z * O.z);
    double tmpF = tmp * (O.x * O.x + O.z * O.z);
        
    a = tmpA * tmpA;
    b = 2.0 * tmpA * tmpB;
    c = 2.0 * tmpA * tmpC + tmpB * tmpB - tmpD;
    d = 2.0 * tmpB * tmpC - tmpE;
    e = tmpC * tmpC - tmpF;

    roots = new double[4];
    solver = Polynomial(a, b, c, d, e);     
    nbRoots = solver.SolveQuartic(roots);
    t = DBL_MAX;
        
    for (int j = 0; j < nbRoots; j++) 
	{
		if (roots[j] > 0.001 && roots[j] < t)
			t = roots[j];
    }
        
    delete(roots);
        
    if (t != DBL_MAX)
       return(true);
    return(false);
}


}