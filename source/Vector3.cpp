#include "Vector3.h"

namespace Framework
{

std::ostream& operator<<(std::ostream &out, const Vector3 &v)
{
	out<<"["<<v.x<<","<<v.y<<","<<v.z<<"]"/*<<std::endl*/;
	return out;
}

Vector3::Vector3(const D3DXVECTOR3& a):x(a.x),y(a.y),z(a.z){}

}