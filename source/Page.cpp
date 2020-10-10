#include "Page.h"
#include "MemUtility.h"
//
namespace Memory
{
	Page::Page()
		:Start( 0 ),
		TotalFree( 0 ),
		MaxChunkFree( 0 ),
		Size( 0 ),
		Allocations( 0 ),
		FirstAlloc( 0 ),
		LastAlloc( 0 )
	{
		//
	}
	//
	Page::~Page()
	{
		Release();
	}
	//
	int Page::Acquire( size_t size )
	{
		//std::cout<<"Acquiring: "<<size<<"\n";
		try
		{
			Mem.Acquire( size );
			//
			Start			= Mem.GetMemory();
			TotalFree		= Mem.Size();
			MaxChunkFree	= Mem.Size();
			Size			= Mem.Size();
		}
		catch( const No_Memory& ex )
		{
			throw ex;
		}
		//
		return 0;
	}
	//
	int Page::Release()
	{
		Mem.Release();
		//
		Start			= 0;
		TotalFree		= 0;
		MaxChunkFree	= 0;
		Allocations		= 0;
		FirstAlloc		= 0;
		LastAlloc		= 0;
		//
		return 0;
	}
	//
	void Page::Print( std::ostream& os, bool showData )
	{
		os<<"[Address: "<<Start
			<<", Size: "<<Size<<"("<<MemSizeToText( Size )<<")"
			<<", Total Free: "<<TotalFree<<"("<<MemSizeToText( TotalFree )<<")"
			<<", Biggest Free Chunk: "<<MaxChunkFree<<"("<<MemSizeToText( MaxChunkFree )<<")"
			<<", #Allocations: "<<Allocations
			<<", First Alloc Addr: "<<FirstAlloc
			<<", Last Alloc Addr: "<<LastAlloc
			<<"]\n";
		if( showData )
		{
			os<<"Raw: "<<Mem<<"\n";
		}
	}
}