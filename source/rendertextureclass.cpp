////////////////////////////////////////////////////////////////////////////////
// Filename: rendertextureclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Precompiled.h"
#include "rendertextureclass.h"
#include "Graphics.h"

namespace Framework {

RenderTextureClass::RenderTextureClass()
{
	m_renderTargetTexture = 0;
	m_renderTargetView = 0;
	m_shaderResourceView[0] = 0;
	m_shaderResourceView[1] = 0;
	m_shaderResourceView[2] = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilView = 0;

	Width = 0;
    Height = 0;
    Format = DXGI_FORMAT_UNKNOWN;
    NumMipLevels = 0;
    MultiSamples = 0;
    MSQuality = 0;
    AutoGenMipMaps = false;
    UAView = NULL;
    ArraySize = 1;
}


RenderTextureClass::RenderTextureClass(const RenderTextureClass& other)
{
}


RenderTextureClass::~RenderTextureClass()
{
	Shutdown();
}

bool RenderTextureClass::Initialize(ID3D11Device* device, int textureWidth, int textureHeight, float screenDepth, float screenNear)
{
	// Store the width and height of the render texture.
	m_textureWidth = textureWidth;
	m_textureHeight = textureHeight;

	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT result;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;


	// Initialize the render target texture description.
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Setup the render target texture description.
	textureDesc.Width = textureWidth;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

	// Create the render target texture.
	result = device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the render target view.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	result = device->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	result = device->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, &m_shaderResourceView[0]);
	if(FAILED(result))
	{
		return false;
	}

	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = textureWidth;
	depthBufferDesc.Height = textureHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	result = device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Initailze the depth stencil view description.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	result = device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the viewport for rendering.
    m_viewport.Width = (float)textureWidth;
    m_viewport.Height = (float)textureHeight;
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;
    m_viewport.TopLeftX = 0.0f;
    m_viewport.TopLeftY = 0.0f;

	// Setup the projection matrix.
	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, ((float)D3DX_PI / 4.0f), ((float)textureWidth / (float)textureHeight), screenNear, screenDepth);

	D3DXVECTOR3 eye( 0.0f, 0.0f, -10.0f );
	D3DXVECTOR3 at( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 up( 0.0f, 1.0f, 0.0f );
	D3DXMATRIX matview;
	D3DXMatrixLookAtLH( &matview, &eye, &at, &up );

	// Create an orthographic projection matrix for 2D rendering.
	D3DXMatrixOrthoLH(&m_orthoMatrix, (float)textureWidth, (float)textureHeight, screenNear, screenDepth);
	D3DXMatrixMultiply( &m_orthoMatrix, &matview, &m_orthoMatrix );

	return true;
}


