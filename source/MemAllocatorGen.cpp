#include <iostream>
//
#include "Global.h"
#include "MemAllocatorGen.h"
//
static Memory::MemAllocatorGen		l_allocator;
//
namespace Memory
{
	MemAllocatorGen::MemAllocatorGen()
	{
		std::cout<<"MemAllocatorGen init\n";
		m_pool.Init( 1, 1 );
		m_ofile.open( MEM_OUTPUT_FILE_PATH, std::ios::out|std::ios::trunc );
		if( !m_ofile.is_open() )
		{
			std::cout<<"Could not open: "<<MEM_OUTPUT_FILE_PATH<<" for writing!\n";
		}
		//
		Framework::g_memAllocator = this;
	}
	//
	MemAllocatorGen::~MemAllocatorGen()
	{
		m_pool.Sweep();
		m_ofile.close();
	}
	//
	Pointer MemAllocatorGen::Allocate( size_t size, AllocType type )
	{
		return m_pool.Allocate( size, type );
	}
	//
	int MemAllocatorGen::Deallocate( Pointer memory, AllocType type )
	{
		return m_pool.Deallocate( memory, type );
	}
	//
	void MemAllocatorGen::PrintLeaks()
	{
		m_pool.PrintLeaks( m_ofile );
	}
	//
	void MemAllocatorGen::PrintStats()
	{
		m_pool.PrintStats( m_ofile );
	}
	///////////////////////////////////////////////////////////////////////////////
	void PrintLeaks()
	{
		Framework::g_memAllocator->PrintLeaks();
	}
	//
	void PrintStats()
	{
		Framework::g_memAllocator->PrintStats();
	}
}
//
/////////////////////////////////////////////////////////////////////////////////

#pragma warning( push )
#pragma warning( disable: 4290 )
//
void*	operator new( size_t size ) throw( std::bad_alloc ) {return l_allocator.Allocate( size );}
void	operator delete( void* ptr ) throw() {l_allocator.Deallocate( ptr );}
//
void*	operator new( size_t size, const std::nothrow_t& ) throw() {return l_allocator.Allocate( size );}
void	operator delete( void* ptr, const std::nothrow_t& ) throw() {l_allocator.Deallocate( ptr );}
//
void*	operator new[]( size_t size ) throw( std::bad_alloc ) {return l_allocator.Allocate( size, Memory::ALLOC_ARRAY );}
void	operator delete[]( void* ptr ) throw() {l_allocator.Deallocate( ptr,  Memory::ALLOC_ARRAY );}
//
void*	operator new[]( size_t size, const std::nothrow_t& ) throw() {return l_allocator.Allocate( size,  Memory::ALLOC_ARRAY );}
void	operator delete[]( void* ptr, const std::nothrow_t& ) throw() {l_allocator.Deallocate( ptr,  Memory::ALLOC_ARRAY );}
//
#pragma warning(pop)

//////////////////////////////////