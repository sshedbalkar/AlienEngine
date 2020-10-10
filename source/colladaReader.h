#pragma once

#include <string>

namespace Framework
{
class ISerializer;

void ReadFloatArray( ISerializer& stream, const char* prop, int count, float** data );
void ReadIntArray( ISerializer& stream, const char* prop, int count, int** data );

class DAESourceFloat
{
	std::string id;
	int stride;
	int count;
	int unit_count;
	float* data;

public:
	DAESourceFloat();
	~DAESourceFloat();

	void Serialize( ISerializer& stream );
	float* GetUnit( int index );
};



}