////////////////////////////////////////////////////////////////
//
//	Utility.h
//	defines various utility functions
//
//	Author: Santosh Shedbalkar
//	Copyright 2011, Digipen Institute of Technology
//
////////////////////////////////////////////////////////////////
#ifndef UTILITY_H
#define UTILITY_H
//#pragma message("Compiling: "__FILE__)
//
#include <sstream>
#include "Containers.h"
#include "VMath.h"
//#include "CollisionMap.h"
//
namespace Framework
{
	class ISerializer;
	class MessageCollide;
}
namespace utility
{
//#pragma message("Including Class: Utility")
	template<typename RefType>
	inline void safeRelease(RefType& interfacePtr)
	{
		if(interfacePtr)
		{
			interfacePtr->Release();
		}
		interfacePtr = NULL;
	}
	//
	template<typename RefType>
	inline void safeDelete(RefType& objectPtr)
	{
		if(objectPtr)
		{
			delete objectPtr;
		}
		objectPtr = NULL;
	}
	//
	void SplitString(const std::string& str, const char delimiter, std::vector< std::string >& result, const unsigned int &maxTokens = -1 );
	//
	template <class T>
	bool from_string(T& t, const std::string& s, std::ios_base& (*f)(std::ios_base&))
	{
		std::istringstream iss(s);
		return !(iss >> f >> t).fail();
	}
	//
	void readArrayFromString(const std::string& str, std::vector<float>& vec);
	std::ostream& operator<<(std::ostream &out, const Framework::Vec2 &v);
	std::ostream& operator<<(std::ostream &out, const Framework::Vec3 &v);
	std::ostream& operator<<(std::ostream &out, const Framework::Vec4 &v);
	std::ostream& operator<<(std::ostream &out, const Framework::Mat4 &v);
	//std::ostream& operator<<(std::ostream &out, const CollisionMap *m);
	//std::ostream& operator<<(std::ostream &out, const MessageCollide *m);
	//
	template<class T>
	inline void deletePointersVec(std::vector<T*>& Vec)
	{
		for(std::vector<T*>::iterator it1 = Vec.begin(), it2 = Vec.end(); it1 != it2; ++it1)
		{
			delete *it1;
		}
	}
	//
	void ToUpper( std::string &str );
	void ToLower( std::string &str );
	void PrintASCII( const char *str );
	// generates a random float number in a range
	float RandomFloat(const float& min, const float& max);
	int RandomInt( int min, int max );
	unsigned int GetUniqueId();

	template <typename T> void Print(T & value)
	{
	  std::cout << value <<std::endl;
	}
}
//
#endif