#include "Precompiled.h"
#include "Blur.h"
#include "Graphics.h"
#include "horizontalblurshaderclass.h"
#include "verticalblurshaderclass.h"
#include "orthowindowclass.h"
#include "d3dclass.h"
#include "rendertextureclass.h"
#include "textureshaderclass.h"
#include "glowshaderclass.h"
#include "glowmapshaderclass.h"

namespace Framework 
{

Blur::Blur() :  
m_HorizontalBlurShader(NULL),
m_VerticalBlurShader(NULL),
m_RenderTextureBlur(NULL),
m_DownSampleTexure(NULL),
m_HorizontalBlurTexture(NULL),
m_VerticalBlurTexture(NULL),
m_UpSampleTexure(NULL),
m_SmallWindow(NULL),
m_FullScreenWindow(NULL),
mOffscreenRTV(NULL),
mOffscreenUAV(NULL),
mOffscreenSRV(NULL)
{
}

Blur::~Blur()
{
	// Release the full screen ortho window object.
	if(m_FullScreenWindow)
	{
		m_FullScreenWindow->Shutdown();
		delete m_FullScreenWindow;
		m_FullScreenWindow = 0;
	}
	// Release the small ortho window object.
	if(m_SmallWindow)
	{
		m_SmallWindow->Shutdown();
		delete m_SmallWindow;
		m_SmallWindow = 0;
	}	
	// Release the up sample render to texture object.
	if(m_UpSampleTexure)
	{
		m_UpSampleTexure->Shutdown();
		delete m_UpSampleTexure;
		m_UpSampleTexure = 0;
	}
	// Release the vertical blur render to texture object.
	if(m_VerticalBlurTexture)
	{
		m_VerticalBlurTexture->Shutdown();
		delete m_VerticalBlurTexture;
		m_VerticalBlurTexture = 0;
	}
	// Release the horizontal blur render to texture object.
	if(m_HorizontalBlurTexture)
	{
		m_HorizontalBlurTexture->Shutdown();
		delete m_HorizontalBlurTexture;
		m_HorizontalBlurTexture = 0;
	}
	// Release the down sample render to texture object.
	if(m_DownSampleTexure)
	{
		m_DownSampleTexure->Shutdown();
		delete m_DownSampleTexure;
		m_DownSampleTexure = 0;
	}	
	// Release the render to texture object.
	if(m_RenderTextureBlur)
	{
		m_RenderTextureBlur->Shutdown();
		delete m_RenderTextureBlur;
		m_RenderTextureBlur = 0;
	}	
	// Release the vertical blur shader object.
	if(m_VerticalBlurShader)
	{
		m_VerticalBlurShader->Shutdown();
		delete m_VerticalBlurShader;
		m_VerticalBlurShader = 0;
	}

	// Release the horizontal blur shader object.
	if(m_HorizontalBlurShader)
	{
		m_HorizontalBlurShader->Shutdown();
		delete m_HorizontalBlurShader;
		m_HorizontalBlurShader = 0;
	}

	if(mOffscreenRTV)
	{
		mOffscreenRTV->Release();
		mOffscreenRTV=0;
	}

	if(mOffscreenUAV)
	{
		mOffscreenUAV->Release();
		mOffscreenUAV = 0;
	}

	if(mOffscreenSRV)
	{
		mOffscreenSRV->Release();
		mOffscreenSRV = 0;
	}	
}

void Blur::BuildOffscreenViews()
{
	ID3D11Device* device = GRAPHICS->GetD3D()->GetDevice();

	// We call this function everytime the window is resized so that the render target is a quarter
	// the client area dimensions.  So Release the previous views before we create new ones.
	if(mOffscreenRTV)
	{
		mOffscreenRTV->Release();
		mOffscreenRTV=0;
	}
	if(mOffscreenUAV)
	{
		mOffscreenUAV->Release();
		mOffscreenUAV = 0;
	}
	if(mOffscreenSRV)
	{
		mOffscreenSRV->Release();
		mOffscreenSRV = 0;
	}

	D3D11_TEXTURE2D_DESC texDesc;
	
	texDesc.Width     = GRAPHICS->screen_width;
	texDesc.Height    = GRAPHICS->screen_height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format    = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count   = 1;  
	texDesc.SampleDesc.Quality = 0;  
	texDesc.Usage          = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags      = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	texDesc.CPUAccessFlags = 0; 
	texDesc.MiscFlags      = 0;

	ID3D11Texture2D* offscreenTex = 0;
	if(FAILED(device->CreateTexture2D(&texDesc, 0, &offscreenTex)))
		__debugbreak();

	// Null description means to create a view to all mipmap levels using 
	// the format the texture was created with.
	if(FAILED(device->CreateShaderResourceView(offscreenTex, 0, &mOffscreenSRV)))
		__debugbreak();
	if(FAILED(device->CreateRenderTargetView(offscreenTex, 0, &mOffscreenRTV)))
		__debugbreak();
	if(FAILED(device->CreateUnorderedAccessView(offscreenTex, 0, &mOffscreenUAV)))
		__debugbreak();

	// View saves a reference to the texture so we can release our reference.
	if(offscreenTex)
	{
		offscreenTex->Release();
		offscreenTex = 0;
	}
}

void Blur::Initialize()
{
	// Blur Inits
	// Set the size to sample down to.
	downSampleWidth = GRAPHICS->screen_width / 2;
	downSampleHeight = GRAPHICS->screen_height / 2;
	bool result;

	HWND hwnd = GRAPHICS->GetH();
	ID3D11Device* device = GRAPHICS->GetD3D()->GetDevice();

	// Create the horizontal blur shader object.
	m_HorizontalBlurShader = new HorizontalBlurShaderClass;
	if(!m_HorizontalBlurShader)
	{
		return;
	}

	// Initialize the horizontal blur shader object.
	result = m_HorizontalBlurShader->Initialize(device, hwnd);
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize the horizontal blur shader object.", "Error", MB_OK);
		return;
	}

