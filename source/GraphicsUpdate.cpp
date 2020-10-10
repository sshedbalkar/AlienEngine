#include "Precompiled.h"
#include <algorithm>
#include <fstream>
#include "Graphics.h"
#include "Message.h"
#include "Core.h"
#include "d3dclass.h"
#include "FilePath.h"
#include "cameraclass.h"
#include "WindowsSystem.h"
//component registration
#include "Factory.h"
#include "ComponentCreator.h"
#include "ModelComponent.h"
#include "lightclass.h"
#include "bitmapclass.h"
#include "FrustumClass.h"
#include "rendertextureclass.h"
#include "debugwindowclass.h"
#include "fogshaderclass.h"
#include "clipplaneshaderclass.h"
#include "rendertexturebitmapclass.h"
#include "fadeshaderclass.h"
#include "textclass.h"
#include "LevelEditor.h"
#include "ModelClass.h"
#include "textureshaderclass.h"
#include "VMath.h"
#include "texturearrayclass.h"
#include "depthshaderclass.h"
#include "particlesystemclass.h"
#include "Global.h"
#include "TextDisplay.h"
#include "TextureDraw.h"
#include "GameLogic.h"
#include "TimeMechanicManager.h"
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
const float FrameRateCacPeriod=2.0f;
void Graphics::Update( float dt )
{
	LOGIC->textOnScreen.Display();
	//Calculate Frame Rate
	FrameRateCacAccuTimer+=dt;
	++FrameRateCacAccuCounter;
	if(FrameRateCacAccuTimer>=FrameRateCacPeriod){
		FrameRate= FrameRateCacAccuCounter/FrameRateCacAccuTimer;
		FrameRateCacAccuCounter=0;FrameRateCacAccuTimer=0;
		m_Text->SetFps( FrameRate, m_D3D->GetDeviceContext() );
	}

	// Generate the view matrix based on the camera's position.
	m_Camera->Render(dt);

	// If fading is complete then render the scene normally using the back buffer.
	if(MsaaOrNumLightsChanged)
	{
		CreateRenderTargets();
		MsaaOrNumLightsChanged = false;
	}

	// Render the scene to back buffer
	RenderScene(dt);
}

void Graphics::AddTwoPointsToLineDrawer(Vector3 start, Vector3 end, Vec4 color)
{
	lineDrawer.drawLineList.push_back( LineVertex(Vector3ToVec3(start),color));
	lineDrawer.drawLineList.push_back( LineVertex(Vector3ToVec3(end),color));

}

void SetCSOutputs(ID3D11DeviceContext* context, ID3D11UnorderedAccessView* uav0, ID3D11UnorderedAccessView* uav1 = NULL,
                    ID3D11UnorderedAccessView* uav2 = NULL, ID3D11UnorderedAccessView* uav3 = NULL,
                    ID3D11UnorderedAccessView* uav4 = NULL, ID3D11UnorderedAccessView* uav5 = NULL)
{
    ID3D11UnorderedAccessView* uavs[6] = { uav0, uav1, uav2, uav3, uav4, uav5 };
    context->CSSetUnorderedAccessViews(0, 6, uavs, NULL);
}

void SetCSShader(ID3D11DeviceContext* context, ID3D11ComputeShader* shader)
{
    context->CSSetShader(shader, NULL, 0);
}

void ClearCSOutputs(ID3D11DeviceContext* context)
{
    SetCSOutputs(context, NULL, NULL, NULL, NULL);
}

