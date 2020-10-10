#pragma once

#include "System.h"
#include "ObjectLinkedList.h"
#include "Vector3.h"
#include "LineDrawer.h"
#define _XM_NO_INTRINSICS_
#include <xnamath.h>
#include <map>


namespace Framework
{

// forward declarations
class D3DClass;//@@
class LineDrawer;
class ModelClass;
class CameraClass;
class MeshShaderClass;
class ModelComponent;
class LightClass;
class BitmapClass;
class AnimateShaderClass;
class ColorShaderClass;
class FrustumClass;
class VisualPlatform;
class LightMapShaderClass;
class AlphaMapShaderClass;
class BumpMapShaderClass;
class SpecMapShaderClass;
class RenderTextureClass;
class DebugWindowClass;
class FogShaderClass;
class ClipPlaneShaderClass;
class TextClass;
class ModelInfo;
class TextureArrayClass;
class RenderTextureBitmapClass;
class FadeShaderClass;
class TextureShaderClass;
class DepthShaderClass;
class GlassShaderClass;
class ParticleSystemClass;
class LightComponent;

class Blur;
class GlowMapShaderClass;
class GlowShaderClass;

class PostProcessor;
struct StructuredBuffer;
struct RWBuffer;
class Deferred;

enum MultiTextureType;
enum BumpType;

const int SHADOWMAP_WIDTH = 128;
const int SHADOWMAP_HEIGHT = 128;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


struct Light
{
    D3DXVECTOR3 Position;
    D3DXVECTOR3 Color;
    float Falloff;
};

class Graphics : public ISystem
{
	friend class TweakBarManager;
public:
	friend class ModelComponent;
	friend class LineDrawer;
	LineDrawer lineDrawer;
	std::vector<Light> DeferredLights;
	void SetNumOfLights(int k){NumLights=k;}
	int GetNoOfLights(){return NumLights;}
	int GetMAXNoOfLights(){return MaxLights;}
	void CreateLightBuffer();

	float GetBloomThreshold() { return BloomThreshold; }
    float GetBloomMagnitude() { return BloomMagnitude; }
    float GetBloomBlurSigma() { return BloomBlurSigma; }
	float GetAdaptationRate() { return AdaptationRate; }
	float GetExposureKeyValue() { return ExposureKeyValue; }
	float GetSpecularIntensity() { return SpecularIntensity; }
	float GetSpecularR() { return SpecularR; }
	float GetSpecularG() { return SpecularG; }
	float GetSpecularB() { return SpecularB; }
	float GetBalance() { return Balance; }
	float GetRoughness() { return Roughness; }
	D3DXVECTOR4 GetConstAmbient() { return ConstAmbient; }
	D3DXVECTOR4 GetSkyColor() { return SkyColor; }
	D3DXVECTOR4 GetGroundColor() { return GroundColor; }
	float GetHemiIntensity() { return HemiIntensity; }

	void SetBloomThreshold(float temp) { BloomThreshold = temp; }
    void SetBloomMagnitude(float temp) { BloomMagnitude = temp; }
    void SetBloomBlurSigma(float temp) { BloomBlurSigma = temp; }
    void SetAdaptationRate(float temp) { AdaptationRate = temp; }
    void SetExposureKeyValue(float temp) { ExposureKeyValue = temp; }
	void SetSpecularIntensity(float temp) { SpecularIntensity = temp; }
	void SetSpecularR(float temp) { SpecularR = temp; }
	void SetSpecularG(float temp) { SpecularG = temp; }
	void SetSpecularB(float temp) { SpecularB = temp; }
	void SetBalance(float temp) { Balance = temp; }
	void SetRoughness(float temp) { Roughness = temp; }
	void SetConstAmbient(D3DXVECTOR4 temp) { ConstAmbient = temp; }
	void SetSkyColor(D3DXVECTOR4 temp) { SkyColor = temp; }
	void SetGroundColor(D3DXVECTOR4 temp) { GroundColor = temp; }
	void SetHemiIntensity(float temp) { HemiIntensity = temp; }
	bool DetermingIfInsideAdjacentToHeroLevelBox(int obj);
	bool DetermingIfInsideSameLevelBox(int obj1, int obj2);

