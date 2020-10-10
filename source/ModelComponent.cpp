#include "Precompiled.h"
#include "ModelComponent.h"
#include "Composition.h"
#include "Transform.h"
#include "Graphics.h"
#include "ModelClass.h"
#include "AnimateShaderClass.h"
#include "lightmapshaderclass.h"
#include "alphamapshaderclass.h"
#include "specmapshaderclass.h"
#include "bumpmapshaderclass.h"
#include "fogshaderclass.h"
#include "clipplaneshaderclass.h"
#include "glassshaderclass.h"
#include "d3dclass.h"
#include "texturearrayclass.h"
#include "Deferred.h"
#include <iostream>
#include "ParticleComponent.h"
#include "Blur.h"

namespace Framework
{

//===================================================
ModelComponent::ModelComponent() : 
transform (NULL), 
pModel( NULL ), 
pTextureArray(NULL), 
animationController(NULL)
{
	TextureName = "";
	SwapTextureName="";
	InitialSwapTextureName="";
	Transparency=0.5f;
	TextMultiplier=1.0f;
}
//===================================================
ModelComponent::~ModelComponent()
{
	if (!this->GetOwner()->OnRemovedList)
		GRAPHICS->ModelComponentList.erase( this );
	if(animationController){
		delete animationController;
		animationController = NULL;
	}
}
void ModelComponent::RemoveComponentFromGame()
{
	GRAPHICS->ModelComponentList.erase( this );
}
void ModelComponent::RestoreComponentToGame()
{
	GRAPHICS->ModelComponentList.push_back( this );
}
//===================================================
void ModelComponent::Serialize( ISerializer& stream )
{
	ISerializer* str = stream.clone();
	if(str->isGood())
	{
		str->readValue("ModelName", ModelName);
		str->readValue("TextureName", TextureName);
		str->readValue("SwapTextureName", SwapTextureName);
		str->readValue("Alpha", Transparency);
		str->readValue("TexMultiplier", TextMultiplier);
		InitialSwapTextureName=SwapTextureName;
		InitialTextureName=TextureName;
	}
	delete str;
}
//===================================================
void ModelComponent::SwapTexture()
{
	if (SwapTextureName.size()>0)
	{
		TextureName.swap(SwapTextureName);
		pTextureArray = GRAPHICS->GetTexture( TextureName );
	}
}
void ModelComponent::ChangeToTexture(std::string & s)
{
	pTextureArray = GRAPHICS->GetTexture( s );
}
void ModelComponent::DefaultTexture()
{
	TextureName=InitialTextureName;
	SwapTextureName=InitialSwapTextureName;
	pTextureArray = GRAPHICS->GetTexture( TextureName );
}

void ModelComponent::Initialize()
{
	transform = GetOwner()->has( Transform );
	if ( !transform ) __debugbreak();
	GRAPHICS->ModelComponentList.push_back( this );
	pModel = GRAPHICS->GetModel( ModelName );
	if(!pModel) __debugbreak();
	//pTextureArray = pModel->m_TextureArray;
	//if(TextureName!= ""){
		pTextureArray = GRAPHICS->GetTexture( TextureName );
		if(!pTextureArray) __debugbreak();
	//}
	pModel->Scale = this->transform->Scale;

	timer = 0.0f;

	if(pModel->animationContent){
		animationController = new AnimationController(pModel->animationContent);
	}

//	TexturedBox* box_material = (TexturedBox*)GRAPHICS->GetMaterial( MAT_TEXTUREDBOX );
//	if ( box_material ){
//		if ( texture ){
//			box_material->SetTexture( &texture, shininess, bloom_intensity, tex_method, flag );
//			box_material->RegisterVolume( transform, 0 );
//		}
//	}
//
//	Material* top_material = GRAPHICS->GetMaterial( top );
//	if ( top_material )
//		top_material->RegisterVolume( transform, 1 );
//	
//	Material* bottom_material = GRAPHICS->GetMaterial( bottom );
//	if ( bottom_material )
//		bottom_material->RegisterVolume( transform, -1 );
}

void ModelComponent::Draw(ID3D11DeviceContext* deviceContext, float dt, const D3DXMATRIX& worldMatrix, const D3DXMATRIX& WorldviewMatrix, const
	D3DXMATRIX& WorldprojectionMatrix, ID3D11ShaderResourceView** textureArray, const D3DXVECTOR3& cameraPosition, const std::vector<LightClass>& lights)
{
	const D3DXVECTOR4& ambientColor = D3DXVECTOR4(0.2f, 0.2f, 0.2f, 0.2f), diffuseColor = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), specularColor = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	const float& specularPower = 250;
	
