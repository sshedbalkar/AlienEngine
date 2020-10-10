#pragma once
//
#include "Singleton.h"
#include <vector>
#include "FunctionBind.h"
#include "Graphics.h"
#include "Utility.h"
#include <iostream>
//
#define MAX_TEXT_LENGTH		128
//
namespace Framework
{
	class TextDisplay;
	class TextClass;
	struct SentenceType;
	class TextClass;
	//
	class DispTextData
	{
	private:
										DispTextData( const unsigned int& id, utility::FunctionObject* f, utility::Object obj, bool debug, bool singleFrame );
										~DispTextData();
		//
		friend							TextDisplay;
		//
		char							m_text[MAX_TEXT_LENGTH];
		unsigned int					m_id;
		SentenceType*					m_sentence;
		int								m_posX;
		int								m_posY;
		utility::FunctionObject*		m_updFunc;
		utility::Object					m_obj;
		bool							m_debug;
		bool							m_1frame;
		//
	public:
		float							Blue();
		void							Blue( const float& val );
		float							Green();
		void							Green( const float& val );
		float							Red();
		void							Red( const float& val );
		//
		int								PosX();
		void							PosX( const int& val );
		int								PosY();
		void							PosY( const int& val );
		bool							DebugText();
		void							DebugText( bool val );
		unsigned int					ID();
		const char*						Text();
	};
	//
	class TextDisplay: public Singleton< TextDisplay >
	{
	public:
							TextDisplay();
							~TextDisplay();
		//
	public:
		template< typename ClassType > 
		inline unsigned int	AddNewText( const char*(ClassType::*f)(DispTextData*), ClassType* obj, bool debug = true, const int& posx = 0, const int& posy = 0, const float& red = 1.0f, const float& green = 1.0f, const float& blue = 1.0f, const char* text = "" )
		{
			DispTextData* d = new DispTextData( utility::GetUniqueId(), utility::Bind(f), obj, debug, false );
			::strcpy_s( d->m_text, MAX_TEXT_LENGTH, text );
			d->m_posX = posx;
			d->m_posY = posy;
			m_textList.push_back( d );
			//
			InitSentence( d, red, green, blue, text );
			//
			return d->m_id;
		}
		//
		int					RemoveText( const unsigned int& id );
		int					Update( bool debug );
		unsigned int		AddNewText1Frame( const char* text, const int& posx, const int& posy );
		unsigned int		AddNewText1Frame( const int& num, const int& posx, const int& posy );
		unsigned int		AddNewText1Frame( const float& num, const int& posx, const int& posy );
		//
	private:
		int					InitSentence( DispTextData* d, const float& red, const float& green, const float& blue, const char* text );
		//
	private:
		typedef std::vector< DispTextData* > TextListType;
		TextListType		m_textList;
	};
}