///////////////////////////////////////////////////////////////////////////////////////
//
//	ModelClassBinary.cpp
//	
//	Authors: Chia-Wei Wu
//	Copyright (c) 2012, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////
#include "Precompiled.h"
#include "ModelClassBinary.h"
#include "FileElements.hpp"
#include "ChunkReader.h"

namespace Framework
{

ModelClassBinary::ModelClassBinary()
{
	d3dVertexBuffer = NULL;
	d3dIndexBuffer = NULL;
	NumVertices = 0;
}

ModelClassBinary::~ModelClassBinary()
{
	SafeRelease( d3dVertexBuffer );
	SafeRelease( d3dIndexBuffer );
}

void ModelClassBinary::ReadAndInitialize(ChunkReader& file, ID3D11Device * device)
{
	HRESULT result;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
	void* vertexBufferData;
	unsigned int * indexBufferData;

	GChunk meshChunk = file.ReadChunkHeader();

	file.Read( VertexTypeIndex );
	if(VertexTypeIndex == 0 ){//static
		VertexSize = sizeof(ModelVertex);

		file.Read( NumIndices );
		indexBufferData = new unsigned int[ NumIndices ];
		file.ReadArray( indexBufferData , NumIndices );

		file.Read( NumVertices );
		//vertexBufferData = new ModelVertex[ NumVertices ];
		ModelVertex* InputVertexBufferData = new ModelVertex[ NumVertices];
		file.ReadArraySize( InputVertexBufferData , NumVertices * VertexSize );
		//get tangent and binomial space for the vertex
		VertexSize = sizeof(ShaderModelVertex);
		ShaderModelVertex* VertexBufferDataWithTangent = new ShaderModelVertex[NumVertices];
		for( unsigned i=0 ; i < NumVertices ; ++i ){
			VertexBufferDataWithTangent[i].pos = InputVertexBufferData[i].pos;
			VertexBufferDataWithTangent[i].tex = InputVertexBufferData[i].tex;
			VertexBufferDataWithTangent[i].norm = InputVertexBufferData[i].norm;
			//garbage data that just used to match shader vertex type
			VertexBufferDataWithTangent[i].tangent = Vec3(0,0,0);
			VertexBufferDataWithTangent[i].binormal = Vec3(0,0,0);
		}
		vertexBufferData = VertexBufferDataWithTangent;
		SafeDelete( InputVertexBufferData );
	}else{//Animated
		VertexSize = sizeof(InputSkinnedModelVertex);

		file.Read( NumIndices );
		indexBufferData = new unsigned int[ NumIndices ];
		file.ReadArray( indexBufferData , NumIndices );

		file.Read( NumVertices );
		//vertexBufferData = new ModelVertex[ NumVertices ];
		vertexBufferData = new unsigned int[ NumVertices * VertexSize ];
		file.ReadArraySize( vertexBufferData , NumVertices * VertexSize );
	}

	// Set up the description of the static vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    //vertexBufferDesc.ByteWidth = sizeof(VertexType) * NumVertices;
	vertexBufferDesc.ByteWidth = VertexSize * NumVertices;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertexBufferData;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	// Now create the vertex buffer.
	result = device->CreateBuffer( &vertexBufferDesc, &vertexData, &d3dVertexBuffer);
	if(FAILED(result)){
		__debugbreak();
		return;
	}

	// Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned int) * NumIndices;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indexBufferData;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &d3dIndexBuffer);
	if(FAILED(result)){
		__debugbreak();
		return ;
	}

	SafeDelete( vertexBufferData );
	SafeDelete( indexBufferData );
}

void ModelClassBinary::SetAndDraw(ID3D11DeviceContext* deviceContext, D3DXVECTOR3 Scale)
{
	unsigned int stride;
	unsigned int offset;
	// Set vertex buffer stride and offset.
	stride = VertexSize;
	offset = 0;
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &d3dVertexBuffer, &stride, &offset);
    // Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(d3dIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

unsigned int ModelClassBinary::GetIndexCount(){
	return NumIndices;
}

ModelClassBinary* LoadModel(std::string filename, ID3D11Device * device)
{
	ChunkReader file;
	file.Open( filename );

	GChunk fileChunk = file.ReadChunkHeader();
	if( fileChunk.Type != FileMark )
		return NULL;
	ModelClassBinary * newModel = new ModelClassBinary();
	newModel->modelType = Static;
	fileChunk = file.PeekChunk();

	while( fileChunk.Type != 0 )
	{
		switch( fileChunk.Type )
		{
			case MarkMesh:
			{
				newModel->ReadAndInitialize( file, device);
			}
			break;
			case MarkSkel:
			{			
				newModel->modelType = Skinned;
				newModel->animationContent = new AnimationContent();

				Skel * newSkel = new Skel();
				newSkel->ReadFrom( file );
				newSkel->Initialize();
				newModel->animationContent->SetSkel( newSkel );
				newSkel->Release();
			}
			break;
			case MarkAnimation:
			{
				Animation * newAnimation = new Animation();
				newAnimation->ReadFrom( file );
				newModel->animationContent->AddAnimation( newAnimation );
				newAnimation->Release();
			}

			default:
				file.SkipChunk(fileChunk);
			break;
		}
		fileChunk = file.PeekChunk();
	}
	return newModel;
}

}
