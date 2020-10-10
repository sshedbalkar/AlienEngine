////////////////////////////////////////////////////////////////////////////////
// Filename: lightclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _LIGHTCLASS_H_
#define _LIGHTCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3dx10math.h>
#include "VMath.h"
#include "LightComponent.h"
namespace Framework {

////////////////////////////////////////////////////////////////////////////////
// Class name: LightClass
////////////////////////////////////////////////////////////////////////////////

class RenderTextureClass;
class D3DClass;
	
class LightClass
{
public:
	LightClass();
	LightClass(const LightClass&);
	~LightClass();

	void SetAmbientColor(float, float, float, float);
	void SetAmbientColor(Vec4 &);
	void SetDiffuseColor(float, float, float, float);
	void SetDiffuseColor(Vec4 &);
	void SetSpecularColor(float, float, float, float);
	void SetSpecularColor(Vec4 &);
	void SetSpecularPower(float &);
	void SetPosition(float, float, float);
	void SetPosition(Vec3 &);
	void GenerateCubeMapViewMatrix();
	void SetDepthTexture(ID3D11ShaderResourceView*);
	void SetRenderTarget(RenderTextureClass*);
	void SetShadow(bool choice);
	void SetAttenuation0(float );
	void SetAttenuation1(float );
	void SetAttenuation2(float );
	const float & GetAttenuation0() const;
	const float & GetAttenuation1() const;
	const float & GetAttenuation2() const;
	void SetLightProperties(LightComponent *);

	void InitializeRenderTarget();

	const D3DXVECTOR4 & GetAmbientColor() const;
	const D3DXVECTOR4 & GetDiffuseColor() const;
	const D3DXVECTOR4 & GetSpecularColor() const;
	const float & GetSpecularPower() const;
	const D3DXVECTOR4 & GetPosition() const;
	void GetCubeMapViewMatrix(D3DXMATRIX[]) const;
	ID3D11ShaderResourceView* GetDepthTexture() const;
	RenderTextureClass* GetRenderTarget() const;	
	bool GetShadow() const;
	void Shutdown();

private:
	D3DXVECTOR4 m_ambientColor;
	D3DXVECTOR4 m_diffuseColor;
	D3DXVECTOR3 m_direction;
	D3DXVECTOR4 m_specularColor;
	float m_specularPower;
	float m_attenuation0;
	float m_attenuation1;
	float m_attenuation2;

	bool CastShadow;

	ID3D11ShaderResourceView* depthTexture;

	D3DXVECTOR4 m_position;
	D3DXMATRIX g_LightViewProjMatrices[6];

	RenderTextureClass* RTO;
};
}
#endif