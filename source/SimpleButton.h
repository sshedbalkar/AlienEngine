#pragma once
//
#include "ReturnCodes.h"
#include "FunctionBind.h"
//
namespace Framework
{
	class InteractiveObject;
	//
	class SimpleButton
	{
	public:
		SimpleButton( unsigned int id );
		~SimpleButton();
		//
	public:
		int							Init( const char* img_Up, const char* img_Over, const char* img_Down, int posX, int posY );
		int							Active( bool val );
		int							SetPosition( int posX, int posY );
		int							Width();
		int							Height();
		int							State();
		void						State( int st );
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
		void						InteractorCallback( unsigned int id, int state );
		void						CallCallback( int state );
		//
	public:
		static const int			LEFT_CLICK;
		static const int			RIGHT_CLICK;
		static const int			MOUSE_OVER;
		static const int			MOUSE_OUT;
		//
	private:
		unsigned int				m_id;
		InteractiveObject*			m_interactor;
		unsigned int				m_idUp;
		unsigned int				m_idOver;
		unsigned int				m_idDown;
		unsigned int				m_idActive;
		bool						m_active;
		utility::FunctionObject*	m_callback;
		utility::Object				m_obj;
		int							m_state;
		int							m_width;
		int							m_height;
	};
}