void Graphics::ComputeLightTiles()
{
	ID3D11DeviceContext* context = m_D3D->GetDeviceContext();

	UINT32 numTilesX = DispatchSize(LightTileSize, depthBuffer->Width);
    UINT32 numTilesY = DispatchSize(LightTileSize, depthBuffer->Height);
	
	XMMATRIX W, V, P, WVP, VP;
	D3DXMATRIX World, View, Projection, WorldViewProjection, ViewProjection;

	m_D3D->GetProjectionMatrix(Projection);
	m_Camera->GetViewMatrix(View);

	P = Def->DmToXm(Projection);
	V = Def->DmToXm(View);

	VP = XMMatrixMultiply(V,P);
	
	Def->csConstants.Data.View = XMMatrixTranspose(V);
    Def->csConstants.Data.Projection = XMMatrixTranspose(P);

	XMVECTOR det;
	Def->csConstants.Data.InvViewProjection = XMMatrixTranspose(XMMatrixInverse(&det, VP));

	XMStoreFloat3(&Def->csConstants.Data.CameraPosWS, Def->DvToXv(m_Camera->GetPosition()));
    
	Def->csConstants.Data.CameraClipPlanes.x = SCREEN_NEAR;
    Def->csConstants.Data.CameraClipPlanes.y = SCREEN_DEPTH;
	Def->csConstants.Data.NumTilesX = numTilesX;
    Def->csConstants.Data.NumTilesY = numTilesY;
    Def->csConstants.Data.DisplaySizeX = depthBuffer->Width;
    Def->csConstants.Data.DisplaySizeY = depthBuffer->Height;
	Def->csConstants.ApplyChanges(context);
    Def->csConstants.SetCS(context, 0);

	int x = sizeof(CSConstants);
	x = x;

    ID3D11ShaderResourceView* srvs[6] = { NULL };
	
	if( lightsBuffer->NumElements<1)
		srvs[0] = NULL;
	else
		srvs[0] = lightsBuffer->SRView;

    srvs[1] = depthBuffer->SRView;
    srvs[2] = normalsTarget->SRView;
    srvs[3] = diffuseAlbedoTarget->SRView;
    srvs[4] = specularAlbedoTarget->SRView;
    srvs[5] = lightingTarget->SRView;
    SetCSOutputs(context, deferredOutputTarget->UAView);    

    context->CSSetShaderResources(0, 6, srvs);
	if(NumSamples == 4)
		SetCSShader(context, Def->lightTilesCS[2]);
	else if(NumSamples == 2)
		SetCSShader(context, Def->lightTilesCS[1]);
	else
		SetCSShader(context, Def->lightTilesCS[0]);

    context->Dispatch(numTilesX, numTilesY, 1);

    ClearCSOutputs(context);

    for(UINT32 i = 0; i < 6; ++i)
        srvs[i] = NULL;
    context->CSSetShaderResources(0, 6, srvs);
}

bool Graphics::DetermingIfInsideAdjacentToHeroLevelBox( int obj)
{
	int hero=HERO->GetLevelBoxId();
	if(obj<0) return true;
	if(obj==hero ) return true;
	if(obj==hero-1 ) return true;
	if(obj==hero+1 ) return true;
	return false;
}

bool Graphics::DetermingIfInsideSameLevelBox(int obj1, int obj2)
{
	if(obj1<0 || obj2<0) return true;
	if(obj1!=obj2) return false;
	return true;

}

