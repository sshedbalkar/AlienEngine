////////////////////////////////////////////////////////////////////////////////
// Filename: lightclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Precompiled.h"
#include "lightclass.h"
#include "rendertextureclass.h"
#include "Graphics.h"
#include "d3dclass.h"

namespace Framework {

LightClass::LightClass()
{
	RTO = 0;
	CastShadow = false;
	depthTexture = 0;
}


LightClass::LightClass(const LightClass& other)
{
	m_ambientColor = other.GetAmbientColor();
	m_diffuseColor = other.GetDiffuseColor();
	m_specularColor = other.GetSpecularColor();
	m_specularPower = other.GetSpecularPower();
	m_position = other.GetPosition();
	other.GetCubeMapViewMatrix(g_LightViewProjMatrices);
	depthTexture = other.GetDepthTexture();
	RTO = other.GetRenderTarget();
	CastShadow = other.CastShadow;
	m_attenuation0 = other.m_attenuation0;
	m_attenuation1 = other.m_attenuation1;
	m_attenuation2 = other.m_attenuation2;
}


LightClass::~LightClass()
{
}

void LightClass::SetAmbientColor(float red, float green, float blue, float alpha)
{
	m_ambientColor = D3DXVECTOR4(red, green, blue, alpha);
	return;
}


void LightClass::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = D3DXVECTOR4(red, green, blue, alpha);
	return;
}

void LightClass::SetPosition(float x, float y, float z)
{
	m_position = D3DXVECTOR4(x, y, z, 1.0f);
	return;
}

void LightClass::SetSpecularColor(float red, float green, float blue, float alpha)
{
	m_specularColor = D3DXVECTOR4(red, green, blue, alpha);
	return;
}

void LightClass::SetSpecularPower(float & power)
{
	m_specularPower = power;
	return;
}


void LightClass::SetAmbientColor(Vec4 & v)
{
	m_ambientColor=v;
}
void LightClass::SetDiffuseColor(Vec4 &v)
{
	m_diffuseColor=v;
}
void LightClass::SetSpecularColor(Vec4 &v)
{
	m_specularColor=v;
}
void LightClass::SetPosition(Vec3 &v)
{
	m_position.x=v.x;
	m_position.y=v.y;
	m_position.z=v.z;
	m_position.w = 1.0f;
}

const D3DXVECTOR4 & LightClass::GetAmbientColor() const
{
	return m_ambientColor;
}

const D3DXVECTOR4 & LightClass::GetDiffuseColor() const
{
	return m_diffuseColor;
}

const D3DXVECTOR4 & LightClass::GetPosition() const
{
	return  m_position;
}

const D3DXVECTOR4 & LightClass::GetSpecularColor() const
{
	return m_specularColor;
}

const float & LightClass::GetSpecularPower() const
{
	return m_specularPower;
}

void LightClass::SetAttenuation0(float f)
{
	m_attenuation0=f;
}
void LightClass::SetAttenuation1(float f)
{
	m_attenuation1=f;
}
void LightClass::SetAttenuation2(float f)
{
	m_attenuation2=f;
}
const float & LightClass::GetAttenuation0() const
{
	return m_attenuation0;
}
const float & LightClass::GetAttenuation1() const
{
	return m_attenuation1;
}
const float & LightClass::GetAttenuation2() const
{
	return m_attenuation2;
}


void LightClass::GetCubeMapViewMatrix(D3DXMATRIX CubeMapViewMatrix[]) const
{
	memcpy(CubeMapViewMatrix,g_LightViewProjMatrices,6*sizeof(D3DXMATRIX));
}