	// Create the vertical blur shader object.
	m_VerticalBlurShader = new VerticalBlurShaderClass;
	if(!m_VerticalBlurShader)
	{
		return;
	}

	// Initialize the vertical blur shader object.
	result = m_VerticalBlurShader->Initialize(device, hwnd);
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize the vertical blur shader object.", "Error", MB_OK);
		return;
	}

	// Create the render to texture object.
	m_RenderTextureBlur = new RenderTextureClass;
	if(!m_RenderTextureBlur)
	{
		return;
	}

	// Initialize the render to texture object.
	result = m_RenderTextureBlur->Initialize(device, GRAPHICS->screen_width, GRAPHICS->screen_height, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize the render to texture object.", "Error", MB_OK);
		return;
	}

	// Create the down sample render to texture object.
	m_DownSampleTexure = new RenderTextureClass;
	if(!m_DownSampleTexure)
	{
		return;
	}

	// Initialize the down sample render to texture object.
	result = m_DownSampleTexure->Initialize(device, downSampleWidth, downSampleHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize the down sample render to texture object.", "Error", MB_OK);
		return;
	}

	// Create the horizontal blur render to texture object.
	m_HorizontalBlurTexture = new RenderTextureClass;
	if(!m_HorizontalBlurTexture)
	{
		return;
	}

	// Initialize the horizontal blur render to texture object.
	result = m_HorizontalBlurTexture->Initialize(device, downSampleWidth, downSampleHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize the horizontal blur render to texture object.", "Error", MB_OK);
		return;
	}

	// Create the vertical blur render to texture object.
	m_VerticalBlurTexture = new RenderTextureClass;
	if(!m_VerticalBlurTexture)
	{
		return;
	}

	// Initialize the vertical blur render to texture object.
	result = m_VerticalBlurTexture->Initialize(device, downSampleWidth, downSampleHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize the vertical blur render to texture object.", "Error", MB_OK);
		return;
	}

	// Create the up sample render to texture object.
	m_UpSampleTexure = new RenderTextureClass;
	if(!m_UpSampleTexure)
	{
		return;
	}

	// Initialize the up sample render to texture object.
	result = m_UpSampleTexure->Initialize(device, GRAPHICS->screen_width, GRAPHICS->screen_height, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize the up sample render to texture object.", "Error", MB_OK);
		return;
	}

	// Create the small ortho window object.
	m_SmallWindow = new OrthoWindowClass;
	if(!m_SmallWindow)
	{
		return;
	}

	// Initialize the small ortho window object.
	result = m_SmallWindow->Initialize(device, downSampleWidth, downSampleHeight);
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize the small ortho window object.", "Error", MB_OK);
		return;
	}

	// Create the full screen ortho window object.
	m_FullScreenWindow = new OrthoWindowClass;
	if(!m_FullScreenWindow)
	{
		return;
	}

	// Initialize the full screen ortho window object.
	result = m_FullScreenWindow->Initialize(device, GRAPHICS->screen_width, GRAPHICS->screen_height);
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize the full screen ortho window object.", "Error", MB_OK);
		return;
	}
}

bool Blur::RenderBlur(ID3D11DeviceContext* context, int indexCount, D3DXMATRIX& World,const D3DXMATRIX& View,const D3DXMATRIX& Projection, ID3D11ShaderResourceView** textureArray)
{
	bool result;

	return true;
}


// Blur defs
bool Blur::RenderSceneToTexture(ID3D11DeviceContext* context, int indexCount, D3DXMATRIX& World,const D3DXMATRIX& View,const D3DXMATRIX& Projection, ID3D11ShaderResourceView** textureArray)
{
	bool result;

	result = GRAPHICS->GetTextureShader()->Render(context, indexCount, World, View, Projection, textureArray);
	if(!result)
	{
		return false;
	}



	return true;
}

