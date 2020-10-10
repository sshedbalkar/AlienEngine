#include "Precompiled.h"
#include "TextDisplay.h"
#include "textclass.h"
#include "ReturnCodes.h"
#include "d3dclass.h"
//
namespace Framework
{
	DispTextData::DispTextData( const unsigned int& id, utility::FunctionObject* f, utility::Object obj, bool debug, bool singleFrame )
		:m_sentence( 0 ),
		m_id( id ),
		m_updFunc( f ),
		m_obj( obj ),
		m_debug( debug ),
		m_1frame( singleFrame )
	{
		//
	}
	//
	DispTextData::~DispTextData()
	{
		GRAPHICS->GetTextObj()->ReleaseSentence( &m_sentence );
		//
		delete m_updFunc;
		m_obj = 0;
		//
		return;
	}
	//
	float DispTextData::Blue()
	{
		return m_sentence->blue;
	}
	//
	void DispTextData::Blue( const float& val )
	{
		m_sentence->blue = val;
	}
	//
	float DispTextData::Green()
	{
		return m_sentence->green;
	}
	//
	void DispTextData::Green( const float& val )
	{
		m_sentence->green= val;
	}
	//
	float DispTextData::Red()
	{
		return m_sentence->red;
	}
	//
	void DispTextData::Red( const float& val )
	{
		m_sentence->red = val;
	}
	//
	int DispTextData::PosX()
	{
		return m_posX;
	}
	//
	void DispTextData::PosX( const int& val )
	{
		m_posX = val;
	}
	//
	int DispTextData::PosY()
	{
		return m_posY;
	}
	//
	void DispTextData::PosY( const int& val )
	{
		m_posY = val;
	}
	bool DispTextData::DebugText()
	{
		return m_debug;
	}
	void DispTextData::DebugText( bool val )
	{
		m_debug = val;
	}
	unsigned int DispTextData::ID()
	{
		return m_id;
	}
	const char* DispTextData::Text()
	{
		return m_text;
	}
	////////////////////////////////////////////////////////////////////////
	TextDisplay::TextDisplay()
	{
		//
	}
	//
	TextDisplay::~TextDisplay()
	{
		for each( DispTextData* d in m_textList )
		{
			delete d;
		}
	}
	//
	int TextDisplay::RemoveText( const unsigned int& id )
	{
		for( TextListType::iterator it1 = m_textList.begin(), it2 = m_textList.end(); it1 != it2; ++it1 )
		{
			if( (*it1)->m_id == id )
			{
				m_textList.erase( it1 );
				return RET_SUCCESS;
			}
		}
		//
		return RET_FAIL;
	}
	//
	int TextDisplay::Update( bool debug )
	{
		char* str;
		TextClass *tc = GRAPHICS->GetTextObj();
		D3DClass *d3d = GRAPHICS->m_D3D;
		D3DXMATRIX matWorld, matOrtho;
		TextListType newTextList;
		//
		d3d->GetWorldMatrix( matWorld );
		d3d->GetOrthoMatrix( matOrtho );
		//
		for each( DispTextData* d in m_textList )
		{
			if( d->DebugText() != debug )
			{
				continue;
			}
			if( !d->m_1frame )
			{
				utility::ArgumentVector v1;
				v1.push_back( d );
				str = d->m_updFunc->Invoke( d->m_obj, v1 ).As< char* >();
				::sprintf_s( d->m_text, MAX_TEXT_LENGTH, "%s", str );
				//
				if( !tc->UpdateSentence(d->m_sentence, str, d->PosX(), d->PosY(), d->m_sentence->red, d->m_sentence->green, d->m_sentence->blue, d3d->GetDeviceContext()) )
				{
					throw "Could not update sentence";
				}
				if( !tc->RenderSentence(d3d->GetDeviceContext(), d->m_sentence, matWorld, matOrtho) )
				{
					throw "Could not Render sentence";
				}
				//
				newTextList.push_back( d );
			}
			else
			{
				if( !tc->UpdateSentence(d->m_sentence, d->m_text, d->PosX(), d->PosY(), d->m_sentence->red, d->m_sentence->green, d->m_sentence->blue, d3d->GetDeviceContext()) )
				{
					throw "Could not update sentence";
				}
				if( !tc->RenderSentence(d3d->GetDeviceContext(), d->m_sentence, matWorld, matOrtho) )
				{
					throw "Could not Render sentence";
				}
				//
				delete d;
			}
		}
		//
		m_textList.swap( newTextList );
		//
		return RET_SUCCESS;
	}
	//
	int TextDisplay::InitSentence( DispTextData* d, const float& red, const float& green, const float& blue, const char* text )
	{
		char str[MAX_TEXT_LENGTH];
		::sprintf_s( str, MAX_TEXT_LENGTH, "%s", text );
		//
		TextClass *tc = GRAPHICS->GetTextObj();
		D3DClass *d3d = GRAPHICS->m_D3D;
		if( !tc->InitializeSentence(&d->m_sentence, MAX_TEXT_LENGTH, d3d->GetDevice()) )
		{
			throw "Could not initialize sentence";
		}
		if( !tc->UpdateSentence(d->m_sentence, str, d->PosX(), d->PosY(), red, green, blue, d3d->GetDeviceContext()) )
		{
			throw "Could not update sentence";
		}
		//
		return RET_SUCCESS;
	}
	//
	unsigned int TextDisplay::AddNewText1Frame( const char* text, const int& posx, const int& posy )
	{
		DispTextData* d = new DispTextData( utility::GetUniqueId(), 0, 0, false, true );
		::strcpy_s( d->m_text, MAX_TEXT_LENGTH, text );
		d->m_posX = posx;
		d->m_posY = posy;
		m_textList.push_back( d );
		//
		InitSentence( d, 1.0f, 1.0f, 1.0f, text );
		//
		return d->m_id;
	}
	//
	unsigned int TextDisplay::AddNewText1Frame( const int& num, const int& posx, const int& posy )
	{
		DispTextData* d = new DispTextData( utility::GetUniqueId(), 0, 0, false, true );
		::sprintf_s( d->m_text, MAX_TEXT_LENGTH, "%d", num );
		d->m_posX = posx;
		d->m_posY = posy;
		m_textList.push_back( d );
		//
		InitSentence( d, 1.0f, 1.0f, 1.0f, "." );
		//
		return d->m_id;
	}
	//
	unsigned int TextDisplay::AddNewText1Frame( const float& num, const int& posx, const int& posy )
	{
		DispTextData* d = new DispTextData( utility::GetUniqueId(), 0, 0, false, true );
		::sprintf_s( d->m_text, MAX_TEXT_LENGTH, "%f", num );
		d->m_posX = posx;
		d->m_posY = posy;
		m_textList.push_back( d );
		//
		InitSentence( d, 1.0f, 1.0f, 1.0f, "." );
		//
		return d->m_id;
	}
}