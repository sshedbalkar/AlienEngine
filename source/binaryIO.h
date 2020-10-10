#pragma once

#include <fstream>
#include <iostream>

// binary file format : 
//		4 byte										- number of structured objects in integar
//		sizeof(structure)*number of objects			- data
//		4 byte										- number of the second objects
//		sizeof(structure2)*number of 2nd objects	- data
//		......

namespace Framework
{

//============================================
class BinaryReader
{
	std::ifstream fin;
public:
	void Open( const char* filename )
	{
		fin.open( filename, std::ios::in | std::ios::binary );
	}
	void Close()
	{
		fin.close();
	}
	template< typename T >
	bool operator() ( T** data, int* size )
	{
		if ( !fin ) { std::cout <<"file not exist/error when reading.\n" ; return false; }
		fin.read( (char*)size, sizeof( int ) );
		*data = new T[*size];
		fin.read( (char*)*data, sizeof(T)*(*size) );
		return true;
	}
};

//===========================================
class BinaryWriter
{
	std::ofstream fout;
public:
	void Open( const char* filename )
	{
		fout.open( filename, std::ios::out | std::ios::binary );
	}
	void Close()
	{
		fout.close();
	}
	template< typename T >
	bool operator() ( const T* data, int size )
	{
		if ( !fout ) { std::cout <<"file not exist/error when writing.\n" ; return false; }
		fout.write( (char*)&size, sizeof(int) );
		fout.write( (const char*)data, size * sizeof(T) );
		return true;
	}
};

}//end namespace