bool Blur::DownSampleTexture(ID3D11DeviceContext* context, int indexCount, D3DXMATRIX& World,const D3DXMATRIX& View,const D3DXMATRIX& Projection, ID3D11ShaderResourceView** textureArray, ID3D11DepthStencilView* tempDSV)
{
	bool result;

	D3DXMATRIX orthoMatrix;

	// Set the render target to be the render to texture.
	m_DownSampleTexure->SetRenderTarget(context, tempDSV);

	// Clear the render to texture.
	m_DownSampleTexure->ClearRenderTarget(context, 0.0f, 0.0f, 0.0f, 0.0f);

	// Get the ortho matrix from the render to texture since texture has different dimensions being that it is smaller.
	m_DownSampleTexure->GetOrthoMatrix(orthoMatrix);
	//GRAPHICS->m_D3D->GetOrthoMatrix(orthoMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	GRAPHICS->GetD3D()->TurnZBufferOff();

	// Put the small ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_SmallWindow->Render(context);

	// Render the small ortho window using the texture shader and the render to texture of the scene as the texture resource.
	result = GRAPHICS->GetTextureShader()->Render(context, m_SmallWindow->GetIndexCount(), World, View, orthoMatrix, 
		GRAPHICS->GetGlowMapRT()->GetShaderResourceView());
	if(!result)
	{
		return false;
	}

	// Turn the Z buffer back on now that all 2D rendering has completed.
	GRAPHICS->GetD3D()->TurnZBufferOn();
	
	return true;
}

bool Blur::RenderHorizontalBlurToTexture(ID3D11DeviceContext* context, int indexCount, D3DXMATRIX& World,const D3DXMATRIX& View,const D3DXMATRIX& Projection, ID3D11ShaderResourceView** textureArray, ID3D11DepthStencilView* tempDSV)
{
	float screenSizeX;
	bool result;

	D3DXMATRIX orthoMatrix;

	// Store the screen width in a float that will be used in the horizontal blur shader.
	screenSizeX = (float)m_HorizontalBlurTexture->GetTextureWidth();
	
	// Set the render target to be the render to texture.
	m_HorizontalBlurTexture->SetRenderTarget(context, tempDSV);

	// Clear the render to texture.
	m_HorizontalBlurTexture->ClearRenderTarget(context, 0.0f, 0.0f, 0.0f, 0.0f);

	// Get the ortho matrix from the render to texture since texture has different dimensions.
	m_HorizontalBlurTexture->GetOrthoMatrix(orthoMatrix);
	//GRAPHICS->m_D3D->GetOrthoMatrix(orthoMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	GRAPHICS->GetD3D()->TurnZBufferOff();

	// Put the small ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_SmallWindow->Render(context);
	
	// Render the small ortho window using the horizontal blur shader and the down sampled render to texture resource.
	result = m_HorizontalBlurShader->Render(context, m_SmallWindow->GetIndexCount(), World, View, orthoMatrix, 
		m_DownSampleTexure->GetShaderResourceView()[0], screenSizeX);
	if(!result)
	{
		return false;
	}

	// Turn the Z buffer back on now that all 2D rendering has completed.
	GRAPHICS->GetD3D()->TurnZBufferOn();

	return true;
}

bool Blur::RenderVerticalBlurToTexture(ID3D11DeviceContext* context, int indexCount, D3DXMATRIX& World,const D3DXMATRIX& View,const D3DXMATRIX& Projection, ID3D11ShaderResourceView** textureArray, ID3D11DepthStencilView* tempDSV)
{
	float screenSizeY;
	bool result;

	D3DXMATRIX orthoMatrix;

	// Store the screen height in a float that will be used in the vertical blur shader.
	screenSizeY = (float)m_VerticalBlurTexture->GetTextureHeight();
	
	// Set the render target to be the render to texture.
	m_VerticalBlurTexture->SetRenderTarget(context, tempDSV);

	// Clear the render to texture.
	m_VerticalBlurTexture->ClearRenderTarget(context, 0.0f, 0.0f, 0.0f, 0.0f);

	// Get the ortho matrix from the render to texture since texture has different dimensions.
	m_VerticalBlurTexture->GetOrthoMatrix(orthoMatrix);
	//GRAPHICS->m_D3D->GetOrthoMatrix(orthoMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	GRAPHICS->GetD3D()->TurnZBufferOff();

	// Put the small ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_SmallWindow->Render(context);
	
	// Render the small ortho window using the vertical blur shader and the horizontal blurred render to texture resource.
	result = m_VerticalBlurShader->Render(context, m_SmallWindow->GetIndexCount(), World, View, orthoMatrix, 
					      m_HorizontalBlurTexture->GetShaderResourceView()[0], screenSizeY);
	if(!result)
	{
		return false;
	}

	// Turn the Z buffer back on now that all 2D rendering has completed.
	GRAPHICS->GetD3D()->TurnZBufferOn();

	return true;
}

