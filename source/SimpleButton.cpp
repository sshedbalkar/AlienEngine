#include "SimpleButton.h"
#include <iostream>
#include "Global.h"
#include "TextureDraw.h"
#include "InteractiveObject.h"
#include "Utility.h"
//
namespace Framework
{
	const int SimpleButton::MOUSE_OUT = 0;
	const int SimpleButton::MOUSE_OVER = 1;
	const int SimpleButton::LEFT_CLICK = 2;
	const int SimpleButton::RIGHT_CLICK = 3;
	//
	SimpleButton::SimpleButton( unsigned int id )
		:m_id( id ),
		m_active( true ),
		m_interactor( 0 ),
		m_callback( 0 )
	{
		//
	}
	//
	SimpleButton::~SimpleButton()
	{
		utility::safeDelete( m_callback );
		utility::safeDelete( m_interactor );
		//
		g_textureDraw->DeleteTexture( m_idUp );
		g_textureDraw->DeleteTexture( m_idOver );
		g_textureDraw->DeleteTexture( m_idDown );
	}
	//
	int SimpleButton::Init( const char* img_Up, const char* img_Over, const char* img_Down, int posX, int posY )
	{
		m_idUp = g_textureDraw->CreateTexture( img_Up );
		g_textureDraw->SetPosition( m_idUp, posX, posY );
		g_textureDraw->SetDisplay( m_idUp, true );
		//
		m_idOver = g_textureDraw->CreateTexture( img_Over );
		g_textureDraw->SetPosition( m_idOver, posX, posY );
		g_textureDraw->SetDisplay( m_idOver, false );
		//
		m_idDown = g_textureDraw->CreateTexture( img_Down );
		g_textureDraw->SetPosition( m_idDown, posX, posY );
		g_textureDraw->SetDisplay( m_idDown, false );
		//
		g_textureDraw->GetDimensions( m_idUp, m_width, m_height );
		//
		m_interactor = new InteractiveObject( 1, posX, posY, m_width, m_height );
		m_interactor->AddCallback( &SimpleButton::InteractorCallback, this );
		//
		m_state = m_interactor->State();
		m_idActive = m_idUp;
		//
		return RET_SUCCESS;
	}
	//
	void SimpleButton::InteractorCallback( unsigned int id, int state )
	{
		
		if( state == MOUSE_OUT )
		{
			m_state = state;
			g_textureDraw->SetDisplay( m_idActive, false );
			g_textureDraw->SetDisplay( m_idUp, true );
			m_idActive = m_idUp;
		}
		else if( state == MOUSE_OVER )
		{
			if( m_state != LEFT_CLICK )
			{
				m_state = state;
				g_textureDraw->SetDisplay( m_idActive, false );
				g_textureDraw->SetDisplay( m_idOver, true );
				m_idActive = m_idOver;
			}
		}
		else if( state == LEFT_CLICK )
		{
			m_state = state;
			g_textureDraw->SetDisplay( m_idActive, false );
			g_textureDraw->SetDisplay( m_idDown, true );
			m_idActive = m_idDown;
			//
			CallCallback( state );
		}
		else if( state == RIGHT_CLICK )
		{

		}
	}
	//
	int SimpleButton::SetPosition( int posX, int posY )
	{
		g_textureDraw->SetPosition( m_idUp, posX, posY );
		g_textureDraw->SetPosition( m_idOver, posX, posY );
		g_textureDraw->SetPosition( m_idDown, posX, posY );
		m_interactor->SetPosition( posX, posY );
		//
		return RET_SUCCESS;
	}
	//
	void SimpleButton::CallCallback( int state )
	{
		if( m_callback )
		{
			utility::ArgumentVector v1;
			v1.push_back( m_id );
			v1.push_back( state );
			m_callback->Invoke( m_obj, v1 );
		}
	}
	//
	int SimpleButton::Active( bool val )
	{
		m_active = val;
		m_interactor->Active( m_active );
		//
		return RET_SUCCESS;
	}
	//
	int SimpleButton::Width()
	{
		return m_width;
	}
	//
	int SimpleButton::Height()
	{
		return m_height;
	}
	//
	int SimpleButton::State()
	{
		return m_state;
	}
	//
	void SimpleButton::State( int st )
	{
		if( st == MOUSE_OUT )
		{
			g_textureDraw->SetDisplay( m_idActive, false );
			g_textureDraw->SetDisplay( m_idUp, true );
			m_idActive = m_idUp;
			m_state = st;
		}
		else if( st == MOUSE_OVER )
		{
			g_textureDraw->SetDisplay( m_idActive, false );
			g_textureDraw->SetDisplay( m_idOver, true );
			m_idActive = m_idOver;
			m_state = st;
		}
		else if( st == LEFT_CLICK )
		{
			g_textureDraw->SetDisplay( m_idActive, false );
			g_textureDraw->SetDisplay( m_idDown, true );
			m_idActive = m_idDown;
			m_state = st;
		}
		else if( st == RIGHT_CLICK )
		{

		}
	}

}