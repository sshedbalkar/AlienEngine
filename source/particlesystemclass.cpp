////////////////////////////////////////////////////////////////////////////////
// Filename: particlesystemclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Precompiled.h"
#include "particlesystemclass.h"
#include "particleshaderclass.h"
#include "Asset.hpp"//SafeDelete SafeRelease
#include "TimeMechanicManager.h"
#include "Graphics.h"

// Set the maximum number of particles allowed in the particle system.
const unsigned m_maxParticles = 100000;

namespace Framework
{

ParticleSystemClass::ParticleSystemClass() : 
m_ParticleShader(NULL), 
m_vertexCount(0), 
m_indexCount(0), 
m_vertices(NULL), 
m_vertexBuffer(NULL), 
m_indexBuffer(NULL)
{
}

ParticleSystemClass::~ParticleSystemClass(){
	// Release the particle shader object.
	SafeDelete(m_ParticleShader);
	// Release the buffers.
	SafeRelease(m_indexBuffer);
	// Release the vertex buffer.
	SafeRelease(m_vertexBuffer);

	if(m_vertices){
		delete [] m_vertices;
		m_vertices = NULL;
	}
	for( std::deque<ParticleVisual*>::iterator it = m_particleDQ.begin(); it != m_particleDQ.end() ; ++it ){
		delete *it;
	}
}

void ParticleSystemClass::ClearAllParticles(){
	for( std::deque<ParticleVisual*>::iterator it = m_particleDQ.begin(); it != m_particleDQ.end() ; ++it ){
		delete *it;
	}
	m_particleDQ.clear();
}

bool ParticleSystemClass::Initialize(ID3D11Device* device, char* textureFilename, const HWND& hwnd)
{
	bool result;
	// Create the particle shader object.
	m_ParticleShader = new ParticleShaderClass;
	if(!m_ParticleShader){return false; }
	// Initialize the particle shader object.
	result = m_ParticleShader->Initialize( device, hwnd);
	if(!result){
		MessageBox(hwnd, "Could not initialize the particle shader object.", "Error", MB_OK);
		return false;
	}
	// Create the buffers that will be used to render the particles with.
	result = InitializeBuffers(device);
	if(!result){return false;}
	
	return true;
}

bool ParticleSystemClass::UpdateParticleSystem(float dt, ID3D11DeviceContext* deviceContext, 
	const D3DXMATRIX& worldMatrix, const D3DXMATRIX& viewMatrix, const D3DXMATRIX& projectionMatrix, const D3DXVECTOR3& cameraPosition, ID3D11ShaderResourceView** texture ){
	// Run the frame processing for the particle system.
	bool result;

	float dt_true;

	switch( TIMEMANAGER->GetTimeState() ){
	case TIME_NORMAL:
	case TIME_FORWARD:
		dt_true = (TIMEMANAGER->IsbTimeListEnd() )? 0 : dt;
		break;
	case TIME_PAUSED:
		dt_true = 0;
		break;
	case TIME_BACK:
		dt_true = (TIMEMANAGER->IsbTimeListEnd() )? 0 : -dt;
		break;
	default://forward
		NULL;
	}
	if( GRAPHICS->IsDebugCameraON() ) dt_true = 0;

	std::deque< int > MarkDeleteDQ;
	// Update the position of the particles.
	//for( std::deque<ParticleVisual*>::iterator it = m_particleDQ.begin(); it != m_particleDQ.end() ; ++it ){
	for( unsigned i = 0 ; i<m_particleDQ.size() ; ++i ){
		if( m_particleDQ[i]->Update(dt_true) ){
			//delete *it;
			MarkDeleteDQ.push_back( i );
		}
	}

	for( int i = MarkDeleteDQ.size() -1 ; i>0 ; --i ){
		delete m_particleDQ[MarkDeleteDQ[i] ];
		m_particleDQ.erase(m_particleDQ.begin() + MarkDeleteDQ[i]);
	}

	// Update the dynamic vertex buffer with the new position of each particle.
	result = UpdateBuffers(deviceContext, cameraPosition);
	if(!result){return false;}
	
	// Put the particle system vertex and index buffers on the graphics pipeline to prepare them for drawing.
	this->RenderBuffers(deviceContext);
	// Render the model using the texture shader.
	result = m_ParticleShader->Render( deviceContext, this->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, cameraPosition, texture );
	if(!result){return false;}

	return true;
}

void ParticleSystemClass::RenderBuffers(ID3D11DeviceContext* deviceContext){
	unsigned int stride;
	unsigned int offset;
	// Set vertex buffer stride and offset.
    stride = sizeof(VertexType); 
	offset = 0;
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
    // Set the index buffer to active in the input assembler so it can be rendered.
    deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    // Set the type of primitive that should be rendered from this vertex buffer.
    deviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
}

int ParticleSystemClass::GetIndexCount() {
	return m_indexCount;
}

bool ParticleSystemClass::InitializeBuffers(ID3D11Device* device)
{
	unsigned long* indices;
	int i;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Set the maximum number of vertices in the vertex array.
	m_vertexCount = m_maxParticles;

	// Set the maximum number of indices in the index array.
	m_indexCount = m_vertexCount;
	// Create the vertex array for the particles that will be rendered.
	m_vertices = new VertexType[m_vertexCount];
	if(!m_vertices){return false;}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if(!indices){return false;}

	// Initialize vertex array to zeros at first.
	memset(m_vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// Initialize the index array.
	for(i=0; i<m_indexCount; i++){
		indices[i] = i;
	}

	// Set up the description of the dynamic vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = m_vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now finally create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result)){return false;}

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
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result)){return false;}

	// Release the index array since it is no longer needed.
	delete [] indices;
	indices = 0;

	return true;
}

bool ParticleSystemClass::UpdateBuffers(ID3D11DeviceContext* deviceContext, const D3DXVECTOR3& cameraPosition)
{
	int index;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;

	// Initialize vertex array to zeros at first.
	memset(m_vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// Now build the vertex array from the particle list array.  Each particle is a quad made out of two triangles.
	index = 0;

	for( std::deque<ParticleVisual*>::const_iterator cit = m_particleDQ.begin(); cit != m_particleDQ.end() ; ++cit ){

		// do not draw the particles with t < 0
		if( (*cit)->duration < 0 ) continue;

		/*Vec3 CenterPos = (*cit)->transform.Position;
		Vec2 m_particleSize = (*cit)->transform.Scale;

		//BillBoard Math
		Vec3 EtoC = ( cameraPosition - (*cit)->transform.Position );
		normalize( EtoC );
		Vec3 side, up;
		D3DXVec3Cross( &side, &yAxis, &EtoC);
		D3DXVec3Cross( &up, &side, &EtoC);*/

		//Point List
		m_vertices[index].position = (*cit)->transform.Position;
		m_vertices[index].color = (*cit)->color;
		m_vertices[index].scale.x = (*cit)->transform.Scale.x;
		m_vertices[index].scale.y = (*cit)->transform.Scale.y;
		index++;
	}
	
	// Lock the vertex buffer.
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result)){return false;}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)m_vertices, (sizeof(VertexType) * m_vertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(m_vertexBuffer, 0);
	
	return true;
}

}
