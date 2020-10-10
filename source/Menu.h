#pragma once
//
#include "Singleton.h"
#include "FunctionBind.h"
#include "ReturnCodes.h"
//
namespace Framework
{
#define FRM_TYPE_FINITE_OPTIONS_SINGLE_SELECTION		1
#define FRM_TYPE_FINITE_OPTIONS_MULTI_SELECTION			2
#define FMR_TYPE_INFINITE_OPTIONS_SINGLE_SELECTION		3
#define MAX_FRM_ID_LEN									8
	//
	class Menu;
	//
	class FrameData
	{
	private:
										FrameData( utility::FunctionObject* func, utility::Object obj, const char* fid );
										~FrameData();
		friend							Menu;
		//
	public:
		char							id[MAX_FRM_ID_LEN];
		utility::FunctionObject*		updFunc;
		utility::Object					instance;
	};
	//
	class Menu: public Singleton<Menu>
	{
	public:
						Menu();
						~Menu();
		//
	public:
		int				CreateMenuFrom( const char* fileName );
		const char*		GetTopFrameID();
		//
		template< typename ClassType >
		inline int ShowFrame( void(ClassType::*f)(const char*, const std::vector<std::string>*), ClassType* obj, const char* id )
		{
			if( m_show ) return RET_IN_PROGRESS;
			return ShowFrameIntern( new FrameData(utility::Bind(f), obj, id) );
		}
		//
		inline int ShowFrame( void(*f)(const char*, const std::vector<std::string>*), const char* id )
		{
			if( m_show ) return RET_IN_PROGRESS;
			return ShowFrameIntern( new FrameData(utility::Bind(f), NULL, id) );
		}
		//
	private:
		int				ShowFrameIntern( FrameData* fd );
		void			ClearStackdata();
		void			ClearFrame();
		//
	private:
		bool			m_show;
	};
}