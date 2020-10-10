#include "Menu.h"
#include "XMLSerialization.h"
#include <hash_map>
#include <stack>
#include <string>
#include <vector>
#include "Utility.h"
//
typedef const char*													cstr;
typedef stdext::hash_map< std::string, Framework::ISerializer* >	FrameMapType;
typedef std::pair< std::string, Framework::ISerializer* >			IDStreamPair;
typedef std::stack<Framework::FrameData*>							FrameStackType;
typedef std::vector<std::string*>									StringList;
//
#define OPTION_INVALID	0
#define OPTION_PREV		1
#define OPTION_CLOSE	2
#define OPTION_VALID	3
//
Framework::XMLReader*		l_reader = 0;
FrameMapType				l_frameMap;
FrameStackType				l_frameStack;
std::string					l_topID("");
Framework::Menu*			l_menu = 0;
//
void	RenderFrame( Framework::ISerializer* frame, int type );
void	RenderType1( std::string& title, Framework::StreamList& options, bool showParent );
void	RenderType3( std::string& title, Framework::ISerializer* option, bool showParent );
void	GetInput( std::string& str );
int		ValidateInput( Framework::StreamList& options, std::string& input, int type );
//
namespace Framework
{
	FrameData::FrameData( utility::FunctionObject* func, utility::Object obj, const char* fid )
		:updFunc( func ),
		instance( obj )
	{
		::strcpy_s( id, fid );
	}
	//
	FrameData::~FrameData()
	{
		delete updFunc;
		instance = 0;
	}
	///////////////////////////////////////////////////////////////////////////////////
	Menu::Menu()
		:m_show( false )
	{
		l_menu = this;
	}
	//
	Menu::~Menu()
	{
		ClearStackdata();
		if( l_reader )
		{
			for( FrameMapType::iterator it1 = l_frameMap.begin(), it2= l_frameMap.end(); it1 != it2; ++it1 )
			{
				delete it1->second;
			}
			delete l_reader;
		}
		l_menu = 0;
	}
	//
	int Menu::CreateMenuFrom( const char* fileName )
	{
		delete l_reader;
		l_reader = new XMLReader();
		if( l_reader->open(fileName) )
		{
			ISerializer* str = l_reader->data().clone(), *frame;
			StreamList list;
			std::string str1;
			int top = -1;
			//
			if ( str->readProperty( "Frames", *str ) )
			{
				str->readProperty("Frame", list);
				//
				for( StreamList::iterator it1 = list.begin(), it2 = list.end(); it1 != it2; ++it1 )
				{
					frame = *it1;
					if( frame->readProperty("id", str1) )
					{
						l_frameMap.insert( IDStreamPair(str1, frame) );
						if( frame->readProperty( "top", top ) && top == 1 )
						{
							l_topID = str1;
						}
					}
				}
			}
			delete str;
			//
			return RET_SUCCESS;
		}
		//
		return RET_FAIL;
	}
	//
	int Menu::ShowFrameIntern( FrameData* fd )
	{
		m_show = true;
		FrameMapType::iterator it = l_frameMap.find( fd->id );
		if( it == l_frameMap.end() )
		{
			return RET_FAIL;
		}
		//
		l_frameStack.push( fd );
		ISerializer* frame = it->second;
		//
		int type;
		frame->readProperty( "type", type );
		//
		RenderFrame( frame, type );
		//
		Framework::StreamList options;
		frame->readProperty( "Option", options );
		//
		std::string input;
		int valid = -1;
		do
		{
			GetInput( input );
			valid = ValidateInput( options, input, type );
			switch( valid )
			{
			case OPTION_PREV:
				{
					if( (l_frameStack.size() > 1) )
					{
						ClearFrame();
						delete l_frameStack.top();
						l_frameStack.pop();
						//
						FrameData* f = l_frameStack.top();
						l_frameStack.pop();
						return ShowFrameIntern( f );
					}
					break;
				}
			case OPTION_CLOSE:
				{
					ClearFrame();
					ClearStackdata();
					break;
				}
			case OPTION_VALID:
				{
					ClearFrame();
					//
					std::vector<std::string> selection;
					selection.push_back( input );
					//
					utility::ArgumentVector v1;
					v1.push_back( &(fd->id[0]) );
					v1.push_back( &selection );
					//
					fd->updFunc->Invoke( fd->instance, v1 );
					//
					if( !l_frameStack.empty() )
					{
						delete l_frameStack.top();
						l_frameStack.pop();
					}
					//
					break;
				}
			case OPTION_INVALID:
				{
					std::cout<<"The value you have entered is invalid, please enter a valid value: ";
					break;
				}
			}
		}
		while( valid == OPTION_INVALID );
		//
		return RET_SUCCESS;
	}
	//
	const char* Menu::GetTopFrameID()
	{
		return l_topID.c_str();
	}
	//
	void Menu::ClearStackdata()
	{
		while( !l_frameStack.empty() )
		{
			delete l_frameStack.top();
			l_frameStack.pop();
		}
	}
	void Menu::ClearFrame()
	{
		m_show = false;
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
//
int ValidateInput( Framework::StreamList& options, std::string& input, int type )
{

	if( !input.compare("p") || !input.compare("P") )
	{
		return OPTION_PREV;
	}
	//
	if( !input.compare("x") || !input.compare("X"))
	{
		return OPTION_CLOSE;
	}
	//
	if( type == FRM_TYPE_FINITE_OPTIONS_SINGLE_SELECTION )
	{
		std::string id;
		for each( Framework::ISerializer* option in options )
		{
			option->readProperty( "id", id );
			if( !input.compare(id) )
			{
				return OPTION_VALID;
			}
		}
	}
	else if( type == FRM_TYPE_FINITE_OPTIONS_MULTI_SELECTION )
	{
		//Yet to implement
	}
	else if( type == FMR_TYPE_INFINITE_OPTIONS_SINGLE_SELECTION )
	{
		Framework::ISerializer* option = options.at( 0 );
		float min, max, val;
		option->readProperty( "min", min );
		option->readProperty( "max", max );
		//
		utility::from_string<float>( val, input, std::dec );
		//
		if( (val >= min) && (val <= max) )
		{
			return OPTION_VALID;
		}
	}
	//
	return OPTION_INVALID;
}
//
void GetInput( std::string& str )
{
	std::getline(std::cin, str);
	std::cout<<"\n";
}
//
void RenderFrame( Framework::ISerializer* frame, int type )
{
	std::string title;
	frame->readProperty( "title", title );
	//
	if( type == FRM_TYPE_FINITE_OPTIONS_SINGLE_SELECTION )
	{
		Framework::StreamList options;
		frame->readProperty( "Option", options );
		RenderType1( title, options, l_frameStack.size() > 1 );
	}
	else if( type == FRM_TYPE_FINITE_OPTIONS_MULTI_SELECTION )
	{
		//Yet to implement
	}
	else if( type == FMR_TYPE_INFINITE_OPTIONS_SINGLE_SELECTION )
	{
		Framework::ISerializer* option = frame->clone();
		option->readProperty( "Option", *option );
		RenderType3( title, option, l_frameStack.size() > 1 );
		delete option;
	}
}
//
void RenderType3( std::string& title, Framework::ISerializer* option, bool showParent )
{
	float min, max;
	option->readProperty( "min", min );
	option->readProperty( "max", max );
	//
	for( int i = title.size()+8; i > 0; --i )
	{
		std::cout<<"-";
	}
	std::cout<<"\n";
	std::cout<<"*** "<<title.c_str()<<" ***\n";
	for( int i = title.size()+8; i > 0; --i )
	{
		std::cout<<"-";
	}
	std::cout<<"\n";
	//
	std::cout<<"A value in the range ("<<min<<", "<<max<<")\n\n";
	//
	if( showParent )
	{
		std::cout<<"p: Go back to the previous Menu\n";
	}
	std::cout<<"x: Close Menu\n";
	std::cout<<"------------------------------------\n";
	std::cout<<"Enter your choice: ";
}
//
void RenderType1( std::string& title, Framework::StreamList& options, bool showParent )
{
	for( int i = title.size()+8; i > 0; --i )
	{
		std::cout<<"-";
	}
	std::cout<<"\n";
	std::cout<<"*** "<<title.c_str()<<" ***\n";
	for( int i = title.size()+8; i > 0; --i )
	{
		std::cout<<"-";
	}
	std::cout<<"\n";
	//
	std::string id, name;
	for each( Framework::ISerializer* option in options )
	{
		option->readProperty( "id", id );
		option->readProperty( "name", name );
		std::cout<<id.c_str()<<": "<<name.c_str()<<"\n";
	}
	std::cout<<"\n";
	if( showParent )
	{
		std::cout<<"p: Go back to the previous Menu\n";
	}
	std::cout<<"x: Close Menu\n";
	std::cout<<"------------------------------------\n";
	std::cout<<"Enter your choice: ";
}