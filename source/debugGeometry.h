/*////////////////////////////////////////////////////////////////////////////////////
//
//	DebugGeometry.h
//
//	Authors: Zheng
//	Instruction:
		Create component DebugGeometry and call Initialize() during loading time.
		After that, you can modify the debug_shape_list when needed( Push new shapes or remove them from the list ).
		Call Free() before delete the entire component.
		
	example( after Creation and Initialization of DebugGeometry Component, add and maintain a debug rectangle ):
		add+initialize:
			DebugGeometry::DebugShapeData;
			shape.type = DebugGeometry::DEBUG_SHAPE_RECTANGLE;
			shape.v0 = Vec3( 0.0f,0.0f,0.0f );
			shape.v1 = Vec3( 0.5f,1.0f,0.0f );
			shape.color = Vec4( 0.0f,0.0f,0.0f,1.0f );
			debug_geometry->debug_shape_list.push_back( shape );

		maintain:
			DebugGeometry::DebugShapeData* shape = &debug_geometry->debug_shape_list[0];
			shape->v1 = Vec3( 1.0f,1.0f,1.0f );

//
/////////////////////////////////////////////////////////////////////////////////////*/
#pragma once

#include "VMath.h"
#include "Component.h"
#include <map>


namespace Framework
{
	class Transform;

	enum DebugShapeType
	{
		DEBUG_SHAPE_LINESEGMENT=0,	
			//// starting point is the point specified by transform->Position, 
			//// the Vec3 parameter passed when calling AddShape or UpdateShape is the offset in (x,y,z) to the starting point
		DEBUG_SHAPE_ARROW,			
			//// line segment with direction
		DEBUG_SHAPE_RECTANGLE,		
			//// 2d rectangle with depth, transform->Position defines the center point
			//// while first two elements of param define the width and height
		DEBUG_SHAPE_CIRCLE,			
			//// 2d circle with depth, transform->Position defines center and depth
			//// first element of param defines the radius
		DEBUG_SHAPE_PRISM			
			//// axis-aligned rectangular prism, transform->Position defines center
			//// while x,y,z of param define width,height,depth respectively
	};

	struct DebugShapeData
	{
		DebugShapeType type;
		Vec3 param;
		Vec4 color;
	};

	class DebugGeometry : public GameComponent
	{
	public:
		typedef std::map<std::string, DebugShapeData*> DebugShapeListType;
		DebugShapeListType debug_shapes;
		Transform * transform;
		DebugGeometry *Next;
		DebugGeometry *Prev;
	public:
		DebugGeometry();
		~DebugGeometry();
		
		virtual void Free();
		virtual void RemoveComponentFromGame();
		void Initialize();
		void AddDebugShape( DebugShapeType type, Vec3& param, Vec4& color, const char* name = 0 );
		void ModifyDebugShape( const char* name, Vec3& param, Vec4& color );
	};

}
