#ifndef MISC_HPP
#define MISC_HPP
#include <boost/mpl/size.hpp>
#include <boost/mpl/vector.hpp>
#include <iterator>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/same_as.hpp>
#include <boost/mpl/placeholders.hpp>
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
#include <boost/preprocessor.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/tuple/to_seq.hpp>
#include <is_empty_tuple.hpp>
#define MISC_NAME_SEQ (data)(cache)(func)(function)(foo)(bar)
#define DECLARE_NAME( NAME ) struct BOOST_PP_CAT( NAME, _tag ){ }
#define DECLARE_NAMES_HELPER( R, DATA, ELEMENT ) DECLARE_NAME( ELEMENT );
#define DECLARE_NAMES( NAME_SEQ ) BOOST_PP_SEQ_FOR_EACH( DECLARE_NAMES_HELPER, _, NAME_SEQ )
#define DEFINE_MULTIPLY_UNIT( t1, t2, t3 ) typedef multiply< t1, t2 >::type t3
#define DEFINE_DIVIDE_UNIT( t1, t2, t3 ) typedef divide< t1, t2 >::type t3
#define DECLARE_POSSIBLE_MEMBER_VARIABLE( NAME ) \
template< typename T, typename SELF > \
constexpr static bool has_member_variable( \
	typename std::enable_if \
	< \
		std::is_same \
		< \
			T, \
			BOOST_PP_CAT( NAME, _tag ) \
		>::value && \
		std::is_member_object_pointer< decltype( & SELF::NAME ) >::value, \
		decltype( construct< SELF * >( )->NAME ) \
	>::type * ) \
	{ return true; } \
	template< typename T, typename SELF > \
	typename enable_if_valid \
	< \
		typename std::enable_if \
		< \
			std::is_same \
			< \
				T, \
				BOOST_PP_CAT( NAME, _tag ) \
			>::value \
		>::type, \
		decltype( construct< SELF * >( )->NAME ) \
	>::type get_member_variable( ) { return static_cast< SELF * >( this )->NAME; } \
	template< typename T, typename SELF, typename K > \
	typename std::enable_if \
	< \
		std::is_same \
		< \
			T, \
			BOOST_PP_CAT( NAME, _tag ) \
		>::value && has_member_variable< T, SELF >( nullptr ), \
	typename enable_if_valid< decltype( construct< SELF * >( )->NAME ) >::type \
	>::type invoke_member_variable( const K & k ) { k( static_cast< SELF * >( this )->NAME ); }
#define DECLARE_POSSIBLE_STATIC_VARIABLE( NAME ) \
template< typename T, typename SELF > \
constexpr static bool has_static_variable( \
	typename std::enable_if \
	< \
		std::is_same \
		< \
			T, \
			BOOST_PP_CAT( NAME, _tag ) \
		>::value && \
		! std::is_member_object_pointer< decltype( & SELF::NAME ) >::value \
	>::type * ) \
	{ return true; } \
	template< typename T, typename SELF > \
	static typename enable_if_valid \
	< \
		typename std::enable_if \
		< \
			std::is_same \
			< \
				T, \
				BOOST_PP_CAT( NAME, _tag ) \
			>::value \
		>::type, \
		decltype( SELF::NAME ) \
	>::type get_static_variable( ) { return SELF::NAME; }
