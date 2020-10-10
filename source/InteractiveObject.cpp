#include "InteractiveObject.h"
#include <iostream>
#include "MsgRecvrComponent.h"
#include "Message.h"
#include "Global.h"
#include "TextureDraw.h"
#include "WindowsSystem.h"
//
namespace Framework
{
	InteractiveObject::InteractiveObject( unsigned int id, int posX, int posY, int width, int height )
		:m_msgs( new MsgRecvrComponent() ),
		m_posX( posX ),
		m_posY( posY ),
		m_active( true ),
		m_state( STATE_OUT ),
		m_callback( 0 ),
		m_id( id ),
		m_width( width ),
		m_height( height )
	{
		m_msgs->Initialize();
		m_msgs->AddCallback( &InteractiveObject::MsgCallback, this );
	}
	//
	InteractiveObject::~InteractiveObject()
	{
		delete m_msgs;
		m_msgs = 0;
		//
		delete m_callback;
		m_callback = 0;
	}
	//
	void InteractiveObject::Active( bool val )
	{
		m_active = val;
	}
	//
	void InteractiveObject::MsgCallback( Message* msg )
	{
		if( !m_active ) return;
		switch( msg->MessageId )
		{
			case Mid::MouseMove:
				{
					MouseMove* mousePos = static_cast<MouseMove*>( msg );
					if( IsOver(mousePos->MousePosition.x, mousePos->MousePosition.y) )
					{
						ChangeState( STATE_OVER );
					}
					else
					{
						ChangeState( STATE_OUT );
					}
					break;
				}
			case Mid::LMouseButton:
				{
					LMouseButton* lmouse = static_cast<LMouseButton*>( msg );
					if( lmouse->ButtonIsPressed )
					{
						if( IsOver(lmouse->MousePosition.x, lmouse->MousePosition.y) )
						{
							ChangeState( STATE_LCLICK );
						}
					}
					break;
				}
			case Mid::RMouseButton:
				{
					RMouseButton* rmouse = static_cast<RMouseButton*>( msg );
					if( rmouse->ButtonIsPressed )
					{
						if( IsOver(rmouse->MousePosition.x, rmouse->MousePosition.y) )
						{
							ChangeState( STATE_RCLICK );
						}
					}
					break;
				}
		}
	}
	//
	int InteractiveObject::SetPosition( int posX, int posY )
	{
		m_posX = posX;
		m_posY = posY;
		//
		return RET_SUCCESS;
	}
	//
	int InteractiveObject::State()
	{
		return m_state;
	}
	//
	int InteractiveObject::ChangeState( int state )
	{
		if( m_state != state )
		{
			m_state = state;
			//
			if( m_callback )
			{
				utility::ArgumentVector v1;
				v1.push_back( m_id );
				v1.push_back( state );
				m_callback->Invoke( m_obj, v1 );
			}
			//
			return RET_SUCCESS;
		}
		return RET_FAIL;
	}
	//
	unsigned int InteractiveObject::Id()
	{
		return m_id;
	}
	//
	bool InteractiveObject::IsOver( int posX, int posY )
	{
		return ( posX >= m_posX && posX <= (m_posX+m_width) && posY >= m_posY && posY <= (m_posY+m_height) );
	}
}