void LightClass::GenerateCubeMapViewMatrix()
{
	float lightX = m_position.x;
	float lightY = m_position.y;
	float lightZ = m_position.z;

	D3DXVECTOR3 g_LightPosition = D3DXVECTOR3(lightX, lightY, lightZ); // World Space

	D3DXMATRIX g_LightProjMatrix;
	D3DXMatrixPerspectiveFovLH(&g_LightProjMatrix,(float)D3DX_PI/2.0f, SHADOWMAP_WIDTH/SHADOWMAP_HEIGHT, 0.1f, 10000.0f);
	
	const FLOAT directionDelta = 0.1f;
	D3DXVECTOR3 LookDir;
    D3DXVECTOR3 UpDir;

	// +x, -x, +y, -y, +z, -z
	LookDir = D3DXVECTOR3(lightX + directionDelta, lightY, lightZ);
    UpDir   = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&g_LightViewProjMatrices[0], &g_LightPosition, &LookDir, &UpDir);
	g_LightViewProjMatrices[0] = g_LightViewProjMatrices[0] * g_LightProjMatrix;

	LookDir = D3DXVECTOR3(lightX - directionDelta, lightY, lightZ);
    UpDir   = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&g_LightViewProjMatrices[1], &g_LightPosition, &LookDir, &UpDir);
	g_LightViewProjMatrices[1] = g_LightViewProjMatrices[1] * g_LightProjMatrix;
	
	LookDir = D3DXVECTOR3(lightX, lightY + directionDelta, lightZ);
    UpDir   = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
	D3DXMatrixLookAtLH(&g_LightViewProjMatrices[2], &g_LightPosition, &LookDir, &UpDir);
	g_LightViewProjMatrices[2] = g_LightViewProjMatrices[2] * g_LightProjMatrix;
	
	LookDir = D3DXVECTOR3(lightX, lightY - directionDelta, lightZ);
    UpDir   = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	D3DXMatrixLookAtLH(&g_LightViewProjMatrices[3], &g_LightPosition, &LookDir, &UpDir);
	g_LightViewProjMatrices[3] = g_LightViewProjMatrices[3] * g_LightProjMatrix;
	
	LookDir = D3DXVECTOR3(lightX, lightY, lightZ + directionDelta);
    UpDir   = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&g_LightViewProjMatrices[4], &g_LightPosition, &LookDir, &UpDir);
	g_LightViewProjMatrices[4] = g_LightViewProjMatrices[4] * g_LightProjMatrix;
	
	LookDir = D3DXVECTOR3(lightX, lightY, lightZ - directionDelta);
    UpDir   = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&g_LightViewProjMatrices[5], &g_LightPosition, &LookDir, &UpDir);
	g_LightViewProjMatrices[5] = g_LightViewProjMatrices[5] * g_LightProjMatrix;

	return;
}

void LightClass::InitializeRenderTarget()
{
	bool result;
	// Create the cube map object.
	RTO = new RenderTextureClass;
	if(!RTO){return;}
	D3DClass* d3d = GRAPHICS->GetD3D();
	// Initialize the render to texture object.
	result = RTO->InitializeCubeMap(d3d->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result){return;}
}

void LightClass::SetDepthTexture(ID3D11ShaderResourceView* _depthTexture)
{
	depthTexture = _depthTexture;
}

ID3D11ShaderResourceView* LightClass::GetDepthTexture() const
{
	return depthTexture;
}

void LightClass::SetRenderTarget(RenderTextureClass* _RTO)
{
	RTO = _RTO;
}

RenderTextureClass* LightClass::GetRenderTarget() const
{
	return RTO;
}

void LightClass::SetShadow(bool choice)
{
	CastShadow = choice;
}

bool LightClass::GetShadow() const
{
	return CastShadow;
}


void LightClass::Shutdown()
{
	if(RTO){
		RTO->Shutdown();
		delete RTO;
		RTO = 0;
	}
}

void LightClass::SetLightProperties(LightComponent *LC)
{
	//SetAmbientColor(LC->AmbientColor);
	//SetDiffuseColor(LC->DiffuseColor);
	//SetSpecularColor(LC->SpecularColor);
	//SetSpecularPower(LC->SpecularPower);
	//SetShadow(LC->CanCastShadow);
	//SetAttenuation0(LC->Attenuation0);
	//SetAttenuation1(LC->Attenuation1);
	//SetAttenuation2(LC->Attenuation2);
}

}