	//transform->QUATERNION *= TransformIntoQuaterion( dt*0.1, Vec3(0,1,0) ); 
	D3DXVECTOR3 lightDirection = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	Mat4 matTrans, matQuadRotation, matScale, meshMatrix, matModelWorldView;
	D3DXMatrixTranslation(&matTrans, transform->Position.x, transform->Position.y, transform->Position.z);
	D3DXMatrixRotationQuaternion(&matQuadRotation, &transform->QUATERNION);
	D3DXMatrixScaling(&matScale, transform->Scale.x, transform->Scale.y, transform->Scale.z);

	meshMatrix = matScale * matQuadRotation * matTrans;
	matModelWorldView = meshMatrix * worldMatrix;
							
	bool result;
	pModel->SetAndDraw(deviceContext, this->transform->Scale );
	
	if(animationController){
		if(!animationController->animPause) animationController->Update(dt);
		//if( GRAPHICS->BindPose )
		if(0)
			animationController->ProcessBindPose(pModel->animationContent->skel);
		else
			animationController->Process(pModel->animationContent->skel);

		D3DXMATRIX boneMatrices[80];
		//boneMatrices.resize( pModel->Controller->skel->bones.size() );
		for(unsigned int i=0;i < pModel->animationContent->skel->bones.size() ; ++i) {
			//The matrices passed to the shader transform the vertex into bone space and then apply the bones animation
			boneMatrices[i] = pModel->animationContent->skel->GetModelToBoneSpaceTransform( i ) * animationController->BoneMatrixBuffer[i];
			D3DXMatrixTranspose( &boneMatrices[i], &boneMatrices[i] );
		}
		//Render the model using the texture shader.
		result = GRAPHICS->m_AnimateShader->Render( deviceContext, pModel->GetIndexCount(), matModelWorldView, WorldviewMatrix, WorldprojectionMatrix, pTextureArray->GetTextureArray(), lightDirection, ambientColor, diffuseColor, cameraPosition, specularColor, specularPower, boneMatrices);
		if(!result){__debugbreak();return;}

		for( unsigned int b=0 ; b < pModel->animationContent->skel->bones.size() ; ++b)
		{
			Bone& bone = pModel->animationContent->skel->bones[b];
			for( unsigned int c=0 ; c<bone.Children.size() ; ++c)
			{
				Vec3 points[2];
				Vec3 p1, p2;
				D3DXMATRIX tempMat1, tempMat2;
				points[0] = GetPosition( animationController->BoneMatrixBuffer[ bone.BoneIndex ] );
				points[1] = GetPosition( animationController->BoneMatrixBuffer[ bone.Children[c]->BoneIndex ] );
				D3DXVec3TransformCoord(&p1, &points[0], &matModelWorldView);
				D3DXVec3TransformCoord(&p2, &points[1], &matModelWorldView);
				//GRAPHICS->m_D3D->TurnZBufferOff();
				//GRAPHICS->lineDrawer.DrawLine(points[0], points[1], Vec4(0,1,0,1), matModelWorldView, WorldviewMatrix, WorldprojectionMatrix );
				//GRAPHICS->m_D3D->TurnZBufferOn();
				GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex( p1, Vec4(0,1,0,1)) );
				GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex( p2, Vec4(0,1,0,1)) );
				//GRAPHICS->pLineDrawer->DrawTransform(points,2,&matModelWorldViewProj,D3DCOLOR_RGBA(0,255,0,255) );
			}
		}
	}else{

		D3DXVECTOR3 LightColor = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		if(this->GetOwner()->has(LightComponent))
			LightColor = this->GetOwner()->has(LightComponent)->Color;

		switch(pTextureArray->mtt){
		case SingleTexture:
		case Bump:
		case TextureColorFR:
		case TextureBlend:
			result = GRAPHICS->m_BumpMapShader->Render(deviceContext, pModel->GetIndexCount(), matModelWorldView, WorldviewMatrix, WorldprojectionMatrix, pTextureArray->GetTextureArray(), cameraPosition, this->transform->Scale, static_cast<unsigned>(pTextureArray->mtt), lights, Transparency, TextMultiplier);
			if(!result){ __debugbreak(); return; }
			break;
		case Transparent:
			GRAPHICS->m_D3D->TurnOnAlphaBlending();
			result = GRAPHICS->m_BumpMapShader->Render(deviceContext, pModel->GetIndexCount(), matModelWorldView, WorldviewMatrix, WorldprojectionMatrix, pTextureArray->GetTextureArray(), cameraPosition, this->transform->Scale, static_cast<unsigned>(pTextureArray->mtt), lights, Transparency, TextMultiplier);
			if(!result){return;}
			GRAPHICS->m_D3D->TurnOffAlphaBlending();
			break;
		case LightMap:
			result = GRAPHICS->m_LightMapShader->Render(deviceContext, pModel->GetIndexCount(), matModelWorldView, WorldviewMatrix, WorldprojectionMatrix, pTextureArray->GetTextureArray());
			if(!result){__debugbreak();return;}
			break;
		case AlphaBlend:
			result = GRAPHICS->m_AlphaMapShader->Render(deviceContext, pModel->GetIndexCount(), matModelWorldView, WorldviewMatrix, WorldprojectionMatrix, pTextureArray->GetTextureArray());
			if(!result){__debugbreak();return;}
			break;
		case Fog:
			result = GRAPHICS->m_FogShader->Render(deviceContext, pModel->GetIndexCount(), matModelWorldView, WorldviewMatrix, WorldprojectionMatrix, 
				     pTextureArray->GetTextureArray(), GRAPHICS->fogStart, GRAPHICS->fogEnd);
			if(!result){return;}
			break;
		case ClipPlane:
			result = GRAPHICS->m_ClipPlaneShader->Render(deviceContext, pModel->GetIndexCount(), matModelWorldView, WorldviewMatrix, WorldprojectionMatrix, 
				     pTextureArray->GetTextureArray(), GRAPHICS->clipPlane);
			if(!result){return;}
			break;
		case lava:
			this->GetOwner()->has(ParticleComponent)->CreateParticlesLava(dt);
			timer += dt;
		case LightBulbColor:
		case TextureColor:
		case DeferredShading:
			GRAPHICS->Def->RenderGBuffer(deviceContext, matModelWorldView, WorldviewMatrix, WorldprojectionMatrix, pTextureArray->GetTextureArray(), pModel->GetIndexCount(), this->transform->Scale, TextMultiplier, pTextureArray->Extendable, static_cast<unsigned>(pTextureArray->mtt), LightColor, timer);
			break;
		case Spec:			
			break;
		case Glass:
			break;
		case Invisible:
			break;
		default:
			__debugbreak();
		}
	}
}

//===================================================
void ModelComponent::Free()
{
}
//===================================================
void ModelComponent::ChangeTextureDebug( )
{
	pTextureArray = GRAPHICS->GetTexture( "white" );
	if(!pTextureArray) __debugbreak();
}
void ModelComponent::ChangeTextureBack( ){
	pTextureArray = GRAPHICS->GetTexture( TextureName );
	if(!pTextureArray) __debugbreak();
}


}//end namespace