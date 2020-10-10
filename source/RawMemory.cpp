#include <iostream>
//
#include "RawMemory.h"
#include "MemDefines.h"
#include "MemUtility.h"
//
namespace Memory
{
	const char* No_Memory::what() const
	{
		return "Cannot allocate Memory!";
	}
	//////////////////////////////////////////////////////
	RawMemory::RawMemory()
		:m_mem( 0 ),
		m_size( 0 )
	{
		//
	}
	//
	RawMemory::~RawMemory()
	{
		Release();
	}
	int RawMemory::Release()
	{
		::free( m_mem );
		m_mem = 0;
		//
		return 0;
	}
	//
	int RawMemory::Acquire( size_t size )
	{
		m_size	= size * sizeof( char );
		m_mem	= ::malloc( m_size );
		//m_mem = ::calloc( size, sizeof(char) );
		if( m_mem == 0 )
		{
			m_size = 0;
			throw No_Memory();
		}
		//
		::memset( m_mem, RAW_MEM, m_size );
		return 0;
	}
	//
	size_t RawMemory::Size()
	{
		return m_size;
	}
	//
	Pointer RawMemory::GetMemory()
	{
		return m_mem;
	}
	//
	std::ostream& operator<<( std::ostream& os, const RawMemory& mem )
	{
		size_t sz = mem.m_size;
		os<<"["<<mem.m_mem<<": "<<MemSizeToText( sz )<<"]: [";
		unsigned char* ptr = static_cast<unsigned char*>( mem.m_mem );
		for( size_t i = 0; i < sz; ++i, ++ptr )
		{
			os<<*ptr;
			if( i%8 == 7 )
				os<<' ';
		}
		os<<"]";
		//
		return os;
	}
}