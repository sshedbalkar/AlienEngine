////////////////////////////////////////////////////////////////////////////////
// Filename: texturearrayclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Precompiled.h"
#include "texturearrayclass.h"

namespace Framework {

TextureArrayClass::TextureArrayClass()
{
	m_textures[0] = 0;
	m_textures[1] = 0;
	m_textures[2] = 0;
	Extendable = false;
}


TextureArrayClass::TextureArrayClass(const TextureArrayClass& other)
{
}


TextureArrayClass::~TextureArrayClass()
{
	Shutdown();
}


bool TextureArrayClass::Initialize(ID3D11Device* device, const char* filename1, const char* filename2, const char* filename3, MultiTextureType _mtt, bool _Extandable)
{
	HRESULT result;

	if(filename1 != "")
	{
		// Load the first texture in.
		result = D3DX11CreateShaderResourceViewFromFile(device, filename1, NULL, NULL, &m_textures[0], NULL);
		if(FAILED(result))
		{
			return false;
		}
	}

	if(filename2 != "" && filename2!=NULL)
	{
		// Load the second texture in.
		result = D3DX11CreateShaderResourceViewFromFile(device, filename2, NULL, NULL, &m_textures[1], NULL);
		if(FAILED(result))
		{
			return false;
		}
	}

	if(filename3 != "" && filename3!=NULL)
	{
		// Load the third texture in.
		result = D3DX11CreateShaderResourceViewFromFile(device, filename3, NULL, NULL, &m_textures[2], NULL);
		if(FAILED(result))
		{
			return false;
		}
	}
	mtt = _mtt;
	Extendable = _Extandable;
	return true;
}


void TextureArrayClass::Shutdown()
{
	// Release the texture resources.
	if(m_textures[0])
	{
		m_textures[0]->Release();
		m_textures[0] = 0;
	}

	if(m_textures[1])
	{
		m_textures[1]->Release();
		m_textures[1] = 0;
	}

	if(m_textures[2])
	{
		m_textures[2]->Release();
		m_textures[2] = 0;
	}

	return;
}


ID3D11ShaderResourceView** TextureArrayClass::GetTextureArray()
{
	return m_textures;
}
}