	void DeferredRender(float dt);
	void GlowRender(float dt);
	void TransparentRender(float dt);
	void Bloom(float dt);
	void Render2D(float dt);
	void RenderUI(float dt);
	void ForwardRender(float dt);

	int renderCount;
	bool renderModel;
	
	D3DXMATRIX IdentityMatrix;

	RenderTextureClass* GetPreBlurRT() { return PreBlurRT; }
	RenderTextureClass* GetGlowMapRT() { return GlowMapRT; }
	RenderTextureClass* GetGlowOutput() { return GlowOutput; }
	GlowMapShaderClass* GetGlowMapShader() { return m_GlowMapShader; }
	GlowShaderClass* GetGlowShader() { return m_GlowShader; }
	TextureShaderClass* GetTextureShader() { return m_TextureShader; }

private:
	HWND hwnd;
	
	CameraClass* m_Camera;
	std::vector<LightClass> m_Lights;
	AnimateShaderClass* m_AnimateShader;
	ColorShaderClass* m_ColorShader;
	BitmapClass* m_Bitmap;
	FrustumClass* m_Frustum;
	LightMapShaderClass* m_LightMapShader;
	AlphaMapShaderClass* m_AlphaMapShader;
	BumpMapShaderClass* m_BumpMapShader;
	SpecMapShaderClass* m_SpecMapShader;
	RenderTextureClass* m_RenderShadowTexture;
	RenderTextureClass* m_RenderFadeTexture;
	RenderTextureClass* m_RenderGlassTexture;
	FogShaderClass* m_FogShader;
	ClipPlaneShaderClass* m_ClipPlaneShader;
	TextClass* m_Text;
	FadeShaderClass* m_FadeShader;
	RenderTextureBitmapClass* m_RenderTextureBitmap;
	DepthShaderClass* m_DepthShader;
	GlassShaderClass* m_GlassShader;
	ParticleSystemClass* m_ParticleSystem;

	TextureShaderClass *m_TextureShader;
	
	// Blur+Glow variables
	Blur* Blerg;
	RenderTextureClass* PreBlurRT;
	RenderTextureClass* GlowMapRT;
	RenderTextureClass* GlowOutput;
	GlowMapShaderClass* m_GlowMapShader;
	GlowShaderClass* m_GlowShader;

	// Debug Windows
	DebugWindowClass* m_DebugWindow1;
	DebugWindowClass* m_DebugWindow2;
	DebugWindowClass* m_DebugWindow3;
	DebugWindowClass* m_DebugWindow4;
	DebugWindowClass* m_DebugWindow5;
	DebugWindowClass* m_DebugWindow6;

	//DEFERRED VARIABLES
	int LightTileSize;
	int numTilesX;
    int numTilesY;
    int numElements;
    DXGI_FORMAT format;
    int stride;	
	int NumSamples;
    int NumLights;
	int MaxLights;
	bool MsaaOrNumLightsChanged;
	int dtWidth;
    int dtHeight;
	RenderTextureClass* colorTargetMSAA;
	RenderTextureClass* colorTarget;
	RenderTextureClass* deferredOutputTarget;
	RenderTextureClass* normalsTarget;
	RenderTextureClass* diffuseAlbedoTarget;
    RenderTextureClass* specularAlbedoTarget;
    RenderTextureClass* lightingTarget;
	RenderTextureClass* depthBuffer;
	PostProcessor* pp;
	StructuredBuffer* lightsBuffer;
	Deferred* Def;
	float BloomThreshold;
    float BloomMagnitude;
    float BloomBlurSigma;
    float AdaptationRate;
    float ExposureKeyValue;
	float SpecularIntensity;
	float SpecularR;
	float SpecularG;
	float SpecularB;
	float Balance;
	float Roughness;
	D3DXVECTOR4 ConstAmbient;
	D3DXVECTOR4 SkyColor;
	D3DXVECTOR4 GroundColor;
	float HemiIntensity;

