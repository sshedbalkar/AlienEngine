#pragma once
//
#include <ostream>
//
#include "MemDefines.h"
//
namespace Memory
{
	class Allocator
	{
	public:
								Allocator();
		virtual					~Allocator();
		//
	public:
		virtual int				Init( UnitAllocSize size, UnitCount max );
		virtual int				Status();
		virtual int				Sweep();
		virtual Pointer			Allocate( size_t size, AllocType type = ALLOC_SINGLE );
		virtual int				Deallocate( Pointer mem, AllocType type = ALLOC_SINGLE );
		virtual void			PrintLeaks( std::ostream& os );
		virtual void			PrintStats( std::ostream& os );
	};
}