void Graphics::DeferredRender(float dt)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	//frustum culling
	//Construct the frustum.
	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);	

	ObjectLinkList<ModelComponent>::iterator it = ModelComponentList.begin();
	ID3D11DeviceContext* context = m_D3D->GetDeviceContext();

	ID3D11RenderTargetView* renderTargets[4] = { NULL };
    ID3D11DepthStencilView* ds = depthBuffer->DSView;

	renderTargets[0] = normalsTarget->RTView;
    renderTargets[1] = diffuseAlbedoTarget->RTView;
    renderTargets[2] = specularAlbedoTarget->RTView;
    renderTargets[3] = lightingTarget->RTView;
	float color[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    for(UINT i = 0; i < 4; ++i)
		context->ClearRenderTargetView(renderTargets[i], color);

	context->OMSetRenderTargets(4, renderTargets, ds);

    context->ClearDepthStencilView(ds, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	if(!DebugDraw)
	{
		// Render deferred here
		for(  ; it!=ModelComponentList.end(); ++it)
		{
			if(it->pTextureArray->mtt != DeferredShading  && it->pTextureArray->mtt != LightBulbColor && it->pTextureArray->mtt != TextureColor && it->pTextureArray->mtt != lava)
				continue;

			if(EDITOR && EDITOR->inEditor && EDITOR->ShowWhatIsExcludedFromBoxes())
			{
				//do nothing here
			}
			else
			{
				if(!DetermingIfInsideAdjacentToHeroLevelBox(it->GetOwner()->LevelBoxId ))
					continue;
			}			

			renderModel=true;
			if(EDITOR && !EDITOR->inEditor)//skip in editor mode
			{
				if (it->GetOwner()->CompositionTypeId==GOC_Type_LightWithoutBulb)//skip for lights without bulb
					continue;
				if (it->GetOwner()->has(RigidBody))
				{
					CollisionPrimitive * colp=it->GetOwner()->has(CollisionPrimitive);
					Vector3 pos=colp->aabb->center;
					Vector3 scale=colp->aabb->halfSize;
					renderModel=m_Frustum->CheckRectangle(pos.x,pos.y,pos.z,scale.x,scale.y,scale.z);
				}
				else
				{
					float radius = sqrt(lengthSquared(it->transform->Scale));
					renderModel = m_Frustum->CheckSphere(it->transform->Position.x, it->transform->Position.y, it->transform->Position.z, radius);
				}
			}

			if(renderModel)	
			{				
				it->Draw( context, dt, worldMatrix, viewMatrix, projectionMatrix, 
						it->pTextureArray->GetTextureArray(), m_Camera->GetPosition(), m_Lights);

				renderCount++;
			}
		}
	}

	for(UINT i = 0; i < 4; ++i)
        renderTargets[i] = NULL;
    context->OMSetRenderTargets(4, renderTargets, NULL);

    ComputeLightTiles();
}

void Graphics::Bloom(float dt)
{
	ID3D11DeviceContext* context = m_D3D->GetDeviceContext();

	if(MsaaEnabled())    
        pp->Downscale(context, deferredOutputTarget->SRView, colorTarget->RTView);
    else 
        context->CopyResource(colorTarget->Texture, deferredOutputTarget->Texture);

    PostProcessor::Constants constants;
    constants.BloomThreshold = BloomThreshold;
    constants.BloomMagnitude = BloomMagnitude;
    constants.BloomBlurSigma = BloomBlurSigma;
    constants.Tau = AdaptationRate;
    constants.KeyValue = ExposureKeyValue;
    constants.TimeDelta = dt;

	pp->SetConstants(constants);
	pp->Render(context, colorTarget->SRView, m_D3D->GetBackBuffer());
}

void Graphics::GlowRender(float dt)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	ID3D11DeviceContext* context = m_D3D->GetDeviceContext();

	ID3D11RenderTargetView* rts[2] = { NULL, NULL };
	
	rts[0] = GlowMapRT->GetRTV();
	rts[1] = PreBlurRT->GetRTV();

	float color[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	
	context->OMSetRenderTargets(2, rts, depthBuffer->DSView);

	for(UINT i = 0; i < 2; ++i)
		context->ClearRenderTargetView(rts[i], color);

	ObjectLinkList<ModelComponent>::iterator it = ModelComponentList.begin();

	if(!DebugDraw)
	{		
		for(  ; it!=ModelComponentList.end(); ++it)
		{
			if(it->pTextureArray->mtt != Glow)
				continue;

			if(EDITOR && EDITOR->inEditor && EDITOR->ShowWhatIsExcludedFromBoxes())
			{
				//do nothing here
			}
			else
			{
				if(!DetermingIfInsideAdjacentToHeroLevelBox(it->GetOwner()->LevelBoxId ))
					continue;
			}
			
			renderModel=true;
			if(EDITOR && !EDITOR->inEditor)//skip in editor mode
			{
				if (it->GetOwner()->has(RigidBody))
				{
					CollisionPrimitive * colp=it->GetOwner()->has(CollisionPrimitive);
					Vector3 pos=colp->aabb->center;
					Vector3 scale=colp->aabb->halfSize;
					renderModel=m_Frustum->CheckRectangle(pos.x,pos.y,pos.z,scale.x,scale.y,scale.z);
				}
				else
				{
					float radius = sqrt(lengthSquared(it->transform->Scale));
					renderModel = m_Frustum->CheckSphere(it->transform->Position.x, it->transform->Position.y, it->transform->Position.z, radius);
				}
			}

			if(renderModel)	
			{				
				Mat4 matTrans, matQuadRotation, matScale, meshMatrix, matModelWorldView;
				D3DXMatrixTranslation(&matTrans, it->transform->Position.x, it->transform->Position.y, it->transform->Position.z);
				D3DXMatrixRotationQuaternion(&matQuadRotation, &it->transform->QUATERNION);
				D3DXMatrixScaling(&matScale, it->transform->Scale.x, it->transform->Scale.y, it->transform->Scale.z);
				meshMatrix = matScale * matQuadRotation * matTrans;
				matModelWorldView = meshMatrix * worldMatrix;
				it->pModel->SetAndDraw( context , it->transform->Scale);								
				m_GlowMapShader->Render(context, it->pModel->GetIndexCount(), matModelWorldView, viewMatrix, projectionMatrix, it->pTextureArray->GetTextureArray()[0], it->pTextureArray->GetTextureArray()[1]);

				renderCount++;
			}
		}
	}

	Blerg->DownSampleTexture(context, 0, IdentityMatrix, IdentityMatrix, IdentityMatrix, 0, depthBuffer->DSView);
	Blerg->RenderHorizontalBlurToTexture(context, 0, IdentityMatrix, IdentityMatrix, IdentityMatrix, 0, depthBuffer->DSView);
	Blerg->RenderVerticalBlurToTexture(context, 0, IdentityMatrix, IdentityMatrix, IdentityMatrix, 0, depthBuffer->DSView);
	Blerg->UpSampleTexture(context, 0, IdentityMatrix, IdentityMatrix, IdentityMatrix, 0, depthBuffer->DSView);
	Blerg->RenderGlowScene(context, 0, IdentityMatrix, IdentityMatrix, IdentityMatrix, 0, depthBuffer->DSView);

	m_D3D->Reset(depthBuffer->DSView);
	m_D3D->GetOrthoMatrix(orthoMatrix);
	bool res;
	// Turn off the Z buffer to begin all 2D rendering.
	GRAPHICS->GetD3D()->TurnZBufferOff();
	Blerg->m_FullScreenWindow->Render(m_D3D->GetDeviceContext());
	// Render the bitmap with the texture shader.
	m_D3D->TurnOnAlphaBlending();
	res = m_TextureShader->Render(context, Blerg->m_FullScreenWindow->GetIndexCount(), IdentityMatrix, IdentityMatrix, orthoMatrix, GlowOutput->GetShaderResourceView());
	if(!res){return;}
	m_D3D->TurnOffAlphaBlending();	
	// Turn off the Z buffer to begin all 2D rendering.
	GRAPHICS->GetD3D()->TurnZBufferOn();
}

void Graphics::ForwardRender(float dt)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	ObjectLinkList<ModelComponent>::iterator it = ModelComponentList.begin();
	m_D3D->Reset(depthBuffer->DSView);

	if(!DebugDraw)
	{
		// Render deferred here
		for(  ; it!=ModelComponentList.end(); ++it)
		{
			if(it->pTextureArray->mtt != SingleTexture && it->pTextureArray->mtt != TextureColorFR)
				continue;

			if(EDITOR && EDITOR->inEditor && EDITOR->ShowWhatIsExcludedFromBoxes())
			{
				//do nothing here
			}
			else
			{
				if(!DetermingIfInsideAdjacentToHeroLevelBox(it->GetOwner()->LevelBoxId ))
					continue;
			}			

			renderModel=true;
			if(EDITOR && !EDITOR->inEditor)//skip in editor mode
			{
				if (it->GetOwner()->CompositionTypeId==GOC_Type_LightWithoutBulb)//skip for lights without bulb
					continue;
				if (it->GetOwner()->has(RigidBody))
				{
					CollisionPrimitive * colp=it->GetOwner()->has(CollisionPrimitive);
					Vector3 pos=colp->aabb->center;
					Vector3 scale=colp->aabb->halfSize;
					renderModel=m_Frustum->CheckRectangle(pos.x,pos.y,pos.z,scale.x,scale.y,scale.z);
				}
				else
				{
					float radius = sqrt(lengthSquared(it->transform->Scale));
					renderModel = m_Frustum->CheckSphere(it->transform->Position.x, it->transform->Position.y, it->transform->Position.z, radius);
				}
			}

			if(renderModel)	
			{				
				it->Draw( m_D3D->GetDeviceContext(), dt, worldMatrix, viewMatrix, projectionMatrix, 
						it->pTextureArray->GetTextureArray(), m_Camera->GetPosition(), m_Lights);

				renderCount++;
			}
		}
	}
}

