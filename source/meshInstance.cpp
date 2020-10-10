#include "MeshInstance.h"
#include "Graphics.h"
#include "Mesh.h"
#include "Composition.h"
#include "DebugDiagnostic.h"
#include <iostream>

namespace Framework
{

//===================================================
MeshInstance::MeshInstance() : dt(0.0f), looping(false), transform(NULL), texture(NULL), animation(NULL), flag(0), is_paused(false)
{
}
//===================================================
MeshInstance::~MeshInstance()
{
	Free();
}
//===================================================
void MeshInstance::Serialize( ISerializer& stream )
{
	std::string str;
	stream.readValue("id", str );

	mesh = GRAPHICS->GetMesh( str.c_str() );
	ErrorIf( !mesh, "Failed instantiating from mesh %s.", str.c_str() );
	mesh->RegisterInstance( this );

	if ( stream.readValue("default_animation", str ) )
	{
		animation = GRAPHICS->GetAnimation( str.c_str() );
	}
	if ( !stream.readValue("FilterFlag", flag ) ) flag = 0;
}
//===================================================
void MeshInstance::Initialize()
{
	dt = 0.0f;
	transform = GetOwner()->has(Transform);
}
//===================================================
void MeshInstance::Load()
{
	dt = 0.0f;
}
//===================================================
void MeshInstance::Unload()
{
}
//===================================================
void MeshInstance::Free()
{
	mesh->RemoveInstance( this );
}
//===================================================
void MeshInstance::SetAnimation( const char* id, bool _looping )
{
	dt = 0.0f;
	looping = _looping;
	is_paused = false;
	Animation* new_anim = GRAPHICS->GetAnimation( id );
	if ( new_anim ) animation = new_anim;
}
//===================================================
void MeshInstance::SetTexture( const char* id )
{
	texture = GRAPHICS->GetTexture( id );
}

}
