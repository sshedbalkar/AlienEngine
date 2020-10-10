////////////////////////////////////////////////////////////////////////////////
// Filename: bumpmapshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _BUMPMAPSHADERCLASS_H_
#define _BUMPMAPSHADERCLASS_H_

/////////////
// GLOBALS //
/////////////
#define MAX_NUM_LIGHTS 9

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
// Class name: BumpMapShaderClass
////////////////////////////////////////////////////////////////////////////////
class BumpMapShaderClass
{
private:
	struct MatrixBufferType
	{
		D3DXMATRIX World;
		D3DXMATRIX WorldView;
		D3DXMATRIX WorldViewProjection;
		D3DXMATRIX WorldViewInverseTranspose;
		D3DXMATRIX WorldLightProjectionTexture[MAX_NUM_LIGHTS][6];
	};

	struct MatrixBufferType2
	{
		D3DXMATRIX World;
		D3DXMATRIX WorldView;
		D3DXMATRIX WorldViewProjection;
		D3DXMATRIX WorldViewInverseTranspose;
		D3DXMATRIX WorldLightProjectionTexture[MAX_NUM_LIGHTS][6];
	};

	struct ScaleBufferType
	{
		D3DXVECTOR3 Scale;
		float padding;
	};

	struct MttBufferType
	{
		unsigned mtt;
		float Transparency;
		float TextMultiplier;
		unsigned NUM_LIGHTS;
	};

	struct LightBufferType
	{
		D3DXVECTOR4 diffuseColor[MAX_NUM_LIGHTS];
		D3DXVECTOR4 ambientColor;
		float specularPower;
		float padding[3];
	};

	struct ScBufferType
	{		
		D3DXVECTOR4 specularColor;
	};

	struct AttBufferType
	{
		D3DXVECTOR4 attenuation[MAX_NUM_LIGHTS];
	};

	struct LightPositionBufferType
	{
		D3DXVECTOR4 lightPosition[MAX_NUM_LIGHTS];
	};

public:
	BumpMapShaderClass();
	BumpMapShaderClass(const BumpMapShaderClass&);
	~BumpMapShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, const D3DXMATRIX&, const D3DXMATRIX&, const D3DXMATRIX&, ID3D11ShaderResourceView**, const D3DXVECTOR3&, const D3DXVECTOR3&, unsigned, const std::vector<LightClass>&, float Transparency, float TextMultiplier);

private:
	bool InitializeShader(ID3D11Device*, HWND, char*, char*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, char*);

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**, const D3DXVECTOR3&, const D3DXVECTOR3&, unsigned, const std::vector<LightClass>&, float Transparency, float TextMultiplier);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_matrixBuffer2;
	ID3D11Buffer* m_cameraBuffer;
	ID3D11Buffer* m_scaleBuffer;
	ID3D11SamplerState* m_sampleStateWrap;
	ID3D11SamplerState* m_shadowMapSampler;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_attBuffer;
	ID3D11Buffer* m_scBuffer;
	ID3D11Buffer* m_lightPositionBuffer;
	ID3D11Buffer* m_mttBuffer;
	ID3D11ShaderResourceView* depthTextures[MAX_NUM_LIGHTS];
};
}
#endif