void Graphics::TransparentRender(float dt)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	if(!DebugDraw)
	{
		//Drawing transparent objects in 2nd pass
		ObjectLinkList<ModelComponent>::iterator it = ModelComponentList.begin();

		for(  ; it!=ModelComponentList.end(); ++it )
		{

			if(it->pTextureArray->mtt != Transparent)
				continue;

			if(EDITOR && EDITOR->inEditor && EDITOR->ShowWhatIsExcludedFromBoxes())
			{
				//do nothing here
			}
			else
			{
				if(!DetermingIfInsideAdjacentToHeroLevelBox(it->GetOwner()->LevelBoxId ))
					continue;
			}

			renderModel=true;
			if(EDITOR && !EDITOR->inEditor)//skip in editor mode
			{
				if (it->GetOwner()->has(RigidBody))
				{
					CollisionPrimitive * colp=it->GetOwner()->has(CollisionPrimitive);
					Vector3 pos=colp->aabb->center;
					Vector3 scale=colp->aabb->halfSize;
					renderModel=m_Frustum->CheckRectangle(pos.x,pos.y,pos.z,scale.x,scale.y,scale.z);
				}
				else
				{
					float radius = sqrt(lengthSquared(it->transform->Scale));
					renderModel = m_Frustum->CheckSphere(it->transform->Position.x, it->transform->Position.y, it->transform->Position.z, radius);
				}
			}

			if(renderModel)	
			{
				if((it)->GetOwner()->CompositionTypeId==GOC_Type_LevelBox || (it)->GetOwner()->CompositionTypeId==GOC_Type_TutorialBox)
				{
					if(EDITOR && EDITOR->inEditor && EDITOR->GetShowLevelBoxes())
					{
						//do nothing here
					}
					else//dont draw the level data boxes
						continue;
				}
				



				// Cubes should be masked while transparent so let them die using original depth stencil state
				if((it)->GetOwner()->CompositionTypeId==GOC_Type_Cube)
				{
					(it)->Draw( m_D3D->GetDeviceContext(), dt, worldMatrix, viewMatrix, projectionMatrix, 
						(it)->pTextureArray->GetTextureArray(), m_Camera->GetPosition(), m_Lights);
				}
				else
				{
					// All other objects should have zero mask while transparent so draw using modified depth stencil state
					m_D3D->TurnOnTransparentDSS();
					(it)->Draw( m_D3D->GetDeviceContext(), dt, worldMatrix, viewMatrix, projectionMatrix, 
						(it)->pTextureArray->GetTextureArray(), m_Camera->GetPosition(), m_Lights);
					m_D3D->TurnOffTransparentDSS();
				}

				renderCount++;
			}
		}
	}
}

