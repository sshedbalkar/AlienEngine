#pragma once
//
#include <ostream>
//
#include "MemDefines.h"
#include "RawMemory.h"
//
namespace Memory
{
	class MemBank_FirstFit;
	class MemBank_Linear;
	class Pool;
	//
	class Page
	{
	private:
		Page();
		~Page();
		friend class MemBank_FirstFit;
		friend class MemBank_Linear;
		friend class Pool;
		//
	public:
		int				Acquire( size_t size );
		int				Release();
		void			Print( std::ostream& os, bool showData );
		//
	private:
		RawMemory		Mem;
		Pointer			Start;
		size_t			TotalFree;
		size_t			MaxChunkFree;
		size_t			Size;
		unsigned		Allocations;
		Pointer			FirstAlloc;
		Pointer			LastAlloc;
	};
}