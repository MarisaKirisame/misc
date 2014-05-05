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
#include <iostream>
#include <type_traits>
#define DECLARE_NAME( NAME ) struct NAME ## _tag{ }
#define DEFINE_MULTIPLY_UNIT( t1, t2, t3 ) typedef multiply< t1, t2 >::type t3
#define DEFINE_DIVIDE_UNIT( t1, t2, t3 ) typedef divide< t1, t2 >::type t3
#define DECLARE_POSSIBLE_MEMBER_VARIABLE( NAME ) \
template< typename T, typename SELF > \
constexpr static bool have_member_variable( \
	typename std::enable_if \
	< \
		std::is_same \
		< \
			T, \
			NAME ## _tag \
		>::value && \
		std::is_member_object_pointer< decltype( & SELF::NAME ) >::value, \
		decltype( construct< SELF * >( )->NAME ) \
	>::type * ) \
{ return true; } \
template< typename T, typename SELF > \
static decltype( construct< SELF * >( )->NAME ) get_member_variable( \
	typename std::enable_if \
	< \
		std::is_same< T, NAME ## _tag >::value \
	>::type * )
#define DECLARE_POSSIBLE_STATIC_VARIABLE( NAME ) \
template< typename T, typename SELF > \
constexpr static bool have_static_variable( \
	typename std::enable_if \
	< \
		std::is_same \
		< \
			T, \
			NAME ## _tag \
		>::value, \
		decltype( SELF::NAME ) \
	>::type * ) \
{ return true; } \
template< typename T, typename SELF > \
static decltype( SELF::NAME ) get_static_variable( \
	typename std::enable_if \
	< \
		std::is_same< T, NAME ## _tag >::value \
	>::type * )
template< typename T >
T construct( );
#define DECLARE_POSSIBLE_MEMBER_FUNCTION( NAME ) \
	template< typename T, typename SELF, typename ... R > \
	constexpr static bool have_member_function( \
		typename std::enable_if \
		< \
			std::is_same \
			< \
				T, \
				NAME ## _tag \
			>::value && \
			std::is_member_function_pointer< decltype( & SELF::NAME ) >::value, \
			typename std::add_pointer \
			< \
				decltype( construct< SELF * >( )->NAME( construct< R >( ) ... ) ) \
			>::type \
		>::type ) \
	{ return true; } \
	template< typename T, typename SELF, typename ... R > \
	typename std::enable_if \
	< \
		std::is_same \
		< \
			T, \
			NAME ## _tag \
		>::value, \
		decltype( construct< SELF * >( )->NAME( construct< R >( ) ... ) ) \
	>::type \
	call_member_function( const R & ...  r ) { return NAME( r ... ); }
#define DECLARE_POSSIBLE_STATIC_FUNCTION( NAME ) \
	template< typename T, typename ... R > \
	constexpr static bool have_static_function( \
		typename std::enable_if \
		< \
			std::is_same \
			< \
				T, \
				NAME ## _tag \
			>::value, \
			typename std::add_pointer \
			< \
				decltype( NAME( construct< R >( ) ... ) ) \
			>::type \
		>::type ) \
	{ return true; } \
	template< typename T, typename ... R > \
	typename std::enable_if \
	< \
		std::is_same \
		< \
			T, \
			NAME ## _tag \
		>::value, \
		decltype( NAME( construct< R >( ) ... ) ) \
	>::type \
	static call_static_function( const R & ...  r ) { return NAME( r ... ); }
