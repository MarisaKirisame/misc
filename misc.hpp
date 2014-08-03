#ifndef MISC_HPP
#define MISC_HPP
#include <boost/range/adaptor/transformed.hpp>
#include <boost/optional.hpp>
#include <deque>
#include <functional>
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
#include <iterator>
#include <type_traits>
#include <boost/preprocessor.hpp>
#include <algorithm>
#include <set>
#include <list>
#include <iterator>
#include <thread>
#include <map>
#include <boost/variant.hpp>
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
	template< typename T >
	constexpr const T & max( const T & a, const T & b ) { return a > b ? a : b; }
	template< typename T, typename ... ARG >
	constexpr const T & max( const T & a, const T & b, const ARG & ... c ) { return a > b ? std::max( a, c ... ) : std::max( b, c ... ); }
	enum class_name
	{
		calc_bc,
		stat,
		physic,
		psych,
		macro,
		WH,
		calc_ab,
		ES,
		HG
	};

	std::ostream & operator << ( std::ostream & os, class_name cn )
	{
		switch ( cn )
		{
			case psych:
				os << "psych  ";
				break;
			case calc_ab:
				os << "calc_ab";
				break;
			case calc_bc:
				os << "calc_bc";
				break;
			case stat:
				os << "stat   ";
				break;
			case physic:
				os << "physic ";
				break;
			case macro:
				os << "macro  ";
				break;
			case WH:
				os << "WH     ";
				break;
			case ES:
				os << "ES     ";
				break;
			case HG:
				os << "HG     ";
				break;
		}
		return os;
	}
	std::set< class_name > pair_class( )
	{
		return
		{
			calc_bc,
			stat,
			physic,
			macro,
			WH,
			calc_ab,
			ES,
			HG
		};
	}
	//physic and enviromental science cannot be std::pair
	struct class_organization
	{
		std::set< std::pair< class_name, class_name > > paired_set;
		static std::set< std::set< std::pair< class_name, class_name > > >  paired_set_out( const std::set< class_name > & rem )
		{
			if ( rem.size( ) == 2 )
			{
				auto b = rem.begin( );
				class_name f = * b;
				++b;
				class_name e = * b;
				std::set< std::set< std::pair< class_name, class_name > > > ret;
				std::set< std::pair< class_name, class_name > > tem;
				tem.insert( std::make_pair( f, e ) );
				ret.insert( tem );
				return ret;
			}
			assert( rem.size( ) % 2 == 0 );
			assert( rem.begin( ) != rem.end( ) );
			class_name f = *rem.begin( );
			auto re = rem;
			re.erase( f );
			std::set< std::set< std::pair< class_name, class_name > > > ret;
			for ( class_name i : re )
			{
				auto tem = re;
				auto n = tem.erase( i );
				assert( n > 0 );
				std::set< std::set< std::pair< class_name, class_name > > > res(  paired_set_out( tem ) );
				transform(
							res.begin( ),
							res.end( ),
							inserter( ret, ret.end( ) ),
							[&]( const std::set< std::pair< class_name, class_name > > & ii )
				{
					auto tem( ii );
					tem.insert( std::make_pair( std::max( f, i ), std::min( f, i ) ) );
					return tem;
				} );
			}
			return ret;
		}
		static std::set< class_organization > all_class_organization( )
		{
			std::set< class_organization > ret;
			auto res =  paired_set_out( pair_class( ) );
			for ( auto i : res )
			{
				ret.insert( class_organization( i, 0 ) );
				ret.insert( class_organization( i, 1 ) );
				ret.insert( class_organization( i, 2 ) );
				ret.insert( class_organization( i, 3 ) );
			}
			return ret;
		}
		size_t psy_position;
		class_organization( const std::set< std::pair< class_name, class_name > > & ps, size_t s ) : paired_set( ps ), psy_position( s ) { }
		static size_t get_conflict( class_name l, class_name r )
		{
			assert( l != r );
			static std::map< std::pair< class_name, class_name >, size_t > conflict_map;
			static auto t = [&]()
			{
				auto insert = [&]( class_name l, class_name r, size_t i ) { conflict_map.insert( std::make_pair( std::make_pair( std::min( l, r ), std::max( l, r ) ), i ) ); };
				insert( calc_bc, stat, 4 );
				insert( calc_bc, physic, 4 );
				insert( calc_bc, psych, 2 );
				insert( calc_bc, macro, 5 );
				insert( calc_bc, WH, 4 );
				insert( calc_bc, calc_ab, 0 );
				insert( calc_bc, ES, 6 );
				insert( calc_bc, HG, 2 );
				insert( stat, physic, 1 );
				insert( stat, psych, 3 );
				insert( stat, macro, 9 );
				insert( stat, WH, 7 );
				insert( stat, calc_ab, 0 );
				insert( stat, ES, 6 );
				insert( stat, HG, 4 );
				insert( physic, psych, 3 );
				insert( physic, macro, 6 );
				insert( physic, WH, 0 );
				insert( physic, calc_ab, 1 );
				insert( physic, ES, 4 );
				insert( physic, HG, 1 );
				insert( psych, macro, 7 );
				insert( psych, WH, 3 );
				insert( psych, calc_ab, 5 );
				insert( psych, ES, 4 );
				insert( psych, HG, 8 );
				insert( macro, WH, 8 );
				insert( macro, calc_ab, 11 );
				insert( macro, ES, 11 );
				insert( macro, HG, 14 );
				insert( WH, calc_ab, 2 );
				insert( WH, ES, 8 );
				insert( WH, HG, 6 );
				insert( calc_ab, ES, 7 );
				insert( calc_ab, HG, 14 );
				insert( HG, ES, 6 );
				return 0;
			}( );
			auto res = conflict_map.find( std::make_pair( std::min( l, r ), std::max( l, r ) ) );
			assert( res != conflict_map.end( ) );
			assert( t == 0 );
			return res->second;
		}
		size_t get_conflict( ) const
		{
			size_t ret = 0;
			size_t ii = 0;
			for ( auto i :  paired_set )
			{
				ret += get_conflict( i.first, i.second );
				if ( ii == psy_position )
				{
					ret += get_conflict( psych, i.first );
					ret += get_conflict( psych, i.second );
				}
				++ii;
			}
			return ret;
		}
		bool operator < ( const class_organization & comp ) const
		{
			if ( get_conflict( ) > comp.get_conflict( ) ) { return true; }
			if ( get_conflict( ) < comp.get_conflict( ) ) { return false; }
			if ( psy_position < comp.psy_position ) { return true; }
			if ( psy_position > comp.psy_position ) { return false; }
			return  paired_set < comp. paired_set;
		}
		friend std::ostream & operator << ( std::ostream & os, const class_organization & co )
		{
			size_t i = 0;
			for ( auto & p : co. paired_set )
			{
				os << p.first << ' ' << p.second;
				if ( co.psy_position == i ) { os << ' ' << psych; }
				os << std::endl;
				++i;
			}
			os << "conflict number: " << co.get_conflict( ) << std::endl;
			return os;
		}
	};
	void conflict_search( )
	{
		auto ret = class_organization::all_class_organization( );
		for ( auto i : ret ) { std::cout << i << std::endl; }
	}
}
#endif //MISC_HPP
