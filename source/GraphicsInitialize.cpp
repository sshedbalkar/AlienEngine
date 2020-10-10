#include "Precompiled.h"
#include <fstream>
#include "Graphics.h"
#include "Message.h"
#include "Core.h"
#include "d3dclass.h"
#include "ModelClass.h"
#include "FilePath.h"
#include "cameraclass.h"
#include "WindowsSystem.h"
//component registration
#include "Factory.h"
#include "ComponentCreator.h"
#include "ModelComponent.h"
#include "lightclass.h"
#include "colorshaderclass.h"
#include "bitmapclass.h"
#include "AnimateShaderClass.h"
#include "FrustumClass.h"
#include "lightmapshaderclass.h"
#include "alphamapshaderclass.h"
#include "bumpmapshaderclass.h"
#include "specmapshaderclass.h"
#include "rendertextureclass.h"
#include "debugwindowclass.h"
#include "fogshaderclass.h"
#include "clipplaneshaderclass.h"
#include "rendertexturebitmapclass.h"
#include "fadeshaderclass.h"
#include "textclass.h"
#include "LevelEditor.h"
#include "GraphicsAssetsLoader.h"
#include "textureshaderclass.h"
#include "VMath.h"
#include "texturearrayclass.h"
#include "depthshaderclass.h"
#include "glassshaderclass.h"
#include "particlesystemclass.h"
#include "LightComponent.h"
#include "ParticleComponent.h"
#include "GameLogic.h"
#include "PostProcessor.h"
#include "ShaderCompilation.h"
#include "Deferred.h"
#include "Blur.h"
#include "orthowindowclass.h"
#include "glowmapshaderclass.h"
#include "glowshaderclass.h"

