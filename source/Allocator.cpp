#include <cstdlib>
#include "Allocator.h"
//
namespace Memory
{
	Allocator::Allocator()
	{
		//
	}
	//
	Allocator::~Allocator()
	{
		Sweep();
	}
	//
	int Allocator::Sweep()
	{
		return 0;
	}
	//
	int Allocator::Init( UnitAllocSize size, UnitCount max )
	{
		return 0;
	}
	//
	int Allocator::Status()
	{
		return PoolStatus::MEM_ACTIVE;
	}
	//
	Pointer Allocator::Allocate( size_t size, AllocType type )
	{
		//std::cout<<"Allocator::Allocate: "<<size<<"\n";
		//return ::operator new( size );
		return ::malloc( size );
	}
	//
	int Allocator::Deallocate( Pointer mem, AllocType type )
	{
		//std::cout<<"Allocator::Deallocate: "<<mem<<"\n";
		//::operator delete( mem );
		::free( mem );
		return 0;
	}
	//
	void Allocator::PrintLeaks( std::ostream& os )
	{
		//
	}
	//
	void Allocator::PrintStats( std::ostream& os )
	{
		//
	}
}