////////////////////////////////////////////////////////////////////////////////
// Filename: texturearrayclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TEXTUREARRAYCLASS_H_
#define _TEXTUREARRAYCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx11tex.h>

namespace Framework {

enum MultiTextureType
{
	SingleTexture = 0,
	TextureBlend = 1,
	LightMap = 2,
	AlphaBlend = 3,
	Fog = 4,
	ClipPlane = 5,
	Transparent = 6,
	Glass = 7,
	Bump = 8, 
	Spec = 9, 
	Invisible = 10,
	LightBulbColor = 11,
	DeferredShading = 12, 
	lava = 13,
	BlurType = 14,
	TextureColor = 15,
	Glow = 16, 
	mtt_Animation = 17,
	TextureColorFR = 18
};

////////////////////////////////////////////////////////////////////////////////
// Class name: TextureArrayClass
////////////////////////////////////////////////////////////////////////////////
class TextureArrayClass
{
public:
	TextureArrayClass();
	TextureArrayClass(const TextureArrayClass&);
	~TextureArrayClass();

	bool Initialize(ID3D11Device*, const char*, const char*, const char*, MultiTextureType _mtt = SingleTexture, bool Extendable = false);
	void Shutdown();

	ID3D11ShaderResourceView** GetTextureArray();

	MultiTextureType mtt;
	bool Extendable;

private:
	ID3D11ShaderResourceView* m_textures[3];
};
}

#endif