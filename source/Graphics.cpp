#include "Precompiled.h"
#include <fstream>
#include "Graphics.h"
#include "Message.h"
#include "Core.h"
#include "d3dclass.h"
#include "ModelClass.h"
#include "ModelClassBinary.h"
#include "ModelClassTangent.h"
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
#include "textureshaderclass.h"
#include "fadeshaderclass.h"
#include "textclass.h"
#include "LevelEditor.h"
#include "GraphicsAssetsLoader.h"
#include "VMath.h"
#include "texturearrayclass.h"
#include "depthshaderclass.h"
#include "glassshaderclass.h"
#include "particlesystemclass.h"
#include "PostProcessor.h"
#include "ShaderCompilation.h"
#include "Deferred.h"
#include "Blur.h"
#include "orthowindowclass.h"
#include "glowmapshaderclass.h"
#include "glowshaderclass.h"

namespace Framework
{
//======================================
Graphics::~Graphics()
{
	for( TextureMap::iterator it = textures.begin();it!=textures.end();++it)
		SafeDelete(it->second);
	for( ModelMap::iterator it = Models.begin();it!=Models.end();++it)
		delete it->second;
	Free();
}
//======================================
void Graphics::Free(){

	// Release the glass shader object.
	if(m_GlassShader)
	{
		m_GlassShader->Shutdown();
		delete m_GlassShader;
		m_GlassShader = 0;
	}

	// Release the fade shader object.
	if(m_FadeShader){
		m_FadeShader->Shutdown();
		delete m_FadeShader;
		m_FadeShader = 0;
	}
	// Release the bitmap object.
	if(m_RenderTextureBitmap){
		m_RenderTextureBitmap->Shutdown();
		delete m_RenderTextureBitmap;
		m_RenderTextureBitmap = 0;
	}

	// Release the text object.
	SafeDelete(m_Text);
	// Release the clip plane shader object.
	if(m_ClipPlaneShader){
		m_ClipPlaneShader->Shutdown();
		delete m_ClipPlaneShader;
		m_ClipPlaneShader = 0;
	}
	// Release the fog shader object.
	if(m_FogShader){
		m_FogShader->Shutdown();
		delete m_FogShader;
		m_FogShader = 0;
	}
	// Release the debug window object.
	if(m_DebugWindow1){
		m_DebugWindow1->Shutdown();
		delete m_DebugWindow1;
		m_DebugWindow1 = 0;
	}
	// Release the debug window object.
	if(m_DebugWindow2){
		m_DebugWindow2->Shutdown();
		delete m_DebugWindow2;
		m_DebugWindow2 = 0;
	}
	// Release the debug window object.
	if(m_DebugWindow3){
		m_DebugWindow3->Shutdown();
		delete m_DebugWindow3;
		m_DebugWindow3 = 0;
	}
	// Release the debug window object.
	if(m_DebugWindow4){
		m_DebugWindow4->Shutdown();
		delete m_DebugWindow4;
		m_DebugWindow4 = 0;
	}
	// Release the debug window object.
	if(m_DebugWindow5){
		m_DebugWindow5->Shutdown();
		delete m_DebugWindow5;
		m_DebugWindow5 = 0;
	}
	// Release the debug window object.
	if(m_DebugWindow6){
		m_DebugWindow6->Shutdown();
		delete m_DebugWindow6;
		m_DebugWindow6 = 0;
	}
	// Release the glow shader object.
	if(m_GlowShader)
	{
		m_GlowShader->Shutdown();
		delete m_GlowShader;
		m_GlowShader = 0;
	}
	// Release the glow map shader object.
	if(m_GlowMapShader)
	{
		m_GlowMapShader->Shutdown();
		delete m_GlowMapShader;
		m_GlowMapShader = 0;
	}
	// Release the render to texture object.
	if(m_RenderShadowTexture){
		m_RenderShadowTexture->Shutdown();
		delete m_RenderShadowTexture;
		m_RenderShadowTexture = 0;
	}
	// Release the render to texture object.
	if(m_RenderFadeTexture){
		m_RenderFadeTexture->Shutdown();
		delete m_RenderFadeTexture;
		m_RenderFadeTexture = 0;
	}
	// Release the render to texture glass object.
	if(m_RenderGlassTexture){
		m_RenderGlassTexture->Shutdown();
		delete m_RenderGlassTexture;
		m_RenderGlassTexture = 0;
	}
	for(unsigned i = 0 ; i < m_Lights.size() ; i++)
	{
		m_Lights[i].Shutdown();
	}
	// Release the depth shader object.
	if(m_DepthShader)
	{
		m_DepthShader->Shutdown();
		delete m_DepthShader;
		m_DepthShader = 0;
	}
	// Release the bump map shader object.
	if(m_BumpMapShader)
	{
		m_BumpMapShader->Shutdown();
		delete m_BumpMapShader;
		m_BumpMapShader = 0;
	}
	// Release the alpha map shader object.
	if(m_AlphaMapShader){
		m_AlphaMapShader->Shutdown();
		delete m_AlphaMapShader;
		m_AlphaMapShader = 0;
	}
	// Release the light map shader object.
	if(m_LightMapShader){
		m_LightMapShader->Shutdown();
		delete m_LightMapShader;
		m_LightMapShader = 0;
	}
	// Release the texture shader object.
	if(m_TextureShader){
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}
	// Release the particle system object.
	SafeDelete(m_ParticleSystem);
	// Release the D3D object.@@
	SafeDelete(m_D3D);
	// Release the camera object.RAD
	SafeDelete(m_Camera);
	// Release the bitmap object.
	SafeDelete(m_Bitmap);
	// Release the light shader object.RAD
	SafeDelete(m_AnimateShader);
	SafeDelete(m_ColorShader);
	// Release the frustum object.
	SafeDelete(m_Frustum);
	SafeDelete(Def);
	SafeDelete(lightsBuffer);
	SafeDelete(pp);
	SafeDelete(colorTargetMSAA);
	SafeDelete(colorTarget);
	SafeDelete(deferredOutputTarget);
	SafeDelete(normalsTarget);
	SafeDelete(diffuseAlbedoTarget);
	SafeDelete(specularAlbedoTarget);
	SafeDelete(lightingTarget);
	SafeDelete(depthBuffer);	

	SafeDelete(Blerg);
	SafeDelete(PreBlurRT);
	SafeDelete(GlowMapRT);
	SafeDelete(GlowOutput);
}
//======================================
void Graphics::Unload(){
	DestroyAllLights();
	GetParticleSystemClass()->ClearAllParticles();
}
//======================================
void Graphics::SendMessage( Message* m )
{	
	switch(m->MessageId){
		case Mid::MouseMove:{
				//The mouse has moved, update the mouse's world position
				MouseMove * mouseMove = (MouseMove*)m;
				/*if(!m_Camera->DebugCameraMode || m_Camera->bMousing){
					m_Camera->m_lookAt += ViewChangeSpeed * (mouseMove->MousePosition.x - screen_width/2) * m_Camera->sideDirection;
					m_Camera->m_lookAt.y -= ViewChangeSpeed * (mouseMove->MousePosition.y - screen_height/2);
					SetCursorPos(screen_width/2, screen_height/2);
					if(m_Camera->m_lookAt.y > 5) m_Camera->m_lookAt.y=5;
					if(m_Camera->m_lookAt.y < -5) m_Camera->m_lookAt.y=-5;
				}
				m_Camera->movingDirection = m_Camera->m_lookAt;
				if( !m_Camera->DebugCameraMode ) m_Camera->movingDirection.y = 0;
				normalize(m_Camera->movingDirection);
				m_Camera->sideDirection = D3DXVECTOR3( m_Camera->movingDirection.z, 0.0f, -m_Camera->movingDirection.x );*/
				break;
			}
		case Mid::RMouseButton:{
				RMouseButton * rmouse = (RMouseButton*)m;
				m_Camera->bMousing = rmouse->ButtonIsPressed;
				if(rmouse->ButtonIsPressed){
					SetCursorPos(screen_width/2, screen_height/2);
					if( m_Camera->DebugCameraMode ){
						m_Camera->ptLastFreeMousePos = rmouse->MousePosition;
						HideCursorTil(false);
					}
				}else{
					if( m_Camera->DebugCameraMode ){
						SetCursorPos( static_cast<int>(m_Camera->ptLastFreeMousePos.x), static_cast<int>(m_Camera->ptLastFreeMousePos.y) );
						ShowCursorTil(true);
					}
				}
				break;
			}
		case Mid::VirtualKey:{
		MessageVirtualKey* key = (MessageVirtualKey*)m;
		switch(key->character){
			case VK_F2:{
			WireFrame = !WireFrame;
			if(WireFrame) m_D3D->TurnWireFrameOn();
			else m_D3D->TurnWireFrameOff();
				break;}
			case VK_F5:{
				for( ObjectLinkList<ModelComponent>::iterator it = ModelComponentList.begin() ; it!=ModelComponentList.end(); ++it){
					if(it->animationController)
						it->animationController->animPause = !it->animationController->animPause;
				}
				break;}
			case VK_NUMPAD1:{
				DW1 = !DW1;
				DW2 = false;
				DW3 = false;
				DW4 = false;
				DW5 = false;
				DW6 = false;
				break;}
			case VK_NUMPAD2:{
				DW1 = false;
				DW2 = !DW2;
				DW3 = false;
				DW4 = false;
				DW5 = false;
				DW6 = false;
				break;}
			case VK_NUMPAD3:{
				DW1 = false;
				DW2 = false;
				DW3 = !DW3;
				DW4 = false;
				DW5 = false;
				DW6 = false;
				break;}
			case VK_NUMPAD4:{
				DW1 = false;
				DW2 = false;
				DW3 = false;
				DW4 = !DW4;
				DW5 = false;
				DW6 = false;
				break;}
			case VK_NUMPAD5:{
				DW1 = false;
				DW2 = false;
				DW3 = false;
				DW4 = false;
				DW5 = !DW5;
				DW6 = false;
				break;}
			case VK_NUMPAD6:{
				DW1 = false;
				DW2 = false;
				DW3 = false;
				DW4 = false;
				DW5 = false;
				DW6 = !DW6;
				break;}
		}
			break;}
		case Mid::MouseWheel:{
			if(m_Camera->DebugCameraMode){
				MouseWheel* wheel = (MouseWheel*)m;
				if(wheel->Delta > 0){
					m_Camera->fowardSpeed += 2.0f;
					m_Camera->backwardSpeed += 2.0f;
					m_Camera->SideMoveSpeed += 2.0f;
					m_Camera->ElevateSpeed += 2.0f;
				}else{
					m_Camera->fowardSpeed -= 2.0f;
					m_Camera->backwardSpeed -= 2.0f;
					m_Camera->SideMoveSpeed -= 2.0f;
					m_Camera->ElevateSpeed -= 2.0f;
				}
			}
			break;}
	//case:
	}
	//if ( msg->MessageId == Mid::Resolution )
	//{
	//	Resolution* m = (Resolution*)msg;
	//	screen_width = m->w;
	//	screen_height = m->h;
	//	BuildSwapChainAndStuff();
	//}
}
//======================================
void Graphics::SetWindowProperties( HWND _hwnd, int width, int height, bool FullScreen ){
	hwnd = _hwnd;
	screen_width = width;
	screen_height = height;
}

void Graphics::LoadModelToMap(const std::string& modelName){
	ModelClassBinary* pNewModel = NULL;
	FilePath Xfile( modelName );
	pNewModel = LoadModel(Xfile.FullPath.c_str(), m_D3D->GetDevice() );
	if( pNewModel != NULL ){
		Models[Xfile.FileName.c_str()] = pNewModel;
	}else{
		ErrorIf(false,"Failed to load Model %s in %s", Xfile.FileName.c_str(), Xfile.FullDirectory.c_str());
		__debugbreak();
	}
}
void Graphics::LoadModelToMap2(const std::string& modelName){
	ModelClassTangent *pNewModel = new ModelClassTangent;
	if(!pNewModel){return;}
	FilePath txtfile(modelName);
	pNewModel->Initialize(m_D3D->GetDevice(), const_cast<char*>(txtfile.FullPath.c_str()) );
	if( pNewModel != NULL ){
		Models[txtfile.FileName.c_str()] = pNewModel;
	}else{
		ErrorIf(false,"Failed to load Model %s", txtfile.FileName.c_str());
		__debugbreak();
	}
}
/*
void Graphics::LoadModelToMapInfo(ModelInfo * mi){
	ModelClassBinary* pNewModel = NULL;
	FilePath Xfile(mi->bin );
	pNewModel = LoadModel(Xfile.FullPath.c_str(), m_D3D->GetDevice(), const_cast<char *>(GetStringBody(mi->texture1)), const_cast<char *>(GetStringBody(mi->texture2)),const_cast<char *>(GetStringBody(mi->texture3)));
	pNewModel->m_TextureArray->mtt = mi->textureType  ;
	if( pNewModel != NULL ){
		Models[Xfile.FileName.c_str()] = pNewModel;
		pNewModel->modelInfo1=mi;
	}else{
		ErrorIf(false,"Failed to load Model %s in %s", Xfile.FileName.c_str(), Xfile.FullDirectory.c_str());
		__debugbreak();
	}
}

void Graphics::LoadModelToMap2Info( ModelInfo * mi)
{
	ModelClassTangent *pNewModel = new ModelClassTangent;
	if(!pNewModel){return;}
	FilePath Xfile(mi->bin);

	pNewModel->Initialize(m_D3D->GetDevice(), const_cast<char*>(Xfile.FullPath.c_str()), const_cast<char *>(GetStringBody(mi->texture1)), const_cast<char *>(GetStringBody(mi->texture2)),const_cast<char *>(GetStringBody(mi->texture3)));
	pNewModel->m_TextureArray->mtt = mi->textureType;
	if( pNewModel != NULL ){
		Models[Xfile.FileName.c_str()] = pNewModel;
	}else{
		ErrorIf(false,"Failed to load Model %s", Xfile.FileName.c_str());
		__debugbreak();
	}
}
*/
ModelClass* Graphics::GetModel(std::string name){
	ModelMap::iterator it = Models.find(name);
	if( it!= Models.end())
		return it->second;
	else
		return NULL;
}

D3DXMATRIX Graphics::GetViewProjMatrix(){
	D3DXMATRIX projectionMatrix, viewMatrix,ViewProj;
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	D3DXMatrixTranspose( &projectionMatrix, &projectionMatrix );
	D3DXMatrixTranspose( &viewMatrix, &viewMatrix );
	D3DXMatrixMultiply( &ViewProj, &projectionMatrix, &viewMatrix );
	D3DXMatrixTranspose( &ViewProj, &ViewProj );
	return ViewProj;
}

void Graphics::ScreenToWorldSpaceRay( Vector3& position, Vector3& direction ){
	Vector3 v;
	D3DXMATRIX projectionMatrix, viewMatrix, inverseViewMatrix;
	m_D3D->GetProjectionMatrix(projectionMatrix);
	// Move the mouse cursor coordinates into the -1 to +1 range.
	v.x = (((2.0f * (float)position.x ) / (float)screen_width ) - 1.0f)/ projectionMatrix._11;
	v.y = ((((2.0f * (float)position.y) / (float)screen_height) - 1.0f) * -1.0f)/ projectionMatrix._22;
	v.z =  1.0f;	
	
	m_Camera->GetViewMatrix(viewMatrix);
	D3DXMatrixInverse(&inverseViewMatrix, NULL, &viewMatrix);
	// Transform the screen space pick ray into 3D space
	direction.x  = v.x*inverseViewMatrix._11 + v.y*inverseViewMatrix._21 + v.z*inverseViewMatrix._31;
	direction.y  = v.x*inverseViewMatrix._12 + v.y*inverseViewMatrix._22 + v.z*inverseViewMatrix._32;
	direction.z  = v.x*inverseViewMatrix._13 + v.y*inverseViewMatrix._23 + v.z*inverseViewMatrix._33;
	position.x = inverseViewMatrix._41;
	position.y = inverseViewMatrix._42;
	position.z = inverseViewMatrix._43;
}

void Graphics::screenPosition( Vector3& position){
	Vector3 center,dir1,dir2;
	center=Vector3(screen_width/2.0,screen_height/2.0,0);
	ScreenToWorldSpaceRay(center,dir1);
	ScreenToWorldSpaceRay(position,dir2);

	dir1.normalise();
	dir2.normalise();
	real a= dir1*dir2;
	a=0.11/a;
	position=position+(dir2*a);
}

void Graphics::ScreenToWorldSpace( Vector3& position,float depth){
	Vec3 v,m1;
	m1=Vec3( (float)position.x, (float)position.y, (float)position.z);
	D3DXMATRIX projectionMatrix, viewMatrix, inverseViewMatrix;
	m_D3D->GetProjectionMatrix(projectionMatrix);

	D3D10_VIEWPORT viewport;
    viewport.Width = screen_width;
    viewport.Height = screen_height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
	m_Camera->GetViewMatrix(viewMatrix);

	D3DXMATRIX world;
	D3DXMatrixIdentity( &world );
	D3DXVec3Unproject( &v, &m1, &viewport, &projectionMatrix, &viewMatrix, &world );

	float scale = (depth-m_Camera->position.z)/0.1f;

	v.x -= m_Camera->position.x;
	v.y -= m_Camera->position.y;
	v.x *= scale;
	v.y *= scale;
	//v.z = 0.0f;
	v.x += m_Camera->position.x;
	v.y += m_Camera->position.y;

	position.x = v.x;
	position.y = v.y;
	position.z = v.z;
}

bool Graphics::IsDebugCameraON(){return m_Camera->DebugCameraMode;}

void Graphics::TurnOnDebugCamera(){
	if(m_Camera){
		m_Camera->DebugCameraMode = true;
		SetCursorPos( static_cast<int>(m_Camera->ptLastFreeMousePos.x), static_cast<int>(m_Camera->ptLastFreeMousePos.y) );
	}
	ShowCursorTil(true);
}
void Graphics::TurnOffDebugCamera(){
	if(m_Camera){
		m_Camera->DebugCameraMode = false;
		m_Camera->resetCameraSpeed();
		m_Camera->ptLastFreeMousePos = Vec2(WINDOWSSYSTEM->MousePosition.x, WINDOWSSYSTEM->MousePosition.y);
	}
	SetCursorPos(screen_width/2, screen_height/2);
	HideCursorTil(false);
}
D3DXVECTOR3 Graphics::GetCameraPosition() const{
	if(m_Camera) return m_Camera->position;
	else return D3DXVECTOR3(0,0,0);
}
D3DXVECTOR3 Graphics::GetCameraLookAt() const{
	if(m_Camera) return m_Camera->m_lookAt;
	else return D3DXVECTOR3(0,0,0);
}

void Graphics::initializeAntTweakBar()
{
	TwInit(TW_DIRECT3D11, m_D3D ->GetDevice ());
	TwWindowSize(screen_width , screen_height);
}

TextClass* Graphics::GetTextObj()
{
	return m_Text;
}

D3DClass* Graphics::GetD3D()
{
	return m_D3D;
}

void Graphics::PushLightToList(LightClass L)
{
	m_Lights.push_back(L);
}

void Graphics::DestroyAllLights()
{
	//DeferredLights.clear();
	m_Lights.clear();
	SetNumOfLights(0);
}

ParticleSystemClass* Graphics::GetParticleSystemClass(){
	return this->m_ParticleSystem;
}

}
