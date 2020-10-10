#include <iostream>
//
#include "Pool.h"
#include "Page.h"
#include "MemUtility.h"
//
namespace Memory
{
	Pool::Pool()
		:m_unitSize( 0 ),
		m_allocs( 0 ),
		m_max_allocs( 0 ),
		m_nextFree( 0 ),
		m_max_units( 0 ),
		m_freeCount( 0 ),
		m_start( 0 ),
		m_end( 0 ),
		m_max_free( 0 )
	{
		//
	}
	//
	Pool::~Pool()
	{
		m_memory.Release();
		//
		m_unitSize		= 0;
		m_allocs		= 0;
		m_max_allocs	= 0;
		m_nextFree		= 0;
		m_freeCount		= 0;
	}
	//
	int Pool::Init( UnitAllocSize size, UnitCount max )
	{
		m_memory.Acquire( UpperPowerOfTwo(size * max) );
		//
		m_unitSize		= size;
		m_nextFree		= m_memory.Start;
		m_max_units		= m_memory.Size / size;
		m_start			= (Address) m_nextFree;
		m_end			= m_start + m_max_units*m_unitSize;
		//
		//std::cout<<"unit size: "<<size<<", Max: "<<m_max_units<<"\n";
		//
		return 0;
	}
	//
	Pointer Pool::Allocate()
	{
		if( m_allocs < m_max_units )
		{
			++m_allocs;
			//
#ifdef MEM_COLLECT_STATS
			if( m_max_allocs < m_allocs )
			{
				m_max_allocs = m_allocs;
			}
#endif
			//
			if( m_freeCount > 0 )
			{
				--m_freeCount;
				return m_freeList[m_freeCount];
			}
			else
			{
				//std::cout<<"Curr: "<<m_nextFree;
				Pointer p = m_nextFree;
				Address a = ((Address)m_nextFree) + m_unitSize;
				m_nextFree = (Pointer) a;
				//std::cout<<", Next: "<<m_nextFree<<"\n";
				return p;
			}
		}
		else
		{
			throw No_Memory();
		}
		return 0;
	}
	//
	int Pool::Deallocate( Pointer mem )
	{
		Address a1 = (Address) mem;
		if( a1 < m_start || a1 > m_end )
		{
			//std::cout<<"Trying to free mem not allocated by this pool\n";
			return 1;
		}
		//
		a1 -= m_start;
		if( a1%m_unitSize != 0 )
		{
			std::cout<<"unknown pointer!\n";
			return 1;
		}
		//
		for( unsigned i = 0; i < m_freeCount; ++i )
		{
			if( m_freeList[i] == mem )
			{
				std::cout<<"Memory is freed more than once: "<<mem<<"\n";
				return 1;
			}
		}
		//
		if( m_freeCount < FREE_LIST_SIZE )
		{
			m_freeList[m_freeCount] = mem;
			++m_freeCount;
			--m_allocs;
			//
#ifdef MEM_COLLECT_STATS
			if( m_max_free < m_freeCount )
			{
				m_max_free = m_freeCount;
			}
#endif
			//
			//std::cout<<"deallocated: "<<mem<<"\n";
			return 0;
		}
		else
		{
			std::cout<<"Cant dealloc, Free is full: "<<m_unitSize<<"\n";
		}
		//
		return 1;
	}
	//
	UnitAllocSize Pool::UnitSize() const
	{
		return m_unitSize;
	}
	//
	size_t Pool::SizeTotal() const
	{
		return m_unitSize*m_max_units;
	}
	//
	size_t Pool::SizeFree() const
	{
		return SizeTotal() - SizeUsed();
	}
	//
	size_t Pool::SizeUsed() const
	{
		return m_unitSize*m_allocs;
	}
	//
	unsigned Pool::MaxAllocs() const
	{
		return m_max_allocs;
	}
	//
	unsigned Pool::MaxFrees() const
	{
		return m_max_free;
	}
	//
	UnitCount Pool::MaxUnits() const
	{
		return m_max_units;
	}
	///////////////////////////////////////////////////////////////////////////
	std::ostream& operator<<( std::ostream& os, const Pool& pool )
	{
		os<<"Block Size: "<<pool.UnitSize()<<", Max possible allocs: "<<pool.MaxUnits()<<", Max used Allocs: "<<pool.MaxAllocs()<<", Max Frees: "<<pool.MaxFrees()<<"\n";
		return os;
	}
}