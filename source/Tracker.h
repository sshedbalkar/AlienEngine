#pragma once
//
#include <ostream>
#include <map>
//
#include "MemDefines.h"
#include "mallocator.hpp"
//
namespace Memory
{
	class Tracker;
	class Node
	{
	public:
		Node();
		~Node();
		friend class Tracker;
		//
	public:
		Address		Caller;
		Pointer		Alloc;
		unsigned	AllocId;
		size_t		Size;
		AllocType	Type;
	};
	//
	class Tracker
	{
	public:
		Tracker();
		~Tracker();
		friend std::ostream&	operator<<( std::ostream& os, const Tracker& tracker );
		//
	public:
		int				Track( size_t size, Pointer mem, AllocType type );
		int				Untrack( Pointer mem, AllocType type );
		int				Sweep();
		//
	protected:
		typedef std::map< Pointer, Node, std::less<Pointer>, Mallocator< std::pair<Pointer, Node>> >		AllocMapType;
		//
		AllocMapType	m_allocs;
		unsigned		m_count;
	};
}