void Graphics::Render2D(float dt)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	bool result;

	lineDrawer.DrawLineFromList(worldMatrix, viewMatrix, projectionMatrix);

	// Turn on alpha blending.
	m_D3D->TurnOnAlphaBlending();
	//Update Particle System
	result = m_ParticleSystem->UpdateParticleSystem(dt, m_D3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, m_Camera->GetPosition(), GetTexture("particle_circle_mask")->GetTextureArray() );
	if(!result){return;}
	// Turn off alpha blending.
	m_D3D->TurnOffAlphaBlending();	

	// Turn off the Z buffer to begin all 2D rendering.
	if (EDITOR){
		if(EDITOR->inEditor)
		{
			m_D3D->TurnZBufferOff();
			EDITOR->Draw();
			lineDrawer.DrawLineFromList(worldMatrix, viewMatrix, projectionMatrix );
			m_D3D->TurnZBufferOn();
		}
	}

	RenderUI(dt);
}

void Graphics::RenderUI(float dt)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	bool result;

	m_D3D->TurnZBufferOff();

	// CROSSHAIR
	// Get the world, view, and ortho matrices from the camera and d3d objects.
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);
	m_D3D->TurnOnAlphaBlending();
	result = m_Bitmap->Render(m_D3D->GetDeviceContext(), screen_width/2.0f-30.0f, screen_height/2.0f-30.0f );
	if(!result){return;}
	// Render the bitmap with the texture shader.
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), IdentityMatrix, IdentityMatrix, orthoMatrix, m_Bitmap->GetTextureArray());
	if(!result){return;}
	g_textureDraw->Render( m_TextureShader, &IdentityMatrix, &IdentityMatrix, &orthoMatrix );
	m_D3D->TurnOffAlphaBlending();


	// RENDER TO TEXTURE WINDOW
	if(DW1){
		// Get the world, view, and ortho matrices from the camera and d3d objects.
		m_D3D->GetWorldMatrix(worldMatrix);
		m_D3D->GetOrthoMatrix(orthoMatrix);	
		// Put the debug window vertex and index buffers on the graphics pipeline to prepare them for drawing.
		result = m_DebugWindow1->Render(m_D3D->GetDeviceContext(), 950, 25);
		if(!result){ return; }
		// Render the debug window using the texture shader.
		result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_DebugWindow1->GetIndexCount(), IdentityMatrix, IdentityMatrix, orthoMatrix, PreBlurRT->GetShaderResourceView());
		if(!result){return;}
		
	}
	if(DW2){
		// Get the world, view, and ortho matrices from the camera and d3d objects.
		m_D3D->GetWorldMatrix(worldMatrix);
		m_D3D->GetOrthoMatrix(orthoMatrix);	
		// Put the debug window vertex and index buffers on the graphics pipeline to prepare them for drawing.
		result = m_DebugWindow2->Render(m_D3D->GetDeviceContext(), 950, 25);
		if(!result){ return; }
		// Render the debug window using the texture shader.
		result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_DebugWindow2->GetIndexCount(), IdentityMatrix, IdentityMatrix, orthoMatrix, Blerg->m_DownSampleTexure->GetShaderResourceView());
		if(!result){return;}
	}
	if(DW3){
		// Get the world, view, and ortho matrices from the camera and d3d objects.
		m_D3D->GetWorldMatrix(worldMatrix);
		m_D3D->GetOrthoMatrix(orthoMatrix);	
		// Put the debug window vertex and index buffers on the graphics pipeline to prepare them for drawing.
		result = m_DebugWindow3->Render(m_D3D->GetDeviceContext(), 950, 25);
		if(!result){ return; }
		// Render the debug window using the texture shader.
		result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_DebugWindow3->GetIndexCount(), IdentityMatrix, IdentityMatrix, orthoMatrix, Blerg->m_HorizontalBlurTexture->GetShaderResourceView());
		if(!result){return;}
	}
	if(DW4){
		// Get the world, view, and ortho matrices from the camera and d3d objects.
		m_D3D->GetWorldMatrix(worldMatrix);
		m_D3D->GetOrthoMatrix(orthoMatrix);	
		// Put the debug window vertex and index buffers on the graphics pipeline to prepare them for drawing.
		result = m_DebugWindow4->Render(m_D3D->GetDeviceContext(), 950, 25);
		if(!result){ return; }
		// Render the debug window using the texture shader.
		result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_DebugWindow4->GetIndexCount(), IdentityMatrix, IdentityMatrix, orthoMatrix, Blerg->m_VerticalBlurTexture->GetShaderResourceView());
		if(!result){return;}
	}
	if(DW5){
		// Get the world, view, and ortho matrices from the camera and d3d objects.
		m_D3D->GetWorldMatrix(worldMatrix);
		m_D3D->GetOrthoMatrix(orthoMatrix);	
		// Put the debug window vertex and index buffers on the graphics pipeline to prepare them for drawing.
		result = m_DebugWindow5->Render(m_D3D->GetDeviceContext(), 950, 25);
		if(!result){ return; }
		// Render the debug window using the texture shader.
		result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_DebugWindow5->GetIndexCount(), IdentityMatrix, IdentityMatrix, orthoMatrix, GlowOutput->GetShaderResourceView());
		if(!result){return;}
	}
	if(DW6){
		// Get the world, view, and ortho matrices from the camera and d3d objects.
		m_D3D->GetWorldMatrix(worldMatrix);
		m_D3D->GetOrthoMatrix(orthoMatrix);	
		// Put the debug window vertex and index buffers on the graphics pipeline to prepare them for drawing.
		result = m_DebugWindow6->Render(m_D3D->GetDeviceContext(), 950, 25);
		if(!result){ return; }
		// Render the debug window using the texture shader.
		result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_DebugWindow6->GetIndexCount(), IdentityMatrix, IdentityMatrix, orthoMatrix, GlowMapRT->GetShaderResourceView());
		if(!result){return;}
	}

	//
	// TEXT
	// Generate the view matrix based on the Text camera's position.
	m_D3D->GetOrthoMatrix(orthoMatrix);
	// Turn on the alpha blending before rendering the text.
	m_D3D->TurnOnAlphaBlending();
	// Render the text strings.
	result = m_Text->Render(m_D3D->GetDeviceContext(), IdentityMatrix, orthoMatrix);
	if(!result){return;}
	g_textDisplay->Update( false );
	// Turn off alpha blending after rendering the text.
	m_D3D->TurnOffAlphaBlending();
	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_D3D->TurnZBufferOn();
}

