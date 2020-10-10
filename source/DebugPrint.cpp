#include "DebugPrint.h"
//
namespace Debug
{
	DebugStream::DebugStream():
		m_file_sbuf(NULL),
		m_cout_sbuf(NULL),
		m_to_cout(1),
		_logLevel(0),
		_logThreshold(1),
		_enable(true)
	{
		init(OUTPUT_FILE);
	}
	DebugStream::DebugStream(const char* file):
		m_file_sbuf(NULL),
		m_cout_sbuf(NULL),
		m_to_cout(1)
	{
		init(file);
	}
	DebugStream::~DebugStream()
	{
		if(m_cout_sbuf)
		{
			std::cout.rdbuf(m_cout_sbuf);
		}
		if(m_file.is_open())
		{
			m_file.close();
			m_file_sbuf = NULL;
		}
	}
	//
	std::streambuf* DebugStream::getFileStreamBuf()
	{
		return m_file_sbuf;
	}
	void DebugStream::toCout(int val)
	{
		m_to_cout = val;
	}
	std::ofstream* DebugStream::getFileStream()
	{
		return &m_file;
	}
	void DebugStream::init(const char* file)
	{
		m_file.open(file);
		m_file_sbuf = m_file.rdbuf();
		m_cout_sbuf = std::cout.rdbuf();
		std::cout.rdbuf(this);
	}
	DebugStream::int_type DebugStream::overflow(DebugStream::int_type c)
	{
		if (!traits_type::eq_int_type(c, traits_type::eof()))
		{
			bool b = _enable && (_logLevel < _logThreshold);
			if(b)
			{
				c = static_cast<int_type>(m_file_sbuf->sputc(c));
				m_file.flush();
			}
			if (m_to_cout && !traits_type::eq_int_type(c, traits_type::eof()))
			{
				if(b)
				{
					c =  static_cast<int_type>(m_cout_sbuf->sputc(c));
				}
			}
			return c;
		}
		else
		{
			return traits_type::not_eof(c);
		}
	}
	int DebugStream::sync()
	{
		int rc = m_file_sbuf->pubsync();
		if (m_to_cout && rc != -1)
		{
			rc = m_cout_sbuf->pubsync();
		}
		return rc;
	}
}