	// Deferred Functions
	void LoadContent();
	
	void ComputeLightTiles();
	void CreateRenderTargets();

	float m_fadeInTime, m_accumulatedTime, m_fadePercentage;
	bool m_fadeDone;
	float fogColor, fogStart, fogEnd;

	D3DXVECTOR4 clipPlane;
	D3DXMATRIX worldMatrix, viewMatrix, orthoMatrix, projectionMatrix;

	bool WireFrame;
	bool DW1;
	bool DW2;
	bool DW3;
	bool DW4;
	bool DW5;
	bool DW6;
	bool DebugDraw;
	int FrameRateCacAccuCounter;
	float FrameRateCacAccuTimer;
	float FrameRate;

	bool RenderFadeTexture(float dt);
	bool RenderShadowTexture(float dt);
	bool RenderFadingScene(float dt);
	void RenderScene(float dt);
	bool RenderCubeMapTexture(float dt, int LightIndex);

	typedef std::map<std::string, TextureArrayClass*> TextureMap;
	TextureMap	textures;
	void LoadTexture(const std::string&, char* filename1, char* filename2, char* filename3, MultiTextureType _mtt, bool Extendable);
	TextureArrayClass* GetTexture(std::string);//Get a texture asset. Will return null if texture is not loaded
	typedef std::map<std::string, ModelClass*> ModelMap;
	ModelMap	Models;
	//void LoadModelToMap(const std::string& filename, char* textureLoc);
	void LoadModelToMap(const std::string& );
	void LoadModelToMap2(const std::string& );
	void LoadModelToMapInfo(ModelInfo*);
	void LoadModelToMap2Info(ModelInfo*);
	ModelClass* GetModel(std::string name);
	ObjectLinkList<ModelComponent> ModelComponentList;//@@@
	
public:
	D3DClass* m_D3D;

	int screen_width;
	int screen_height;

	Graphics();
	~Graphics();
	virtual void Update( float dt );
	virtual void Initialize();
	virtual void Free();
	virtual void SendMessage( Message* m );
	virtual void Unload();

	virtual std::string GetName() { return "Graphics"; }
	void SetWindowProperties( HWND hwnd, int width, int height, bool FullScreen  );
	void initializeAntTweakBar();
	void ScreenToWorldSpaceRay( Vector3& , Vector3&  );
	void ScreenToWorldSpace( Vector3& out,float depth);
	void screenPosition( Vector3& position);
	void TurnOnDebugCamera();
	void TurnOffDebugCamera();
	bool Graphics::IsDebugCameraON();
	void PushLightToList(LightClass);
	void DestroyAllLights();//for the editor
	void SetDebugDraw(bool b){DebugDraw =b;}
	void AddTwoPointsToLineDrawer(Vector3 , Vector3 , D3DXVECTOR4 a=D3DXVECTOR4(1,0,0,1));

	bool MsaaEnabled()
	{
		return ((NumSamples == 2) || (NumSamples == 4));
	}

	// Computes a compute shader dispatch size given a thread group size, and number of elements to process
	UINT32 DispatchSize(UINT32 tgSize, UINT32 numElements)
	{
		UINT32 dispatchSize = numElements / tgSize;
		dispatchSize += numElements % tgSize > 0 ? 1 : 0;
		return dispatchSize;
	}

	D3DXVECTOR3 GetCameraPosition() const;
	D3DXVECTOR3 GetCameraLookAt() const;
	D3DXMATRIX GetViewProjMatrix();
	TextClass* GetTextObj();
	D3DClass* GetD3D();
	HWND GetH() {return hwnd;}
	CameraClass* GetCamera() {return m_Camera;}
	ParticleSystemClass* GetParticleSystemClass();
};
extern Graphics* GRAPHICS;
}

