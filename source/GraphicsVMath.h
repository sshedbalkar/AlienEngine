///////////////////////////////////////////////////////////////////////////////////////
///
///	\file GraphicsVMath.h
///	Typedefs the DirectX Extension math library and provides some utility functions.
///
///	Authors: Chia-Wei Wu
///	Copyright (c) 2012, Digipen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////////////
#pragma once //Makes sure this header is only included once
#include "VMath.h"

namespace Framework{
#define UseXNAMath 0
#if UseXNAMath
typedef XMFLOAT2 Vec2;
typedef XMFLOAT3 Vec3;
typedef XMFLOAT4 Vec4;
typedef XMMATRIX Mat4;
#else
#endif

inline float LengthSquared(const Vec2& a){
		return a.x * a.x + a.y * a.y;
}

inline float Normalize(Vec2& a){
	float len = sqrt( LengthSquared(a) );	
	a.x /= len;
	a.y /= len;
	return len;
}

inline float Dot(const Vec2& a, const Vec2& b){
	return a.x * b.x + a.y * b.y;
}

inline D3DXMATRIX BuildTransform( const Vec3& translation , const D3DXQUATERNION& rotation )
{
	D3DXMATRIX matrix;
	D3DXMatrixRotationQuaternion( &matrix, &rotation );
	matrix._41 = translation.x;
	matrix._42 = translation.y;
	matrix._43 = translation.z;
	return matrix;
}

inline Vec3 GetPosition(D3DXMATRIX& matrix)
{
	return Vec3( matrix._41 , matrix._42 , matrix._43 );
}
}
