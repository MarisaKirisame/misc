#ifndef MISC_HPP
#define MISC_HPP
#include <iterator>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/same_as.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <array>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/range/join.hpp>
#include <boost/range/iterator_range.hpp>
#include <memory>
#include <boost/mpl/vector_c.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/plus.hpp>
#include <boost/mpl/arg.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/minus.hpp>
#include <boost/mpl/apply.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/tag.hpp>
#include <boost/mpl/back_inserter.hpp>
#include <boost/range/rbegin.hpp>
#include <random>
#include <boost/iterator/filter_iterator.hpp>
namespace misc
{
	using namespace boost::mpl::placeholders;
	typedef boost::mpl::vector_c< int, 1, 0, 0, 0, 0, 0, 0 > mass;
	typedef boost::mpl::vector_c< int, 0, 1, 0, 0, 0, 0, 0 > time;
	typedef boost::mpl::vector_c< int, 0, 0, 1, 0, 0, 0, 0 > length;
	typedef boost::mpl::vector_c< int, 0, 0, 0, 1, 0, 0, 0 > temperature;
	typedef boost::mpl::vector_c< int, 0, 0, 0, 0, 1, 0, 0 > light_intensity;
	typedef boost::mpl::vector_c< int, 0, 0, 0, 0, 0, 1, 0 > charge;
	typedef boost::mpl::vector_c< int, 0, 0, 0, 0, 0, 0, 1 > substance_quantity;
	typedef boost::mpl::vector_c< int, 0, 0, 0, 0, 0, 0, 0 > ratio;
	template< typename t1, typename t2 >
	struct multiply :
			boost::mpl::transform
			<
				t1,
				t2,
				boost::mpl::plus< _1, _2 >
			>
	{ };
	template< typename t1, typename t2 >
	struct divide : boost::mpl::transform
			<
				t1,
				t2,
				boost::mpl::minus< _1, _2 >
			>
	{ };
#define DEFINE_MULTIPLY_UNIT( t1, t2, t3 ) typedef multiply< t1, t2 >::type t3
#define DEFINE_DIVIDE_UNIT( t1, t2, t3 ) typedef divide< t1, t2 >::type t3
	DEFINE_DIVIDE_UNIT( length, time, velocity );
	DEFINE_DIVIDE_UNIT( velocity, time, accleration );
	DEFINE_MULTIPLY_UNIT( mass, accleration, force );
	DEFINE_MULTIPLY_UNIT( length, length, area );
	DEFINE_MULTIPLY_UNIT( length, area, volume );
	DEFINE_DIVIDE_UNIT( force, area, pressure );
	typedef time period;
	DEFINE_DIVIDE_UNIT( ratio, period, frequency );
	typedef length distance;
	DEFINE_MULTIPLY_UNIT( force, distance, work );
	typedef work energy;
	DEFINE_MULTIPLY_UNIT( mass, velocity, momentum );
	DEFINE_MULTIPLY_UNIT( force, time, impluse );
	DEFINE_DIVIDE_UNIT( mass, volume, density );
	typedef divide< divide< work, mass >::type, temperature >::type specific_heat;
	template< typename T >
	T construct( );
	template< typename T, typename ... R >
	struct can_call
	{
		template< typename t >
		static boost::mpl::true_ SFINAE( t tt, typename boost::remove_reference< decltype( tt( construct< R >( )... ) ) >::type * = nullptr )
		{ return boost::mpl::true_( ); }
		static boost::mpl::false_ SFINAE( ... ) { return boost::mpl::false_( ); }
		static constexpr bool value = decltype( SFINAE( construct< T >( ) ) )::value;
	};

	template< typename T1, typename T2, bool b1 = can_call< T1 >::value, bool b2 = can_call< T2 >::value >
	struct expansion;
	template< typename T1, typename T2 >
	struct expansion< T1, T2, false, false >
	{
		T1 first;
		T2 second;
		template< typename T, typename ... R >
		decltype( first( construct< T >( ), construct< R >( )... ) ) operator ( )( const T & t, const R & ... r ) { return first( t, r ... ); }
		template< typename ... R >
		decltype( second( construct< R >( )... ) ) operator ( )( const R & ... r ) { return second( r ... ); }
		expansion( const T1 & t1, const T2 & t2 ) : first( t1 ), second( t2 ) { }
	};
	template< typename T1, typename T2 >
	struct expansion< T1, T2, false, true >
	{
		T1 first;
		T2 second;
		decltype( construct< T2 >( )( ) ) operator ( )( ) { return second( ); }
		template< typename T, typename ... R >
		decltype( first( construct< T >( ), construct< R >( )... ) ) operator ( )( const T & t, const R & ... r ) { return first( t, r ... ); }
		template< typename ... R >
		decltype( second( construct< R >( )... ) ) operator ( )( const R & ... r ) { return second( r ... ); }
		expansion( const T1 & t1, const T2 & t2 ) : first( t1 ), second( t2 ) { }
	};
	template< typename T1, typename T2, bool b >
	struct expansion< T1, T2, true, b >
	{
		T1 first;
		T2 second;
		decltype( construct< T1 >( )( ) ) operator ( )( ) { return first( ); }
		template< typename T, typename ... R >
		decltype( first( construct< T >( ), construct< R >( )... ) ) operator ( )( const T & t, const R & ... r ) { return first( t, r ... ); }
		template< typename ... R >
		decltype( second( construct< R >( )... ) ) operator ( )( const R & ... r ) { return second( r ... ); }
		expansion( const T1 & t1, const T2 & t2 ) : first( t1 ), second( t2 ) { }
	};

