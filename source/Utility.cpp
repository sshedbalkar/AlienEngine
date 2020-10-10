#include "Utility.h"
#include <algorithm>
#include <iostream>
//#include "Physics.h"
//
static unsigned int m_counter = 0;
namespace utility
{
	void SplitString(const std::string& str, const char delimiter, std::vector< std::string >& result, const unsigned int &maxTokens )
	{
		unsigned int count = 0;
		std::string::size_type i = 0;
		std::string::size_type j = str.find( delimiter );
		if( j == std::string::npos )
		{
			result.push_back( str );
		}
		else
		{
			while( j != std::string::npos && count < maxTokens )
			{
				result.push_back( str.substr(i, j - i) );
				if( ++count == maxTokens )
				{
					result.push_back( str.substr(++j, str.size()) );
				}
				else
				{
					i = ++j;
					j = str.find( delimiter, j );
					if ( j == std::string::npos && count < maxTokens )
					{
						result.push_back( str.substr(i, str.size()) );
					}
				}
			}
		}
	}
	void readArrayFromString(const std::string& str, std::vector<float>& vec)
	{
		std::vector<std::string> strVec;
		SplitString(str, ',', strVec);
		//
		for(std::vector<std::string>::iterator it1 = strVec.begin(), it2 = strVec.end(); it1 != it2; ++it1)
		{
			float f;
			from_string<float>(f, *it1, std::dec);
			vec.push_back(f);
		}
	}
	std::ostream& operator<<(std::ostream &out, const Framework::Vec2 &v)
	{
		return out<<"["<<v.x<<", "<<v.y<<"]";
	}
	std::ostream& operator<<(std::ostream &out, const Framework::Vec3 &v)
	{
		return out<<"["<<v.x<<", "<<v.y<<", "<<v.z<<"]";
	}
	std::ostream& operator<<(std::ostream &out, const Framework::Vec4 &v)
	{
		return out<<"["<<v.x<<", "<<v.y<<", "<<v.z<<", "<<v.w<<"]";
	}
	std::ostream& operator<<(std::ostream &out, const Framework::Mat4 &v)
	{
		out<<"[("<<v._11<<", "<<v._12<<", "<<v._13<<", "<<v._14<<"), "
			<<"("<<v._21<<", "<<v._22<<", "<<v._23<<", "<<v._24<<"), "
			<<"("<<v._31<<", "<<v._32<<", "<<v._33<<", "<<v._34<<"), "
			<<"("<<v._41<<", "<<v._42<<", "<<v._43<<", "<<v._44<<")]";
		return out;
	}
	// generates a random float number in a range
	float RandomFloat( float min, float max )
	{
		float r = ( (float)::rand() ) / ( float )RAND_MAX;
		return min + ( r * (max - min) );
	}
	// generates a random int number in a range
	int RandomInt( int min, int max )
	{
		return ( ::rand() % (max-min) )+ min;
	}
	/*
	std::ostream& operator<<(std::ostream &out, const CollisionMap *m)
	{
		return m->print(out);
	}
	std::ostream& operator<<(std::ostream &out, const MessageCollide *m)
	{
		return out<<"{Collision: Normal: "<<m->ContactNormal<<", Impulse: "<<m->Impulse<<" With: Name: "<<m->CollidedWith->getName()<<", Id: "<<m->CollidedWith->getId()<<"}";
	}
	*/
	void ToUpper( std::string &str )
	{
		std::transform( str.begin(), str.end(), str.begin(), ::toupper );
	}
	void ToLower( std::string &str )
	{
		std::transform( str.begin(), str.end(), str.begin(), ::tolower );
	}
	void PrintASCII( const char *str )
	{
		unsigned int i = 0;
		while( str[ i ] != '\0' )
		{
			std::cout<<int( str[i] )<<",";
			++i;
		}
	}
	unsigned int GetUniqueId()
	{
		return ++m_counter;
	}
}