void Graphics::RenderScene(float dt)
{
	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	renderCount = 0;
	renderModel = false;

	// -----------------------------------------------------------------------------------------------------
	// Begin 3D Rendering
	// -----------------------------------------------------------------------------------------------------	

	// -----------------------------------------------------------------------------------------------------
	// Deferred Rendering
	// -----------------------------------------------------------------------------------------------------	
	DeferredRender(dt);

	// -----------------------------------------------------------------------------------------------------
	// Bloom
	// -----------------------------------------------------------------------------------------------------	
	Bloom(dt);

	// -----------------------------------------------------------------------------------------------------
	//Prepare for Forward Rendering
	// -----------------------------------------------------------------------------------------------------
	m_D3D->Reset(depthBuffer->DSView);

	// -----------------------------------------------------------------------------------------------------
	// Glow
	// -----------------------------------------------------------------------------------------------------
	GlowRender(dt);

	// -----------------------------------------------------------------------------------------------------
	// Forward Rendering
	// -----------------------------------------------------------------------------------------------------
	ForwardRender(dt);

	// -----------------------------------------------------------------------------------------------------
	// Transparent
	// -----------------------------------------------------------------------------------------------------
	TransparentRender(dt);
	
	// -----------------------------------------------------------------------------------------------------
	// End 3D Rendering
	// -----------------------------------------------------------------------------------------------------	
	print("renderCount: ", renderCount);		


	// -----------------------------------------------------------------------------------------------------
	// Begin 2D Rendering
	// -----------------------------------------------------------------------------------------------------

	Render2D(dt);	

	// -----------------------------------------------------------------------------------------------------
	// End 2D Rendering
	// -----------------------------------------------------------------------------------------------------

	// Present the rendered scene to the screen.
	m_D3D->EndScene();
}
}

