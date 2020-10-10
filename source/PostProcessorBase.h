////////////////////////////////////////////////////////////////////////////////
// Filename: postprocessorbase.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#define _XM_NO_INTRINSICS_
#include <xnamath.h>
#include <fstream>
using namespace std;

namespace Framework
{

class PostProcessorBase
{

public:

	float max(float a, float b) { return (a>b) ? a : b; }

    PostProcessorBase();
    ~PostProcessorBase();

    virtual void Initialize(ID3D11Device* device, HWND hwnd);

	virtual void Render(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* input, ID3D11RenderTargetView* output);

    virtual void AfterReset(UINT width, UINT height);
	
	// Returns a size suitable for creating a constant buffer, by rounding up
	// to the next multiple of 16
	inline UINT CBSize(UINT size)
	{
		UINT cbsize = size + (16 - (size % 16));
		return cbsize;
	}

protected:

    static const UINT_PTR MaxInputs = 4;

    struct TempRenderTarget
    {
        ID3D11Texture2D* Texture;
        ID3D11ShaderResourceView* SRView;
        ID3D11RenderTargetView* RTView;
        ID3D11UnorderedAccessView* UAView;
        UINT Width;
        UINT Height;
        DXGI_FORMAT Format;
        UINT MSCount;
        UINT MSQuality;
        BOOL InUse;
    };

    struct PSConstants
    {
        XMFLOAT2 InputSize[MaxInputs];
        XMFLOAT2 OutputSize;
    };

    struct QuadVertex
    {
        XMFLOAT4 Position;
        XMFLOAT2 TexCoord;
		XMFLOAT3 Normal;
		XMFLOAT3 Tangent;
		XMFLOAT3 Binormal;
    };

    TempRenderTarget* GetTempRenderTarget(UINT width, UINT height, DXGI_FORMAT format, UINT msCount = 1, 
                                            UINT msQuality = 0, UINT mipLevels = 1, BOOL generateMipMaps = false, 
                                            BOOL useAsUAV = false);
    void ClearTempRenderTargetCache();

    void PostProcess(ID3D11ShaderResourceView* input, ID3D11RenderTargetView* output, ID3D11PixelShader* pixelShader, const WCHAR* name);
    virtual void PostProcess(ID3D11PixelShader* pixelShader, const WCHAR* name);

    std::vector<TempRenderTarget*> tempRenderTargets;
    ID3D11Device* device;
    ID3D11DeviceContext* context;

    // Full screen quad
    ID3D11Buffer* quadVB;
    ID3D11Buffer* quadIB;
    ID3D11VertexShader* quadVS;
    ID3D11InputLayout* quadInputLayout;

    // Sampler states
    ID3D11SamplerState* pointSamplerState;
    ID3D11SamplerState* linearSamplerState;
    ID3D11SamplerState* linearWrapSamplerState;
    ID3D11SamplerState* linearBorderSamplerState;
    ID3D11SamplerState* pointBorderSamplerState;

    // Device states
    ID3D11DepthStencilState* dsState;
    ID3D11BlendState* blendState;
    ID3D11RasterizerState* rastState;    

    // Inputs and outputs
    std::vector<ID3D11ShaderResourceView*> inputs;
    std::vector<ID3D11RenderTargetView*> outputs;
    std::vector<ID3D11UnorderedAccessView*> uaViews;

    // Constant buffer
    ID3D11Buffer* psConstants;

    UINT inputWidth;
    UINT inputHeight;
};

}
