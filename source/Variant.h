#pragma once
//
namespace utility
{
	struct Variant
	{
		union
		{
			int		IntVal;
			long	LongVal;
			float	FloatVal;
			double	DoubleVal;
			void*	Pointer;
		};
		//
		template< typename type >
		Variant( const type& value )
		{
			As< type >() = value;
		}
		//
		Variant( void )
		{
			//
		}
		//
		template< typename type >
		type& As()
		{
			return *( (type*) &IntVal );
		}
	};
	//
	typedef void* Object;
}