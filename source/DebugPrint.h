////////////////////////////////////////////////////////////////
//
//	DebugPrint.h
//	Adds a file stream to the std::cout function so that any debug msg output is later available in a file
//
//	Author: Santosh Shedbalkar
//	Copyright 2011, Digipen Institute of Technology
//
////////////////////////////////////////////////////////////////
#ifndef DEBUG_PRINT_H
#define DEBUG_PRINT_H
//#pragma message("Compiling: "__FILE__)
//
#include <streambuf>
#include <iostream>
#include <fstream>
//
#define OUTPUT_FILE "debugOutput.txt"
//
namespace Debug
{
	class DebugStream: public std::streambuf
	{
//#pragma message("Including Class: DebugStream")
	public:
		DebugStream();
		DebugStream(const char* file);
		~DebugStream();
		//
		std::streambuf* getFileStreamBuf();
		void toCout(int val);
		std::ofstream* getFileStream();
		inline void logLevel(int i)
		{
			_logLevel = i;
		}
		inline int logLevel()
		{
			return _logLevel;
		}
		inline void threshold(int i)
		{
			_logThreshold = i;
		}
		inline int threshold()
		{
			return _logThreshold;
		}
		inline void enable(bool b)
		{
			_enable = b;
		}
		inline int enable()
		{
			return _enable;
		}
	protected:
		std::ofstream m_file;
		std::streambuf* m_file_sbuf;
		std::streambuf* m_cout_sbuf;
		int m_to_cout;
		//
		typedef std::char_traits<char> traits_type;
		typedef traits_type::int_type int_type;
		//
		void init(const char* file);
		int_type overflow(int_type c);
		int sync();
		int _logLevel;
		int _logThreshold;
		bool _enable;
	};
	//
	static DebugStream* DEBUG_STREAM;
}
//
#endif