bool RenderTextureClass::InitDeferredRT(ID3D11Device* device,
										int width,
										int height,
										DXGI_FORMAT format,
										int numMipLevels,
										int multiSamples,
										int msQuality,
										BOOL autoGenMipMaps,
										BOOL createUAV,
										int arraySize,
										BOOL cubeMap)
{
    D3D11_TEXTURE2D_DESC desc;
    desc.Width = width;
    desc.Height = height;
    desc.ArraySize = arraySize;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET;
    if(createUAV)
        desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;

    desc.CPUAccessFlags = 0;
    desc.Format = format;
    desc.MipLevels = numMipLevels;
    desc.MiscFlags = (autoGenMipMaps && numMipLevels > 1) ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
    desc.SampleDesc.Count = multiSamples;
    desc.SampleDesc.Quality = msQuality;
    desc.Usage = D3D11_USAGE_DEFAULT;

    if(cubeMap)
    {
        _ASSERT(arraySize == 6);
        desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
    }

	if (FAILED((device->CreateTexture2D(&desc, NULL, &Texture))))
		__debugbreak();

    ArraySlices.clear();
    for(int i = 0; i < arraySize; ++i)
    {
        ID3D11RenderTargetView* rtView;
        D3D11_RENDER_TARGET_VIEW_DESC rtDesc;
        rtDesc.Format = format;

        if (arraySize == 1)
        {
            rtDesc.ViewDimension = multiSamples > 1 ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
            rtDesc.Texture2D.MipSlice = 0;
        }
        else
        {
            rtDesc.ViewDimension = multiSamples > 1 ? D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY : D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
            rtDesc.Texture2DArray.ArraySize = 1;
            rtDesc.Texture2DArray.FirstArraySlice = i;
            rtDesc.Texture2DArray.MipSlice = 0;
        }
		if (FAILED((device->CreateRenderTargetView(Texture, &rtDesc, &rtView))))
			__debugbreak();

        ArraySlices.push_back(rtView);
    }

    RTView = ArraySlices[0];

	if (FAILED((device->CreateShaderResourceView(Texture, NULL, &SRView))))
		__debugbreak();

    Width = width;
    Height = height;
    NumMipLevels = numMipLevels;
    MultiSamples = multiSamples;
    Format = format;
    AutoGenMipMaps = autoGenMipMaps;
    ArraySize = arraySize;
    CubeMap = cubeMap;

    if(createUAV)
	{
		if (FAILED((device->CreateUnorderedAccessView(Texture, NULL, &UAView))))
			__debugbreak();
	}


	// Setup the viewport for rendering.
    m_viewport.Width = (float)width;
    m_viewport.Height = (float)height;
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;
    m_viewport.TopLeftX = 0.0f;
    m_viewport.TopLeftY = 0.0f;
	// Setup the projection matrix.
	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, ((float)D3DX_PI / 4.0f), ((float)width / (float)height), SCREEN_NEAR, SCREEN_DEPTH);
	// Create an orthographic projection matrix for 2D rendering.
	D3DXMatrixOrthoLH(&m_orthoMatrix, (float)width, (float)height, SCREEN_NEAR, SCREEN_DEPTH);



	return true;
};



