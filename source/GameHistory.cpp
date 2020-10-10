#include "GameHistory.h"
#include "Utility.h"
#include "ReturnCodes.h"
#include "Core.h"
#include "TimeMechanicManager.h"
#include "DebugDiagnostic.h"
//
namespace Framework
{
	GameFrameData::GameFrameData( unsigned l_id, unsigned a_id, HistoryDataListWrapper* data )
		:LogicFrameId( l_id ),
		ActualFrameId( a_id ),
		Payload( data )
	{
		
	}
	//
	GameFrameData::~GameFrameData()
	{
		utility::safeDelete( Payload );
	}
	//
	////////////////////////////////////////////////////////////////////////////////////
	
	GameHistory* GAMEHISTORY = 0;

	GameHistory::GameHistory()
		:m_iter( m_list.end() )
	{
		ErrorIf(GAMEHISTORY!=NULL,"GAMEHISTORY already initialized");
			GAMEHISTORY = this;
	}
	//
	void GameHistory::Initialize()
	{
		DeleteAllFrameData();
		GameFrameData* f;
		if( m_list.empty() )
		{
			f = new GameFrameData( 0, 0, 0 );
			m_list.push_back( f );
		}
	}
	//
	int GameFrameData::GetBytesUsed()
	{
		int mem=0;
		mem += sizeof(GameFrameData);
		mem += Payload->GetBytesUsed();
		return mem;
	}
	//
	GameHistory::~GameHistory()
	{
		for( DataList::iterator it1 = m_list.begin(), it2 = m_list.end(); it1 != it2; ++it1 )
		{
			delete *it1;
		}
	}
	//
	int GameHistory::GetBytesUsed()
	{
		int mem=0;
		for( DataList::iterator it1 = m_list.begin(), it2 = m_list.end(); it1 != it2; ++it1 )
		{
			mem += (*it1)->GetBytesUsed();
		}
		return mem;
	}
	//
	int GameHistory::NewFrameData( unsigned frame_id, HistoryDataListWrapper* data )
	{
		GameFrameData* f;
	/*	if( m_list.empty() )
		{
			f = new GameFrameData( frame_id, CORE->FrameId(), 0 );
			m_list.push_back( f );
		}*/
		//
		f = new GameFrameData( frame_id, CORE->FrameId(), data );
		m_list.push_back( f );
		//
		if( m_list.size() > MAX_HISTORY_SIZE )
		{

			DataList::iterator it1 = m_list.begin();
			delete *it1;
			m_list.erase( it1 );
		}
		//
		m_iter = --m_list.end();
		//
		return RET_SUCCESS;
	}
	//
	HistoryDataListWrapper* GameHistory::GetFrameData( unsigned frame_id )
	{
		for( DataList::reverse_iterator it1 = m_list.rbegin(), it2 = m_list.rend(); it1 != it2; ++it1 )
		{
			if( (*it1)->LogicFrameId == frame_id )
			{
				return (*it1)->Payload;
			}
		}
		//
		return 0;
	}
	//
	int GameHistory::DeleteFrameDataById( unsigned l_id )
	{
		for( DataList::iterator it1 = m_list.begin(), it2 = m_list.end(); it1 != it2; ++it1 )
		{
			if( (*it1)->LogicFrameId == l_id )
			{
				delete *it1;
				m_list.erase( it1 );
				//
				return RET_SUCCESS;
			}
		}
		//
		return RET_FAIL;
	}
	//
	HistoryDataListWrapper* GameHistory::GetLastFrameData()
	{
		DataList::iterator it1 = --m_list.end();
		return ( *it1 )->Payload;
	}
	//
	HistoryDataListWrapper* GameHistory::GetPrevFrameData( unsigned curr_frame_id, FramePosition& pos )
	{
		if(curr_frame_id==1)
		{
			int k = 1;
		}
		if( m_iter != m_list.begin() )
		{
			DataList::iterator it = m_iter;
			--it;
			pos = FRM_MIDDLE;
			if( (*it)->LogicFrameId == (curr_frame_id - 1) )
			{
				m_iter = it;
				return ( *m_iter )->Payload;
			}
			return 0;
		}
		pos = FRM_BEGIN;
		return 0;
	}
	//
	HistoryDataListWrapper* GameHistory::GetNextFrameData( unsigned curr_frame_id, FramePosition& pos )
	{
		
		if( m_iter != --m_list.end() )
		{
			DataList::iterator it = m_iter;
			++it;
			pos = FRM_MIDDLE;
			if( (*it)->LogicFrameId == (curr_frame_id + 1) )
			{
				m_iter = it;
				return ( *m_iter )->Payload;
			}
			return 0;
		}
		pos = FRM_END;
		return 0;
	}
	//
	HistoryDataListWrapper* GameHistory::GetCurrentFrameData()
	{
		if( m_iter != m_list.end() )
		{
			return ( *m_iter )->Payload;
		}
		return 0;
	}
	//
	int GameHistory::DeleteSucceedingFrameData()
	{
		/*DeleteAllFrameData();
			return 1;*/
		DataList::iterator it1 = m_iter;
		DataList::iterator it2 = m_list.end();
		while( it1 != it2 )
		{
			delete *it1;
			++it1;
		}
		//
		it1 = m_iter;
		m_list.erase( it1, it2 );
		//
		m_iter = --m_list.end();
		//
		return RET_SUCCESS;
	}

	int GameHistory::DeleteAllFrameData()
	{
		for( DataList::iterator it1 = m_list.begin(), it2 = m_list.end(); it1 != it2; ++it1 )
		{
			delete *it1;
		}
		m_list.clear();
		m_iter = m_list.end();
		//
		return RET_SUCCESS;
	}
}
