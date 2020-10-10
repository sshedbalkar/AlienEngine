#pragma once
//
#include "Singleton.h"
//
#define MAX_TEX_FILE_NAME_LEN	128
//
struct ID3D11ShaderResourceView;
struct D3DXMATRIX;
//
namespace Framework
{
	class TextureDraw;
	class BitmapClass;
	class TextureShaderClass;
	//
	class TextureData
	{
	private:
								TextureData( unsigned int id, const char* file );
								~TextureData();
	friend						TextureDraw;
	//
	public:
		//
	private:
		int						Init();
		//
	private:
		BitmapClass*			m_bitmap;
		unsigned int			m_id;
		char					m_fileName[ MAX_TEX_FILE_NAME_LEN ];
		bool					m_render;
		float					m_posX;
		float					m_posY;
		int						m_width;
		int						m_height;
		//
	};
	//
	class TextureDraw: public Singleton <TextureDraw>
	{
	public:
								TextureDraw();
								~TextureDraw();
	//
	public:
	unsigned int				CreateTexture( const char* file );
	static int					GetTextureDimensions( ID3D11ShaderResourceView* srv, int& width, int& height );
	int							Render( TextureShaderClass* texShader, D3DXMATRIX* worldMatrix, D3DXMATRIX* viewMatrix, D3DXMATRIX* projectionMatrix );
	int							SetPosition( unsigned int id, const float& posx, const float& posy );
	int							SetDisplay( unsigned int id, bool val );
	int							GetDimensions( unsigned int id, int& width, int& height );
	int							DeleteTexture( unsigned int id );
	//
	private:
	//
	};
}
//