bool RenderTextureClass::InitDeferredDB(ID3D11Device* device,
										int width,
										int height,
										DXGI_FORMAT format,
										BOOL useAsShaderResource,
										int multiSamples,
										int msQuality,
										int arraySize)
{
    int bindFlags = D3D11_BIND_DEPTH_STENCIL;
    if (useAsShaderResource)
        bindFlags |= D3D11_BIND_SHADER_RESOURCE;

    DXGI_FORMAT dsTexFormat;
    if (!useAsShaderResource)
        dsTexFormat = format;
    else if (format == DXGI_FORMAT_D16_UNORM)
        dsTexFormat = DXGI_FORMAT_R16_TYPELESS;
    else if(format == DXGI_FORMAT_D24_UNORM_S8_UINT)
        dsTexFormat = DXGI_FORMAT_R24G8_TYPELESS;
    else
        dsTexFormat = DXGI_FORMAT_R32_TYPELESS;

    D3D11_TEXTURE2D_DESC desc;
    desc.Width = width;
    desc.Height = height;
    desc.ArraySize = arraySize;
    desc.BindFlags = bindFlags;
    desc.CPUAccessFlags = 0;
    desc.Format = dsTexFormat;
    desc.MipLevels = 1;
    desc.MiscFlags = 0;
    desc.SampleDesc.Count = multiSamples;
    desc.SampleDesc.Quality = msQuality;
    desc.Usage = D3D11_USAGE_DEFAULT;
	if (FAILED((device->CreateTexture2D(&desc, NULL, &Texture))))
		__debugbreak();

    DArraySlices.clear();
    for (int i = 0; i < arraySize; ++i)
    {
        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
        ID3D11DepthStencilView* dsView;
        dsvDesc.Format = format;

        if (arraySize == 1)
        {
            dsvDesc.ViewDimension = multiSamples > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
            dsvDesc.Texture2D.MipSlice = 0;
        }
        else
        {
            dsvDesc.ViewDimension = multiSamples > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY : D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
            dsvDesc.Texture2DArray.ArraySize = 1;
            dsvDesc.Texture2DArray.FirstArraySlice = i;
            dsvDesc.Texture2DArray.MipSlice = 0;
        }

        dsvDesc.Flags = 0;
		if (FAILED((device->CreateDepthStencilView(Texture, &dsvDesc, &dsView))))
			__debugbreak();

        DArraySlices.push_back(dsView);

        if (i == 0)
        {
            // Also create a read-only DSV
            dsvDesc.Flags = D3D11_DSV_READ_ONLY_DEPTH;
            if (format == DXGI_FORMAT_D24_UNORM_S8_UINT || format == DXGI_FORMAT_D32_FLOAT_S8X24_UINT)
                dsvDesc.Flags |= D3D11_DSV_READ_ONLY_STENCIL;
			if (FAILED((device->CreateDepthStencilView(Texture, &dsvDesc, &ReadOnlyDSView))))
				__debugbreak();
            dsvDesc.Flags = 0;
        }
    }

    DSView = DArraySlices[0];

    if (useAsShaderResource)
    {
        DXGI_FORMAT dsSRVFormat;
        if (format == DXGI_FORMAT_D16_UNORM)
            dsSRVFormat = DXGI_FORMAT_R16_UNORM;
        else if(format == DXGI_FORMAT_D24_UNORM_S8_UINT)
            dsSRVFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS ;
        else
            dsSRVFormat = DXGI_FORMAT_R32_FLOAT;

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        srvDesc.Format = dsSRVFormat;

        if (arraySize == 1)
        {
            srvDesc.ViewDimension = multiSamples > 1 ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = 1;
            srvDesc.Texture2D.MostDetailedMip = 0;
        }
        else
        {
            srvDesc.ViewDimension = multiSamples > 1 ? D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY : D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
            srvDesc.Texture2DArray.ArraySize = arraySize;
            srvDesc.Texture2DArray.FirstArraySlice = 0;
            srvDesc.Texture2DArray.MipLevels = 1;
            srvDesc.Texture2DArray.MostDetailedMip = 0;
        }

		if (FAILED((device->CreateShaderResourceView(Texture, &srvDesc, &SRView))))
			__debugbreak();
    }
    else
        SRView = NULL;

    Width = width;
    Height = height;
    MultiSamples = multiSamples;
    Format = format;
    ArraySize = arraySize;


	// Setup the viewport for rendering.
    m_viewport.Width = (float)width;
    m_viewport.Height = (float)height;
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;
    m_viewport.TopLeftX = 0.0f;
    m_viewport.TopLeftY = 0.0f;
	// Setup the projection matrix.
	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, ((float)D3DX_PI / 4.0f), ((float)width / (float)height), SCREEN_NEAR, SCREEN_DEPTH);
	// Create an orthographic projection matrix for 2D rendering.
	D3DXMatrixOrthoLH(&m_orthoMatrix, (float)width, (float)height, SCREEN_NEAR, SCREEN_DEPTH);


	return true;
}