	template< typename T1, typename T2 >
	expansion< T1, T2 > make_expansion( const T1 & t1, const T2 & t2 )
	{
		return expansion< T1, T2 >( t1, t2 );
	}

	struct non_returnable
	{
		non_returnable( non_returnable && ) = delete;
		non_returnable( const non_returnable & ) = delete;
		non_returnable( ) = delete;
	};

	template< typename T1, typename T2, bool b1 = can_call< T1 >::value, bool b2 = can_call< T2 >::value >
	struct restriction;
	template< typename T1, typename T2 >
	struct restriction< T1, T2, false, false >
	{
		T1 first;
		T2 second;
		template< typename ... R >
		typename boost::mpl::if_< can_call< T2, R ... >, non_returnable, decltype( first( construct< R >( )... ) ) >::type operator ( )( const R & ... r ) { return first( r ... ); }
		restriction( const T1 & t1, const T2 & t2 ) : first( t1 ), second( t2 ) { }
	};
	template< typename T1, typename T2 >
	struct restriction< T1, T2, false, true >
	{
		T1 first;
		T2 second;
		template< typename ... R >
		typename boost::mpl::if_< can_call< T2, R ... >, non_returnable, decltype( first( construct< R >( )... ) ) >::type operator ( )( const R & ... r ) { return first( r ... ); }
		restriction( const T1 & t1, const T2 & t2 ) : first( t1 ), second( t2 ) { }
	};
	template< typename T1, typename T2 >
	struct restriction< T1, T2, true, false >
	{
		T1 first;
		T2 second;
		decltype( construct< T1 >( )( ) ) operator ( )( ) { return first( ); }
		template< typename ... R >
		typename boost::mpl::if_< can_call< T2, R ... >, non_returnable, decltype( first( construct< R >( )... ) ) >::type operator ( )( const R & ... r ) { return first( r ... ); }
		restriction( const T1 & t1, const T2 & t2 ) : first( t1 ), second( t2 ) { }
	};
	template< typename T1, typename T2 >
	struct restriction< T1, T2, true, true >
	{
		T1 first;
		T2 second;
		template< typename ... R >
		typename boost::mpl::if_< can_call< T2, R ... >, non_returnable, decltype( first( construct< R >( )... ) ) >::type operator ( )( const R & ... r ) { return first( r ... ); }
		restriction( const T1 & t1, const T2 & t2 ) : first( t1 ), second( t2 ) { }
	};
	template< typename T1, typename T2 >
	restriction< T1, T2 > make_restriction( const T1 & t1, const T2 & t2 )
	{
		return restriction< T1, T2 >( t1, t2 );
	}
	template< typename ... R >
	struct last_template_argument;
	template< typename T >
	struct last_template_argument< T > { typedef T type; };
	template< typename T, typename ... R >
	struct last_template_argument< T, R ... > { typedef typename last_template_argument< R ... >::type type; };
	template< typename T >
	T last_argument( const T & t ) { return t; }
	template< typename T, typename ... R >
	typename last_template_argument< R ... >::type last_argument( const T &, const R & ... r )
	{ return last_argument( std::forward< const R & >( r ) ... ); }
	template< typename T >
	struct CPS
	{
		T t;
		template< typename ... R >
		void operator ( )( const R & ... r )
		{ rem_first( last_argument( r ... ), std::forward< const R & >( r ) ... ); }
		template< typename TT >
		void operator ( )( const TT & tt ) { tt( t( ) ); }
		struct loop_cycle_tag{ };
		template< typename TT, typename ... R >
		void rem_first( const TT & cb, const R & ... r )
		{ rem_first_inner_loop( cb, std::forward< const R & >( r ) ..., loop_cycle_tag( ) ); }
		template< typename TT, typename ARG1, typename ... R >
		void rem_first_inner_loop( const TT & tt, const ARG1 & a, const R & ... r )
		{ rem_first_inner_loop( tt, std::forward< const R & >( r ) ..., a ); }
		template< typename TT, typename ARG1, typename ... R >
		void rem_first_inner_loop( const TT & tt, const ARG1 &, const loop_cycle_tag &, const R & ... r )
		{ tt( t( std::forward< const R & >( r ) ... ) ); }
		template< typename TT, typename ARG1 >
		void rem_first( const TT & cb, const ARG1 & a ) { cb( a ); }
		CPS( const T & t ) : t( t ) { }
		CPS( T && t ) : t( std::move( t ) ) { }
	};
	template< typename T >
	CPS< T > make_CPS( const T & t ) { return CPS< T >( t ); }
	template< typename T >
	CPS< T > make_CPS( T && t ) { return CPS< T >( std::move( t ) ); }
}
#endif //MISC_HPP
