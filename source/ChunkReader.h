///////////////////////////////////////////////////////////////////////////////////////
//
//	ChunkReader.hpp
//	Simple binary chunk based file reader.
//
//	Authors: Chris Peters
//	Copyright 2009, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////

#include <fstream>
namespace Framework{
struct GChunk
{
	unsigned int Type;
	unsigned int Size;
	unsigned int StartPos;
	unsigned int EndPos;
};

class ChunkReader
{
public:
	ChunkReader(){};

	void Open( std::string filename )
	{
		file.open(filename.c_str() , std::ios::binary  | std::ios::in  );
	}

	void Close()
	{
		file.close();
	}

	GChunk PeekChunk( )
	{
		GChunk c = ReadChunkHeader();
		file.seekg( -int( sizeof(unsigned int )*2 ) , std::ios::cur  );
		return c;
	}

	GChunk ReadChunkHeader()
	{
		GChunk c;
		c.Type = 0;
		file.read((char*)&c.Type , sizeof(unsigned int )*2  );
		c.StartPos = (unsigned int)file.tellg();
		c.EndPos = c.StartPos + c.Size;
		return c;
	}

	GChunk ReadUntilChunk(unsigned int  chunktype)
	{
		GChunk Chunk = ReadChunkHeader();		
		while( Chunk.Type != chunktype && Chunk.Type!=0 )
		{
			SkipChunk( Chunk );
			Chunk = ReadChunkHeader();		
		}
		return Chunk;
	}

	void SkipChunk(GChunk& c)
	{
		file.seekg( c.EndPos );
	}

	void MoveToChunk(GChunk& c)
	{
		file.clear();
		file.seekg( c.StartPos );
	}

	template< typename type>
	void Read( type& data)
	{
		file.read((char*)&data , sizeof(type) );	
	}

	void Read(std::string& str)
	{
		byte strSize = 0;
		Read(strSize);
		str.resize(strSize);
		ReadArray( &str[0] , strSize );
	}

	template< typename type>
	void ReadArray( type * data , int Number )
	{
		file.read( (char*)data , sizeof(type) * Number );
	}

	template< typename type>
	void ReadArraySize( type * data , int Size )
	{
		file.read( (char*)data , Size );
	}

private:
	std::ifstream file;
};
}
