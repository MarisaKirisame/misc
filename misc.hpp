#ifndef MISC_HPP
#define MISC_HPP
#include <boost/mpl/bind.hpp>
#include <array>
#include <algorithm>
#include <boost/mpl/size.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/same_as.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <cmath>
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
#include <iterator>
#include <type_traits>
#include <boost/preprocessor.hpp>
#include <algorithm>
#include <set>
#include <list>
#include <iterator>
#include <thread>
#define DEFINE_MULTIPLY_UNIT( t1, t2, t3 ) typedef multiply< t1, t2 >::type t3
#define DEFINE_DIVIDE_UNIT( t1, t2, t3 ) typedef divide< t1, t2 >::type t3
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
	struct divide :
			boost::mpl::transform
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
	template< typename T, typename ... R >
	struct can_call
	{
		template< typename t >
		static constexpr boost::mpl::true_ SFINAE( t, typename boost::remove_reference< decltype( std::declval< t >( )( std::declval< R >( )... ) ) >::type * = nullptr )
		{ return boost::mpl::true_( ); }
		static constexpr boost::mpl::false_ SFINAE( ... ) { return boost::mpl::false_( ); }
		static constexpr bool value = decltype( SFINAE( std::declval< T >( ) ) )::value;
	};
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
		typename boost::mpl::if_< can_call< T2, R ... >, non_returnable, decltype( first( std::declval< R >( )... ) ) >::type
		operator ( )( const R & ... r ) { return first( r ... ); }
		restriction( const T1 & t1, const T2 & t2 ) : first( t1 ), second( t2 ) { }
	};
	template< typename T1, typename T2 >
	struct restriction< T1, T2, false, true >
	{
		T1 first;
		T2 second;
		template< typename ... R >
		typename boost::mpl::if_< can_call< T2, R ... >, non_returnable, decltype( first( std::declval< R >( )... ) ) >::type
		operator ( )( const R & ... r ) { return first( r ... ); }
		restriction( const T1 & t1, const T2 & t2 ) : first( t1 ), second( t2 ) { }
	};
	template< typename T1, typename T2 >
	struct restriction< T1, T2, true, false >
	{
		T1 first;
		T2 second;
		decltype( std::declval< T1 >( )( ) ) operator ( )( ) { return first( ); }
		template< typename ... R >
		typename boost::mpl::if_< can_call< T2, R ... >, non_returnable, decltype( first( std::declval< R >( )... ) ) >::type
		operator ( )( const R & ... r ) { return first( r ... ); }
		restriction( const T1 & t1, const T2 & t2 ) : first( t1 ), second( t2 ) { }
	};
	template< typename T1, typename T2 >
	struct restriction< T1, T2, true, true >
	{
		T1 first;
		T2 second;
		template< typename ... R >
		typename boost::mpl::if_< can_call< T2, R ... >, non_returnable, decltype( first( std::declval< R >( )... ) ) >::type
		operator ( )( const R & ... r ) { return first( r ... ); }
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
	bool is_palindrome( const std::string & str )
	{ return std::equal( str.begin( ), str.end( ), str.rbegin( ) ); }
	bool can_be_palindrome( const std::string & str )
	{ return std::equal( str.begin( ), str.end( ), str.rbegin( ), []( char a, char b ){ return a == '*' || b == '*' || a == b; } ); }
	std::set< std::string > generate_palindrome( const std::string & str, const std::set< std::string > & dictionary )
	{
		if ( ! can_be_palindrome( str ) ) { return { }; }
		if ( str.find( '*' ) == str.npos )
		{
			if ( is_palindrome( str ) ) { return { str }; }
			else { return { }; }
		}
		else
		{
			if ( std::equal( str.begin( ), str.end( ), str.rbegin( ) ) )
			{
				std::set< std::string > ret;
				for ( const std::string & insert_key : dictionary )
				{
					size_t pos = -1;
					pos = str.find( std::string( insert_key.length( ), '*' ).c_str( ), pos + 1, insert_key.length( ) );
					if ( pos == str.npos ) { continue; }
					std::string new_string( str );
					std::copy( insert_key.begin( ), insert_key.end( ), new_string.begin( ) + pos );
					auto res = generate_palindrome( new_string, dictionary );
					std::copy( res.begin( ), res.end( ), std::insert_iterator< std::set< std::string > >( ret, ret.end( ) ) );
				}
				return ret;
			}
			else
			{
				auto i = str.begin( );
				auto ii = str.rbegin( );
				while ( i != str.end( ) )
				{
					if ( * i != * ii ) { goto http; }
					++i;
					++ii;
				}
				throw std::logic_error( "string is equalized. no need to equalize it." );
				http://stackoverflow.com
				if ( * i != '*' && * ii != '*' ) { return { }; }
				char c;
				if ( * i == '*' ) { c = * ii; }
				else
				{
					assert( * ii == '*' );
					c = * i;
				}
				std::set< std::string > ret;
				for ( const std::string & insert_key : dictionary )
				{
					size_t pos = -1;
					while ( true )
					{
						pos = insert_key.find_first_of( c, pos + 1 );
						if ( pos == str.npos ) { break; }
						if ( * i == '*' )
						{
							if ( static_cast< size_t >( std::distance( str.begin( ), i ) ) < pos ) { continue; }
							if ( static_cast< size_t >( std::distance( i - pos, str.end( ) ) ) < insert_key.size( ) ) { continue; }
							if ( std::all_of( i - pos, i - pos + insert_key.size( ), []( char ch ){ return ch == '*'; } ) )
							{
								std::string new_string( str );
								std::copy( insert_key.begin( ), insert_key.end( ), new_string.begin( ) + std::distance( str.begin( ), i ) - pos );
								auto res = generate_palindrome( new_string, dictionary );
								std::copy( res.begin( ), res.end( ), std::insert_iterator< std::set< std::string > >( ret, ret.end( ) ) );
							}
						}
						else
						{
							assert( * ii == '*' );
							if ( static_cast< size_t >( std::distance( ii, str.rend( ) ) - 1 ) < pos ) { continue; }
							if ( static_cast< size_t >( std::distance( str.rbegin( ), ii + pos ) + 1 ) < insert_key.size( ) ) { continue; }
							if ( std::all_of( ii + pos + 1 - insert_key.size( ), ii + pos + 1, []( char ch ){ return ch == '*'; } ) )
							{
								std::string new_string( str );
								std::copy(
											insert_key.begin( ),
											insert_key.end( ),
											new_string.begin( ) + ( new_string.size( ) - std::distance( str.begin( ), i ) - 1 ) - pos );
								auto res = generate_palindrome( new_string, dictionary );
								std::copy( res.begin( ), res.end( ), std::insert_iterator< std::set< std::string > >( ret, ret.end( ) ) );
							}
						}
					}
				}
				return ret;
			}
		}
	}
	std::set< std::string > generate_palindrome( size_t length, const std::set< std::string > & dictionary )
	{ return generate_palindrome( std::string( length, '*' ), dictionary ); }
	static const std::set< std::string > & test_dic( )
	{
		static std::set< std::string > ret =
		{
			"a",
			"man",
			"plan",
			"cat",
			"canal",
			"panama",
			"ham",
			"yat",
			"hat",
			"pasta",
			"canoe",
			"heros",
			"rajahs",
			"coloratura",
			"maps",
			"snipe",
			"percale",
			"macaroni",
			"gag",
			"banana",
			"bag",
			"tan",
			"tag",
			"again",
			"camel",
			"crepe",
			"pins",
			"spam",
			"rut",
			"rolo",
			"cash",
			"jar",
			"sore",
			"hats",
			"peon"
		};
		return ret;
	}
	void display( std::set< std::string > ** get )
	{
		std::list< std::set< std::string > > data;
		while ( true )
		{
			if ( * get != nullptr )
			{
				data.push_back( ** get );
				* get = nullptr;
			}
			if ( ! data.empty( ) )
			{
				auto & res = data.front( );
				for ( auto & i : res )
				{
					if ( * get != nullptr )
					{
						data.push_back( ** get );
						* get = nullptr;
					}
					std::cout << i << std::endl;
					std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
				}
				data.pop_front( );
			}
		}
	}
	void example( )
	{
		assert( is_palindrome( "amanaplanacatacanalpanama" ) );
		assert( ! is_palindrome( "this is not a palindrome" ) );
		std::set< std::string > * ptr = nullptr;
		std::thread t( display, & ptr );
		for ( int i = 0;; ++i )
		{
			auto res = generate_palindrome( i, test_dic( ) );
			ptr = & res;
			while ( ptr != nullptr ) { std::this_thread::yield( ); }
		}
	}
	template< bool ... >
	struct assert_bool;
	template< >
	struct assert_bool< >
	{
		template< bool b >
		struct apply{ typedef apply type; };
	};
	template< bool T >
	struct assert_bool< T >
	{
		template< bool b >
		struct apply
		{
			static_assert( b == T, "" );
			typedef apply type;
		};
	};
	template< typename T >
	constexpr const T & max( const T & a, const T & b ) { return a > b ? a : b; }
	template< typename T, typename ... ARG >
	constexpr const T & max( const T & a, const T & b, const ARG & ... c ) { return a > b ? max( a, c ... ) : max( b, c ... ); }
	template< size_t >
	struct variable { };
	template< typename lhs, typename rhs >
	struct addition
	{ typedef addition< lhs, rhs > type; };
	template< size_t v >
	struct constant
	{
		typedef constant< v > type;
		constexpr static size_t val = v;
	};
	template< size_t lhs, size_t rhs >
	struct addition< constant< lhs >, constant< rhs > >
	{ typedef typename constant< lhs + rhs >::type type; };
	template< typename T >
	struct addition< T, constant< 0 > > : T { };
	template< >
	struct addition< constant< 0 >, constant< 0 > > : constant< 0 > { };
	template< typename T >
	struct addition< constant< 0 >, T > : T { };
	template< typename T >
	struct negation { typedef negation< T > type; };
	template< typename T >
	struct negation< negation< T > > : T { };
	template< typename lhs, typename rhs >
	struct negation< addition< lhs, rhs > > :
			addition
			<
				typename negation< lhs >::type,
				typename negation< rhs >::type
			>::type { };
	template< typename T >
	struct addition< T, negation< T > > : constant< 0 >::type { };
	template< typename T >
	struct addition< negation< T >, T > : constant< 0 >::type { };
	template< typename lhs, typename rhs >
	struct addition< lhs, addition< rhs, rhs > >
	{
		typedef typename
		std::conditional
		<
			! std::is_same
			<
				typename decltype( lhs( ) + rhs( ) )::type,
				decltype( lhs( ) + rhs( ) )
			>::value,
			addition< typename addition< lhs, rhs >::type, rhs >,
			addition< lhs, addition< rhs, rhs > >
		>::type type;
	};
	template< typename lhs, typename rhs1, typename rhs2 >
	struct addition< lhs, addition< rhs1, rhs2 > >
	{
		typedef typename
		std::conditional
		<
			! std::is_same
			<
				typename decltype( lhs( ) + rhs1( ) )::type,
				addition< lhs, rhs1 >
			>::value,
			typename addition< typename addition< lhs, rhs1 >::type, rhs2 >::type,
			typename std::conditional
			<
				! std::is_same
				<
					typename decltype( lhs( ) + rhs2( ) )::type,
					addition< lhs, rhs2 >
				>::value,
				typename addition< typename addition< lhs, rhs2 >::type, rhs1 >::type,
				addition< lhs, typename addition< rhs1, rhs2 >::type >
			>::type
		>::type type;
	};
	template< typename lhs, typename rhs >
	struct addition< constant< 0 >, addition< lhs, rhs > > : addition< lhs, rhs > { };
	template< typename rhs >
	struct addition< constant< 0 >, addition< rhs, rhs > > : addition< rhs, rhs > { };
	template< size_t s >
	struct addition< constant< s >, constant< 0 > > : constant< s >{ };
	template< size_t s >
	struct addition< constant< 0 >, constant< s > > : constant< s >{ };
	template< >
	struct negation< constant< 0 > > : constant< 0 > { };
	template< typename lhs, typename rhs >
	constexpr auto operator + ( const lhs &, const rhs & ) { return typename addition< lhs, rhs >::type( ); }
	template< typename lhs, typename rhs >
	constexpr auto operator - ( const lhs & l, const rhs & ) { return l + typename negation< rhs >::type( ); }
	template< typename T >
	struct reciprocal { };
	template< typename lhs, typename rhs >
	struct multiplication { typedef multiplication< lhs, rhs > type; };
	template< typename T >
	struct multiplication< T, constant< 0 > > { typedef constant< 0 > type; };
	template< typename T >
	struct multiplication< constant< 0 >, T > { typedef constant< 0 > type; };
	template< typename T >
	struct multiplication< constant< 1 >, T > { typedef T type; };
	template< typename lhs, typename rhs1, typename rhs2 >
	struct multiplication< lhs, addition< rhs1, rhs2 > > :
			addition
			<
				typename multiplication< lhs, rhs1 >::type,
				typename multiplication< lhs, rhs2 >::type
			> { };
	template< typename lhs, typename rhs1, typename rhs2 >
	struct multiplication< addition< rhs1, rhs2 >, lhs > : multiplication< lhs, addition< rhs1, rhs2 > >::type { };
	template< typename lhs, size_t rhs >
	struct multiplication< constant< rhs >, lhs > : multiplication< lhs, constant< rhs > >::type { };
	template< typename lhs, typename rhs >
	constexpr auto operator * ( const lhs &, const rhs & ) { return typename multiplication< lhs, rhs >::type( ); }
	template< typename lhs, typename rhs >
	constexpr auto operator / ( const lhs & l, const rhs & ) { return l * reciprocal< rhs >( ); }
	template< typename T >
	constexpr bool can_decompose( const T & ) { return true; }
	template< size_t s >
	constexpr bool can_decompose( const constant< s > & ) { return false; }
	template< typename T, typename V >
	struct differentiate
	{
		static_assert( ! can_decompose( T( ) ), "" );
		static_assert( ! can_decompose( V( ) ), "" );
		typedef constant< 0 > type;
	};
	template< typename lhs, typename rhs, typename V >
	struct differentiate< addition< lhs, rhs >, V >
	{
		typedef typename
		addition
		<
			typename differentiate< lhs, V >::type,
			typename differentiate< rhs, V >::type
		>::type type;
	};
	template< typename T, typename V >
	struct differentiate< negation< T >, V > { typedef typename negation< typename differentiate< T, V >::type >::type type; };
	template< typename lhs, typename rhs, typename V >
	struct differentiate< multiplication< lhs, rhs >, V >
	{
		typedef typename
		addition
		<
			typename multiplication
			<
				typename differentiate< lhs, V >::type,
				rhs
			>::type,
			typename multiplication
			<
				typename differentiate< rhs, V >::type,
				lhs
			>::type
		>::type type;
	};
	template< typename T, typename V >
	struct differentiate< reciprocal< T >, V >
	{
		typedef typename
		negation
		<
			typename decltype(
				typename differentiate< T, V >::type( ) /
				( T( ) * T( ) ) )::type
		>::type type;
	};
	template< typename T >
	struct differentiate< T, T > { typedef constant< 1 >::type type; };
	template< typename T >
	constexpr bool has_reciprocal( const T & ) { return false; }
	template< typename T >
	struct remove_reciprocal_by_multiply_nonzero
	{
		typedef T type;
	};
	template< typename T, typename div >
	struct get_remainder
	{
		typedef typename
		std::conditional
		<
			std::is_same< T, div >::value,
			constant< 0 >,
			T
		>::type type;
	};
	template< typename lhs, typename rhs, typename div >
	struct get_remainder< addition< lhs, rhs >, div > :
			addition
			<
				typename get_remainder< lhs, div >::type,
				typename get_remainder< rhs, div >::type
			> { };
	template< typename lhs, typename rhs, typename div >
	struct get_remainder< multiplication< lhs, rhs >, div > :
			multiplication
			<
				typename get_remainder< lhs, div >::type,
				typename get_remainder< rhs, div >::type
			> { };
	template< typename lhs, typename rhs >
	struct get_remainder< multiplication< lhs, rhs >, lhs > : constant< 0 >{ };
	template< typename lhs, typename rhs >
	struct get_remainder< multiplication< lhs, rhs >, rhs > : constant< 0 >{ };
	template< typename T >
	struct get_remainder< multiplication< T, T >, T > : constant< 0 >{ };
	template< typename T, typename div >
	struct get_remainder< negation< T >, div > : negation< typename get_remainder< T, div >::type > { };
	template< typename T, typename div >
	struct get_modulant
	{
		typedef constant< 0 > type;
	};
	template< typename T >
	struct get_variable { typedef T type; };
	template< typename lhs, typename rhs >
	struct get_variable< addition< lhs, rhs > >
	{
		typedef typename
		std::conditional
		<
			has_variable( lhs( ) ),
			typename get_variable< lhs >::type,
			typename get_variable< rhs >::type
		>::type type;
	};
	template< typename lhs, typename rhs >
	struct get_variable< multiplication< lhs, rhs > >
	{
		typedef typename
		std::conditional
		<
			has_variable( lhs( ) ),
			typename get_variable< lhs >::type,
			typename get_variable< rhs >::type
		>::type type;
	};
	template< typename T >
	constexpr bool has_variable( const T & ) { return true; }
	template< size_t T >
	constexpr bool has_variable( const constant< T > & ) { return false; }
	template< typename T, typename assert >
	struct is_zero;
	template< size_t T, typename assert >
	struct is_zero< constant< T >, assert > : boost::mpl::bool_< std::is_same< constant< T >, constant< 0 > >::value > { };
	template< typename T, typename assert = assert_bool< > >
	struct is_zero
	{
		template< bool b, typename TT, typename aassert > struct inner;
		template< typename TT, typename aassert >
		struct inner< true, TT, aassert >
		{
			typedef boost::mpl::false_ type;
			typename assert::template apply< false >::type s;
		};
		template< typename TT, typename aassert >
		struct inner< false, TT, aassert >
		{
			typedef
			boost::mpl::bool_
			<
				is_zero< typename get_remainder< TT, typename get_variable< TT >::type >::type >::value &&
				is_zero< typename get_modulant< TT, typename get_variable< TT >::type >::type >::value
			> type;
			typename assert::template apply< type::value >::type s;
		};
		typedef typename
		std::conditional
		<
			has_reciprocal( T( ) ),
			is_zero< typename remove_reciprocal_by_multiply_nonzero< T >::type >,
			typename inner
			<
				std::is_same
				<
					T,
					typename get_remainder< T, typename get_variable< T >::type >::type
				>::value,
				T,
				assert
			>::type
		>::type type;
		constexpr static bool value = type::value;
	};
	template< typename lhs, typename rhs, typename assert >
	struct is_zero< multiplication< lhs, rhs >, assert > : boost::mpl::bool_< is_zero< lhs >::value || is_zero< rhs >::value > { };
	template< typename T >
	struct is_zero< negation< T > > : is_zero< T > { };
	template< typename lhs, typename rhs, typename assert = assert_bool< > >
	struct equal : is_zero< decltype( lhs( ) - rhs( ) ), assert > { };
	template< typename lhs, typename rhs >
	constexpr bool operator == ( const lhs &, const rhs & ) { return equal< typename lhs::type, typename rhs::type >::value; }
	struct x { typedef x type; };
	struct y { typedef y type; };
	constexpr bool can_decompose( const x & ) { return false; }
	constexpr bool can_decompose( const y & ) { return false; }
	static_assert(
			equal
			<
				typename differentiate< decltype( x( ) * x( ) + y( ) * y( ) ), x >::type,
				decltype( constant< 2 >( ) * x( ) ),
				assert_bool< true >
			>::value, "" );
	static_assert(
			equal
			<
				decltype( x( ) * x( ) + x( ) * x( ) ),
				decltype( constant< 2 >( ) * x( ) * x( ) ),
				assert_bool< true >
			>::value, "" );
	static_assert(
			equal
			<
				typename differentiate< decltype( constant< 3 >( ) * x( ) * x( ) * x( ) + constant< 2 >( ) * x( ) * x( ) ), x >::type,
				decltype( constant< 9 >( ) * x( ) * x( ) + constant< 4 >( ) * x( ) ),
				assert_bool< true >
			>::value, "" );
}
#endif //MISC_HPP
