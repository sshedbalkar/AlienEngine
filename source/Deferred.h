////////////////////////////////////////////////////////////////////////////////
// Filename: Deferred.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _DEFERRED_H_
#define _DEFERRED_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#define _XM_NO_INTRINSICS_
#include <xnamath.h>
#include "rendertextureclass.h"
#include "cameraclass.h"

namespace Framework {

struct CSConstants
{
    XMMATRIX View;
	XMMATRIX Projection;
	XMMATRIX InvViewProjection;
	__declspec(align(16)) XMFLOAT3 CameraPosWS;
	__declspec(align(16)) XMFLOAT2 CameraClipPlanes;
	UINT32 NumTilesX;
	UINT32 NumTilesY;
	UINT32 DisplaySizeX;
	UINT32 DisplaySizeY;

public:
	CSConstants()
	{
		View = XMMatrixIdentity();
		Projection = XMMatrixIdentity();
		InvViewProjection = XMMatrixIdentity();

		CameraPosWS = XMFLOAT3(0.0f,0.0f,0.0f);
		CameraClipPlanes = XMFLOAT2(0.0f,0.0f);
		NumTilesX = 0;
		NumTilesY = 0;
		DisplaySizeX = 0;
		DisplaySizeY = 0;
	}
};

template<typename T> class ConstantBuffer
{
public:

    T Data;

protected:

    ID3D11Buffer* buffer;
    bool initialized;

public:

    ConstantBuffer() : initialized(false)
    {
        ZeroMemory(&Data, sizeof(T));
    }

    ID3D11Buffer* Buffer() const
    {
        return buffer;
    }

    void Initialize(ID3D11Device* device)
    {
        D3D11_BUFFER_DESC desc;
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags = 0;
        desc.ByteWidth = static_cast<int>(sizeof(T) + (16 - (sizeof(T) % 16)));

       if(FAILED(device->CreateBuffer(&desc, NULL, &buffer)))
		   __debugbreak();

        initialized = true;
    }

    void ApplyChanges(ID3D11DeviceContext* deviceContext)
    {
        _ASSERT(initialized);

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        if(FAILED(deviceContext->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
			__debugbreak();
        CopyMemory(mappedResource.pData, &Data, sizeof(T));
        deviceContext->Unmap(buffer, 0);
    }

    void SetVS(ID3D11DeviceContext* deviceContext, int slot) const
    {
        _ASSERT(initialized);

        ID3D11Buffer* bufferArray[1];
        bufferArray[0] = buffer;
        deviceContext->VSSetConstantBuffers(slot, 1, bufferArray);
    }

    void SetPS(ID3D11DeviceContext* deviceContext, int slot) const
    {
        _ASSERT(initialized);

        ID3D11Buffer* bufferArray[1];
        bufferArray[0] = buffer;
        deviceContext->PSSetConstantBuffers(slot, 1, bufferArray);
    }

    void SetGS(ID3D11DeviceContext* deviceContext, int slot) const
    {
        _ASSERT(initialized);

        ID3D11Buffer* bufferArray[1];
        bufferArray[0] = buffer;
        deviceContext->GSSetConstantBuffers(slot, 1, bufferArray);
    }

    void SetHS(ID3D11DeviceContext* deviceContext, int slot) const
    {
        _ASSERT(initialized);

        ID3D11Buffer* bufferArray[1];
        bufferArray[0] = buffer;
        deviceContext->HSSetConstantBuffers(slot, 1, bufferArray);
    }

    void SetDS(ID3D11DeviceContext* deviceContext, int slot) const
    {
        _ASSERT(initialized);

        ID3D11Buffer* bufferArray[1];
        bufferArray[0] = buffer;
        deviceContext->DSSetConstantBuffers(slot, 1, bufferArray);
    }

    void SetCS(ID3D11DeviceContext* deviceContext, int slot) const
    {
        _ASSERT(initialized);

        ID3D11Buffer* bufferArray[1];
        bufferArray[0] = buffer;
        deviceContext->CSSetConstantBuffers(slot, 1, bufferArray);
    }
};

struct RWBuffer
{
    ID3D11Buffer* Buffer;
    ID3D11ShaderResourceView* SRView;
    ID3D11UnorderedAccessView* UAView;
    int Size;
    int Stride;
    int NumElements;
    BOOL RawBuffer;
    DXGI_FORMAT Format;

	RWBuffer() : Size(0), Stride(0), NumElements(0), Format(DXGI_FORMAT_UNKNOWN), RawBuffer(FALSE) {}

    bool Initialize(ID3D11Device* device, DXGI_FORMAT format, int stride, int numElements, BOOL rawBuffer = FALSE);
};

struct StructuredBuffer
{
    ID3D11Buffer* Buffer;
    ID3D11ShaderResourceView* SRView;
    ID3D11UnorderedAccessView* UAView;
    int Size;
    int Stride;
    int NumElements;

    StructuredBuffer() : Size(0), Stride(0), NumElements(0) {}

    bool Initialize(ID3D11Device* device, int stride, int numElements, BOOL useAsUAV = false,
                            BOOL appendConsume = false, BOOL useAsDrawIndirect = false, const void* initData = NULL);

  void WriteToFile(const WCHAR* path, ID3D11Device* device, ID3D11DeviceContext* context);
  void ReadFromFile(const WCHAR* path, ID3D11Device* device);
};

// Constant buffers
struct MeshVSConstants
{
    __declspec(align(16)) XMMATRIX World;
    __declspec(align(16)) XMMATRIX View;
    __declspec(align(16)) XMMATRIX WorldViewProjection;
	__declspec(align(16)) D3DXVECTOR4 Scale;
	float TextMultiplier;
	UINT32 Extendable;
};

struct MeshPSConstants
{
    __declspec(align(16)) XMFLOAT3 SpecularAlbedo;
    float Balance;
    float Roughness;
    UINT32 EnableNormalMapping;
	__declspec(align(16))D3DXVECTOR4 ConstAmbient;
	__declspec(align(16))D3DXVECTOR4 SkyColor;
	__declspec(align(16))D3DXVECTOR4 GroundColor;
	float HemiIntensity;
	UINT32 mtt;
	__declspec(align(16))D3DXVECTOR3 LightColor;
	float timer;
};

__declspec(align(16)) struct SH9Color
{
    XMVECTOR c[9];

    static SH9Color Zero();
};

struct SHConstants
{
    SH9Color AmbientDiffuse;
};

// Represents a bouncing sphere for a MeshPart
struct Sphere
{
    XMFLOAT3 Center;
    float Radius;
};

// Represents the 6 planes of a frustum
__declspec(align(16)) struct Frustum
{
    XMVECTOR Planes[6];
};



class BlendStates
{

protected:

    ID3D11BlendState* blendDisabled;
    ID3D11BlendState* additiveBlend;
    ID3D11BlendState* alphaBlend;
    ID3D11BlendState* pmAlphaBlend;
    ID3D11BlendState* noColor;
    ID3D11BlendState* alphaToCoverage;
    ID3D11BlendState* opacityBlend;

public:

    void Initialize(ID3D11Device* device);

    ID3D11BlendState* BlendDisabled () { return blendDisabled; };
    ID3D11BlendState* AdditiveBlend () { return additiveBlend; };
    ID3D11BlendState* AlphaBlend () { return alphaBlend; };
    ID3D11BlendState* PreMultipliedAlphaBlend () { return pmAlphaBlend; };
    ID3D11BlendState* ColorWriteDisabled () { return noColor; };
    ID3D11BlendState* AlphaToCoverage () { return alphaToCoverage; };
    ID3D11BlendState* OpacityBlend() { return opacityBlend; };

    static D3D11_BLEND_DESC BlendDisabledDesc();
    static D3D11_BLEND_DESC AdditiveBlendDesc();
    static D3D11_BLEND_DESC AlphaBlendDesc();
    static D3D11_BLEND_DESC PreMultipliedAlphaBlendDesc();
    static D3D11_BLEND_DESC ColorWriteDisabledDesc();
    static D3D11_BLEND_DESC AlphaToCoverageDesc();
    static D3D11_BLEND_DESC OpacityBlendDesc();
};


class RasterizerStates
{

protected:

    ID3D11RasterizerState* noCull;
    ID3D11RasterizerState* cullBackFaces;
    ID3D11RasterizerState* cullBackFacesScissor;
    ID3D11RasterizerState* cullFrontFaces;
    ID3D11RasterizerState* cullFrontFacesScissor;
    ID3D11RasterizerState* noCullNoMS;
    ID3D11RasterizerState* noCullScissor;
    ID3D11RasterizerState* wireframe;

public:

    void Initialize(ID3D11Device* device);

    ID3D11RasterizerState* NoCull() { return noCull; };
    ID3D11RasterizerState* BackFaceCull() { return cullBackFaces; };
    ID3D11RasterizerState* BackFaceCullScissor() { return cullBackFacesScissor; };
    ID3D11RasterizerState* FrontFaceCull() { return cullFrontFaces; };
    ID3D11RasterizerState* FrontFaceCullScissor() { return cullFrontFacesScissor; };
    ID3D11RasterizerState* NoCullNoMS() { return noCullNoMS; };
    ID3D11RasterizerState* NoCullScissor() { return noCullScissor; };
    ID3D11RasterizerState* Wireframe() { return wireframe; };

    static D3D11_RASTERIZER_DESC NoCullDesc();
    static D3D11_RASTERIZER_DESC FrontFaceCullDesc();
    static D3D11_RASTERIZER_DESC FrontFaceCullScissorDesc();
    static D3D11_RASTERIZER_DESC BackFaceCullDesc();
    static D3D11_RASTERIZER_DESC BackFaceCullScissorDesc();
    static D3D11_RASTERIZER_DESC NoCullNoMSDesc();
    static D3D11_RASTERIZER_DESC NoCullScissorDesc();
    static D3D11_RASTERIZER_DESC WireframeDesc();
};


class DepthStencilStates
{
    ID3D11DepthStencilState* depthDisabled;
    ID3D11DepthStencilState* depthEnabled;
    ID3D11DepthStencilState* revDepthEnabled;
    ID3D11DepthStencilState* depthWriteEnabled;
    ID3D11DepthStencilState* revDepthWriteEnabled;
    ID3D11DepthStencilState* depthStencilWriteEnabled;
    ID3D11DepthStencilState* stencilEnabled;

public:

    void Initialize(ID3D11Device* device);

    ID3D11DepthStencilState* DepthDisabled() { return depthDisabled; };
    ID3D11DepthStencilState* DepthEnabled() { return depthEnabled; };
    ID3D11DepthStencilState* ReverseDepthEnabled() { return revDepthEnabled; };
    ID3D11DepthStencilState* DepthWriteEnabled() { return depthWriteEnabled; };
    ID3D11DepthStencilState* ReverseDepthWriteEnabled() { return revDepthWriteEnabled; };
    ID3D11DepthStencilState* DepthStencilWriteEnabled() { return depthStencilWriteEnabled; };
    ID3D11DepthStencilState* StencilTestEnabled() { return depthStencilWriteEnabled; };

    static D3D11_DEPTH_STENCIL_DESC DepthDisabledDesc();
    static D3D11_DEPTH_STENCIL_DESC DepthEnabledDesc();
    static D3D11_DEPTH_STENCIL_DESC ReverseDepthEnabledDesc();
    static D3D11_DEPTH_STENCIL_DESC DepthWriteEnabledDesc();
    static D3D11_DEPTH_STENCIL_DESC ReverseDepthWriteEnabledDesc();
    static D3D11_DEPTH_STENCIL_DESC DepthStencilWriteEnabledDesc();
    static D3D11_DEPTH_STENCIL_DESC StencilEnabledDesc();
};


class SamplerStates
{

    ID3D11SamplerState* linear;
    ID3D11SamplerState* linearClamp;
    ID3D11SamplerState* linearBorder;
    ID3D11SamplerState* point;
    ID3D11SamplerState* anisotropic;
    ID3D11SamplerState* shadowMap;
    ID3D11SamplerState* shadowMapPCF;
public:

    void Initialize(ID3D11Device* device);

    ID3D11SamplerState* Linear() { return linear; };
    ID3D11SamplerState* LinearClamp() { return linearClamp; };
    ID3D11SamplerState* LinearBorder() { return linearBorder; };
    ID3D11SamplerState* Point() { return point; };
    ID3D11SamplerState* Anisotropic() { return anisotropic; };
    ID3D11SamplerState* ShadowMap() { return shadowMap; };
    ID3D11SamplerState* ShadowMapPCF() { return shadowMapPCF; };

    static D3D11_SAMPLER_DESC LinearDesc();
    static D3D11_SAMPLER_DESC LinearClampDesc();
    static D3D11_SAMPLER_DESC LinearBorderDesc();
    static D3D11_SAMPLER_DESC PointDesc();
    static D3D11_SAMPLER_DESC AnisotropicDesc();
    static D3D11_SAMPLER_DESC ShadowMapDesc();
    static D3D11_SAMPLER_DESC ShadowMapPCFDesc();
};






class Deferred {

public:

Deferred()
{	
	m_layout = 0;
}

~Deferred()
{
	// Release the layout.
	if(m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}
}

XMVECTOR DvToXv(const D3DXVECTOR3& _temp)
{
	XMVECTOR temp;

	XMFLOAT3 tempf(_temp.x,_temp.y,_temp.z);

	temp = XMLoadFloat3(&tempf);
	
	return temp;
}

XMVECTOR DvToXv(D3DXVECTOR4 _temp)
{
	XMVECTOR temp;

	XMFLOAT4 tempf(_temp.x,_temp.y,_temp.z,_temp.w);

	temp = XMLoadFloat4(&tempf);
	
	return temp;
}

XMMATRIX DmToXm(D3DXMATRIX _temp)
{
	XMMATRIX temp;

	XMFLOAT4X4 tempm(_temp._11,_temp._12,_temp._13,_temp._14,_temp._21,_temp._22,_temp._23,_temp._24,_temp._31,_temp._32,_temp._33,_temp._34,_temp._41,_temp._42,_temp._43,_temp._44);

	temp = XMLoadFloat4x4(&tempm);

	return temp;
}

ID3D11Device* device;

BlendStates blendStates;
RasterizerStates rasterizerStates;
DepthStencilStates depthStencilStates;
SamplerStates samplerStates;

//Model* model;
XMFLOAT3 lightDir;

std::vector<Sphere> boundingSpheres;
std::vector<UINT> frustumTests;
UINT numSuccessfulTests;

//DepthStencilBuffer shadowMap;

UINT_PTR currModel;

std::vector<ID3D11InputLayout*> meshGBufferInputLayouts;
ID3D11VertexShader* meshGBufferVS;
ID3D11PixelShader* meshGBufferPS;
ID3D10Blob* compiledMeshGBufferVS;

ConstantBuffer<CSConstants> csConstants;
ID3D11ComputeShader* lightTilesCS[3];	

ConstantBuffer<MeshVSConstants> meshVSConstants;
ConstantBuffer<MeshPSConstants> meshPSConstants;
ConstantBuffer<SHConstants> shConstants;

ID3D11InputLayout* m_layout;

void Initialize(ID3D11Device* device);
void RenderGBuffer(ID3D11DeviceContext* context,const  D3DXMATRIX& World,const D3DXMATRIX& View,const D3DXMATRIX& Projection, ID3D11ShaderResourceView** textureArray, int indexCount, const D3DXVECTOR3& Scale, float TextMultiplier, bool Extendable, unsigned mtt, D3DXVECTOR3& LightColor, float timer);

float timer;

};

}

#endif