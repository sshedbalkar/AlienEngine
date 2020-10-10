#pragma once
//
#include "Composition.h"
#include "FunctionBind.h"
#include "ReturnCodes.h"
//
namespace Framework
{
	class MsgRecvrComponent: public GameComponent
	{
	public:
						MsgRecvrComponent();
						~MsgRecvrComponent();
		//
	public:
		virtual void	SendMessage( Message* msg );
		virtual void	Initialize();
		virtual void	Serialize( ISerializer& stream );
		virtual void    RemoveComponentFromGame();
		bool			Deleted();
		//
		template< typename ClassType > 
		inline int		AddCallback( void(ClassType::*f)(Message*), ClassType* obj )
		{
			m_callback = utility::Bind( f );
			m_obj = obj;
			//
			return RET_SUCCESS;
		}
		//
	private:
		utility::FunctionObject*	m_callback;
		utility::Object				m_obj;
		bool						m_deleted;
	};
}