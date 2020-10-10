#include "Precompiled.h"
#include "TextureDraw.h"
#include <iostream>
#include <hash_map>
#include "bitmapclass.h"
#include "Utility.h"
#include "Graphics.h"
#include "d3dclass.h"
#include "ReturnCodes.h"
#include "textureshaderclass.h"
//
typedef const char*														cstr;
typedef stdext::hash_map< unsigned int, Framework::TextureData* >		TextureListType;
typedef std::pair< unsigned int, Framework::TextureData* >				IDTexturePair;
//
TextureListType		l_textures;
//
namespace Framework
{
	TextureData::TextureData( unsigned int id, const char* file )
		:m_id( id ),
		m_bitmap( 0 ),
		m_render( true )
	{
		::sprintf_s( m_fileName, MAX_TEX_FILE_NAME_LEN, "%s", file );
	}
	//
	TextureData::~TextureData()
	{
		utility::safeDelete( m_bitmap );
	}
	//
	int TextureData::Init()
	{
		m_bitmap = new BitmapClass();
		if( !m_bitmap->Initialize(GRAPHICS->m_D3D->GetDevice(), (float)GRAPHICS->screen_width, (float)GRAPHICS->screen_height, m_fileName, 0.0f, 0.0f) )
		{
			return RET_FAILED_TO_CREATE_BITMAP;
		}
		TextureDraw::GetTextureDimensions( m_bitmap->GetTextureArray()[0], m_width, m_height );
		m_bitmap->SetDimension( (float)m_width, (float)m_height );
		//
		return RET_SUCCESS;
	}
	////////////////////////////////////////////////////////////////
	TextureDraw::TextureDraw()
	{
		//
	}
	//
	TextureDraw::~TextureDraw()
	{
		for( TextureListType::iterator it1 = l_textures.begin(), it2= l_textures.end(); it1 != it2; ++it1 )
		{
			delete it1->second;
		}
	}
	//
	int TextureDraw::Render( TextureShaderClass* texShader, D3DXMATRIX* worldMatrix, D3DXMATRIX* viewMatrix, D3DXMATRIX* projectionMatrix )
	{
		int fail = 0;
		ID3D11DeviceContext* dev = GRAPHICS->m_D3D->GetDeviceContext();
		TextureData* tex;
		//
		for( TextureListType::iterator it1 = l_textures.begin(), it2= l_textures.end(); it1 != it2; ++it1 )
		{
			tex = it1->second;
			if( tex->m_render )
			{
				if( !tex->m_bitmap->Render(dev, tex->m_posX, tex->m_posY) )
				{
					++fail;
				}
				else if( !texShader->Render(dev, tex->m_bitmap->GetIndexCount(), *worldMatrix, *viewMatrix, *projectionMatrix, tex->m_bitmap->GetTextureArray()) )
				{
					++fail;
				}
			}
		}
		//
		return fail;
	}
	//
	int TextureDraw::SetPosition( unsigned int id, const float& posx, const float& posy )
	{
		TextureListType::iterator it1 = l_textures.find( id );
		if( it1 != l_textures.end() )
		{
			it1->second->m_posX = posx;
			it1->second->m_posY = posy;
			//
			return RET_SUCCESS;
		}
		//
		return RET_FAIL;
	}
	//
	int TextureDraw::SetDisplay( unsigned int id, bool val )
	{
		TextureListType::iterator it1 = l_textures.find( id );
		if( it1 != l_textures.end() )
		{
			it1->second->m_render = val;
			//
			return RET_SUCCESS;
		}
		//
		return RET_FAIL;
	}
	//
	int TextureDraw::DeleteTexture( unsigned int id )
	{
		TextureListType::iterator it1 = l_textures.find( id );
		if( it1 != l_textures.end() )
		{
			delete it1->second;
			l_textures.erase( it1 );
			//
			return RET_SUCCESS;
		}
		//
		return RET_FAIL;
	}
	//
	int TextureDraw::GetDimensions( unsigned int id, int& width, int& height )
	{
		TextureListType::iterator it1 = l_textures.find( id );
		if( it1 != l_textures.end() )
		{
			width = it1->second->m_width;
			height = it1->second->m_height;
			//
			return RET_SUCCESS;
		}
		//
		return RET_FAIL;
	}
	//
	unsigned int TextureDraw::CreateTexture( const char* file )
	{
		TextureData* tex = new TextureData( utility::GetUniqueId(), file );
		if( tex->Init() )
		{
			std::cout<<"Could not create bitmap: "<<file<<"\n";
		}
		l_textures.insert( IDTexturePair(tex->m_id, tex) );
		//
		return tex->m_id;
	}
	//
	int TextureDraw::GetTextureDimensions( ID3D11ShaderResourceView* srv, int& width, int& height )
	{
		ID3D11Resource* tex;
		ID3D11Texture2D* tex2d;
		D3D11_TEXTURE2D_DESC desc;
		//
		srv->GetResource( &tex );
		tex2d = static_cast<ID3D11Texture2D*>( tex );
		tex2d->GetDesc( &desc );
		//
		width = desc.Width;
		height = desc.Height;
		//
		return RET_SUCCESS;
	}
}