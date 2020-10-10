///////////////////////////////////////////////////////////////////////////////////////
//
//	Transform.cpp - The Transform Component
//	Authors: 
//	Copyright 2010, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////
#include "Precompiled.h"
#include "Transform.h"
#include "DebugTools.h"
#include "Utility.h"

namespace Framework
{
	Transform::Transform()
	{
		Position = Vec3(0,0,0);
		Scale = Vec3(1,1,1);
		QUATERNION = D3DXQUATERNION(0,0,0,1);
	}

	void Transform::Serialize(ISerializer& stream)
	{
		float arr[4]={0.0,0.0,0.0,0.0};
		ISerializer* str = stream.clone();
		if(str->isGood())
		{
			if(str->readValue("Position", arr, 3))
			{
				Position.x = arr[0];
				Position.y = arr[1];
				Position.z = arr[2];
			}
			if(str->readValue("Scale", arr, 3))
			{
				Scale.x = arr[0];
				Scale.y = arr[1];
				Scale.z = arr[2];
			}
			if(str->readValue("Orientation", arr, 4))
			{
				QUATERNION.x = arr[0];
				QUATERNION.y = arr[1];
				QUATERNION.z = arr[2];
				QUATERNION.w = arr[3];
			}
		}
		delete str;
	}

	void Transform::RemoveComponentFromGame()
	{

	}

	bool Transform::Deserialize(ISerializer& stream)
	{
		const int length = 100;
		char str[length];
		ISerializer* strm = stream.clone();
		strm->writeNode( "Component" );
		strm->readProperty( "Component", *strm );
		//
		strm->writeProperty( "Type", GetName() );
		//
		sprintf_s( str, length, "%4.2f,%4.2f,%4.2f", Position.x, Position.y, Position.z );
		strm->writeNodeAndValue( "Position", str );
		//
		sprintf_s( str, length, "%4.2f,%4.2f,%4.2f", Scale.x, Scale.y, Scale.z );
		strm->writeNodeAndValue( "Scale", str );
		//
		//sprintf_s( str, length, "%3.4f,%3.4f,%3.4f,%3.4f", rotation.x, rotation.y, rotation.z , rotation.w);
		//strm->writeNodeAndValue( "Rotation", str );
		//
		delete strm;
		//
		return true;
	}
	//void Transform::BuildMatrix( Mat4& mat )
	//{
	//	Mat4 mat_rotation;
	//	Mat4 mat_scale;
	//	D3DXMatrixRotationYawPitchRoll( &mat_rotation, Rotation, RotationX, RotationZ );
	//	D3DXMatrixScaling( &mat_scale, Scale.x, Scale.y, Scale.z );
	
	//	D3DXMatrixMultiply( &mat, &mat_rotation, &mat_scale );
	//	mat._41 = Position.x;
	//	mat._42 = Position.y;
	//	mat._43 = Position.z;


	//}

	//void Transform::BuildMatrixNoScale( Mat4& mat )
	//{
	//	D3DXMatrixRotationYawPitchRoll( &mat, Rotation, RotationX, RotationZ );
	//	mat._41 = Position.x;
	//	mat._42 = Position.y;
	//	mat._43 = Position.z;
	//}
	
	float Transform::GetArea()
	{
		return Scale.x * Scale.y;
	}

	D3DXQUATERNION TransformIntoQuaterion(float DoubleAngle, const Vec3& axis ){
		float sine = sin(DoubleAngle);
		return D3DXQUATERNION(sine*axis.x, sine*axis.y, sine*axis.z, cos(DoubleAngle));
	}
}



