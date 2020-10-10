#pragma once

#include <D3D10.h>
#include "VMath.h"
#include <string>

namespace Framework
{
		
enum MaterialType
{
	MAT_NONE = -1,
	//surface materials
	MAT_SPIKES,
	MAT_GRASS,
	//box materials
	MAT_WATER,
	MAT_TEXTUREDBOX,
	MAT_SKYLINE,
	MAX_MATERIALS
};

class Shader;
class Transform;

class Material
{
protected:
	Shader* shader;
public:
	virtual ~Material() {}
	virtual void Initialize( ID3D10Device* device ) = 0;
	virtual void Update( float timeslice ) {static_cast<void>(timeslice);}
	virtual void Draw( ID3D10Device* device, Mat4& mat_view, Mat4& mat_proj, float* reflect_altitude = NULL ) = 0;
	virtual void RegisterVolume( Transform* transform, int flag ) = 0;
	virtual void Free() = 0;
	virtual void Unload() = 0;
	void SetShader( Shader* _shader ) { shader = _shader; }

	static MaterialType GetMaterialTypeFromString( std::string& str )
	{
		if ( str == "none" )
		{ return MAT_NONE;}
		else if ( str == "grass" )
		{ return MAT_GRASS;}
		else if ( str == "spikes" )
		{ return MAT_SPIKES;}
		else if ( str == "skyline" )
		{ return MAT_SKYLINE;}
		else if ( str == "water" )
		{ return MAT_WATER; }
		else return MAT_NONE;
	}
};

}
