#pragma once
//
#include <fstream>
//
#include "MemDefines.h"
//
#ifdef MEM_DEBUG
#include "Allocator.h"
#include "AllocatorDebug.h"
#else
#include "AllocatorRelease.h"
#endif
//
namespace Memory
{
	class MemAllocatorGen
	{
	public:
		MemAllocatorGen();
		~MemAllocatorGen();
		//
	public:
		Pointer			Allocate( size_t size, AllocType type = AllocType::ALLOC_SINGLE );
		int				Deallocate( Pointer memory, AllocType type = AllocType::ALLOC_SINGLE );
		void			PrintLeaks();
		void			PrintStats();
		//
	private:
#ifdef MEM_DEBUG
		AllocatorDebug		m_pool;
		//Allocator			m_pool;
#else
		AllocatorRelease	m_pool;
#endif
		std::ofstream		m_ofile;
	};
	//
	void	PrintLeaks();
	void	PrintStats();
}