#define DECLARE_TYPE( TYPE ) \
	template< typename ... > \
	constexpr static bool have_member_function( ... ) { return false; } \
	template< typename ... > \
	constexpr static bool have_static_function( ... ) { return false; } \
	template< typename ... > \
	constexpr static bool have_member_variable( ... ) { return false; } \
	template< typename ... > \
	constexpr static bool have_static_variable( ... ) { return false; } \
	static TYPE * get_this( )
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
		static constexpr bool value = SFINAE( construct< T >( ) ).value;
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
	{ return last_argument( r ... ); }
	template< typename T >
	struct CPS
	{
		T t;
		template< typename ... R >
		void operator ( )( const R & ... r )
		{ rem_first( last_argument( r ... ), r ... ); }
		template< typename K >
		void operator ( )( const K & k ) { k( t( ) ); }
		struct loop_cycle_end_tag{ };
		template< typename K, typename ... R >
		void rem_first( const K & k, const R & ... r )
		{ rem_first_inner_loop( k, r ..., loop_cycle_end_tag( ) ); }
		template< typename K, typename ARG1, typename ... R >
		void rem_first_inner_loop( const K & k, const ARG1 & a, const R & ... r )
		{ rem_first_inner_loop( k, r ..., a ); }
		template< typename K, typename ARG1, typename ... R >
		void rem_first_inner_loop( const K & k, const ARG1 &, const loop_cycle_end_tag &, const R & ... r )
		{ k( t( r ... ) ); }
		template< typename K, typename ARG1 >
		void rem_first( const K & k, const ARG1 & a ) { k( a ); }
		CPS( const T & t ) : t( t ) { }
		CPS( T && t ) : t( std::move( t ) ) { }
	};
	template< typename T >
	CPS< T > make_CPS( const T & t ) { return CPS< T >( t ); }
	template< typename T >
	CPS< T > make_CPS( T && t ) { return CPS< T >( std::move( t ) ); }
	DECLARE_NAME( data );
	DECLARE_NAME( cache );
	DECLARE_NAME( func );
	DECLARE_NAME( function );
	DECLARE_NAME( foo );
	struct test
	{
		DECLARE_TYPE( test );
		int data;
		DECLARE_POSSIBLE_MEMBER_VARIABLE( data );
		DECLARE_POSSIBLE_MEMBER_VARIABLE( cache );
		void func( int ) { }
		DECLARE_POSSIBLE_MEMBER_FUNCTION( func );
		static int function( );
		DECLARE_POSSIBLE_STATIC_FUNCTION( function );
		DECLARE_POSSIBLE_MEMBER_FUNCTION( foo );
		DECLARE_POSSIBLE_STATIC_FUNCTION( foo );
		static void * cache;
		DECLARE_POSSIBLE_STATIC_VARIABLE( cache );
	};
	template< typename TYPE, typename NAME >
	struct have_member_variable { static constexpr bool value = TYPE::template have_member_variable< NAME, TYPE >( nullptr ); };
	template< typename TYPE, typename NAME >
	struct member_variable_type { typedef decltype( TYPE::template get_member_variable< NAME, TYPE >( nullptr ) ) type; };
	template< typename TYPE, typename NAME >
	struct have_static_variable { static constexpr bool value = TYPE::template have_static_variable< NAME, TYPE >( nullptr ); };
	template< typename TYPE, typename NAME >
	struct static_variable_type { typedef decltype( TYPE::template get_static_variable< NAME, TYPE >( nullptr ) ) type; };
	template< typename TYPE, typename NAME, typename ... ARG >
	struct have_member_function { static constexpr bool value = TYPE::template have_member_function< NAME, TYPE, ARG ... >( nullptr ); };
	template< typename TYPE, typename NAME, typename ... ARG >
	struct member_function_return_type
	{
		typedef decltype(
				construct< TYPE * >( )->template call_member_function< NAME, TYPE >(
					construct< ARG >( ) ... ) ) type;
	};
	template< typename TYPE, typename NAME, typename ... ARG >
	struct static_function_return_type
	{
		typedef decltype(
				TYPE::template call_static_function< NAME >(
					construct< ARG >( ) ... ) ) type;
	};
	template< typename TYPE, typename NAME, typename ... ARG >
	struct have_static_function { static constexpr bool value = TYPE::template have_static_function< NAME, ARG ... >( nullptr ); };
	static_assert( have_member_variable< test, data_tag >::value, "" );
	static_assert( ! have_member_variable< test, cache_tag >::value, "" );
	static_assert( std::is_same< member_variable_type< test, data_tag >::type, int >::value, "" );
	static_assert( have_member_function< test, func_tag, long >::value, "" );
	static_assert( ! have_member_function< test, func_tag, void * >::value, "" );
	static_assert( std::is_same< member_function_return_type< test, func_tag, long >::type, void >::value, "" );
	static_assert( ! have_static_function< test, func_tag >::value, "" );
	static_assert( have_static_function< test, function_tag >::value, "" );
	static_assert( ! have_static_function< test, func_tag >::value, "" );
	static_assert( std::is_same< static_function_return_type< test, function_tag >::type, int >::value, "" );
	static_assert( ! have_member_function< test, foo_tag >::value, "" );
	static_assert( ! have_static_function< test, foo_tag >::value, "" );
	static_assert( ! have_member_function< test, function_tag >::value, "" );
	static_assert( have_static_variable< test, cache_tag >::value, "" );
	static_assert( ! have_static_variable< test, data_tag >::value, "" );
	static_assert( have_static_variable< test, cache_tag >::value, "" );
	static_assert( std::is_same< static_variable_type< test, cache_tag >::type, void * >::value, "" );
}
#endif //MISC_HPP
