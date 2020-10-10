#include "Matrix3.h"


namespace Framework
{

Matrix3 Matrix3::linearInterpolate(const Matrix3& a, const Matrix3& b, real prop)
{
    Matrix3 result;
    for (unsigned i = 0; i < 9; i++) 
	{
        result.data[i] = a.data[i] * (1-prop) + b.data[i] * prop;
    }
    return result;
}


std::ostream& operator<<(std::ostream &out, const Matrix3 &m)
{
	out<<"| ";
	for(int i=0;i<9;i++)
	{
		out<<m.data[i]<<" ";
		if(i==2 || i==5 || i==8)
			out<<"|"<<std::endl;
		if(i==2||i==5)
			out<<"| ";
	}

	return out;
}

}