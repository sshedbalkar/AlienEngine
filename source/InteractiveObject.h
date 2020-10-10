#pragma once
//
#include "ReturnCodes.h"
#include "FunctionBind.h"
//
#define STATE_OUT		0
#define STATE_OVER		1
#define STATE_LCLICK	2
#define STATE_RCLICK	3
//
namespace Framework
{
	class MsgRecvrComponent;
	class Message;
	//
	class InteractiveObject
	{
	public:
		InteractiveObject( unsigned int id, int posX, int posY, int width, int height );
		~InteractiveObject();
		//
	public:
		void						Active( bool val );
		unsigned int				Id();
		int							State();
		int							SetPosition( int posX, int posY );
		//
		template< typename ClassType > 
		inline int					AddCallback( void(ClassType::*f)(unsigned int, int), ClassType* obj )
		{
			delete m_callback;
			m_callback = utility::Bind( f );
			m_obj = obj;
			//
			return RET_SUCCESS;
		}
		//
	private:
		void						MsgCallback( Message* msg );
		bool						IsOver( int posX, int posY );
		int							ChangeState( int state );
		//
	private:
		int							m_posX;
		int							m_posY;
		int							m_width;
		int							m_height;
		MsgRecvrComponent*			m_msgs;
		//unsigned int				m_imgId;
		bool						m_active;
		int							m_state;
		utility::FunctionObject*	m_callback;
		utility::Object				m_obj;
		unsigned int				m_id;
		//
	};
}