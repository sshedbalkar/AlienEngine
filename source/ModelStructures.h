#pragma once

namespace Framework
{

#define ANIM_NAME_LENGTH			32

struct VertexTypeStaticMesh
{
	float position[3];
	float normal[3];
	float tangent[3];
	float binormal[3];
	float uv[2];
};

struct VertexTypeAnimatedMesh
{
	float position[3];
	float normal[3];
	float tangent[3];
	float binormal[3];
	float uv[2];
	int indices[4];
	float weights[4];
};

struct VertexTypeStaticMeshLite
{
	float position[3];
	void Set( VertexTypeStaticMesh& vertex )
	{
		memcpy( position, vertex.position, sizeof(float) * 3 );
	};
};

struct VertexTypeAnimatedMeshLite
{
	float position[3];
	int indices[4];
	float weights[4];
	void Set( VertexTypeAnimatedMesh& vertex )
	{
		memcpy( position, vertex.position, sizeof(float) * 3 );
		memcpy( indices, vertex.indices, sizeof(int) * 4 );
		memcpy( weights, vertex.weights, sizeof(float) * 4 );
	};
};



}
