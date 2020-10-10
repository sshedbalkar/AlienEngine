#pragma once
//
#include <vector>
#include "Variant.h"
//#include <iostream>
//
namespace utility
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	typedef std::vector< Variant > ArgumentVector;
	//
	typedef void ( *VariantFunction )( Object object, ArgumentVector& args );
	//
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	class FunctionObject
	{
	public:
		virtual ~FunctionObject(){};
		virtual Variant Invoke( Object object = NULL, ArgumentVector& args = ArgumentVector() ) = 0;
	};
	//
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	template< typename ReturnType, typename Param0Type, typename Param1Type, typename Param2Type, typename Param3Type >
	class SimpleFunction: public FunctionObject
	{
	public:
		typedef ReturnType ( *MyFunctionType )( Param0Type, Param1Type, Param2Type, Param3Type );

		MyFunctionType Function;
		SimpleFunction( MyFunctionType f )
			:Function( f )
		{
			//
		}
		//
		Variant Invoke( Object object, ArgumentVector& args ) override
		{
			return Variant(( *Function )( args[0].As< Param0Type >(), args[1].As< Param1Type >(), args[2].As< Param2Type >(), args[3].As< Param3Type >() ));
		}
	};
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	template< typename Param0Type, typename Param1Type, typename Param2Type, typename Param3Type >
	class SimpleFunction< void, Param0Type, Param1Type, Param2Type, Param3Type >: public FunctionObject
	{
	public:
		typedef void ( *MyFunctionType )( Param0Type, Param1Type, Param2Type, Param3Type );

		MyFunctionType Function;
		SimpleFunction( MyFunctionType f )
			:Function( f )
		{
			//
		}
		//
		Variant Invoke( Object object, ArgumentVector& args ) override
		{
			( *Function )( args[0].As< Param0Type >(), args[1].As< Param1Type >(), args[2].As< Param2Type >(), args[3].As< Param3Type >() );
			return Variant();
		}
	};
	//
	template< typename Param0Type, typename Param1Type, typename Param2Type >
	class SimpleFunction< void, Param0Type, Param1Type, Param2Type, void >: public FunctionObject
	{
	public:
		typedef void ( *MyFunctionType )( Param0Type, Param1Type, Param2Type );

		MyFunctionType Function;
		SimpleFunction( MyFunctionType f )
			:Function( f )
		{
			//
		}
		//
		Variant Invoke( Object object, ArgumentVector& args ) override
		{
			( *Function )( args[0].As< Param0Type >(), args[1].As< Param1Type >(), args[2].As< Param2Type >() );
			return Variant();
		}
	};
	//
	template< typename Param0Type, typename Param1Type >
	class SimpleFunction< void, Param0Type, Param1Type, void, void >: public FunctionObject
	{
	public:
		typedef void ( *MyFunctionType )( Param0Type, Param1Type );

		MyFunctionType Function;
		SimpleFunction( MyFunctionType f )
			:Function( f )
		{
			//
		}
		//
		Variant Invoke( Object object, ArgumentVector& args ) override
		{
			( *Function )( args[0].As< Param0Type >(), args[1].As< Param1Type >() );
			return Variant();
		}
	};
	//
	template< typename Param0Type >
	class SimpleFunction< void, Param0Type, void, void, void >: public FunctionObject
	{
	public:
		typedef void ( *MyFunctionType )( Param0Type );

		MyFunctionType Function;
		SimpleFunction( MyFunctionType f )
			:Function( f )
		{
			//
		}
		//
		Variant Invoke( Object object, ArgumentVector& args ) override
		{
			( *Function )( args[0].As< Param0Type >() );
			return Variant();
		}
	};
	//
	template<>
	class SimpleFunction< void, void, void, void, void >: public FunctionObject
	{
	public:
		typedef void( *MyFunctionType )( void );

		MyFunctionType Function;
		SimpleFunction( MyFunctionType f )
			:Function( f )
		{
			//
		}
		//
		Variant Invoke( Object object = NULL, ArgumentVector& args = ArgumentVector() ) override
		{
			( *Function )();
			return Variant();
		}
	};
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//
	inline FunctionObject* Bind( void( *Func )( void ) )
	{
		//std::cout<<"Binding Simple function1: void, void\n";
		return new SimpleFunction< void, void, void, void, void >( Func );
	}
	//
	template< typename ParamType0 >
	inline FunctionObject* Bind( void( *Func )( ParamType0 ) )
	{
		//std::cout<<"Binding Simple function1: void, Param0\n";
		return new SimpleFunction< void, ParamType0, void, void, void >( Func );
	}
	//
	template< typename ParamType0, typename ParamType1 >
	inline FunctionObject* Bind( void( *Func )( ParamType0, ParamType1 ) )
	{
		//std::cout<<"Binding Simple function1: void, Param0, Param1\n";
		return new SimpleFunction< void, ParamType0, ParamType1, void, void >( Func );
	}
	//
	template< typename ParamType0, typename ParamType1, typename ParamType2 >
	inline FunctionObject* Bind( void( *Func )( ParamType0, ParamType1, ParamType2 ) )
	{
		//std::cout<<"Binding Simple function1: void, Param0, Param1, Param2\n";
		return new SimpleFunction< void, ParamType0, ParamType1, ParamType2, void >( Func );
	}
	//
	template< typename ParamType0, typename ParamType1, typename ParamType2, typename ParamType3 >
	inline FunctionObject* Bind( void( *Func )( ParamType0, ParamType1, ParamType2, ParamType3 ) )
	{
		//std::cout<<"Binding Simple function1: void, Param0, Param1, Param2, Param3\n";
		return new SimpleFunction< void, ParamType0, ParamType1, ParamType2, ParamType3 >( Func );
	}
	//
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	template< typename ReturnType, typename Param0Type, typename Param1Type, typename Param2Type >
	class SimpleFunction< ReturnType, Param0Type, Param1Type, Param2Type, void >: public FunctionObject
	{
	public:
		typedef ReturnType ( *MyFunctionType )( Param0Type, Param1Type, Param2Type );

		MyFunctionType Function;
		SimpleFunction( MyFunctionType f )
			:Function( f )
		{
			//
		}
		//
		Variant Invoke( Object object, ArgumentVector& args ) override
		{
			return Variant(( *Function )( args[0].As< Param0Type >(), args[1].As< Param1Type >(), args[2].As< Param2Type >()));
		}
	};
	//
	template< typename ReturnType, typename Param0Type, typename Param1Type >
	class SimpleFunction< ReturnType, Param0Type, Param1Type, void, void >: public FunctionObject
	{
	public:
		typedef ReturnType ( *MyFunctionType )( Param0Type, Param1Type );

		MyFunctionType Function;
		SimpleFunction( MyFunctionType f )
			:Function( f )
		{
			//
		}
		//
		Variant Invoke( Object object, ArgumentVector& args ) override
		{
			return Variant(( *Function )( args[0].As< Param0Type >(), args[1].As< Param1Type >() ));
		}
	};
	//
	template< typename ReturnType, typename Param0Type >
	class SimpleFunction< ReturnType, Param0Type, void, void, void >: public FunctionObject
	{
	public:
		typedef ReturnType ( *MyFunctionType )( Param0Type );

		MyFunctionType Function;
		SimpleFunction( MyFunctionType f )
			:Function( f )
		{
			//
		}
		//
		Variant Invoke( Object object, ArgumentVector& args ) override
		{
			return Variant(( *Function )( args[0].As< Param0Type >() ));
		}
	};
	//
	template< typename ReturnType >
	class SimpleFunction< ReturnType, void, void, void, void >: public FunctionObject
	{
	public:
		typedef ReturnType ( *MyFunctionType )( void );

		MyFunctionType Function;
		SimpleFunction( MyFunctionType f )
			:Function( f )
		{
			//
		}
		//
		Variant Invoke( Object object, ArgumentVector& args ) override
		{
			return Variant(( *Function )());
		}
	};
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	template< typename ReturnType >
	inline FunctionObject* Bind( ReturnType ( *Func )( void ) )
	{
		//std::cout<<"Binding Simple function1: RetType, void\n";
		return new SimpleFunction< ReturnType, void, void, void, void >( Func );
	}
	//
	template< typename ReturnType, typename ParamType0 >
	inline FunctionObject* Bind( ReturnType ( *Func )( ParamType0 ) )
	{
		//std::cout<<"Binding Simple function1: RetType, Param0\n";
		return new SimpleFunction< ReturnType, ParamType0, void, void, void >( Func );
	}
	//
	template< typename ReturnType, typename ParamType0, typename ParamType1 >
	inline FunctionObject* Bind( ReturnType ( *Func )( ParamType0, ParamType1 ) )
	{
		//std::cout<<"Binding Simple function1: RetType, Param0, Param1\n";
		return new SimpleFunction< ReturnType, ParamType0, ParamType1, void, void >( Func );
	}
	//
	template< typename ReturnType, typename ParamType0, typename ParamType1, typename ParamType2 >
	inline FunctionObject* Bind( ReturnType ( *Func )( ParamType0, ParamType1, ParamType2 ) )
	{
		//std::cout<<"Binding Simple function1: RetType, Param0, Param1, Param2\n";
		return new SimpleFunction< ReturnType, ParamType0, ParamType1, ParamType2, void >( Func );
	}
	//
	template< typename ReturnType, typename ParamType0, typename ParamType1, typename ParamType2, typename ParamType3 >
	inline FunctionObject* Bind( ReturnType ( *Func )( ParamType0, ParamType1, ParamType2, ParamType3 ) )
	{
		//std::cout<<"Binding Simple function1: RetType, Param0, Param1, Param2, Param3\n";
		return new SimpleFunction< ReturnType, ParamType0, ParamType1, ParamType2, ParamType3 >( Func );
	}
	//
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template< typename ClassType, typename ReturnType, typename Param0Type, typename Param1Type, typename Param2Type, typename Param3Type >
	class ClassFunction: public FunctionObject
	{
	public:
		typedef ReturnType( ClassType::*MyFunctionType )( Param0Type, Param1Type, Param2Type, Param3Type );

		MyFunctionType Function;
		ClassFunction( MyFunctionType f )
			:Function( f )
		{
			//
		}
		//
		Variant Invoke( Object object, ArgumentVector& args ) override
		{
			ClassType* instance = ( ClassType* )object;
			return Variant(( instance->*Function )( args[0].As<Param0Type>(), args[1].As<Param1Type>(), args[2].As<Param2Type>(), args[3].As<Param3Type>() ));
		}
	};
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template< typename ClassType, typename Param0Type, typename Param1Type, typename Param2Type, typename Param3Type >
	class ClassFunction< ClassType, void, Param0Type, Param1Type, Param2Type, Param3Type >: public FunctionObject
	{
	public:
		typedef void( ClassType::*MyFunctionType )( Param0Type, Param1Type, Param2Type, Param3Type );

		MyFunctionType Function;
		ClassFunction( MyFunctionType f )
			:Function( f )
		{
			//
		}
		//
		Variant Invoke( Object object, ArgumentVector& args ) override
		{
			ClassType* instance = ( ClassType* )object;
			( instance->*Function )( args[0].As<Param0Type>(), args[1].As<Param1Type>(), args[2].As<Param2Type>(), args[3].As<Param3Type>() );
			return Variant();
		}
	};
	//
	template< typename ClassType, typename Param0Type, typename Param1Type, typename Param2Type >
	class ClassFunction< ClassType, void, Param0Type, Param1Type, Param2Type, void >: public FunctionObject
	{
	public:
		typedef void( ClassType::*MyFunctionType )( Param0Type, Param1Type, Param2Type );

		MyFunctionType Function;
		ClassFunction( MyFunctionType f )
			:Function( f )
		{
			//
		}
		//
		Variant Invoke( Object object, ArgumentVector& args ) override
		{
			ClassType* instance = ( ClassType* )object;
			( instance->*Function )( args[0].As<Param0Type>(), args[1].As<Param1Type>(), args[2].As<Param2Type>() );
			return Variant();
		}
	};
	//
	template< typename ClassType, typename Param0Type, typename Param1Type >
	class ClassFunction< ClassType, void, Param0Type, Param1Type, void, void >: public FunctionObject
	{
	public:
		typedef void( ClassType::*MyFunctionType )( Param0Type, Param1Type );

		MyFunctionType Function;
		ClassFunction( MyFunctionType f )
			:Function( f )
		{
			//
		}
		//
		Variant Invoke( Object object, ArgumentVector& args ) override
		{
			ClassType* instance = ( ClassType* )object;
			( instance->*Function )( args[0].As<Param0Type>(), args[1].As<Param1Type>() );
			return Variant();
		}
	};
	//
	template< typename ClassType, typename Param0Type >
	class ClassFunction< ClassType, void, Param0Type, void, void, void >: public FunctionObject
	{
	public:
		typedef void( ClassType::*MyFunctionType )( Param0Type );

		MyFunctionType Function;
		ClassFunction( MyFunctionType f )
			:Function( f )
		{
			//
		}
		//
		Variant Invoke( Object object, ArgumentVector& args ) override
		{
			ClassType* instance = ( ClassType* )object;
			( instance->*Function )( args[0].As<Param0Type>() );
			return Variant();
		}
	};
	//
	template< typename ClassType >
	class ClassFunction< ClassType, void, void, void, void, void >: public FunctionObject
	{
	public:
		typedef void( ClassType::*MyFunctionType )( void );

		MyFunctionType Function;
		ClassFunction( MyFunctionType f )
			:Function( f )
		{
			//
		}
		//
		Variant Invoke( Object object, ArgumentVector& args = ArgumentVector() ) override
		{
			ClassType* instance = ( ClassType* )object;
			( instance->*Function )();
			return Variant();
		}
	};
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	template< typename ClassType >
	inline FunctionObject* Bind( void( ClassType::*Func )( void ) )
	{
		//std::cout<<"Binding Class function1: void, void\n";
		return new ClassFunction< ClassType, void, void, void, void, void >( Func );
	}
	//
	template< typename ClassType, typename ParamType0 >
	inline FunctionObject* Bind( void( ClassType::*Func )( ParamType0 ) )
	{
		//std::cout<<"Binding Class function1: void, Param0\n";
		return new ClassFunction< ClassType, void, ParamType0, void, void, void >( Func );
	}
	//
	template< typename ClassType, typename ParamType0, typename ParamType1 >
	inline FunctionObject* Bind( void( ClassType::*Func )( ParamType0, ParamType1 ) )
	{
		//std::cout<<"Binding Class function1: void, Param0, Param1\n";
		return new ClassFunction< ClassType, void, ParamType0, ParamType1, void, void >( Func );
	}
	//
	template< typename ClassType, typename ParamType0, typename ParamType1, typename ParamType2 >
	inline FunctionObject* Bind( void( ClassType::*Func )( ParamType0, ParamType1, ParamType2 ) )
	{
		//std::cout<<"Binding Class function1: void, Param0, Param1, Param2\n";
		return new ClassFunction< ClassType, void, ParamType0, ParamType1, ParamType2, void >( Func );
	}
	//
	template< typename ClassType, typename ParamType0, typename ParamType1, typename ParamType2, typename ParamType3 >
	inline FunctionObject* Bind( void( ClassType::*Func )( ParamType0, ParamType1, ParamType2, ParamType3 ) )
	{
		//std::cout<<"Binding Class function1: void, Param0, Param1, Param2, Param3\n";
		return new ClassFunction< ClassType, void, ParamType0, ParamType1, ParamType2, ParamType3 >( Func );
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	template< typename ClassType, typename ReturnType, typename Param0Type, typename Param1Type, typename Param2Type >
	class ClassFunction< ClassType, ReturnType, Param0Type, Param1Type, Param2Type, void >: public FunctionObject
	{
	public:
		typedef ReturnType( ClassType::*MyFunctionType )( Param0Type, Param1Type, Param2Type );

		MyFunctionType Function;
		ClassFunction( MyFunctionType f )
			:Function( f )
		{
			//
		}
		//
		Variant Invoke( Object object, ArgumentVector& args ) override
		{
			ClassType* instance = ( ClassType* )object;
			return Variant(( instance->*Function )( args[0].As<Param0Type>(), args[1].As<Param1Type>(), args[2].As<Param2Type>() ));
		}
	};
	//
	template< typename ClassType, typename ReturnType, typename Param0Type, typename Param1Type >
	class ClassFunction< ClassType, ReturnType, Param0Type, Param1Type, void, void >: public FunctionObject
	{
	public:
		typedef ReturnType( ClassType::*MyFunctionType )( Param0Type, Param1Type );

		MyFunctionType Function;
		ClassFunction( MyFunctionType f )
			:Function( f )
		{
			//
		}
		//
		Variant Invoke( Object object, ArgumentVector& args ) override
		{
			ClassType* instance = ( ClassType* )object;
			return Variant(( instance->*Function )( args[0].As<Param0Type>(), args[1].As<Param1Type>() ));
		}
	};
	//
	template< typename ClassType, typename ReturnType, typename Param0Type >
	class ClassFunction< ClassType, ReturnType, Param0Type, void, void, void >: public FunctionObject
	{
	public:
		typedef ReturnType( ClassType::*MyFunctionType )( Param0Type );

		MyFunctionType Function;
		ClassFunction( MyFunctionType f )
			:Function( f )
		{
			//
		}
		//
		Variant Invoke( Object object, ArgumentVector& args ) override
		{
			ClassType* instance = ( ClassType* )object;
			return Variant(( instance->*Function )( args[0].As<Param0Type>() ));
		}
	};
	//
	template< typename ClassType, typename ReturnType >
	class ClassFunction< ClassType, ReturnType, void, void, void, void >: public FunctionObject
	{
	public:
		typedef ReturnType( ClassType::*MyFunctionType )( void );

		MyFunctionType Function;
		ClassFunction( MyFunctionType f )
			:Function( f )
		{
			//
		}
		//
		Variant Invoke( Object object, ArgumentVector& args = ArgumentVector() ) override
		{
			ClassType* instance = ( ClassType* )object;
			return Variant(( instance->*Function )());
		}
	};
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	template< typename ClassType, typename ReturnType >
	inline FunctionObject* Bind( ReturnType( ClassType::*Func )( void ) )
	{
		//std::cout<<"Binding Class function1: ReturnType, void\n";
		return new ClassFunction< ClassType, ReturnType, void, void, void, void >( Func );
	}
	//
	template< typename ClassType, typename ReturnType, typename ParamType0 >
	inline FunctionObject* Bind( ReturnType( ClassType::*Func )( ParamType0 ) )
	{
		//std::cout<<"Binding Class function1: ReturnType, Param0\n";
		return new ClassFunction< ClassType, ReturnType, ParamType0, void, void, void >( Func );
	}
	//
	template< typename ClassType, typename ReturnType, typename ParamType0, typename ParamType1 >
	inline FunctionObject* Bind( ReturnType( ClassType::*Func )( ParamType0, ParamType1 ) )
	{
		//std::cout<<"Binding Class function1: ReturnType, Param0, Param1\n";
		return new ClassFunction< ClassType, ReturnType, ParamType0, ParamType1, void, void >( Func );
	}
	//
	template< typename ClassType, typename ReturnType, typename ParamType0, typename ParamType1, typename ParamType2 >
	inline FunctionObject* Bind( ReturnType( ClassType::*Func )( ParamType0, ParamType1, ParamType2 ) )
	{
		//std::cout<<"Binding Class function1: ReturnType, Param0, Param1, Param2\n";
		return new ClassFunction< ClassType, ReturnType, ParamType0, ParamType1, ParamType2, void >( Func );
	}
	//
	template< typename ClassType, typename ReturnType, typename ParamType0, typename ParamType1, typename ParamType2, typename ParamType3 >
	inline FunctionObject* Bind( ReturnType( ClassType::*Func )( ParamType0, ParamType1, ParamType2, ParamType3 ) )
	{
		//std::cout<<"Binding Class function1: ReturnType, Param0, Param1, Param2, Param3\n";
		return new ClassFunction< ClassType, ReturnType, ParamType0, ParamType1, ParamType2, ParamType3 >( Func );
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*
	template < typename F >
	struct Wrapper
	{
		//
	};
	//
	template < typename ParamType0 >
	struct Wrapper< void( ParamType0 ) >
	{
		static void Wrap( void ( *Func )( ParamType0 ), ArgumentVector& args )
		{
			std::cout<<"Wrapping Simple void function\n";
			( *Func )( args[0].As< ParamType0 >() );
		}
	};
	//
	template < typename RetType, typename ParamType0 >
	struct Wrapper< RetType( ParamType0 ) >
	{
		static RetType Wrap( RetType ( *Func )( ParamType0 ), ArgumentVector& args )
		{
			std::cout<<"Wrapping Simple retType function\n";
			RetType r =  Func( args[0].As< ParamType0 >() );
			return r;
		}
	};
	*/
	//
	/*
	template < typename ClassType, typename ParamType0 >
	struct Wrapper< void( ClassType::*)( ParamType0 ) >
	{
		static void Wrap( Object object, void( ClassType::*Func )( ParamType0 ), ArgumentVector& args )
		{
			std::cout<<"Wrapping Class void function\n";
			ClassType* instance = ( ClassType* )object;
			( instance->*Func )( args[0].As< ParamType0 >() );
		}
	};
	*/
	/*
	template < typename RetType, typename ClassType, typename ParamType0 >
	struct Wrapper< RetType( ClassType::*)( ParamType0 ) >
	{
		static RetType Wrap( Object object, RetType( ClassType::*Func )( ParamType0 ), ArgumentVector& args )
		{
			std::cout<<"Wrapping Class RetType function\n";
			ClassType* instance = ( ClassType* )object;
			return ( instance->*Func )( args[0].As< ParamType0 >() );
		}
	};

	*/
}