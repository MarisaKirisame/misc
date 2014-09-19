#ifndef EXPANSION_HPP
#define EXPANSION_HPP
#include "can_call.hpp"
namespace misc
{
	template< typename T1, typename T2 >
	expansion< T1, T2 > make_expansion( const T1 & t1, const T2 & t2 ) { return expansion< T1, T2 >( t1, t2 ); }
	template< typename T1, typename T2, bool b1 = can_call< T1 >::value, bool b2 = can_call< T2 >::value >
	struct expansion;
	template< typename T1, typename T2 >
	struct expansion< T1, T2, false, false >
	{
		T1 first;
		T2 second;
		template< typename T, typename ... R >
		decltype( first( std::declval< T >( ), std::declval< R >( )... ) ) operator ( )( const T & t, const R & ... r ) const { return first( t, r ... ); }
		template< typename ... R >
		decltype( second( std::declval< R >( )... ) ) operator ( )( const R & ... r ) const { return second( r ... ); }
		expansion( const T1 & t1, const T2 & t2 ) : first( t1 ), second( t2 ) { }
	};
	template< typename T1, typename T2 >
	struct expansion< T1, T2, false, true >
	{
		T1 first;
		T2 second;
		decltype( std::declval< T2 >( )( ) ) operator ( )( ) const { return second( ); }
		template< typename T, typename ... R >
		decltype( first( std::declval< T >( ), std::declval< R >( )... ) ) operator ( )( const T & t, const R & ... r ) const { return first( t, r ... ); }
		template< typename ... R >
		decltype( second( std::declval< R >( )... ) ) operator ( )( const R & ... r ) const { return second( r ... ); }
		expansion( const T1 & t1, const T2 & t2 ) : first( t1 ), second( t2 ) { }
	};
	template< typename T1, typename T2, bool b >
	struct expansion< T1, T2, true, b >
	{
		T1 first;
		T2 second;
		decltype( std::declval< T1 >( )( ) ) operator ( )( ) const { return first( ); }
		template< typename T, typename ... R >
		decltype( first( std::declval< T >( ), std::declval< R >( )... ) ) operator ( )( const T & t, const R & ... r ) const { return first( t, r ... ); }
		template< typename ... R >
		decltype( second( std::declval< R >( )... ) ) operator ( )( const R & ... r ) const { return second( r ... ); }
		expansion( const T1 & t1, const T2 & t2 ) : first( t1 ), second( t2 ) { }
	};
}
#endif // EXPANSION_HPP
