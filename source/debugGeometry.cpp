#include "debugGeometry.h"
#include "Graphics.h"

namespace Framework
{
//==============================================
DebugGeometry::DebugGeometry():transform(NULL)
{
}
//==============================================
DebugGeometry::~DebugGeometry()
{
	//GRAPHICS->RemoveDebugGeometry( this );
	Free();
}
//==============================================
void DebugGeometry::AddDebugShape( DebugShapeType type, Vec3& param, Vec4& color, const char* name )
{
	DebugShapeData* shape = new DebugShapeData;
	shape->type = type;
	shape->param = param;
	shape->color = color;

	if ( !name ) 
	{
		char s[8];
		sprintf_s( s, "shape%d", debug_shapes.size() );
		debug_shapes[s] =shape;
	}
	else debug_shapes[name]=shape;
}
//==============================================
void DebugGeometry::ModifyDebugShape( const char* name, Vec3& param, Vec4& color )
{
	DebugShapeData* shape = debug_shapes[name];
	if ( !shape ) return;
	shape->param = param;
	shape->color = color;
}
//==============================================
void DebugGeometry::Free()
{
	DebugShapeListType::iterator it;
	for ( it = debug_shapes.begin(); it != debug_shapes.end(); ++it )
	{
		S_DELETE( it->second );
	}
	debug_shapes.clear();
}

void DebugGeometry::RemoveComponentFromGame()
{
	
}

void DebugGeometry::Initialize()
{ 
	//transform = GetOwner()->has( Transform );
	//GRAPHICS->RegisterDebugGeometry( this );
}

}