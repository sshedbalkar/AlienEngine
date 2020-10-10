////////////////////////////////////////////////////////////////////////////////
// Filename: glassshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GLASSSHADERCLASS_H_
#define _GLASSSHADERCLASS_H_


//////////////
// INCLUDES //
//////////////

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
using namespace std;

namespace Framework {

////////////////////////////////////////////////////////////////////////////////
// Class name: GlassShaderClass
////////////////////////////////////////////////////////////////////////////////
class GlassShaderClass
{
private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	struct GlassBufferType
	{
		float refractionScale;
		D3DXVECTOR3 padding;
	};


public:
	GlassShaderClass();
	GlassShaderClass(const GlassShaderClass&);
	~GlassShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**, ID3D11ShaderResourceView**, float);

	void GenerateViewMatrix();
	void GenerateProjectionMatrix(float, float);

	void SetPosition(float, float, float);
	void SetLookAt(float, float, float);
	D3DXVECTOR4 GetPosition() const;
	D3DXVECTOR3 GetLookAt() const;
	void GetViewMatrix(D3DXMATRIX&) const;
	void GetProjectionMatrix(D3DXMATRIX&) const;

private:
	bool InitializeShader(ID3D11Device*, HWND, char*, char*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, char*);

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**, ID3D11ShaderResourceView**, float);
	void RenderShader(ID3D11DeviceContext*, int);

	

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_glassBuffer;

	D3DXVECTOR4 m_position;
	D3DXVECTOR3 m_lookAt;
	D3DXMATRIX m_viewMatrix;
	D3DXMATRIX m_projectionMatrix;
};
}
#endif