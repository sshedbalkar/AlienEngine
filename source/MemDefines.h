#pragma once
//
#define MEM_DEBUG
#define MEM_COLLECT_STATS
//
#define MEM_OUTPUT_FILE_PATH	"Memory_Report.txt"
#define MEM_INVALID_ADDR		0x00000000
//
namespace Memory
{
	typedef unsigned char	Byte;
	typedef __int64			Address;
	typedef const char*		Cstr;
	typedef unsigned		UnitAllocSize;
	typedef unsigned		UnitCount;
	typedef void*			Pointer;
	//
	const unsigned char RAW_MEM					= '*';
	const unsigned MAX_MEM_PAGES_PER_BANK		= 256;
	const unsigned MEM_PER_PAGE					= 128 * 1024 * 1024;	// 128 MB
	const Pointer PAGE_END						= 0;
	const unsigned MAX_POOLS					= 32;
	const unsigned MAX_FILE_NAME_LENGTH			= 256;
	//
	const int		ERR_MEM_COULD_NOT_FIND_ADDR	= 23;
	const unsigned	MEM_BLOCK_DIFF_THRESHOLD	= 0x00000008;
	//
	class MemAllocatorGen;
	//
	enum PoolStatus
	{
		MEM_UNALLOCATED,
		MEM_EMPTY,
		MEM_ACTIVE,
		MEM_FULL,
		MEM_CORRUPT
	};
	//
	enum AllocType
	{
		ALLOC_SINGLE,
		ALLOC_ARRAY
	};
}