namespace Framework
{
Graphics* GRAPHICS = NULL;
//======================================
Graphics::Graphics() : 
m_D3D(NULL), 
m_Camera(NULL),
m_AnimateShader(NULL),
m_Bitmap(NULL), 
m_ColorShader(NULL),
m_Frustum(NULL),
m_LightMapShader(NULL),
m_AlphaMapShader(NULL),
m_BumpMapShader(NULL),
WireFrame(false),
FrameRateCacAccuCounter(0), 
FrameRateCacAccuTimer(0.0f), 
FrameRate(9999.0f), 
m_SpecMapShader(NULL),
m_RenderShadowTexture(NULL),
m_RenderFadeTexture(NULL),
m_RenderGlassTexture(NULL),
m_FogShader(NULL),
m_ClipPlaneShader(NULL),
m_Text(NULL),
m_FadeShader(NULL),
m_RenderTextureBitmap(NULL),
m_TextureShader(NULL),
m_DepthShader(NULL),
m_GlassShader(NULL), 
m_ParticleSystem(NULL),
DebugDraw(false),
colorTargetMSAA(NULL),
colorTarget(NULL),
deferredOutputTarget(NULL),
normalsTarget(NULL),
diffuseAlbedoTarget(NULL),
specularAlbedoTarget(NULL),
lightingTarget(NULL),
depthBuffer(NULL),
pp(NULL),
lightsBuffer(NULL),
Def(NULL),
Blerg(NULL),
renderModel(false),
renderCount(0),
m_DebugWindow1(NULL),
m_DebugWindow2(NULL),
m_DebugWindow3(NULL),
m_DebugWindow4(NULL),
m_DebugWindow5(NULL),
m_DebugWindow6(NULL),
DW1(false), 
DW2(false), 
DW3(false), 
DW4(false), 
DW5(false), 
DW6(false), 
PreBlurRT(NULL),
GlowMapRT(NULL),
GlowOutput(NULL),
m_GlowMapShader(NULL),
m_GlowShader(NULL)
{
	GRAPHICS = this;
	
	MsaaOrNumLightsChanged = false;

	NumSamples = 1;
	
    NumLights = 0;

	MaxLights = 256;

	DeferredLights.resize(MaxLights);

	// Post Process constants
	BloomThreshold = 3.0f;
    BloomMagnitude = 1.0f;
    BloomBlurSigma = 0.8f;
    AdaptationRate = 0.0f;
    ExposureKeyValue = 0.015f;

	// Lighting constants
	SpecularIntensity = 0.001f;
	SpecularR = 1.0f;
	SpecularG = 1.0f;
	SpecularB = 1.0f;
	Balance = 0.5f;
	Roughness = 0.5f;

	// Ambient constants
	ConstAmbient = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	SkyColor = D3DXVECTOR4(1.0f,1.0f,1.0f,1.0f);
	GroundColor = D3DXVECTOR4(0.235f, 0.235f, 0.294f, 1.0f);
	HemiIntensity = 0.6f;

	/*Light  & light=GRAPHICS->DeferredLights[0];

		light.Position = Vec3(-24,95,40);
		light.Color  = Vec3(1,0,0);
		light.Falloff  = 10;*/
    
	
}

inline XMFLOAT3 Normalize(const XMFLOAT3& val)
{
    float len = std::sqrtf(val.x * val.x + val.y * val.y + val.z * val.z);
    XMFLOAT3 ret(val.x / len, val.y / len, val.z / len);
    return ret;
}



//Load a specific texture file and add it to the asset texture map
void Graphics::LoadTexture( const std::string& textureName, char* filename1, char* filename2 = NULL, char* filename3 = NULL, MultiTextureType _mtt = SingleTexture, bool _Extendable = false)
{
	FilePath texturefile1(filename1);
	TextureArrayClass* newTextureArray = new TextureArrayClass;
	bool result = newTextureArray->Initialize(m_D3D->GetDevice(), filename1, filename2, filename3, _mtt, _Extendable);
	if(result){
		textures[textureName.c_str()] = newTextureArray;
	}else{
		printf("loading global texture failed %s, %s, %s \n", filename1, filename2, filename3);
	}
}

TextureArrayClass* Graphics::GetTexture(std::string texture){
	TextureMap::iterator it = textures.find(texture);
	if( it!= textures.end())
		return it->second;
	else
		return NULL;
}


void Graphics::LoadContent()
{
	ID3D11Device* Device = m_D3D->GetDevice();
	
	XMFLOAT3 SunColor = XMFLOAT3(10.0f, 8.0f, 5.0f);
	XMFLOAT3 SunDirection = Normalize(XMFLOAT3(0.2f, 0.977f, -0.4f));

	//Initialize deferred
	Def->Initialize(Device);

	// Init post-processor
	pp = new PostProcessor();
	if(!pp) {return;}
	pp->Initialize(Device, hwnd);
	
	// Initialize constants
	Def->csConstants.Initialize(Device);

	CompileOptions opt1, opt2, opt3;
	
	opt1.Add("MSAASamples_", 1);
    opt1.Add("MaxLights", MaxLights);
    Def->lightTilesCS[0]=(CompileCSFromFile(Device, L"Shaders//LightTiles.hlsl", "LightTiles", "cs_5_0", opt1.Defines()));
	
	opt2.Add("MSAASamples_", 2);
    opt2.Add("MaxLights", MaxLights);
	Def->lightTilesCS[1]=(CompileCSFromFile(Device, L"Shaders//LightTiles.hlsl", "LightTiles", "cs_5_0", opt2.Defines()));
	
	opt3.Add("MSAASamples_", 4);
    opt3.Add("MaxLights", MaxLights);
	Def->lightTilesCS[2]=(CompileCSFromFile(Device, L"Shaders//LightTiles.hlsl", "LightTiles", "cs_5_0", opt3.Defines()));
}

static float RandFloat()
{
    return rand() / static_cast<float>(RAND_MAX);
}

void Graphics::CreateLightBuffer()
{
	bool result;
	if(DeferredLights.size() <= 0)
		result = lightsBuffer->Initialize(m_D3D->GetDevice(), sizeof(Light), NumLights, false, false, false, 0);
	else
		result = lightsBuffer->Initialize(m_D3D->GetDevice(), sizeof(Light), NumLights, false, false, false, &DeferredLights[0]);


	if(!result)
		__debugbreak();
}

void Graphics::CreateRenderTargets()
{
	bool result;

	// Initialize the render to texture object.
	result = colorTargetMSAA->InitDeferredRT(m_D3D->GetDevice(), screen_width, screen_height, DXGI_FORMAT_R16G16B16A16_FLOAT, 1, NumSamples, 0);
	if(!result){return;}

	// Initialize the render to texture object.
	result = colorTarget->InitDeferredRT(m_D3D->GetDevice(), screen_width, screen_height, DXGI_FORMAT_R16G16B16A16_FLOAT, 1, 1, 0);
	if(!result){return;}

	dtWidth = MsaaEnabled() ? screen_width * 2 : screen_width;
	dtHeight = (MsaaEnabled() && NumSamples == 4)? screen_height * 2 : screen_height;
	
	// Initialize the render to texture object.
	result = deferredOutputTarget->InitDeferredRT(m_D3D->GetDevice(), dtWidth, dtHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, 1, 1, 0, false, true);

	// Initialize the render to texture object.
	result = depthBuffer->InitDeferredDB(m_D3D->GetDevice(), screen_width, screen_height, DXGI_FORMAT_D24_UNORM_S8_UINT, true, NumSamples, 0);

	// Initialize the render to texture object.
	result = normalsTarget->InitDeferredRT(m_D3D->GetDevice(), screen_width, screen_height, DXGI_FORMAT_R10G10B10A2_UNORM, 1, NumSamples, 0);
    
	// Initialize the render to texture object.
	result = diffuseAlbedoTarget->InitDeferredRT(m_D3D->GetDevice(), screen_width, screen_height, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 1, NumSamples, 0);
    
	// Initialize the render to texture object.
	result = specularAlbedoTarget->InitDeferredRT(m_D3D->GetDevice(), screen_width, screen_height, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 1, NumSamples, 0);
    
	// Initialize the render to texture object.
	result = lightingTarget->InitDeferredRT(m_D3D->GetDevice(), screen_width, screen_height, DXGI_FORMAT_R11G11B10_FLOAT, 1, NumSamples, 0);
		

	LightTileSize = 16;
	numTilesX = DispatchSize(LightTileSize, screen_width);
    numTilesY = DispatchSize(LightTileSize, screen_height);
    numElements = numTilesX * numTilesY * NumLights;

    format = DXGI_FORMAT_R8_UINT;
    stride = sizeof(UINT8);

    if(NumLights >= 255)
    {
        format = DXGI_FORMAT_R16_UINT;
        stride = sizeof(UINT16);
    }
	
	CreateLightBuffer();
}

//======================================
void Graphics::Initialize()
{
	printf("Initializing graphics..\n");
	//@@
	const bool FULL_SCREEN = false;
	const bool VSYNC_ENABLED = false;
	

	bool result;
	// Create the Direct3D object.
	m_D3D = new D3DClass();
	if(!m_D3D){
		return ;
	}
	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screen_width, screen_height, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result){
		MessageBox(hwnd, "Could not initialize Direct3D.", "Error", MB_OK);
		return ;
	}
	// Create the camera object.
	m_Camera = new CameraClass;
	if(!m_Camera){ return; }
	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -2.0f);
	SetCursorPos( static_cast<int>(screen_width/2.0f), static_cast<int>(screen_height/2.0f) );

	// Create an orthographic projection matrix for 2D rendering.
	D3DXVECTOR3 eye( 0.0f, 0.0f, -10.0f );
	D3DXVECTOR3 at( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 up( 0.0f, 1.0f, 0.0f );
	D3DXMATRIX matview;
	D3DXMatrixLookAtLH( &matview, &eye, &at, &up );
	m_Text = new TextClass;
	if(!m_Text){ return; }
	// Initialize the text object.
	result = m_Text->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), hwnd, screen_width, screen_height, matview);
	if(!result){
		MessageBox(hwnd, "Could not initialize the text object.", "Error", MB_OK);
		return;
	}

	// Create the bitmap object.
	m_Bitmap = new BitmapClass;
	if(!m_Bitmap){ return; }
	// Initialize the bitmap object.
	result = m_Bitmap->Initialize(m_D3D->GetDevice(), screen_width, screen_height, "Assets//textures//crosshair.png", 55.0f, 62.5f);
	if(!result){
		MessageBox(hwnd, "Could not initialize the bitmap object.", "Error", MB_OK);
		return;
	}

	//Create the animate shader object
	m_AnimateShader = new AnimateShaderClass;
	if(!m_AnimateShader){ return; }
	// Initialize the animate shader object.
	result = m_AnimateShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result){
		MessageBox(hwnd, "Could not initialize the animate shader object.", "Error", MB_OK);
		return;
	}
	//Create the color shader object
	m_ColorShader = new ColorShaderClass;
	if(!m_ColorShader){ return; }
	// Initialize the animate shader object.
	result = m_ColorShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result){
		MessageBox(hwnd, "Could not initialize the animate shader object.", "Error", MB_OK);
		return;
	}

	// Create the frustum object.
	m_Frustum = new FrustumClass;
	if(!m_Frustum){ return; }

	// Create the texture shader object.
	m_TextureShader = new TextureShaderClass;
	if(!m_TextureShader){ return; }
	// Initialize the texture shader object.
	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result) {
		MessageBox(hwnd, "Could not initialize the texture shader object.", "Error", MB_OK);
		return;
	}

	// Create the light map shader object.
	m_LightMapShader = new LightMapShaderClass;
	if(!m_LightMapShader) { return; }
	// Initialize the light map shader object.
	result = m_LightMapShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result){
		MessageBox(hwnd, "Could not initialize the light map shader object.", "Error", MB_OK);
		return;
	}

	// Create the alpha map shader object.
	m_AlphaMapShader = new AlphaMapShaderClass;
	if(!m_AlphaMapShader){return;}
	// Initialize the alpha map shader object.
	result = m_AlphaMapShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result){
		MessageBox(hwnd, "Could not initialize the alpha map shader object.", "Error", MB_OK);
		return;
	}
	// Create the bump map shader object.
	m_BumpMapShader = new BumpMapShaderClass;
	if(!m_BumpMapShader){return;}
	// Initialize the bump map shader object.
	result = m_BumpMapShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result){
		MessageBox(hwnd, "Could not initialize the bump map shader object.", "Error", MB_OK);
		return;
	}
	
	// Create the particle system object.
	m_ParticleSystem = new ParticleSystemClass;
	if(!m_ParticleSystem){return;}
	// Initialize the particle system object.
	result = m_ParticleSystem->Initialize(m_D3D->GetDevice(), NULL, hwnd);
	if(!result){return;}

	// Create the render to texture object.
	m_RenderShadowTexture = new RenderTextureClass;
	if(!m_RenderShadowTexture){return;}
	// Initialize the render to texture object.
	result = m_RenderShadowTexture->Initialize(m_D3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result){return;}

	// Create the render to texture fade object.
	m_RenderFadeTexture = new RenderTextureClass;
	if(!m_RenderFadeTexture){return;}
	// Initialize the render to texture object.
	result = m_RenderFadeTexture->Initialize(m_D3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result){return;}

	// Create the debug window object.
	m_DebugWindow1 = new DebugWindowClass;
	if(!m_DebugWindow1){return;}
	// Initialize the debug window object.
	result = m_DebugWindow1->Initialize(m_D3D->GetDevice(), screen_width, screen_height, 300, 300);
	if(!result){
		MessageBox(hwnd, "Could not initialize the debug window object.", "Error", MB_OK);
		return;
	}
	// Create the debug window object.
	m_DebugWindow2 = new DebugWindowClass;
	if(!m_DebugWindow2){return;}
	// Initialize the debug window object.
	result = m_DebugWindow2->Initialize(m_D3D->GetDevice(), screen_width, screen_height, 300, 300);
	if(!result){
		MessageBox(hwnd, "Could not initialize the debug window object.", "Error", MB_OK);
		return;
	}
	// Create the debug window object.
	m_DebugWindow3 = new DebugWindowClass;
	if(!m_DebugWindow3){return;}
	// Initialize the debug window object.
	result = m_DebugWindow3->Initialize(m_D3D->GetDevice(), screen_width, screen_height, 300, 300);
	if(!result){
		MessageBox(hwnd, "Could not initialize the debug window object.", "Error", MB_OK);
		return;
	}
	// Create the debug window object.
	m_DebugWindow4 = new DebugWindowClass;
	if(!m_DebugWindow4){return;}
	// Initialize the debug window object.
	result = m_DebugWindow4->Initialize(m_D3D->GetDevice(), screen_width, screen_height, 300, 300);
	if(!result){
		MessageBox(hwnd, "Could not initialize the debug window object.", "Error", MB_OK);
		return;
	}
	// Create the debug window object.
	m_DebugWindow5 = new DebugWindowClass;
	if(!m_DebugWindow5){return;}
	// Initialize the debug window object.
	result = m_DebugWindow5->Initialize(m_D3D->GetDevice(), screen_width, screen_height, 300, 300);
	if(!result){
		MessageBox(hwnd, "Could not initialize the debug window object.", "Error", MB_OK);
		return;
	}
	// Create the debug window object.
	m_DebugWindow6 = new DebugWindowClass;
	if(!m_DebugWindow6){return;}
	// Initialize the debug window object.
	result = m_DebugWindow6->Initialize(m_D3D->GetDevice(), screen_width, screen_height, 300, 300);
	if(!result){
		MessageBox(hwnd, "Could not initialize the debug window object.", "Error", MB_OK);
		return;
	}

	// Create the depth shader object.
	m_DepthShader = new DepthShaderClass;
	if(!m_DepthShader){return;}

	// Initialize the depth shader object.
	result = m_DepthShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize the depth shader object.", "Error", MB_OK);
		return;
	}

	// Create the fog shader object.
	m_FogShader = new FogShaderClass;
	if(!m_FogShader){return;}
	// Initialize the fog shader object.
	result = m_FogShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result){
		MessageBox(hwnd, "Could not initialize the fog shader object.", "Error", MB_OK);
		return;
	}

	// Create the clip plane shader object.
	m_ClipPlaneShader = new ClipPlaneShaderClass;
	if(!m_ClipPlaneShader){return;}
	// Initialize the clip plane shader object.
	result = m_ClipPlaneShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result){
		MessageBox(hwnd, "Could not initialize the clip plane shader object.", "Error", MB_OK);
		return;
	}

	// Create the bitmap object.
	m_RenderTextureBitmap = new RenderTextureBitmapClass;
	if(!m_RenderTextureBitmap){return;}
	// Initialize the bitmap object.
	result = m_RenderTextureBitmap->Initialize(m_D3D->GetDevice(), screen_width, screen_height, screen_width, screen_height);
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize the bitmap object.", "Error", MB_OK);
		return;
	}

	// Create the fade shader object.
	m_FadeShader = new FadeShaderClass;
	if(!m_FadeShader)
	{
		return;
	}

	// Initialize the fade shader object.
	result = m_FadeShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize the fade shader object.", "Error", MB_OK);
		return;
	}

	// Create the glass shader object.
	m_GlassShader = new GlassShaderClass;
	if(!m_GlassShader)
	{
		return;
	}

	// Initialize the glass shader object.
	result = m_GlassShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize the glass shader object.", "Error", MB_OK);
		return;
	}

	m_GlassShader->SetPosition(0.0f, 1.5f, 0.0f);
	m_GlassShader->SetLookAt(0.0f, 0.0f, 0.0f);
	m_GlassShader->GenerateProjectionMatrix(SCREEN_DEPTH, SCREEN_NEAR);

	D3DXVECTOR4 AC = D3DXVECTOR4(0.2f,0.2f, 0.2f, 1.0f);
	float SP = 250.0f;

	//Set I to Identity Matrix
	D3DXMatrixIdentity(&IdentityMatrix);
	// Setup a clipping plane.
	// vec4(xaxis, yaxis, zaxis, height of cull)
	clipPlane = D3DXVECTOR4(0.0f, 1.0f, 0.0f, -20.0f);

	// Set the color of the fog to grey.
	fogColor = 0.5f;
	// Set the start and end of the fog.
	fogStart = 0.0f;
	fogEnd = 10.0f;

	// Set the fade in time to 3000 milliseconds.
	m_fadeInTime = 1.0f;

	// Initialize the accumulated time to zero milliseconds.
	m_accumulatedTime = 0;

	// Initialize the fade percentage to zero at first so the scene is black.
	m_fadePercentage = 0;

	// Set the fading in effect to not done.
	m_fadeDone = true;

	Blerg = new Blur();
	if(!Blerg){return;}

	Blerg->Initialize();

	Def = new Deferred();
	if(!Def){return;}

	LoadContent();

	colorTargetMSAA = new RenderTextureClass;
	if(!colorTargetMSAA){return;}
	// Initialize the render to texture object.
	result = colorTargetMSAA->InitDeferredRT(m_D3D->GetDevice(), screen_width, screen_height, DXGI_FORMAT_R16G16B16A16_FLOAT, 1, NumSamples, 0);
	if(!result){return;}

	colorTarget = new RenderTextureClass;
	if(!colorTarget){return;}
	// Initialize the render to texture object.
	result = colorTarget->InitDeferredRT(m_D3D->GetDevice(), screen_width, screen_height, DXGI_FORMAT_R16G16B16A16_FLOAT, 1, 1, 0);
	if(!result){return;}

	dtWidth = MsaaEnabled() ? screen_width * 2 : screen_width;
	dtHeight = (MsaaEnabled() && NumSamples == 4)? screen_height * 2 : screen_height;
	
	deferredOutputTarget = new RenderTextureClass;
	if(!deferredOutputTarget){return;}
	// Initialize the render to texture object.
	result = deferredOutputTarget->InitDeferredRT(m_D3D->GetDevice(), dtWidth, dtHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, 1, 1, 0, false, true);

	depthBuffer = new RenderTextureClass;
	if(!depthBuffer){return;}
	// Initialize the render to texture object.
	result = depthBuffer->InitDeferredDB(m_D3D->GetDevice(), screen_width, screen_height, DXGI_FORMAT_D24_UNORM_S8_UINT, true, NumSamples, 0);

	normalsTarget = new RenderTextureClass;
	if(!normalsTarget){return;}
	// Initialize the render to texture object.
	result = normalsTarget->InitDeferredRT(m_D3D->GetDevice(), screen_width, screen_height, DXGI_FORMAT_R10G10B10A2_UNORM, 1, NumSamples, 0);
    
	diffuseAlbedoTarget = new RenderTextureClass;
	if(!diffuseAlbedoTarget){return;}
	// Initialize the render to texture object.
	result = diffuseAlbedoTarget->InitDeferredRT(m_D3D->GetDevice(), screen_width, screen_height, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 1, NumSamples, 0);
    
	specularAlbedoTarget = new RenderTextureClass;
	if(!specularAlbedoTarget){return;}
	// Initialize the render to texture object.
	result = specularAlbedoTarget->InitDeferredRT(m_D3D->GetDevice(), screen_width, screen_height, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 1, NumSamples, 0);
    
	lightingTarget = new RenderTextureClass;
	if(!lightingTarget){return;}
	// Initialize the render to texture object.
	result = lightingTarget->InitDeferredRT(m_D3D->GetDevice(), screen_width, screen_height, DXGI_FORMAT_R11G11B10_FLOAT, 1, NumSamples, 0);


	//Blur+Glow Inits
	// Create the render to texture fade object.
	PreBlurRT = new RenderTextureClass;
	if(!PreBlurRT){return;}
	// Initialize the render to texture object.
	result = PreBlurRT->Initialize(m_D3D->GetDevice(), screen_width, screen_height, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result){return;}
	// Create the render to texture fade object.
	GlowMapRT = new RenderTextureClass;
	if(!GlowMapRT){return;}
	// Initialize the render to texture object.
	result = GlowMapRT->Initialize(m_D3D->GetDevice(), screen_width, screen_height, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result){return;}
	GlowOutput = new RenderTextureClass;
	if(!GlowOutput){return;}
	// Initialize the render to texture object.
	result = GlowOutput->Initialize(m_D3D->GetDevice(), screen_width, screen_height, SCREEN_DEPTH, SCREEN_NEAR);
	// Create the glow map shader object.
	m_GlowMapShader = new GlowMapShaderClass;
	if(!m_GlowMapShader) { return; }
	// Initialize the glow map shader object.
	result = m_GlowMapShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize the glow map shader object.", "Error", MB_OK);
		return;
	}
	// Create the glow shader object.
	m_GlowShader = new GlowShaderClass;
	if(!m_GlowShader) { return;	}
	// Initialize the glow shader object.
	result = m_GlowShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize the glow shader object.", "Error", MB_OK);
		return;
	}
	

	LightTileSize = 16;
	numTilesX = DispatchSize(LightTileSize, screen_width);
    numTilesY = DispatchSize(LightTileSize, screen_height);
    numElements = numTilesX * numTilesY * NumLights;

    format = DXGI_FORMAT_R8_UINT;
    stride = sizeof(UINT8);

    if(NumLights >= 255)
    {
        format = DXGI_FORMAT_R16_UINT;
        stride = sizeof(UINT16);
    }
	
	lightsBuffer = new StructuredBuffer;
	if(!lightsBuffer) {return;}

	CreateLightBuffer();

	//TODO: When window is resized, call CreateRenderTargets() followed by this line below
	pp->AfterReset(screen_width, screen_height);

	LoadModelToMap("Assets//models//sphere.bin");
	LoadModelToMap("Assets//models//quad.bin");
	LoadModelToMap2("Assets//models//cube.txt");
	LoadModelToMap("Assets//models//hero.bin");
	LoadModelToMap("Assets//models//ramp.bin");
	LoadModelToMap("Assets//models//newbutton.bin");
	LoadModelToMap("Assets//models//sword.bin");
	LoadModelToMap("Assets//models//knot.bin");
	LoadModelToMap("Assets//models//RoboCube.bin");
	LoadModelToMap("Assets//models//laseremitter.bin");
	LoadModelToMap("Assets//models//cubethrower.bin");
	LoadModelToMap("Assets//models//domino.bin");
	LoadModelToMap("Assets//models//tube.bin");
	LoadModelToMap("Assets//models//cylinder.bin");
	LoadModelToMap("Assets//models//cone.bin");
	LoadModelToMap("Assets//models//button.bin");
	LoadModelToMap("Assets//models//hex.bin");
	LoadModelToMap("Assets//models//door.bin");
	LoadModelToMap("Assets//models//gun.bin");
	
	
	LoadTexture( "white", "Assets/textures/white.png", NULL, NULL, DeferredShading);
	LoadTexture( "WhiteLight", "Assets/textures/offwhite.png", NULL, NULL, LightBulbColor);
	LoadTexture( "cream", "Assets/textures/cream.png", NULL, NULL, DeferredShading);
	LoadTexture( "cube", "Assets//textures//cube.png", NULL, NULL, DeferredShading);
	LoadTexture( "brick", "Assets//textures//brick.png", NULL, NULL, DeferredShading);
	LoadTexture( "transparentbrick", "Assets//textures//brick.png", NULL, NULL, Transparent);
	LoadTexture( "invisible", "", NULL, NULL, Invisible);
	LoadTexture( "ButtonTexture", "Assets//textures//ButtonTexture.png", NULL, NULL, DeferredShading);
	LoadTexture( "door", "Assets//textures//doortest.png", NULL, NULL, DeferredShading);
	LoadTexture( "dark_blue", "Assets//textures//dark_blue.png", NULL, NULL, DeferredShading);
	LoadTexture( "red", "Assets//textures//particle_0xE42700.jpg", NULL, NULL, DeferredShading);
	LoadTexture( "dark_blue_trans", "Assets//textures//dark_blue.png", NULL, NULL, Transparent);
	LoadTexture( "green", "Assets//textures//green.png", NULL, NULL, DeferredShading);
	LoadTexture( "wallunselected", "Assets//textures//orange_platform.png", NULL, NULL, DeferredShading);
	LoadTexture( "wallselected", "Assets//textures//orange_platform.png", "Assets//textures//darkblue.png", NULL, TextureBlend);
	LoadTexture( "transparentbridge", "Assets//textures//blueTile.jpg", NULL, NULL, Transparent);
	LoadTexture( "transparentwall", "Assets//textures//glass.jpg", NULL, NULL, Transparent);
	LoadTexture( "floor", "Assets//textures//shipfloor.png", "Assets//textures//shipfloor_NRM.png", NULL, DeferredShading, true);
	LoadTexture( "floor1", "Assets//textures//shipwall.png", "Assets//textures//shipwall_NRM.png", NULL, DeferredShading,true);
	//LoadTexture( "wall", "Assets//textures//shipceiling.jpg", "Assets//textures//shipceiling_NRM.png", NULL, DeferredShading, true);
	LoadTexture( "wall", "Assets//textures//shipfloor2.png", "Assets//textures//shipfloor_NRM.png", NULL, DeferredShading, true);
	LoadTexture( "ceiling", "Assets/textures/shipfloor.png", "Assets//textures//shipceiling_NRM.png", NULL, DeferredShading, true);
	LoadTexture( "conveyer", "Assets//textures//conveyer.jpg", NULL, NULL, DeferredShading);
	LoadTexture( "lava", "Assets//textures//lava.jpg", NULL, "Assets//textures//lava_DISP.jpg", lava);//"Assets//textures//lava_DISP.jpg", lava);a
	LoadTexture( "vectical_slider", "Assets//textures//BeigePaintedBoards.jpg", NULL, NULL, DeferredShading);
	LoadTexture( "generic", "Assets//textures//woodtexture.png", NULL, NULL, TextureColorFR);
	LoadTexture( "teleporter", "Assets//textures//teleporter.png", NULL, NULL, DeferredShading);
	LoadTexture( "metal_sphere", "Assets//textures//pluto.jpg",NULL, NULL, DeferredShading);
	LoadTexture( "rope", "Assets//textures//rope.jpg", NULL, NULL, DeferredShading);
	LoadTexture( "metal", "Assets//textures//metal.jpg", NULL, NULL, DeferredShading);
	LoadTexture( "SwordTexture", "Assets//textures//SwordTexture.png",NULL, NULL, DeferredShading);
	LoadTexture( "GunTexture", "Assets//textures//GunTexture.png",NULL, NULL, DeferredShading);
	LoadTexture( "CubeTexture", "Assets//textures//CubeTexture.png",NULL, NULL, DeferredShading);
	LoadTexture( "DoorTexture", "Assets//textures//MetalPlate.jpg", "Assets//textures//MetalPlate_NRM.png", NULL, DeferredShading, true);
	LoadTexture( "LaserEmitter", "Assets//textures//LaserEmitter.png",NULL, NULL, DeferredShading);
	LoadTexture( "transparentcube", "Assets//textures//CubeTexture.png", NULL, NULL, Transparent);
	LoadTexture( "transparentrope", "Assets//textures//rope.jpg", NULL, NULL, Transparent);
	LoadTexture( "transparentfloor", "Assets//textures//floor.png", "Assets//textures//floor_NRM.dds", NULL, Transparent);
	LoadTexture( "CubeThrowerTexture", "Assets//textures//CubeThrowerTexture.png",NULL, NULL, DeferredShading);	
	LoadTexture( "earth", "Assets//textures//earth.png", NULL, NULL, DeferredShading);
	LoadTexture( "mandm", "Assets//textures//mandm.jpg", NULL, NULL, DeferredShading);
	LoadTexture( "tron", "Assets//textures//tron.png", NULL, NULL, DeferredShading);	
	LoadTexture( "tron_trans", "Assets//textures//tron.png", NULL, NULL, Transparent);	//
	LoadTexture( "ice", "Assets//textures//wall.png", NULL, NULL, DeferredShading,true);	//
	LoadTexture( "TimeCancelField", "Assets//textures//Color.png", NULL, NULL, Transparent);	//
	LoadTexture( "particle_circle_mask", "Assets//textures//particle_circle_mask.jpg", NULL, NULL, DeferredShading);	
	LoadTexture( "CubeGlow", "Assets//textures//green.png", "Assets//textures//greenglow.png", NULL, Glow);
	LoadTexture( "CubeST", "Assets//textures//dark_blue.png", NULL, NULL, TextureColorFR);
	LoadTexture( "lasercolor", "Assets/textures/red.jpg", "Assets//textures//white.png", NULL, Glow);
	LoadTexture( "glowinactive", "Assets/textures/orange.jpg", "Assets//textures//white.png", NULL, Glow);
	LoadTexture( "glowactive", "Assets/textures/green.jpg", "Assets//textures//white.png", NULL, Glow);
	LoadTexture( "glowactive1", "Assets/textures/green.png", "Assets//textures//white.png", NULL, Glow);
	LoadTexture( "glowinactive1", "Assets/textures/grey.png", NULL, NULL, DeferredShading);
	//LoadTexture( "glowactive1", "Assets/textures/green.jpg", "Assets//textures//white.png", NULL, Glow);
	//LoadTexture( "glowactive1", "Assets//textures//shipfloor2.png", "Assets//textures//shipfloor_NRM.png", NULL, DeferredShading, true);
	LoadModelToMap("Assets//models//goober@walk.bin");
	LoadTexture( "goober_UV", "Assets/textures/goober_UV.tif", NULL, NULL, SingleTexture);
	LoadTexture( "whiteF", "Assets/textures/white.png", NULL, NULL, SingleTexture);




	//ALPHABET
	LoadModelToMap("Assets//models//Alphabet//lettera.bin");
	LoadModelToMap("Assets//models//Alphabet//letterb.bin");
	LoadModelToMap("Assets//models//Alphabet//letterc.bin");
	LoadModelToMap("Assets//models//Alphabet//letterd.bin");
	LoadModelToMap("Assets//models//Alphabet//lettere.bin");
	LoadModelToMap("Assets//models//Alphabet//letterf.bin");
	LoadModelToMap("Assets//models//Alphabet//letterg.bin");
	LoadModelToMap("Assets//models//Alphabet//letterh.bin");
	LoadModelToMap("Assets//models//Alphabet//letteri.bin");
	LoadModelToMap("Assets//models//Alphabet//letterj.bin");
	LoadModelToMap("Assets//models//Alphabet//letterk.bin");
	LoadModelToMap("Assets//models//Alphabet//letterl.bin");
	LoadModelToMap("Assets//models//Alphabet//letterm.bin");
	LoadModelToMap("Assets//models//Alphabet//lettern.bin");
	LoadModelToMap("Assets//models//Alphabet//lettero.bin");
	LoadModelToMap("Assets//models//Alphabet//letterp.bin");
	LoadModelToMap("Assets//models//Alphabet//letterq.bin");
	LoadModelToMap("Assets//models//Alphabet//letterr.bin");
	LoadModelToMap("Assets//models//Alphabet//letters.bin");
	LoadModelToMap("Assets//models//Alphabet//lettert.bin");
	LoadModelToMap("Assets//models//Alphabet//letteru.bin");
	LoadModelToMap("Assets//models//Alphabet//letterv.bin");
	LoadModelToMap("Assets//models//Alphabet//letterw.bin");
	LoadModelToMap("Assets//models//Alphabet//letterx.bin");
	LoadModelToMap("Assets//models//Alphabet//lettery.bin");
	LoadModelToMap("Assets//models//Alphabet//letterz.bin");




	RegisterComponent(LightComponent);
	RegisterComponent(Transform);
	RegisterComponent(ModelComponent);
	RegisterComponent(ParticleComponent);
}

}
