#include "colladaReader.h"
#include "Serialization.h"
#include <iostream>
#include <sstream>

namespace Framework
{
//========================================================
void ReadFloatArray( ISerializer& stream, const char* prop, int count, float** data )
{
	std::string str;
	stream.readValue( prop, str );
	std::stringstream ss(str);
	*data = new float[count];
	for ( int i = 0; i < count; ++i ) ss >>(*data)[i];
}
//========================================================
void ReadIntArray( ISerializer& stream, const char* prop, int count, int** data )
{
	std::string str;
	stream.readValue( prop, str );
	std::stringstream ss(str);
	*data = new int[count];
	for ( int i = 0; i < count; ++i ) ss >>(*data)[i];
}
//========================================================
DAESourceFloat::DAESourceFloat() : data( NULL )
{
}
//========================================================
DAESourceFloat::~DAESourceFloat()
{
	if (data) delete [] data;
}
//========================================================
void DAESourceFloat::Serialize( ISerializer& stream )
{
	stream.readProperty( "id", id );
	ISerializer* float_array = stream.clone();
	ISerializer* accessor = stream.clone();
	stream.readProperty( "float_array", *float_array );
	float_array->readProperty( "count", count );

	ReadFloatArray( stream, "float_array", count, &data );
	
	stream.readProperty( "technique_common", *accessor );
	accessor->readProperty( "accessor", *accessor );
	accessor->readProperty( "count", unit_count );
	accessor->readProperty( "stride", stride );
	
	delete accessor;
	delete float_array;
}
//========================================================
float* DAESourceFloat::GetUnit( int index )
{
	if ( unit_count <= index ) return NULL;
	return data + index * stride;
}



}