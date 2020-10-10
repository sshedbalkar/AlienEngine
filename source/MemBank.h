#pragma once
//
#include "MemDefines.h"
//
namespace Memory
{
	class MemBank
	{
	public:
		virtual ~MemBank(){}
		virtual Pointer		Allocate( size_t size )		= 0;
		virtual int			Deallocate( Pointer ptr )	= 0;
		virtual int			Sweep()						= 0;
		virtual size_t		SizeTotal() const			= 0;
		virtual size_t		SizeFree() const			= 0;
		virtual size_t		SizeUsed() const			= 0;
	};
}