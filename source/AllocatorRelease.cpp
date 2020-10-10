#include <iostream>
//
#include "AllocatorRelease.h"
//
namespace Memory
{
	AllocatorRelease::AllocatorRelease()
	{
		//
	}
	//
	AllocatorRelease::~AllocatorRelease()
	{
		Sweep();
	}
	//
	int AllocatorRelease::Sweep()
	{
		return m_allocator.Sweep();
	}
	//
	int AllocatorRelease::Init( UnitAllocSize size, UnitCount max )
	{
		m_allocator.Init();
		return 0;
	}
	//
	int AllocatorRelease::Status()
	{
		return PoolStatus::MEM_ACTIVE;
	}
	//
	Pointer AllocatorRelease::Allocate( size_t size, AllocType type )
	{
		return m_allocator.Allocate( size );
	}
	//
	int AllocatorRelease::Deallocate( Pointer mem, AllocType type )
	{
		return m_allocator.Deallocate( mem );
	}
	//
	void AllocatorRelease::PrintStats( std::ostream& os )
	{
		os<<"--------------Allocator stats--------------\n";
		os<<m_allocator;
		os.flush();
	}
	//
	void AllocatorRelease::PrintLeaks( std::ostream& os )
	{
		//
	}
}