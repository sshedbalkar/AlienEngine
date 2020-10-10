#include "Precompiled.h"
#include "Graphics.h"
#include "Asset.hpp"
#include "d3dclass.h"
#include "colorshaderclass.h"

namespace Framework
{

void LineDrawer::DrawLineFromList(const D3DMATRIX& worldMatrix, const D3DMATRIX& viewMatrix, const D3DMATRIX& projectionMatrix){
	if(drawLineList.size()==0) return;
	LineVertex* Vertices = new LineVertex[ drawLineList.size() ];
	for(unsigned i = 0; i < drawLineList.size() ; ++i){
		Vertices[i].position = drawLineList[i].position;
		Vertices[i].color = drawLineList[i].color;
	}

	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* indexBuffer;
	D3D11_BUFFER_DESC vertexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData;
	// Set the number of vertices in the vertex array.
	int m_vertexCount = drawLineList.size();
	// Set up the description of the static vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(LineVertex) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = Vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	HRESULT result;
	result = GRAPHICS->m_D3D->GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &VertexBuffer);
	if( FAILED(result) ){
		__debugbreak();
		return;
	}

	// Set the number of indices in the index array.
	unsigned m_indexCount = drawLineList.size();
	D3D11_BUFFER_DESC indexBufferDesc;
    D3D11_SUBRESOURCE_DATA indexData;
	unsigned long* indices =  new unsigned long[m_indexCount];
	// Load the index array with data.
	for(unsigned i =0 ; i<m_indexCount ; ++i){
		indices[i] = i;
	}
	// Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	// Create the index buffer.
	result = GRAPHICS->m_D3D->GetDevice()->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
	if(FAILED(result))
	{
		return ;
	}

	UINT stride = sizeof(LineVertex);
	UINT offset = 0;
	GRAPHICS->m_D3D->GetDeviceContext()->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
	// Set the index buffer to active in the input assembler so it can be rendered.
	GRAPHICS->m_D3D->GetDeviceContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	GRAPHICS->m_D3D->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	//Render the model using the texture shader.
	result = GRAPHICS->m_ColorShader->Render( GRAPHICS->m_D3D->GetDeviceContext(), drawLineList.size(), worldMatrix, viewMatrix, projectionMatrix);
	if(!result){
		__debugbreak();
		return;
	}
	// Release the vertex buffer.
	SafeRelease(VertexBuffer);
	SafeRelease(indexBuffer);
	delete [] indices;
	delete [] Vertices;
	indices = 0;

	drawLineList.clear();
}
/*
void LineDrawer::DrawLine(D3DXVECTOR3 p1, D3DXVECTOR3 p2, D3DXVECTOR4 lineColor, const D3DMATRIX& worldMatrix, const D3DMATRIX& viewMatrix, const D3DMATRIX& projectionMatrix ) {
	LineVertex Vertices[2] = {
		LineVertex(p1, lineColor),//p1
		LineVertex(p2, lineColor)//p2
	};

	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* indexBuffer;
	D3D11_BUFFER_DESC vertexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData;
	// Set the number of vertices in the vertex array.
	int m_vertexCount = 2;
	// Set up the description of the static vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(LineVertex) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = Vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	HRESULT result;
	result = GRAPHICS->m_D3D->GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &VertexBuffer);
	if( FAILED(result) ){
		__debugbreak();
		return;
	}

	// Set the number of indices in the index array.
	unsigned m_indexCount = 3;
	D3D11_BUFFER_DESC indexBufferDesc;
    D3D11_SUBRESOURCE_DATA indexData;
	unsigned long* indices =  new unsigned long[m_indexCount];
	// Load the index array with data.
	for(unsigned i =0 ; i<m_indexCount ; ++i){
		indices[i] = i;
	}
	// Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	// Create the index buffer.
	result = GRAPHICS->m_D3D->GetDevice()->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
	if(FAILED(result))
	{
		return ;
	}

	UINT stride = sizeof(LineVertex);
	UINT offset = 0;
	GRAPHICS->m_D3D->GetDeviceContext()->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
	// Set the index buffer to active in the input assembler so it can be rendered.
	GRAPHICS->m_D3D->GetDeviceContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	GRAPHICS->m_D3D->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	//Render the model using the texture shader.
	result = GRAPHICS->m_ColorShader->Render( GRAPHICS->m_D3D->GetDeviceContext(), 2, worldMatrix, viewMatrix, projectionMatrix);
	if(!result){
		__debugbreak();
		return;
	}
	// Release the vertex buffer.
	SafeRelease(VertexBuffer);
	SafeRelease(indexBuffer);
	delete [] indices;
	indices = 0;
 }
 */
}