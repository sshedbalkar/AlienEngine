#include "AllocatorDebug.h"
#include "MemUtility.h"

//
//Memory::Tracker				l_tracker;
//Memory::MemBank_Pooled		l_allocator_2;
//
namespace Memory
{
	AllocatorDebug::AllocatorDebug()
	{
		InitDebug();
	}
	//
	AllocatorDebug::~AllocatorDebug()
	{
		Sweep();
	}
	//
	int AllocatorDebug::Sweep()
	{
		m_allocator.Sweep();
		m_tracker.Sweep();
		return 0;
	}
	//
	int AllocatorDebug::Init( UnitAllocSize size, UnitCount max )
	{
		m_allocator.Init();
		return 0;
	}
	//
	int AllocatorDebug::Status()
	{
		return PoolStatus::MEM_ACTIVE;
	}
	//
	Pointer AllocatorDebug::Allocate( size_t size, AllocType type )
	{
		Pointer mem = m_allocator.Allocate( size );
		m_tracker.Track( size, mem, type );
		return mem;
	}
	//
	int AllocatorDebug::Deallocate( Pointer mem, AllocType type )
	{
		m_tracker.Untrack( mem, type );
		m_allocator.Deallocate( mem );
		return 0;
	}
	//
	void AllocatorDebug::PrintLeaks( std::ostream& os )
	{
		os<<"\n--------------Tracker stats--------------\n";
		os<<m_tracker;
		os.flush();
	}
	//
	void AllocatorDebug::PrintStats( std::ostream& os )
	{
		os<<"--------------Allocator stats--------------\n";
		os<<m_allocator;
		os.flush();
	}
}