////////////////////////////////////////////////////////////////////////////////
// Filename: particlesystemclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _PARTICLESYSTEMCLASS_H_
#define _PARTICLESYSTEMCLASS_H_

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "texturearrayclass.h"
#include "ParticleVisual.h"
#include "ParticleComponent.h"
#include <deque>

namespace Framework
{

class ParticleShaderClass;
class ParticleComponent;
////////////////////////////////////////////////////////////////////////////////
// Class name: ParticleSystemClass
////////////////////////////////////////////////////////////////////////////////
class ParticleSystemClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR4 color;
		D3DXVECTOR2 scale;
	};

public:
	ParticleSystemClass();
	~ParticleSystemClass();

	bool Initialize(ID3D11Device*, char*, const HWND&);

	bool UpdateParticleSystem(float, ID3D11DeviceContext*, const D3DXMATRIX&, const D3DXMATRIX&, const D3DXMATRIX&, const D3DXVECTOR3& cameraPosition, ID3D11ShaderResourceView**);
	
	void RenderBuffers(ID3D11DeviceContext*);
	int GetIndexCount();

	void ClearAllParticles();//change level for example

	std::deque<ParticleVisual*> m_particleDQ;
private:
	
	bool InitializeBuffers(ID3D11Device*);
	bool UpdateBuffers(ID3D11DeviceContext*, const D3DXVECTOR3& cameraPosition);

	ParticleShaderClass* m_ParticleShader;

	int m_vertexCount;
	int m_indexCount;
	VertexType* m_vertices;
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
};
}
#endif

