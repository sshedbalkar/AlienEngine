////////////////////////////////////////////////////////////////////////////////
// Filename: postprocessor.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _POSTPROCESSOR_H_
#define _POSTPROCESSOR_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include "PostProcessorBase.h"
#include "Deferred.h"

namespace Framework {

	class PostProcessor : public PostProcessorBase
{

public:

    struct Constants
    {
        float BloomThreshold;
        float BloomMagnitude;
        float BloomBlurSigma;
        float Tau;
        float TimeDelta;
        float KeyValue;
    };

    void Initialize(ID3D11Device* device, HWND hwnd);

    void SetConstants(const Constants& constants);
    void Render(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* input,
                ID3D11RenderTargetView* output);
    void AfterReset(UINT width, UINT height);

    void Downscale(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* input,
                    ID3D11RenderTargetView* output);

    const Constants& GetConstants() const { return constantBuffer.Data; }

protected:

    void CalcAvgLuminance(ID3D11ShaderResourceView* input);
    void Bloom(ID3D11ShaderResourceView* input, TempRenderTarget*& bloomTarget);
    void ToneMap(ID3D11ShaderResourceView* input, ID3D11ShaderResourceView* bloom,
                 ID3D11RenderTargetView* output);

    ID3D11PixelShader* bloomThreshold;
    ID3D11PixelShader* bloomBlurH;
    ID3D11PixelShader* bloomBlurV;
    ID3D11PixelShader* luminanceMap;
    ID3D11PixelShader* composite;
    ID3D11PixelShader* scale;
    ID3D11PixelShader* adaptLuminance;
    ID3D11PixelShader* reflections;

    RenderTextureClass* adaptedLuminance[2];
    RenderTextureClass* exposureMap;

    UINT_PTR currLumTarget;

    ConstantBuffer<Constants> constantBuffer;
};

}
#endif