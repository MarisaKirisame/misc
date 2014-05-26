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
		assert( is_palindrome( "amanaplanacanoepastaherosrajahsacoloraturamapssnipepercalemacaroniagagabananabagatanatagabananabagagainoracamelacrepepinsspamarutarolocashajarsorehatsapeonacanalpanama" ) );
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
	template< typename T >
	constexpr const T & max( const T & a, const T & b ) { return a > b ? a : b; }
	template< typename T, typename ... ARG >
	constexpr const T & max( const T & a, const T & b, const ARG & ... c ) { return a > b ? max( a, c ... ) : max( b, c ... ); }
	#include <cassert>
	#include <algorithm>
	#include <type_traits>
	#include <tuple>
	template< int placeholder >
	struct place_holder{ };
	template< typename K, typename ... RES >
	struct binded_function
	{
		template< typename ... >
		struct GET_LEAST_NUM_ARG
		{ static constexpr size_t value = 0; };
		template< typename T, typename ... ARG >
		struct GET_LEAST_NUM_ARG< T, ARG ... >
		{ static constexpr size_t value = GET_LEAST_NUM_ARG< ARG ... >::value; };
		template< size_t T, typename ... ARG >
		struct GET_LEAST_NUM_ARG< place_holder< T >, ARG ... >
		{ static constexpr size_t value = max( T, GET_LEAST_NUM_ARG< ARG ... >::value ); };
		static constexpr size_t LEAST_NUM_ARG = GET_LEAST_NUM_ARG< RES ... >::value;
		K k;
		struct tag{ };
		template< typename NNEXT, size_t pos >
		struct parameter_delegate
		{
			typedef NNEXT NEXT;
			template< typename ... BEFORE >
			struct wrapper
			{
				template< typename ... ORIGINAL >
				struct wrapper2
				{
					template< typename SFINAE, typename ... REMAINING >
					static auto function(
							const binded_function * that,
							const parameter_delegate * pd,
							const BEFORE & ... before,
							const typename std::enable_if< ! std::is_same< void, typename SFINAE::NEXT >::value, tag >::type & t,
							const ORIGINAL & ... original,
							const tag &,
							const REMAINING & ... rem )
					{
						return std::decay< decltype( * pd->next ) >::type::
								template wrapper< BEFORE ..., std::tuple_element< pos, std::tuple< ORIGINAL ... > >::type >::
								template wrapper2< ORIGINAL ... >::template function< typename SFINAE::NEXT >(
									that, pd->next, before ..., std::get< pos >( std::tie( original ... ) ), t, original ..., t, rem ... ); }
					template< typename SFINAE, typename ... REMAINING >
					static auto function(
							const binded_function * that,
							const parameter_delegate *,
							const BEFORE & ... before,
							const typename std::enable_if< std::is_same< void, typename SFINAE::NEXT >::value, tag >::type &,
							const ORIGINAL & ... original,
							const tag &,
							const REMAINING & ... rem )
					{ return that->k( before ..., std::get< pos >( std::tie( original ... ) ), rem ... ); }
				};
			};
			NEXT * next;
			~parameter_delegate( ) { clean_if_necessary< NEXT >( ); }
			template< typename SFINAE >
			typename std::enable_if< ! std::is_same< void, SFINAE >::value >::type clean_if_necessary( ) { delete next; }
			template< typename SFINAE >
			typename std::enable_if< std::is_same< void, SFINAE >::value >::type clean_if_necessary( ) { }
			template< typename SFINAE, typename ... ARG  >
			typename std::enable_if< ! std::is_same< void, SFINAE >::value >::type new_if_necessary( const ARG & ... arg ) { next = new NEXT( arg ... ); }
			template< typename SFINAE, typename ... ARG >
			typename std::enable_if< std::is_same< void, SFINAE >::value >::type new_if_necessary( const ARG & ... ) { }
			template< typename ... ARG >
			parameter_delegate( const ARG & ... arg ) { new_if_necessary< NEXT >( arg ... ); }
		};
		template< typename NNEXT, typename T >
		struct constant_delegate
		{
			typedef NNEXT NEXT;
			T value;
			NEXT * next;
			template< typename ... BEFORE >
			struct wrapper
			{
				template< typename ... ORIGINAL >
				struct wrapper2
				{
					template< typename SFINAE, typename ... REMAINING >
					static auto function(
							const binded_function * that,
							const constant_delegate * cd,
							const BEFORE & ... before,
							const typename std::enable_if< ! std::is_same< void, typename SFINAE::NEXT >::value, tag >::type & t,
							const ORIGINAL & ... original,
							const tag &,
							const REMAINING & ... rem )
					{ return std::decay< decltype( * cd->next ) >::type::
								template wrapper< BEFORE ..., T >::template wrapper2< ORIGINAL ... >::template function< typename SFINAE::NEXT >(
									that, cd->next, before ..., cd->value, t, original ..., t, rem ... ); }
					template< typename SFINAE, typename ... REMAINING >
					static auto function(
							const binded_function * that,
							const constant_delegate * cd,
							const BEFORE & ... before,
							const typename std::enable_if< std::is_same< void, typename SFINAE::NEXT >::value, tag >::type &,
							const ORIGINAL & ...,
							const tag &,
							const REMAINING & ... rem )
					{ return that->k( before ..., cd->value, rem ... ); }
				};
			};
			~constant_delegate( ) { clean_if_necessary< NEXT >( ); }
			template< typename SFINAE >
			typename std::enable_if< ! std::is_same< void, SFINAE >::value >::type clean_if_necessary( ) { delete next; }
			template< typename SFINAE >
			typename std::enable_if< std::is_same< void, SFINAE >::value >::type clean_if_necessary( ) { }
			template< typename SFINAE, typename ... ARG  >
			typename std::enable_if< ! std::is_same< void, SFINAE >::value >::type new_if_necessary( const ARG & ... arg ) { next = new NEXT( arg ... ); }
			template< typename SFINAE, typename ... ARG >
			typename std::enable_if< std::is_same< void, SFINAE >::value >::type new_if_necessary( const ARG & ... ) { }
			template< typename F, typename ... ARG >
			constant_delegate( const F & f, const ARG & ... arg ) : value( f ) { new_if_necessary< NEXT, ARG ... >( arg ... ); }
			template< size_t F, typename ... ARG >
			constant_delegate( const place_holder< F > &, const ARG & ... arg ) : constant_delegate( arg ... ) { }
		};
		template< typename ... >
		struct GET_DELEGATE
		{ typedef void type; };
		template< typename T, typename ... ARG >
		struct GET_DELEGATE< T, ARG ... >
		{ typedef constant_delegate< typename GET_DELEGATE< ARG ... >::type, T > type; };
		template< size_t T, typename ... ARG >
		struct GET_DELEGATE< place_holder< T >, ARG ... >
		{ typedef parameter_delegate< typename GET_DELEGATE< ARG ... >::type, T > type; };
		typedef typename GET_DELEGATE< RES ... >::type DELEGATE;
		DELEGATE d;
		template< typename ... ARG >
		struct operator_delegate
		{
			const binded_function * that;
			std::tuple< const ARG & ... > data;
			template< size_t NUM_ARG_EATENED, typename FIRST_REMAINING, typename ... REST_REMAINING >
			auto function(
					const ARG & ... bef,
					const typename std::enable_if< NUM_ARG_EATENED != LEAST_NUM_ARG, FIRST_REMAINING >::type &,
					const REST_REMAINING & ... rem ) const
			{ return function< NUM_ARG_EATENED + 1, REST_REMAINING ... >( that, bef ..., rem ... ); }
			template< size_t NUM_ARG_EATENED, typename FIRST_REMAINING, typename ... REST_REMAINING >
			auto function(
					const ARG & ... bef,
					const typename std::enable_if< NUM_ARG_EATENED == LEAST_NUM_ARG, FIRST_REMAINING >::type &,
					const REST_REMAINING & ... rem ) const
			{ return second_function< ARG ... >::function( that, this, bef ..., rem ... ); }
			template< typename ... BEFORE >
			struct second_function
			{
				template< typename ... AFTER >
				static auto function(
						const binded_function * that,
						const operator_delegate *,
						const BEFORE & ... bef,
						const AFTER & ... aft )
				{ return DELEGATE::template wrapper< >::template wrapper2< BEFORE ... >::template function< DELEGATE >(
								that, & that->d, tag( ), bef ..., tag( ), aft ... ); }
			};
			operator_delegate( const binded_function * that, const ARG & ... arg ) : that( that ), data( std::tie( arg ... ) ) { }
			auto operator ( )( const ARG & ... arg ) const { return function< 0, ARG ... >( arg ..., arg ... ); }
		};
		template< typename ... ARG >
		auto operator ( )( const ARG & ... arg ) const { return operator_delegate< ARG ... >( this, arg ... )( arg ... ); }
		template< typename ... ARG >
		binded_function( const K & k, const ARG & ... arg ) : k( k ), d( arg ... ) { }
	};

	template< typename T, typename ... ARG >
	auto bind( const T & t, const ARG & ... arg ) { return binded_function< T, ARG ... >( t, arg ... ); }
	#include <iostream>
	struct max_s
	{
		template< typename ... ARG >
		auto operator ( )( const ARG & ... arg ) const { return max( arg ... ); }
	};
	void bound_example( )
	{
		static_assert( max( 1, 2, 3, 4 ) == 4, "" );
		static_assert( max( 1.0, 2.0 ) == 2.0, "" );
		bind( [](int a,int b){ assert( a == 1 && b == 2 ); }, 1, place_holder< 0 >( ) )( 2 );
		std::cout << max( 100, 2, 4 );
		auto f = bind( max_s( ), 100, place_holder< 0 >( ) );
		std::cout << f( 2, 3, 450 );
	}
}
#endif //MISC_HPP
