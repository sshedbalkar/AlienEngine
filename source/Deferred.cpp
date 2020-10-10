////////////////////////////////////////////////////////////////////////////////
// Filename: postprocessor.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Precompiled.h"
#include "Deferred.h"
#include "graphics.h"
#include "d3dclass.h"
#include "ShaderCompilation.h"

namespace Framework {

static const float Backup = 20.0f;
static const float NearClip = 1.0f;
static const float CascadeSplits[4] = { 0.125f, 0.25f, 0.5f, 1.0f };
static const float Bias = 0.005f;
static const UINT NumCascades = 4;

bool RWBuffer::Initialize(ID3D11Device* device, DXGI_FORMAT format, int stride, int numElements, BOOL rawBuffer)
{
    Format = format;
    Size = stride * numElements;
    Stride = stride;
    NumElements = numElements;
    RawBuffer = rawBuffer;

    if(rawBuffer)
    {
        _ASSERT(stride == 4);
        Format = DXGI_FORMAT_R32_TYPELESS;
    }

    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.ByteWidth = stride * numElements;
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = rawBuffer ? D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS : 0;
    bufferDesc.StructureByteStride = 0;
    if(FAILED(device->CreateBuffer(&bufferDesc, NULL, &Buffer)))
		__debugbreak();

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = Format;

    if(rawBuffer)
    {
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
        srvDesc.BufferEx.FirstElement = 0;
        srvDesc.BufferEx.NumElements = numElements;
        srvDesc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
    }
    else
    {
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        srvDesc.Buffer.ElementOffset = 0;
        srvDesc.Buffer.ElementWidth = numElements;
    }

    if(FAILED(device->CreateShaderResourceView(Buffer, &srvDesc, &SRView)))
		__debugbreak();

    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
    uavDesc.Format = format;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uavDesc.Buffer.FirstElement = 0;
    uavDesc.Buffer.NumElements = numElements;
    uavDesc.Buffer.Flags = rawBuffer ? D3D11_BUFFER_UAV_FLAG_RAW : 0;

    if(FAILED(device->CreateUnorderedAccessView(Buffer, &uavDesc, &UAView)))
		__debugbreak();

	return true;
}

bool StructuredBuffer::Initialize(ID3D11Device* device, int stride, int numElements, BOOL useAsUAV,
                                    BOOL appendConsume, BOOL useAsDrawIndirect, const void* initData)
{
	if(numElements<1)
		return true;
    Size = stride * numElements;
    Stride = stride;
    NumElements = numElements;

    if(appendConsume || useAsDrawIndirect)
        useAsUAV = true;

    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.ByteWidth = stride * numElements;
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    bufferDesc.BindFlags |= useAsUAV ? D3D11_BIND_UNORDERED_ACCESS : 0;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bufferDesc.MiscFlags |= useAsDrawIndirect ? D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS : 0;
    bufferDesc.StructureByteStride = stride;

    D3D11_SUBRESOURCE_DATA subresourceData;
    subresourceData.pSysMem = initData;
    subresourceData.SysMemPitch = 0;
    subresourceData.SysMemSlicePitch = 0;

    if(FAILED(device->CreateBuffer(&bufferDesc, initData != NULL ? &subresourceData : NULL, &Buffer)))
		__debugbreak();

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.ElementOffset = 0;
    srvDesc.Buffer.ElementWidth = numElements;
    if(FAILED(device->CreateShaderResourceView(Buffer, &srvDesc, &SRView)))
		__debugbreak();

    if(useAsUAV)
    {
        D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
        uavDesc.Format = DXGI_FORMAT_UNKNOWN;
        uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        uavDesc.Buffer.FirstElement = 0;
        uavDesc.Buffer.Flags = appendConsume ? D3D11_BUFFER_UAV_FLAG_APPEND : 0;
        uavDesc.Buffer.NumElements = numElements;
        if(FAILED(device->CreateUnorderedAccessView(Buffer, &uavDesc, &UAView)))
			__debugbreak();
    }

	return true;
}


void BlendStates::Initialize(ID3D11Device* device)
{
    if(FAILED(device->CreateBlendState(&BlendDisabledDesc(), &blendDisabled)))
		__debugbreak();
    if(FAILED(device->CreateBlendState(&AdditiveBlendDesc(), &additiveBlend)))
		__debugbreak();
    if(FAILED(device->CreateBlendState(&AlphaBlendDesc(), &alphaBlend)))
		__debugbreak();
    if(FAILED(device->CreateBlendState(&PreMultipliedAlphaBlendDesc(), &pmAlphaBlend)))
		__debugbreak();
    if(FAILED(device->CreateBlendState(&ColorWriteDisabledDesc(), &noColor)))
		__debugbreak();
    if(FAILED(device->CreateBlendState(&AlphaToCoverageDesc(), &alphaToCoverage)))
		__debugbreak();
    if(FAILED(device->CreateBlendState(&OpacityBlendDesc(), &opacityBlend)))
		__debugbreak();
}

D3D11_BLEND_DESC BlendStates::BlendDisabledDesc()
{
    D3D11_BLEND_DESC blendDesc;
    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.IndependentBlendEnable = false;
    for (UINT i = 0; i < 8; ++i)
    {
        blendDesc.RenderTarget[i].BlendEnable = false;
        blendDesc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        blendDesc.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
    }

    return blendDesc;
}

D3D11_BLEND_DESC BlendStates::AdditiveBlendDesc()
{
    D3D11_BLEND_DESC blendDesc;
    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.IndependentBlendEnable = false;
    for (UINT i = 0; i < 8; ++i)
    {
        blendDesc.RenderTarget[i].BlendEnable = true;
        blendDesc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[i].DestBlend = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        blendDesc.RenderTarget[i].SrcBlend = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
    }

    return blendDesc;
}

D3D11_BLEND_DESC BlendStates::AlphaBlendDesc()
{
    D3D11_BLEND_DESC blendDesc;
    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.IndependentBlendEnable = true;
    for (UINT i = 0; i < 8; ++i)
    {
        blendDesc.RenderTarget[i].BlendEnable = true;
        blendDesc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        blendDesc.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
    }

    blendDesc.RenderTarget[0].BlendEnable = true;

    return blendDesc;
}

D3D11_BLEND_DESC BlendStates::PreMultipliedAlphaBlendDesc()
{
    D3D11_BLEND_DESC blendDesc;
    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.IndependentBlendEnable = false;
    for (UINT i = 0; i < 8; ++i)
    {
        blendDesc.RenderTarget[i].BlendEnable = false;
        blendDesc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        blendDesc.RenderTarget[i].SrcBlend = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
    }

    return blendDesc;
}

D3D11_BLEND_DESC BlendStates::ColorWriteDisabledDesc()
{
    D3D11_BLEND_DESC blendDesc;
    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.IndependentBlendEnable = false;
    for (UINT i = 0; i < 8; ++i)
    {
        blendDesc.RenderTarget[i].BlendEnable = false;
        blendDesc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[i].RenderTargetWriteMask = 0;
        blendDesc.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
    }

    return blendDesc;
}

D3D11_BLEND_DESC BlendStates::AlphaToCoverageDesc()
{
    D3D11_BLEND_DESC blendDesc;
    blendDesc.AlphaToCoverageEnable = true;
    blendDesc.IndependentBlendEnable = false;
    for (UINT i = 0; i < 8; ++i)
    {
        blendDesc.RenderTarget[i].BlendEnable = false;
        blendDesc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        blendDesc.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
    }

    return blendDesc;
}

D3D11_BLEND_DESC BlendStates::OpacityBlendDesc()
{
    D3D11_BLEND_DESC blendDesc;
    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.IndependentBlendEnable = false;
    for (UINT i = 0; i < 8; ++i)
    {
        blendDesc.RenderTarget[i].BlendEnable = true;
        blendDesc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC1_COLOR;
        blendDesc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        blendDesc.RenderTarget[i].SrcBlend = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
    }

    return blendDesc;
}

void RasterizerStates::Initialize(ID3D11Device* device)
{
    if(FAILED(device->CreateRasterizerState(&NoCullDesc(), &noCull)))
		__debugbreak();
    if(FAILED(device->CreateRasterizerState(&FrontFaceCullDesc(), &cullFrontFaces)))
		__debugbreak();
    if(FAILED(device->CreateRasterizerState(&FrontFaceCullScissorDesc(), &cullFrontFacesScissor)))
		__debugbreak();
    if(FAILED(device->CreateRasterizerState(&BackFaceCullDesc(), &cullBackFaces)))
		__debugbreak();
    if(FAILED(device->CreateRasterizerState(&BackFaceCullScissorDesc(), &cullBackFacesScissor)))
		__debugbreak();
    if(FAILED(device->CreateRasterizerState(&NoCullNoMSDesc(), &noCullNoMS)))
		__debugbreak();
    if(FAILED(device->CreateRasterizerState(&NoCullScissorDesc(), &noCullScissor)))
		__debugbreak();
    if(FAILED(device->CreateRasterizerState(&WireframeDesc(), &wireframe)))
		__debugbreak();
}

D3D11_RASTERIZER_DESC RasterizerStates::NoCullDesc()
{
    D3D11_RASTERIZER_DESC rastDesc;

    rastDesc.AntialiasedLineEnable = FALSE;
    rastDesc.CullMode = D3D11_CULL_NONE;
    rastDesc.DepthBias = 0;
    rastDesc.DepthBiasClamp = 0.0f;
    rastDesc.DepthClipEnable = TRUE;
    rastDesc.FillMode = D3D11_FILL_SOLID;
    rastDesc.FrontCounterClockwise = false;
    rastDesc.MultisampleEnable = true;
    rastDesc.ScissorEnable = false;
    rastDesc.SlopeScaledDepthBias = 0;

    return rastDesc;
}

D3D11_RASTERIZER_DESC RasterizerStates::FrontFaceCullDesc()
{
    D3D11_RASTERIZER_DESC rastDesc;

    rastDesc.AntialiasedLineEnable = FALSE;
    rastDesc.CullMode = D3D11_CULL_FRONT;
    rastDesc.DepthBias = 0;
    rastDesc.DepthBiasClamp = 0.0f;
    rastDesc.DepthClipEnable = TRUE;
    rastDesc.FillMode = D3D11_FILL_SOLID;
    rastDesc.FrontCounterClockwise = false;
    rastDesc.MultisampleEnable = true;
    rastDesc.ScissorEnable = false;
    rastDesc.SlopeScaledDepthBias = 0;

    return rastDesc;
}

D3D11_RASTERIZER_DESC RasterizerStates::FrontFaceCullScissorDesc()
{
    D3D11_RASTERIZER_DESC rastDesc;

    rastDesc.AntialiasedLineEnable = FALSE;
    rastDesc.CullMode = D3D11_CULL_FRONT;
    rastDesc.DepthBias = 0;
    rastDesc.DepthBiasClamp = 0.0f;
    rastDesc.DepthClipEnable = TRUE;
    rastDesc.FillMode = D3D11_FILL_SOLID;
    rastDesc.FrontCounterClockwise = TRUE;
    rastDesc.MultisampleEnable = TRUE;
    rastDesc.ScissorEnable = TRUE;
    rastDesc.SlopeScaledDepthBias = 0;

    return rastDesc;
}

D3D11_RASTERIZER_DESC RasterizerStates::BackFaceCullDesc()
{
    D3D11_RASTERIZER_DESC rastDesc;

    rastDesc.AntialiasedLineEnable = FALSE;
    rastDesc.CullMode = D3D11_CULL_BACK;
    rastDesc.DepthBias = 0;
    rastDesc.DepthBiasClamp = 0.0f;
    rastDesc.DepthClipEnable = TRUE;
    rastDesc.FillMode = D3D11_FILL_SOLID;
    rastDesc.FrontCounterClockwise = false;
    rastDesc.MultisampleEnable = true;
    rastDesc.ScissorEnable = false;
    rastDesc.SlopeScaledDepthBias = 0;

    return rastDesc;
}

D3D11_RASTERIZER_DESC RasterizerStates::BackFaceCullScissorDesc()
{
    D3D11_RASTERIZER_DESC rastDesc;

    rastDesc.AntialiasedLineEnable = FALSE;
    rastDesc.CullMode = D3D11_CULL_BACK;
    rastDesc.DepthBias = 0;
    rastDesc.DepthBiasClamp = 0.0f;
    rastDesc.DepthClipEnable = TRUE;
    rastDesc.FillMode = D3D11_FILL_SOLID;
    rastDesc.FrontCounterClockwise = FALSE;
    rastDesc.MultisampleEnable = TRUE;
    rastDesc.ScissorEnable = TRUE;
    rastDesc.SlopeScaledDepthBias = 0;

    return rastDesc;
}

D3D11_RASTERIZER_DESC RasterizerStates::NoCullNoMSDesc()
{
    D3D11_RASTERIZER_DESC rastDesc;

    rastDesc.AntialiasedLineEnable = FALSE;
    rastDesc.CullMode = D3D11_CULL_NONE;
    rastDesc.DepthBias = 0;
    rastDesc.DepthBiasClamp = 0.0f;
    rastDesc.DepthClipEnable = TRUE;
    rastDesc.FillMode = D3D11_FILL_SOLID;
    rastDesc.FrontCounterClockwise = false;
    rastDesc.MultisampleEnable = false;
    rastDesc.ScissorEnable = false;
    rastDesc.SlopeScaledDepthBias = 0;

    return rastDesc;
}

D3D11_RASTERIZER_DESC RasterizerStates::NoCullScissorDesc()
{
    D3D11_RASTERIZER_DESC rastDesc;

    rastDesc.AntialiasedLineEnable = FALSE;
    rastDesc.CullMode = D3D11_CULL_NONE;
    rastDesc.DepthBias = 0;
    rastDesc.DepthBiasClamp = 0.0f;
    rastDesc.DepthClipEnable = TRUE;
    rastDesc.FillMode = D3D11_FILL_SOLID;
    rastDesc.FrontCounterClockwise = false;
    rastDesc.MultisampleEnable = true;
    rastDesc.ScissorEnable = true;
    rastDesc.SlopeScaledDepthBias = 0;

    return rastDesc;
}

D3D11_RASTERIZER_DESC RasterizerStates::WireframeDesc()
{
    D3D11_RASTERIZER_DESC rastDesc;

    rastDesc.AntialiasedLineEnable = FALSE;
    rastDesc.CullMode = D3D11_CULL_NONE;
    rastDesc.DepthBias = 0;
    rastDesc.DepthBiasClamp = 0.0f;
    rastDesc.DepthClipEnable = TRUE;
    rastDesc.FillMode = D3D11_FILL_WIREFRAME;
    rastDesc.FrontCounterClockwise = false;
    rastDesc.MultisampleEnable = true;
    rastDesc.ScissorEnable = false;
    rastDesc.SlopeScaledDepthBias = 0;

    return rastDesc;
}

void DepthStencilStates::Initialize(ID3D11Device* device)
{
    if(FAILED(device->CreateDepthStencilState(&DepthDisabledDesc(), &depthDisabled)))
		__debugbreak();
    if(FAILED(device->CreateDepthStencilState(&DepthEnabledDesc(), &depthEnabled)))
		__debugbreak();
    if(FAILED(device->CreateDepthStencilState(&ReverseDepthEnabledDesc(), &revDepthEnabled)))
		__debugbreak();
    if(FAILED(device->CreateDepthStencilState(&DepthWriteEnabledDesc(), &depthWriteEnabled)))
		__debugbreak();
    if(FAILED(device->CreateDepthStencilState(&ReverseDepthWriteEnabledDesc(), &revDepthWriteEnabled)))
		__debugbreak();
    if(FAILED(device->CreateDepthStencilState(&DepthStencilWriteEnabledDesc(), &depthStencilWriteEnabled)))
		__debugbreak();
    if(FAILED(device->CreateDepthStencilState(&StencilEnabledDesc(), &stencilEnabled)))
		__debugbreak();
}

D3D11_DEPTH_STENCIL_DESC DepthStencilStates::DepthDisabledDesc()
{
    D3D11_DEPTH_STENCIL_DESC dsDesc;
    dsDesc.DepthEnable = false;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    dsDesc.StencilEnable = false;
    dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    dsDesc.BackFace = dsDesc.FrontFace;

    return dsDesc;
}

D3D11_DEPTH_STENCIL_DESC DepthStencilStates::DepthEnabledDesc()
{
    D3D11_DEPTH_STENCIL_DESC dsDesc;
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    dsDesc.StencilEnable = false;
    dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    dsDesc.BackFace = dsDesc.FrontFace;

    return dsDesc;
}

D3D11_DEPTH_STENCIL_DESC DepthStencilStates::ReverseDepthEnabledDesc()
{
    D3D11_DEPTH_STENCIL_DESC dsDesc;
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
    dsDesc.StencilEnable = false;
    dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    dsDesc.BackFace = dsDesc.FrontFace;

    return dsDesc;
}

D3D11_DEPTH_STENCIL_DESC DepthStencilStates::DepthWriteEnabledDesc()
{
    D3D11_DEPTH_STENCIL_DESC dsDesc;
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    dsDesc.StencilEnable = false;
    dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    dsDesc.BackFace = dsDesc.FrontFace;

    return dsDesc;
}

D3D11_DEPTH_STENCIL_DESC DepthStencilStates::ReverseDepthWriteEnabledDesc()
{
    D3D11_DEPTH_STENCIL_DESC dsDesc;
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
    dsDesc.StencilEnable = false;
    dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    dsDesc.BackFace = dsDesc.FrontFace;

    return dsDesc;
}

D3D11_DEPTH_STENCIL_DESC DepthStencilStates::DepthStencilWriteEnabledDesc()
{
    D3D11_DEPTH_STENCIL_DESC dsDesc;
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    dsDesc.StencilEnable = true;
    dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    dsDesc.BackFace = dsDesc.FrontFace;

    return dsDesc;
}

D3D11_DEPTH_STENCIL_DESC DepthStencilStates::StencilEnabledDesc()
{
    D3D11_DEPTH_STENCIL_DESC dsDesc;
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    dsDesc.StencilEnable = true;
    dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    dsDesc.StencilWriteMask = 0;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
    dsDesc.BackFace = dsDesc.FrontFace;

    return dsDesc;
}

void SamplerStates::Initialize(ID3D11Device* device)
{
    if(FAILED(device->CreateSamplerState(&LinearDesc(), &linear)))
		__debugbreak();    
    if(FAILED(device->CreateSamplerState(&LinearClampDesc(), &linearClamp)))
		__debugbreak();
    if(FAILED(device->CreateSamplerState(&LinearBorderDesc(), &linearBorder)))
		__debugbreak();
    if(FAILED(device->CreateSamplerState(&PointDesc(), &point)))
		__debugbreak();
    if(FAILED(device->CreateSamplerState(&AnisotropicDesc(), &anisotropic)))
		__debugbreak();
    if(FAILED(device->CreateSamplerState(&ShadowMapDesc(), &shadowMap)))
		__debugbreak();
    if(FAILED(device->CreateSamplerState(&ShadowMapPCFDesc(), &shadowMapPCF)))
		__debugbreak();
}

D3D11_SAMPLER_DESC SamplerStates::LinearDesc()
{
    D3D11_SAMPLER_DESC sampDesc;

    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.MipLODBias = 0.0f;
    sampDesc.MaxAnisotropy = 1;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    sampDesc.BorderColor[0] = sampDesc.BorderColor[1] = sampDesc.BorderColor[2] = sampDesc.BorderColor[3] = 0;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    return sampDesc;
}

D3D11_SAMPLER_DESC SamplerStates::LinearClampDesc()
{
    D3D11_SAMPLER_DESC sampDesc;

    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.MipLODBias = 0.0f;
    sampDesc.MaxAnisotropy = 1;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    sampDesc.BorderColor[0] = sampDesc.BorderColor[1] = sampDesc.BorderColor[2] = sampDesc.BorderColor[3] = 0;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    return sampDesc;
}

D3D11_SAMPLER_DESC SamplerStates::LinearBorderDesc()
{
    D3D11_SAMPLER_DESC sampDesc;

    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    sampDesc.MipLODBias = 0.0f;
    sampDesc.MaxAnisotropy = 1;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    sampDesc.BorderColor[0] = sampDesc.BorderColor[1] = sampDesc.BorderColor[2] = sampDesc.BorderColor[3] = 0;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    return sampDesc;
}

D3D11_SAMPLER_DESC SamplerStates::PointDesc()
{
    D3D11_SAMPLER_DESC sampDesc;

    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.MipLODBias = 0.0f;
    sampDesc.MaxAnisotropy = 1;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    sampDesc.BorderColor[0] = sampDesc.BorderColor[1] = sampDesc.BorderColor[2] = sampDesc.BorderColor[3] = 0;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    return sampDesc;
}

D3D11_SAMPLER_DESC SamplerStates::AnisotropicDesc()
{
    D3D11_SAMPLER_DESC sampDesc;

    sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.MipLODBias = 0.0f;
    sampDesc.MaxAnisotropy = 16;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    sampDesc.BorderColor[0] = sampDesc.BorderColor[1] = sampDesc.BorderColor[2] = sampDesc.BorderColor[3] = 0;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    return sampDesc;
}

D3D11_SAMPLER_DESC SamplerStates::ShadowMapDesc()
{
    D3D11_SAMPLER_DESC sampDesc;

    sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.MipLODBias = 0.0f;
    sampDesc.MaxAnisotropy = 1;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
    sampDesc.BorderColor[0] = sampDesc.BorderColor[1] = sampDesc.BorderColor[2] = sampDesc.BorderColor[3] = 0;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    return sampDesc;
}

D3D11_SAMPLER_DESC SamplerStates::ShadowMapPCFDesc()
{
    D3D11_SAMPLER_DESC sampDesc;

    sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.MipLODBias = 0.0f;
    sampDesc.MaxAnisotropy = 1;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
    sampDesc.BorderColor[0] = sampDesc.BorderColor[1] = sampDesc.BorderColor[2] = sampDesc.BorderColor[3] = 0;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    return sampDesc;
}

void Deferred::Initialize(ID3D11Device* device)
{
	this->device = device;

    blendStates.Initialize(device);
    rasterizerStates.Initialize(device);
    depthStencilStates.Initialize(device);
    samplerStates.Initialize(device);

    meshVSConstants.Initialize(device);
    meshPSConstants.Initialize(device);

    this->lightDir = lightDir;

    shConstants.Initialize(device);

    meshGBufferVS = CompileVSFromFile(device, L"Shaders//GBuffer.hlsl", "VS", "vs_5_0", NULL, NULL, &compiledMeshGBufferVS);
    meshGBufferPS = CompilePSFromFile(device, L"Shaders//GBuffer.hlsl", "PS", "ps_5_0");
		
	D3D11_INPUT_ELEMENT_DESC polygonLayout[5];
	unsigned int numElements;

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClassTangent and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;		
		
	polygonLayout[3].SemanticName = "TANGENT";
	polygonLayout[3].SemanticIndex = 0;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[3].InputSlot = 0;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[3].InstanceDataStepRate = 0;

	polygonLayout[4].SemanticName = "BINORMAL";
	polygonLayout[4].SemanticIndex = 0;
	polygonLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[4].InputSlot = 0;
	polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[4].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	if(FAILED(device->CreateInputLayout(polygonLayout, numElements,
            compiledMeshGBufferVS->GetBufferPointer(), compiledMeshGBufferVS->GetBufferSize(), &m_layout)))
		__debugbreak();
}

//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------

void Deferred::RenderGBuffer(ID3D11DeviceContext* context,const  D3DXMATRIX& World,const D3DXMATRIX& View,const D3DXMATRIX& Projection, ID3D11ShaderResourceView** textureArray, int indexCount, const D3DXVECTOR3& Scale, float TextMultiplier, bool Extendable, unsigned mtt, D3DXVECTOR3& LightColor, float timer)
{	
	XMMATRIX W, V, P, WVP, VP;

	P = DmToXm(Projection);
	V = DmToXm(View);
	W = DmToXm(World);

	VP = XMMatrixMultiply(V,P);
	WVP = XMMatrixMultiply(W,VP);

    // Set states
    float blendFactor[4] = {1, 1, 1, 1};
    context->OMSetBlendState(blendStates.BlendDisabled(), blendFactor, 0xFFFFFFFF);

    context->OMSetDepthStencilState(depthStencilStates.DepthWriteEnabled(), 0);

    context->RSSetState(rasterizerStates.BackFaceCull());

	ID3D11SamplerState* sampStates[1] = {
        samplerStates.Anisotropic(),
    };
    context->PSSetSamplers(0, 1, sampStates);

    // Set constant buffers
	meshVSConstants.Data.World = XMMatrixTranspose(W);
	meshVSConstants.Data.View = XMMatrixTranspose(V);
    meshVSConstants.Data.WorldViewProjection = XMMatrixTranspose(WVP);
	meshVSConstants.Data.Scale = D3DXVECTOR4(Scale, 0.0f);
	meshVSConstants.Data.TextMultiplier = TextMultiplier;
	meshVSConstants.Data.Extendable = Extendable;
	meshVSConstants.ApplyChanges(context);
    meshVSConstants.SetVS(context, 0);
    
	float si = GRAPHICS->GetSpecularIntensity();

    meshPSConstants.Data.SpecularAlbedo = XMFLOAT3(GRAPHICS->GetSpecularR() * si, GRAPHICS->GetSpecularG() * si, GRAPHICS->GetSpecularB() * si);
    meshPSConstants.Data.Balance = GRAPHICS->GetBalance();
    meshPSConstants.Data.Roughness = GRAPHICS->GetRoughness();
    meshPSConstants.Data.EnableNormalMapping = textureArray[1] != NULL;
	meshPSConstants.Data.ConstAmbient = GRAPHICS->GetConstAmbient();
	meshPSConstants.Data.SkyColor = GRAPHICS->GetSkyColor();
	meshPSConstants.Data.GroundColor = GRAPHICS->GetGroundColor();
	meshPSConstants.Data.HemiIntensity = GRAPHICS->GetHemiIntensity();
	meshPSConstants.Data.mtt = mtt;
	meshPSConstants.Data.LightColor = LightColor;
	meshPSConstants.Data.timer = timer;
    meshPSConstants.ApplyChanges(context);
    meshPSConstants.SetPS(context, 0);

	// Set shaders
    context->DSSetShader(NULL, NULL, 0);
    context->HSSetShader(NULL, NULL, 0);
    context->GSSetShader(NULL, NULL, 0);
	context->CSSetShader(NULL, NULL, 0);
    context->VSSetShader(meshGBufferVS , NULL, 0);
    context->PSSetShader(meshGBufferPS, NULL, 0);

	// Set the input layout
	context->IASetInputLayout(m_layout);

	// Set the textures
    context->PSSetShaderResources(0, 3, textureArray);

    context->DrawIndexed(indexCount, 0, 0);

    ID3D11ShaderResourceView* nullSRVs[2] = { NULL };
    context->PSSetShaderResources(0, 2, nullSRVs);
}




}