#define DECLARE_POSSIBLE_MEMBER_FUNCTION( NAME ) \
	template< typename T, typename SELF, typename ... R > \
	constexpr static bool has_member_function( \
		typename std::enable_if \
		< \
			std::is_same \
			< \
				T, \
				BOOST_PP_CAT( NAME, _tag ) \
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
			BOOST_PP_CAT( NAME, _tag ) \
		>::value, \
		decltype( construct< SELF * >( )->NAME( construct< R >( ) ... ) ) \
	>::type \
	call_member_function( const R & ...  r ) { return NAME( r ... ); } \
	template< typename T, typename SELF, typename ... R > \
	static typename std::enable_if \
	< \
		std::is_same \
		< \
			T, \
			BOOST_PP_CAT( NAME, _tag ) \
		>::value, \
		decltype( construct< SELF * >( )->NAME( construct< R >( ) ... ) ) \
	>::type \
	call_member_function( SELF * t, const R & ...  r ) { return t->NAME( r ... ); }
#define DECLARE_POSSIBLE_STATIC_FUNCTION( NAME ) \
	template< typename T, typename SELF, typename ... R > \
	constexpr static bool has_static_function( \
		typename std::enable_if \
		< \
			std::is_same \
			< \
				T, \
				BOOST_PP_CAT( NAME, _tag ) \
			>::value, \
			typename std::add_pointer \
			< \
				decltype( SELF::NAME( construct< R >( ) ... ) ) \
			>::type \
		>::type ) \
	{ return true; } \
	template< typename T, typename SELF, typename ... R > \
	typename std::enable_if \
	< \
		std::is_same \
		< \
			T, \
			BOOST_PP_CAT( NAME, _tag ) \
		>::value, \
		decltype( SELF::NAME( construct< R >( ) ... ) ) \
	>::type \
	static call_static_function( const R & ...  r ) { return SELF::NAME( r ... ); }
#define INVOKE_ALL_MEMBER_VARIABLE_HELPER( R, DATA, ELEMENT ) \
	invoke_member_variable \
	< \
		BOOST_PP_CAT( ELEMENT, _tag ), \
		typename std::remove_reference< typename std::remove_cv< decltype( * this ) >::type >::type, \
		typename std::remove_cv< decltype( DATA ) >::type \
	>( DATA );
#define INVOKE_ALL_MEMBER_VARIABLE( K, NAME_SEQ ) \
	BOOST_PP_SEQ_FOR_EACH( INVOKE_ALL_MEMBER_VARIABLE_HELPER, K, NAME_SEQ )
#define DECLARE_TYPE( TYPE, NAME_SEQ ) \
	template< typename ... > \
	constexpr static bool has_member_function( ... ) { return false; } \
	template< typename ... > \
	constexpr static bool has_static_function( ... ) { return false; } \
	template< typename ... > \
	constexpr static bool has_member_variable( ... ) { return false; } \
	template< typename ... > \
	constexpr static bool has_static_variable( ... ) { return false; } \
	template< typename ... > \
	void invoke_member_variable( ... ) { } \
	template< typename K > \
	void invoke_all_member_variable( const K & k ) \
	{\
		INVOKE_ALL_MEMBER_VARIABLE( k, NAME_SEQ );\
	}\
	DECLARE_ALL_POSSIBLE( NAME_SEQ )
#define DECLARE_POSSIBLE( NAME ) \
	DECLARE_POSSIBLE_MEMBER_FUNCTION( NAME ); \
	DECLARE_POSSIBLE_MEMBER_VARIABLE( NAME ); \
	DECLARE_POSSIBLE_STATIC_FUNCTION( NAME ); \
	DECLARE_POSSIBLE_STATIC_VARIABLE( NAME )
#define DECLARE_ALL_POSSIBLE_HELPER( R, DATA, ELEMENT ) DECLARE_POSSIBLE( ELEMENT );
#define DECLARE_ALL_POSSIBLE( NAME_SEQ ) BOOST_PP_SEQ_FOR_EACH( DECLARE_ALL_POSSIBLE_HELPER, _, NAME_SEQ )
#define HAS_MEMBER_VARIABLE( TYPE, NAME ) has_member_variable< TYPE, BOOST_PP_CAT( NAME, _tag ) >::value
#define MEMBER_VARIABLE_TYPE( TYPE, NAME ) member_variable_type< TYPE, BOOST_PP_CAT( NAME, _tag ) >::type
#define HAS_STATIC_VARIABLE( TYPE, NAME ) has_static_variable< TYPE, BOOST_PP_CAT( NAME, _tag ) >::value
#define STATIC_VARIABLE_TYPE( TYPE, NAME ) static_variable_type< TYPE, BOOST_PP_CAT( NAME, _tag ) >::type
#define EXPAND_TUPLE_ARGUMENT_HELPER( R, DATA, ELEMENT ) , ELEMENT
#define EAT_ARG( ... )
#define EXPAND_TUPLE_ARGUMENT( ARGUMENT_TUPLE ) \
	BOOST_PP_IIF( \
		IS_EMPTY_TUPLE( ARGUMENT_TUPLE ), \
		EAT_ARG, \
		BOOST_PP_SEQ_FOR_EACH )\
		( \
				EXPAND_TUPLE_ARGUMENT_HELPER, \
				_, \
				BOOST_PP_TUPLE_TO_SEQ( ARGUMENT_TUPLE ) )
#define HAS_MEMBER_FUNCTION( TYPE, NAME, ARGUMENT_TUPLE ) \
	has_member_function \
	< \
		TYPE, \
		BOOST_PP_CAT( NAME, _tag ) \
		EXPAND_TUPLE_ARGUMENT( ARGUMENT_TUPLE ) \
	>::value
#define MEMBER_FUNCTION_RETURN_TYPE( TYPE, NAME, ARGUMENT_TUPLE ) \
	member_function_return_type \
	< \
		TYPE, \
		BOOST_PP_CAT( NAME, _tag ) \
		EXPAND_TUPLE_ARGUMENT( ARGUMENT_TUPLE ) \
	>::type
#define HAS_STATIC_FUNCTION( TYPE, NAME, ARGUMENT_TUPLE ) \
	has_static_function \
	< \
		TYPE, \
		BOOST_PP_CAT( NAME, _tag ) \
		EXPAND_TUPLE_ARGUMENT( ARGUMENT_TUPLE ) \
	>::value
#define STATIC_FUNCTION_RETURN_TYPE( TYPE, NAME, ARGUMENT_TUPLE ) \
	static_function_return_type \
	< \
		TYPE, \
		BOOST_PP_CAT( NAME, _tag ) \
		EXPAND_TUPLE_ARGUMENT( ARGUMENT_TUPLE ) \
	>::type
#define DECLTYPE_HELPER( D, DATA, ELEMENT ) decltype( ELEMENT )
#define CALL_STATIC_FUNCTION( TYPE, NAME, ARGUMENT_TUPLE ) \
	call_static_function \
	< \
		TYPE, \
		BOOST_PP_CAT( NAME, _tag ) \
		EXPAND_TUPLE_ARGUMENT( \
			BOOST_PP_SEQ_TO_TUPLE( \
				BOOST_PP_SEQ_TRANSFORM( \
					DECLTYPE_HELPER, \
					_, \
					BOOST_PP_TUPLE_TO_SEQ( ARGUMENT_TUPLE ) ) ) ) \
	>::function( BOOST_PP_TUPLE_ENUM( ARGUMENT_TUPLE ) )
#define CALL_MEMBER_FUNCTION( THIS, NAME, ARGUMENT_TUPLE ) \
	call_member_function \
	< \
		std::remove_pointer< decltype( THIS ) >::type, \
		BOOST_PP_CAT( NAME, _tag ) \
		EXPAND_TUPLE_ARGUMENT( \
			BOOST_PP_SEQ_TO_TUPLE( \
				BOOST_PP_SEQ_TRANSFORM( \
					DECLTYPE_HELPER, \
					_, \
					BOOST_PP_TUPLE_TO_SEQ( ARGUMENT_TUPLE ) ) ) ) \
	>::function( THIS, BOOST_PP_TUPLE_ENUM( ARGUMENT_TUPLE ) )
namespace misc
{
	template< typename T, typename RET = void >
	struct enable_if_valid { typedef RET type; };
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
	DECLARE_NAMES( MISC_NAME_SEQ )
	struct test
	{
		DECLARE_TYPE( test, MISC_NAME_SEQ );
		int data = 12450;
		double func( int ) { return 0; }
		static int function( ) { return 1; }
		static void * cache;
		static decltype( & function ) bar( long, long time ) { if ( time == 0 ) { } return & function; }
	};
	template< typename TYPE, typename NAME >
	struct has_member_variable { static constexpr bool value = TYPE::template has_member_variable< NAME, TYPE >( nullptr ); };
	template< typename TYPE, typename NAME >
	struct member_variable_type { typedef decltype( construct< TYPE * >( )->template get_member_variable< NAME, TYPE >( ) ) type; };
	template< typename TYPE, typename NAME >
	struct has_static_variable { static constexpr bool value = TYPE::template has_static_variable< NAME, TYPE >( nullptr ); };
	template< typename TYPE, typename NAME >
	struct static_variable_type { typedef decltype( TYPE::template get_static_variable< NAME, TYPE >( ) ) type; };
	template< typename TYPE, typename NAME, typename ... ARG >
	struct has_member_function { static constexpr bool value = TYPE::template has_member_function< NAME, TYPE, ARG ... >( nullptr ); };
	template< typename TYPE, typename NAME >
	struct has_member_function< TYPE, NAME, void > { static constexpr bool value = TYPE::template has_member_function< NAME, TYPE >( nullptr ); };
	template< typename TYPE, typename NAME, typename ... ARG >
	struct member_function_return_type
	{
		typedef decltype(
				construct< TYPE * >( )->template call_member_function< NAME, TYPE >(
					construct< ARG >( ) ... ) ) type;
	};
	template< typename TYPE, typename NAME >
	struct member_function_return_type< TYPE, NAME, void >
	{
		typedef decltype(
				construct< TYPE * >( )->template call_member_function< NAME, TYPE >( ) ) type;
	};
	template< typename TYPE, typename NAME, typename ... ARG >
	struct static_function_return_type
	{
		typedef decltype(
				TYPE::template call_static_function< NAME, TYPE >(
					construct< ARG >( ) ... ) ) type;
	};
	template< typename TYPE, typename NAME >
	struct static_function_return_type< TYPE, NAME, void >
	{
		typedef decltype(
				TYPE::template call_static_function< NAME, TYPE >( ) ) type;
	};
	template< typename TYPE, typename NAME, typename ... ARG >
	struct has_static_function { static constexpr bool value = TYPE::template has_static_function< NAME, TYPE, ARG ... >( nullptr ); };
	template< typename TYPE, typename NAME >
	struct has_static_function< TYPE, NAME, void > { static constexpr bool value = TYPE::template has_static_function< NAME, TYPE >( nullptr ); };	template< typename TYPE, typename NAME, typename ... ARG >
	struct call_static_function
	{
		static decltype( TYPE::template call_static_function< NAME, TYPE >( construct< ARG >( ) ... ) )
		function( const ARG & ... r )
		{ return TYPE::template call_static_function< NAME, TYPE >( r ... ); }
	};
	template< typename TYPE, typename NAME >
	struct call_static_function< TYPE, NAME, void >
	{
		static decltype( TYPE::template call_static_function< NAME, TYPE >( ) )
		function( )
		{ return TYPE::template call_static_function< NAME, TYPE >( ); }
	};
	template< typename TYPE, typename NAME, typename ... ARG >
	struct call_member_function
	{
		static decltype( TYPE::template call_member_function< NAME, TYPE >( construct< typename std::add_pointer< TYPE >::type >( ), construct< ARG >( ) ... ) )
		function( TYPE * t,const ARG & ... r )
		{ return TYPE::template call_member_function< NAME, TYPE >( t, r ... ); }
	};
	template< typename TYPE, typename NAME >
	struct call_member_function< TYPE, NAME, void >
	{
		static decltype( TYPE::template call_member_function< NAME, TYPE >( ) )
		function( TYPE * t )
		{ return TYPE::template call_member_function< NAME, TYPE >( t ); }
	};
	static_assert( HAS_MEMBER_FUNCTION( test, func, ( long ) ), "" );
	static_assert( ! HAS_MEMBER_FUNCTION( test, func, ( void * ) ), "" );
	static_assert( std::is_same< MEMBER_FUNCTION_RETURN_TYPE( test, func, ( double ) ), double >::value, "" );
	static_assert( ! HAS_STATIC_FUNCTION( test, func, ( void ) ), "" );
	static_assert( HAS_STATIC_FUNCTION( test, function, ( ) ), "" );
	static_assert( ! HAS_STATIC_FUNCTION( test, func, ( void ) ), "" );
	static_assert( std::is_same< STATIC_FUNCTION_RETURN_TYPE( test, function, ( void ) ), int >::value, "" );
	static_assert( ! HAS_MEMBER_FUNCTION( test, foo, ( void ) ), "" );
	static_assert( ! HAS_STATIC_FUNCTION( test, foo, ( ) ), "" );
	static_assert( HAS_STATIC_VARIABLE( test, cache ), "" );
	static_assert( ! HAS_STATIC_VARIABLE( test, data ), "" );
	static_assert( HAS_STATIC_VARIABLE( test, cache ), "" );
	static_assert( std::is_same< STATIC_VARIABLE_TYPE( test, cache ), void * >::value, "" );
	static_assert( HAS_STATIC_FUNCTION( test, bar, ( int, int ) ), "" );
	static_assert(
			std::is_same
			<
				decltype( CALL_STATIC_FUNCTION( test, bar, ( 0, 1 ) )( ) ),
				int
			>::value, "" );
	static_assert(
			std::is_same
			<
				decltype( CALL_MEMBER_FUNCTION( construct< test * >( ), func, ( 1 ) ) ),
				double
			>::value, "" );
	static_assert( HAS_MEMBER_VARIABLE( test, data ), "" );
	static_assert( ! HAS_MEMBER_VARIABLE( test, cache ), "" );
	static_assert( std::is_same< MEMBER_VARIABLE_TYPE( test, data ), int >::value, "" );
}
#endif //MISC_HPP
