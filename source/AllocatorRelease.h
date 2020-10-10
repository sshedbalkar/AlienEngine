#pragma once
//
#include "MemDefines.h"
#include "Allocator.h"
#include "MemBank_Pooled.h"
//
namespace Memory
{
	class AllocatorRelease: public Allocator
	{
	public:
							AllocatorRelease();
							~AllocatorRelease();
		//
	public:
		int					Init( UnitAllocSize size, UnitCount max );
		int					Status();
		int					Sweep();
		Pointer				Allocate( size_t size, AllocType type = ALLOC_SINGLE );
		int					Deallocate( Pointer mem, AllocType type = ALLOC_SINGLE );
		void				PrintStats( std::ostream& os );
		void				PrintLeaks( std::ostream& os );
		//
	private:
		MemBank_Pooled		m_allocator;
	};
}