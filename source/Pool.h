#pragma once
//
#include <ostream>
//
#include "MemDefines.h"
#include "Page.h"
//
namespace Memory
{
	class MemBank_Pooled;
	class Pool
	{
	private:
		Pool();
		virtual ~Pool();
		friend class MemBank_Pooled;
		friend std::ostream& operator<<( std::ostream& os, const Pool& pool );
		//
	public:
		static const int	FREE_LIST_SIZE = 8192;
		//
		int				Init( UnitAllocSize size, UnitCount max );
		Pointer			Allocate();
		int				Deallocate( Pointer mem );
		UnitAllocSize	UnitSize() const;
		size_t			SizeTotal() const;
		size_t			SizeFree() const;
		size_t			SizeUsed() const;
		unsigned		MaxAllocs() const;
		unsigned		MaxFrees() const;
		UnitCount		MaxUnits() const;
		//
	private:
		UnitAllocSize	m_unitSize;
		unsigned		m_allocs;
		unsigned		m_max_allocs;
		Pointer			m_nextFree;
		UnitCount		m_max_units;
		Page			m_memory;
		Memory::Pointer	m_freeList[FREE_LIST_SIZE];
		unsigned		m_freeCount;
		unsigned		m_max_free;
		Address			m_start;
		Address			m_end;
	};
}