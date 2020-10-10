#include "FileRW.h"
#include <iostream>
#include <fstream>
#include "ReturnCodes.h"
#include "Utility.h"
//
namespace utility
{
	//////////////////////////////////////////////////////////////////////////////////
	FileInfo::FileInfo()
		:Name( "" ),
		Size( 0L ),
		FullName( "" )
	{
		//
	}
	FileInfo::FileInfo( const char* name, long long &size )
		:Name( name ),
		Size( size ),
		FullName( "" )
	{
		//
	}
	FileInfo::FileInfo( const char* name, long long &size, const char *fullName )
		:Name( name ),
		Size( size ),
		FullName( fullName )
	{

	}
	FileInfo::~FileInfo()
	{
		//
	}
	//////////////////////////////////////////////////////////////////////////////////
	int ListFilesInDirectory( const char *fileName, FileInfoList &filenameList )
	{
		std::string dir, ffName;
		std::vector< std::string > vec;
		utility::SplitString( fileName, '/', vec );
		for( std::vector< std::string >::iterator it1 = vec.begin(), it2 = vec.end() - 1; it1 != it2; ++it1 )
		{
			
			//
			dir.append( (*it1) );
			dir.append( "/" );
		}
		//
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind = ::FindFirstFile( fileName, &FindFileData );
		LARGE_INTEGER size;
		if( hFind == INVALID_HANDLE_VALUE ) 
		{
			std::cout<<"[ERR: FileRW: 1]: "<<::GetLastError()<<"\n";
			return RET_FAIL;
		}
		//
		size.LowPart = FindFileData.nFileSizeLow;
		size.HighPart = FindFileData.nFileSizeHigh;
		//
		ffName.append( dir );
		ffName.append( FindFileData.cFileName );
		//
		filenameList.push_back( FileInfo(FindFileData.cFileName, size.QuadPart, ffName.c_str()) );
		//
		while( ::FindNextFile(hFind, &FindFileData) )
		{
			size.LowPart = FindFileData.nFileSizeLow;
			size.HighPart = FindFileData.nFileSizeHigh;
			//
			ffName.assign( dir );
			ffName.append( FindFileData.cFileName );
			//
			filenameList.push_back( FileInfo(FindFileData.cFileName, size.QuadPart, ffName.c_str()) );
		}
		//
		DWORD dwError = ::GetLastError();
		if( dwError != ERROR_NO_MORE_FILES )
		{
			std::cout<<"[ERR: FileRW: 2]: "<<dwError<<"\n";
		}
		//
		::FindClose( hFind );
		//
		return RET_SUCCESS;
	}
	//
	int ReadNextLine( char *buff, const int &buffLen, FILE *hFile )
	{
		if( ::fgets(buff, buffLen, hFile) == NULL )
		{
			if( ::ferror(hFile) )
			{
				std::cout << "[ERROR: FileRW: 3]:fgets error\n";
				return RET_FAIL;
			}
		}
		return RET_SUCCESS;
	}
	//
	bool CanOpenFile( const char* file )
	{
		std::fstream filestr;
		filestr.open( file );
		if( filestr.is_open() )
		{
			filestr.close();
			return true;
		}
		return false;
	}
	//
	char* ReadFile( const char* file )
	{
		int length;
		std::ifstream is;
		char *buffer;
		//
		is.open( file, std::ios::binary );
		if( is.is_open() )
		{
			// get length of file:
			is.seekg( 0, std::ios::end );
			length = static_cast<int>( is.tellg() );
			is.seekg( 0, std::ios::beg );
			//
			// allocate memory:
			buffer = new char [ length + 1 ];
			//
			// read data as a block:
			is.read( buffer, length );
			is.close();
			buffer[ length ] = '\0';
			//
			return buffer;
		}
		else
		{
			std::cout<<"[ERR: FileRW: 4]: " <<file<<"\n";
			return 0;
		}
	}
	std::ostream& operator<<( std::ostream &out, const FileInfo &f )
	{
		return out<<"["<< f.Name.c_str()<<" : "<<f.Size<<" : "<<f.FullName.c_str()<<"]";
	}
}