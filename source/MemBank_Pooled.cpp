#include <iostream>
//
#include "MemBank_Pooled.h"
#include "MemUtility.h"
//
namespace Memory
{
	float			l_log2;
	unsigned		l_minExp;
	////////////////////////////////////////////////////
	Wrong_Size::Wrong_Size( unsigned sz )
		:size( sz )
	{
		//
	}
	//
	const char* Wrong_Size::what() const
	{
		return "Given Block size is wrong!";
	}
	//////////////////////////////////////////////////////
	MemBank_Pooled::MemBank_Pooled()
		:m_minSize( 0 ),
		m_maxSize( 0 ),
		m_count( 0 )
	{
		//
	}
	//
	MemBank_Pooled::~MemBank_Pooled()
	{
		Sweep();
	}
	//
	int MemBank_Pooled::Init()
	{
		m_pools[0].Init( 1,			128 );		// block size(in bytes) * #blocks
		m_pools[1].Init( 2,			128 );		// block size(in bytes) * #blocks
		m_pools[2].Init( 4,			512 );		// block size(in bytes) * #blocks
		m_pools[3].Init( 8,			8192 );		// block size(in bytes) * #blocks
		m_pools[4].Init( 16,		16384 );
		m_pools[5].Init( 32,		4096 );
		m_pools[6].Init( 64,		1024 );
		m_pools[7].Init( 128,		512 );
		m_pools[8].Init( 256,		128 );
		m_pools[9].Init( 512,		128 );
		m_pools[10].Init( 1024,		256 );
		m_pools[11].Init( 2048,		64 );
		m_pools[12].Init( 4096,		16 );
		m_pools[13].Init( 8192,		16 );
		m_pools[14].Init( 16384,	16 );
		m_pools[15].Init( 32768,	8 );
		m_pools[16].Init( 65536,	16 );
		m_pools[17].Init( 131072,	512 );
		m_pools[18].Init( 262144,	8 );
		//m_pools[17].Init( 524288,	32 );
		//m_pools[18].Init( 1048576,	32 );
		//m_pools[19].Init( 2097152,	32 );
		//
		m_count		= 19;			// total #pools used(count of above)
		//
		m_minSize	= 1;			// min block size
		m_maxSize	= 262144;		// max block size
		//
		l_log2		= ::log( 2.0 );
		l_minExp	= Round( ::log((float)m_minSize)/l_log2 );
		//
		return 0;
	}
	//
	Pointer MemBank_Pooled::Allocate( size_t size )
	{
		unsigned sz = UpperPowerOfTwo( size );
		if( sz < m_minSize || sz > m_maxSize )
		{
			std::cout<<"Wrong size: "<<size<<"("<<sz<<")\n";
			return AllocInvalidSize( size );
			//throw Wrong_Size( sz );
			//return 0;
		}
		unsigned lg = Round( ::log((float)sz)/l_log2 );
		lg -= l_minExp;
		//
		return m_pools[lg].Allocate();
	}
	//
	int MemBank_Pooled::Deallocate( Pointer ptr )
	{
		if( ptr == 0 ) return 0;
		//
		unsigned i;
		for( i = 0; i < m_count && m_pools[i].Deallocate( ptr ); ++i )
		{
			//
		}
		//
		if( i == m_count )
		{
			std::cout<<"Freeing invalid size ptr: "<<ptr<<"\n";
			return DeallocInvalidSize( ptr );
		}
		//
		return 0;
	}
	//
	Pointer MemBank_Pooled::AllocInvalidSize( size_t size )
	{
		return ::malloc( size );
	}
	//
	int MemBank_Pooled::DeallocInvalidSize( Pointer ptr )
	{
		//::free( ptr );
		return 0;
	}
	//
	int MemBank_Pooled::Sweep()
	{
		return 0;
	}
	//
	size_t MemBank_Pooled::SizeTotal() const
	{
		size_t sz = 0;
		for( unsigned i = 0; i < m_count; ++i )
		{
			sz += m_pools[i].SizeTotal();
		}
		//
		return sz;
	}
	//
	size_t MemBank_Pooled::SizeFree() const
	{
		size_t sz = 0;
		for( unsigned i = 0; i < m_count; ++i )
		{
			sz += m_pools[i].SizeFree();
		}
		//
		return sz;
	}
	//
	size_t MemBank_Pooled::SizeUsed() const
	{
		size_t sz = 0;
		for( unsigned i = 0; i < m_count; ++i )
		{
			sz += m_pools[i].SizeUsed();
		}
		//
		return sz;
	}
	//////////////////////////////////////////////////////////////////////////////
	std::ostream& operator<<( std::ostream& os, const MemBank_Pooled& bank )
	{
		unsigned s1 = bank.SizeTotal();
		unsigned s2 = bank.SizeUsed();
		os<<"Total size: "<<MemSizeToText(s1);
		os<<", Used: "<<MemSizeToText(s2)<<"\n";
		for( unsigned i = 0; i < bank.m_count; ++i )
		{
			os<<bank.m_pools[i];
		}
		//
		return os;
	}
}