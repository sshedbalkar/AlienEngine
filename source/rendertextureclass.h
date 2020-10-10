////////////////////////////////////////////////////////////////////////////////
// Filename: rendertextureclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _RENDERTEXTURECLASS_H_
#define _RENDERTEXTURECLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#define _XM_NO_INTRINSICS_
#include <xnamath.h>

namespace Framework {

#define MIPLEVELS 1


////////////////////////////////////////////////////////////////////////////////
// Class name: RenderTextureClass
////////////////////////////////////////////////////////////////////////////////
class RenderTextureClass
{
public:
	RenderTextureClass();
	RenderTextureClass(const RenderTextureClass&);
	~RenderTextureClass();

	bool Initialize(ID3D11Device*, int, int, float, float);
	bool InitializeCubeMap(ID3D11Device*, int, int, float, float);

	bool InitDeferredRT(ID3D11Device* device,
                        int width,
                        int height,
                        DXGI_FORMAT format,
                        int numMipLevels = 1,
                        int multiSamples = 1,
                        int msQuality = 0,
                        BOOL autoGenMipMaps = false,
                        BOOL createUAV = false,
                        int arraySize = 1,
                        BOOL cubeMap = false);

	bool InitDeferredDB(ID3D11Device* device,
                        int width,
                        int height,
                        DXGI_FORMAT format = DXGI_FORMAT_D24_UNORM_S8_UINT,
                        BOOL useAsShaderResource = false,
                        int multiSamples = 1,
                        int msQuality = 0,
                        int arraySize = 1);

	void Shutdown();
	void ClearCubeDB(ID3D11DeviceContext* deviceContext);
	void SetRenderTarget(ID3D11DeviceContext*);
	void SetRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*);
	void SetEmptyRenderTarget(ID3D11DeviceContext*);
	void ClearRenderTarget(ID3D11DeviceContext*, float, float, float, float);
	ID3D11ShaderResourceView** GetShaderResourceView();
	void GetProjectionMatrix(D3DXMATRIX&);
	void GetOrthoMatrix(D3DXMATRIX&);
	ID3D11RenderTargetView* GetRTV() {return m_renderTargetView;}
	
	int GetTextureWidth();

	int GetTextureHeight();


private:
	int m_textureWidth, m_textureHeight;
	ID3D11Texture2D* m_renderTargetTexture;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11ShaderResourceView* m_shaderResourceView[3];
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilView* m_depthStencilView;
	D3D11_VIEWPORT m_viewport;
	D3DXMATRIX m_projectionMatrix;
	D3DXMATRIX m_orthoMatrix;

public:
	//Deferred variables
	ID3D11Texture2D* Texture;
    ID3D11RenderTargetView* RTView;
    ID3D11ShaderResourceView* SRView;
    ID3D11UnorderedAccessView* UAView;
	ID3D11DepthStencilView* DSView;
    ID3D11DepthStencilView* ReadOnlyDSView;
    int Width;
    int Height;
    int NumMipLevels;
    int MultiSamples;
    int MSQuality;
    DXGI_FORMAT Format;
    BOOL AutoGenMipMaps;
    int ArraySize;
    BOOL CubeMap;
    std::vector<ID3D11RenderTargetView*> ArraySlices;
	std::vector<ID3D11DepthStencilView*> DArraySlices;
};

}
#endif