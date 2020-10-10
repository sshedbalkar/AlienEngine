#include "Matrix4.h"


namespace Framework
{

std::ostream& operator<<(std::ostream &out, const Matrix4 &m)
{
	out<<"| ";
	for(int i=0;i<12;i++)
	{
		out<<m.data[i]<<" ";
		if(i==3 || i==7 || i==11)
			out<<"|"<<std::endl;
		if(i==3||i==7)
			out<<"| ";
	}

	return out;
}

}