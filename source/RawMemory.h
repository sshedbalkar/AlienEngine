#pragma once
//
#include <exception>
#include <ostream>
//
#include "MemDefines.h"
//
namespace Memory
{
	class Page;
	//
	class No_Memory: public std::exception
	{
	public:
		const char* what() const;
	};
	/////////////////////////////////////
	class RawMemory
	{
	private:
		RawMemory();
		~RawMemory();
		//
		friend class			Page;
		friend std::ostream&	operator<<( std::ostream& os, const RawMemory& mem );
		//
	protected:
		int				Acquire( size_t size ) throw( No_Memory );
		int				Release();
		//
	public:
		size_t			Size();
		Pointer			GetMemory();
		//
	private:
		Pointer			m_mem;
		size_t			m_size;
	};
}