bool Blur::UpSampleTexture(ID3D11DeviceContext* context, int indexCount, D3DXMATRIX& World,const D3DXMATRIX& View,const D3DXMATRIX& Projection, ID3D11ShaderResourceView** textureArray, ID3D11DepthStencilView* tempDSV)
{
	bool result;

	D3DXMATRIX orthoMatrix;

	// Set the render target to be the render to texture.
	m_UpSampleTexure->SetRenderTarget(context, tempDSV);

	// Clear the render to texture.
	m_UpSampleTexure->ClearRenderTarget(context, 0.0f, 0.0f, 0.0f, 0.0f);

	// Get the ortho matrix from the render to texture since texture has different dimensions.
	m_UpSampleTexure->GetOrthoMatrix(orthoMatrix);
	//GRAPHICS->m_D3D->GetOrthoMatrix(orthoMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	GRAPHICS->GetD3D()->TurnZBufferOff();

	// Put the full screen ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_FullScreenWindow->Render(context);

	// Render the full screen ortho window using the texture shader and the small sized final blurred render to texture resource.
	result = GRAPHICS->GetTextureShader()->Render(context, m_FullScreenWindow->GetIndexCount(), World, View, orthoMatrix, 
					 m_VerticalBlurTexture->GetShaderResourceView());
	if(!result)
	{
		return false;
	}

	// Turn the Z buffer back on now that all 2D rendering has completed.
	GRAPHICS->GetD3D()->TurnZBufferOn();
	
	return true;
}

bool Blur::Render2DTextureScene(ID3D11DeviceContext* context, int indexCount, D3DXMATRIX& World,const D3DXMATRIX& View,const D3DXMATRIX& Projection, ID3D11ShaderResourceView** textureArray, ID3D11DepthStencilView* tempDSV)
{
	bool result;

	D3DXMATRIX orthoMatrix;
	GRAPHICS->GetD3D()->GetOrthoMatrix(orthoMatrix);

	

	// Turn off the Z buffer to begin all 2D rendering.
	GRAPHICS->GetD3D()->TurnZBufferOff();

	// Put the full screen ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_FullScreenWindow->Render(context);

	// Render the full screen ortho window using the texture shader and the full screen sized blurred render to texture resource.
	result = GRAPHICS->GetTextureShader()->Render(context, m_FullScreenWindow->GetIndexCount(), World, View, orthoMatrix, 
					 m_UpSampleTexure->GetShaderResourceView());
	if(!result)
	{
		return false;
	}

	// Turn the Z buffer back on now that all 2D rendering has completed.
	GRAPHICS->GetD3D()->TurnZBufferOn();
	
	// Present the rendered scene to the screen.
	//GRAPHICS->GetD3D()->EndScene();

	return true;
}

bool Blur::RenderTextureColor(ID3D11DeviceContext* context, int indexCount, D3DXMATRIX& World,const D3DXMATRIX& View,const D3DXMATRIX& Projection, ID3D11ShaderResourceView** textureArray, ID3D11DepthStencilView* tempDSV)
{
	return true;
}

bool Blur::RenderGlowScene(ID3D11DeviceContext* context, int indexCount, D3DXMATRIX& World,const D3DXMATRIX& View,const D3DXMATRIX& Projection, ID3D11ShaderResourceView** textureArray, ID3D11DepthStencilView* tempDSV)
{
	D3DXMATRIX orthoMatrix;
	GRAPHICS->GetD3D()->GetOrthoMatrix(orthoMatrix);

	// Set the render target to be the render to texture.
	GRAPHICS->GetGlowOutput()->SetRenderTarget(context, tempDSV);
	// Clear the render to texture.
	GRAPHICS->GetGlowOutput()->ClearRenderTarget(context, 0.0f, 0.0f, 0.0f, 0.0f);

	// Turn off the Z buffer to begin all 2D rendering.
	GRAPHICS->GetD3D()->TurnZBufferOff();

	// Put the full screen ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_FullScreenWindow->Render(context);

	// Render the full screen ortho window using the texture shader and the full screen sized blurred render to texture resource.
	GRAPHICS->GetGlowShader()->Render(context, m_FullScreenWindow->GetIndexCount(), World, View, orthoMatrix, 
			     GRAPHICS->GetPreBlurRT()->GetShaderResourceView()[0], m_UpSampleTexure->GetShaderResourceView()[0], 2.0f);

	// Turn the Z buffer back on now that all 2D rendering has completed.
	GRAPHICS->GetD3D()->TurnZBufferOn();
	
	return true;
}

}