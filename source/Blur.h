#pragma once

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <xnamath.h>
#include <fstream>

using namespace std;

namespace Framework 
{

class HorizontalBlurShaderClass;
class VerticalBlurShaderClass;
class OrthoWindowClass;
class RenderTextureClass;
class RenderTextureBitmapClass;

class Blur
{
public:
	Blur();
	~Blur();
	void Initialize();
	void BuildOffscreenViews();
	// Blur stuff
	bool RenderBlur(ID3D11DeviceContext* context, int indexCount, D3DXMATRIX& World,const D3DXMATRIX& View,const D3DXMATRIX& Projection, ID3D11ShaderResourceView** textureArray);
	bool RenderSceneToTexture(ID3D11DeviceContext* context, int indexCount, D3DXMATRIX& World,const D3DXMATRIX& View,const D3DXMATRIX& Projection, ID3D11ShaderResourceView** textureArray);
	bool DownSampleTexture(ID3D11DeviceContext* context, int indexCount, D3DXMATRIX& World,const D3DXMATRIX& View,const D3DXMATRIX& Projection, ID3D11ShaderResourceView** textureArray, ID3D11DepthStencilView* tempDSV);
	bool RenderHorizontalBlurToTexture(ID3D11DeviceContext* context, int indexCount, D3DXMATRIX& World,const D3DXMATRIX& View,const D3DXMATRIX& Projection, ID3D11ShaderResourceView** textureArray, ID3D11DepthStencilView* tempDSV);
	bool RenderVerticalBlurToTexture(ID3D11DeviceContext* context, int indexCount, D3DXMATRIX& World,const D3DXMATRIX& View,const D3DXMATRIX& Projection, ID3D11ShaderResourceView** textureArray, ID3D11DepthStencilView* tempDSV);
	bool UpSampleTexture(ID3D11DeviceContext* context, int indexCount, D3DXMATRIX& World,const D3DXMATRIX& View,const D3DXMATRIX& Projection, ID3D11ShaderResourceView** textureArray, ID3D11DepthStencilView* tempDSV);
	bool Render2DTextureScene(ID3D11DeviceContext* context, int indexCount, D3DXMATRIX& World,const D3DXMATRIX& View,const D3DXMATRIX& Projection, ID3D11ShaderResourceView** textureArray, ID3D11DepthStencilView* tempDSV);
	bool RenderGlowScene(ID3D11DeviceContext* context, int indexCount, D3DXMATRIX& World,const D3DXMATRIX& View,const D3DXMATRIX& Projection, ID3D11ShaderResourceView** textureArray, ID3D11DepthStencilView* tempDSV);
	bool RenderTextureColor(ID3D11DeviceContext* context, int indexCount, D3DXMATRIX& World,const D3DXMATRIX& View,const D3DXMATRIX& Projection, ID3D11ShaderResourceView** textureArray, ID3D11DepthStencilView* tempDSV);

	ID3D11ShaderResourceView* mOffscreenSRV;
	ID3D11UnorderedAccessView* mOffscreenUAV;
	ID3D11RenderTargetView* mOffscreenRTV;

	RenderTextureClass *m_RenderTextureBlur, *m_DownSampleTexure, *m_HorizontalBlurTexture, *m_VerticalBlurTexture, *m_UpSampleTexure;
	OrthoWindowClass *m_SmallWindow, *m_FullScreenWindow;

private:
	HorizontalBlurShaderClass* m_HorizontalBlurShader;
	VerticalBlurShaderClass* m_VerticalBlurShader;
	RenderTextureBitmapClass* m_RenderTextureBitmapBlur;
	int downSampleWidth, downSampleHeight;
};

}