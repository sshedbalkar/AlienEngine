#include "MsgRecvrComponent.h"
#include "GameLogic.h"
//
namespace Framework
{
	MsgRecvrComponent::MsgRecvrComponent()
		:m_callback( 0 ),
		m_deleted( false )
	{
		//
	}
	//
	MsgRecvrComponent::~MsgRecvrComponent()
	{
		LOGIC->RemoveMsgRecvr( this );
		m_deleted = true;
		delete m_callback;
		m_callback = 0;
	}
	//
	void MsgRecvrComponent::SendMessage( Message* msg )
	{
		if( m_callback )
		{
			utility::ArgumentVector v1;
			v1.push_back( msg );
			m_callback->Invoke( m_obj, v1 );
		}
	}
	//
	void MsgRecvrComponent::Initialize()
	{
		LOGIC->AddMsgRecvr( this );
	}
	//
	void MsgRecvrComponent::Serialize( ISerializer& stream )
	{
		//
	}

	void MsgRecvrComponent::RemoveComponentFromGame()
	{
	}
	//
	bool MsgRecvrComponent::Deleted()
	{
		return m_deleted;
	}
}