bool RenderTextureClass::InitializeCubeMap(ID3D11Device* device, int textureWidth, int textureHeight, float screenDepth, float screenNear)
{	
	HRESULT result;

	// Create cubic depth stencil texture
    D3D11_TEXTURE2D_DESC dstex;
	// Initialize the render target texture description.
	//ZeroMemory(&dstex, sizeof(dstex));
    dstex.Width = textureWidth;
    dstex.Height = textureHeight;
    dstex.MipLevels = 1;
    dstex.ArraySize = 6;
    dstex.SampleDesc.Count = 1;
    dstex.SampleDesc.Quality = 0;
    dstex.Format = DXGI_FORMAT_R32_TYPELESS;
    dstex.Usage = D3D11_USAGE_DEFAULT;
    dstex.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    dstex.CPUAccessFlags = 0;
    dstex.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	// Create the render target texture.
    result = ( device->CreateTexture2D( &dstex, NULL, &m_depthStencilBuffer ));	
	if(FAILED(result))
	{
		return false;
	}

	// Create the depth stencil view for the entire cube
    D3D11_DEPTH_STENCIL_VIEW_DESC DescDS;
	// Initialize the render target texture description.
	//ZeroMemory(&DescDS, sizeof(DescDS));
    DescDS.Format = DXGI_FORMAT_D32_FLOAT;
	DescDS.Flags = 0;
    DescDS.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
    DescDS.Texture2DArray.FirstArraySlice = 0;
    DescDS.Texture2DArray.ArraySize = 6;
    DescDS.Texture2DArray.MipSlice = 0;
    result = ( device->CreateDepthStencilView( m_depthStencilBuffer, &DescDS, &m_depthStencilView ));	
	if(FAILED(result))
	{
		return false;
	}

	// Create the shader resource view for the cubic env map
    D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	// Initialize the render target texture description.
    //ZeroMemory( &SRVDesc, sizeof(SRVDesc) );
    SRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
    SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
    SRVDesc.TextureCube.MipLevels = 1;
    SRVDesc.TextureCube.MostDetailedMip = 0;
    result = ( device->CreateShaderResourceView( m_depthStencilBuffer, &SRVDesc, m_shaderResourceView ));
	if(FAILED(result))
	{
		return false;
	}

	// Setup the viewport for rendering.
    m_viewport.Width = (float)textureWidth;
    m_viewport.Height = (float)textureHeight;
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;
    m_viewport.TopLeftX = 0.0f;
    m_viewport.TopLeftY = 0.0f;

	// Setup the projection matrix.
	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, ((float)D3DX_PI / 2.0f), ((float)textureWidth / (float)textureHeight), screenNear, screenDepth);

	// Create an orthographic projection matrix for 2D rendering.
	D3DXMatrixOrthoLH(&m_orthoMatrix, (float)textureWidth, (float)textureHeight, screenNear, screenDepth);

	return true;
}

void RenderTextureClass::Shutdown()
{
	if(m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if(m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	// Release the texture resources.
	if(m_shaderResourceView[0])
	{
		m_shaderResourceView[0]->Release();
		m_shaderResourceView[0] = 0;
	}

	if(m_shaderResourceView[1])
	{
		m_shaderResourceView[1]->Release();
		m_shaderResourceView[1] = 0;
	}

	if(m_shaderResourceView[2])
	{
		m_shaderResourceView[2]->Release();
		m_shaderResourceView[2] = 0;
	}

	if(m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if(m_renderTargetTexture)
	{
		m_renderTargetTexture->Release();
		m_renderTargetTexture = 0;
	}

	return;
}


void RenderTextureClass::SetRenderTarget(ID3D11DeviceContext* deviceContext)
{
	// Bind the render target view and depth stencil buffer to the output render pipeline.
	deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
	
	// Set the viewport.
	deviceContext->RSSetViewports(1, &m_viewport);
	
	return;
}

void RenderTextureClass::SetRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* tempDSV)
{
	// Bind the render target view and depth stencil buffer to the output render pipeline.
	deviceContext->OMSetRenderTargets(1, &m_renderTargetView, tempDSV);
	
	// Set the viewport.
	deviceContext->RSSetViewports(1, &m_viewport);
	
	return;
}

void RenderTextureClass::ClearRenderTarget(ID3D11DeviceContext* deviceContext, float red, float green, float blue, float alpha)
{
	float color[4];


	// Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the back buffer.
	deviceContext->ClearRenderTargetView(m_renderTargetView, color);
    
	// Clear the depth buffer.
	deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void RenderTextureClass::ClearCubeDB(ID3D11DeviceContext* deviceContext)
{
    
	// Clear the depth buffer.
	deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void RenderTextureClass::SetEmptyRenderTarget(ID3D11DeviceContext* deviceContext)
{
	//ID3D11RenderTargetView* rtv[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {NULL};
	//deviceContext->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, rtv, m_depthStencilView); 
	deviceContext->OMSetRenderTargets(0, NULL, m_depthStencilView);

	// Set the viewport.
	deviceContext->RSSetViewports(1, &m_viewport);

}


ID3D11ShaderResourceView** RenderTextureClass::GetShaderResourceView()
{
	return m_shaderResourceView;
}


void RenderTextureClass::GetProjectionMatrix(D3DXMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}


void RenderTextureClass::GetOrthoMatrix(D3DXMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
	return;
}

int RenderTextureClass::GetTextureWidth()
{
	return m_textureWidth;
}


int RenderTextureClass::GetTextureHeight()
{
	return m_textureHeight;
}

}