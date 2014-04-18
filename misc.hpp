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
	template< typename i >
	auto arrow( i ii ) { return ii.operator->( ); }
	template< typename i >
	auto arrow( i * ii ) { return ii; }
	template< typename iterator, size_t skip_size >
	struct skipping_iterator : std::iterator< std::random_access_iterator_tag, typename std::iterator_traits< iterator >::value_type >
	{
		iterator current;
		size_t distance_to_end;
		decltype( * current ) operator * ( ) const { return * current; }
		skipping_iterator & operator ++ ( )
		{
			if ( distance_to_end <= skip_size ) { distance_to_end = 0; }
			else
			{
				std::advance( current, skip_size );
				distance_to_end -= skip_size;
			}
			return * this;
		}
		skipping_iterator & operator -- ( )
		{
			current -= skip_size;
			distance_to_end += skip_size;
			return * this;
		}
		bool is_end( ) const { return distance_to_end == 0; }
		bool operator == ( const skipping_iterator & comp ) const
		{ return ( is_end( ) && comp.is_end( ) ) || ( current == comp.current && distance_to_end == comp.distance_to_end ); }
		bool operator != ( const skipping_iterator & comp ) const { return ! ( *this == comp ); }
		decltype( arrow( current ) ) operator -> ( ) const { return current.operator->( ); }
		decltype( * current ) operator ++ ( int )
		{
			auto tem = **this;
			*this++;
			return tem;
		}
		skipping_iterator & operator += ( int n )
		{
			if ( n > 0 )
			{
				while ( n > 0 )
				{
					--n;
					++*this;
				}
			}
			else
			{
				while ( n < 0 )
				{
					++n;
					--*this;
				}
			}
			return * this;
		}
		skipping_iterator operator + ( int n ) const
		{
			skipping_iterator tem = *this;
			return tem += n;
		}
		friend skipping_iterator operator + ( int n, const skipping_iterator & i ) { return i + n; }
		skipping_iterator & operator -= ( int n ) { return *this += -n; }
		skipping_iterator operator - ( int n )
		{
			skipping_iterator tem = *this;
			return tem -= n;
		}
		size_t operator - ( const skipping_iterator & rhs ) const
		{
			if ( ( rhs.distance_to_end - distance_to_end ) % skip_size == 0 )
			{ return ( rhs.distance_to_end - distance_to_end ) / skip_size; }
			else
			{ return ( ( rhs.distance_to_end - distance_to_end ) - ( rhs.distance_to_end - distance_to_end ) % skip_size ) / skip_size + 1; }
		}
		decltype( * current ) operator [ ]( size_t n ) const { return *( *this + n ); }
		bool operator < ( const skipping_iterator & comp ) { return ( comp - *this ) > 0; }
		bool operator > ( const skipping_iterator & comp ) { return comp < *this; }
		bool operator >= ( const skipping_iterator & comp ) { return !( *this < comp ); }
		bool operator <= ( const skipping_iterator & comp ) { return !( *this > comp ); }
		skipping_iterator( iterator current, iterator end ) : current( current ), distance_to_end( std::distance( current, end ) ) { }
		skipping_iterator( iterator current, size_t distance_to_end ) : current( current ), distance_to_end( distance_to_end ) { }
	};
	template< typename it, typename op, size_t n >
	void split( it begin, it end, op out )
	{
		std::transform( boost::counting_iterator< size_t >( 0 ),
										boost::counting_iterator< size_t >( n ),
										out,
										[&]( size_t )
		{
			auto ret = skipping_iterator< it, n >( begin, end );
			++begin;
			return std::make_pair( ret, skipping_iterator< it, n >( begin, 0 ) );
		} );
	}
	struct square
	{
		square & operator ++ ( ) { ++num; return * this; }
		size_t num;
		bool empty( ) const { return num == 0; }
		operator size_t( ) const { return num == 0 ? 0 : std::pow( 2, num ); }
		template< typename iterator >
		static bool can_merge( iterator begin, iterator end )
		{
			if ( std::distance( begin, end ) <= 1 ) { return false; }
			else
			{
				auto second = begin;
				++second;
				if ( ( begin->num != 0 && second->num == 0 ) || ( begin->num != 0 && begin->num == second->num ) ) { return true; }
				else { return can_merge( second, end ); }
			}
		}
		bool operator == ( const square & comp ) { return num == comp.num; }
		template< typename iiterator, typename oiterator >
		static void merge( iiterator ibegin, iiterator iend, oiterator obegin )
		{
			if ( ibegin == iend ) { }
			if ( std::distance( ibegin, iend ) == 1 ) { *obegin = *ibegin; }
			else
			{
				auto second = ibegin;
				++second;
				std::vector< square > tem;
				std::copy_if(
							ibegin,
							iend,
							std::back_inserter( tem ),
							[&]( const square & s ) { return ! s.empty( ); } );
					std::vector< square > ret;
					while ( ! tem.empty( ) )
					{
						square s =  tem.back( );
						tem.pop_back( );
						if ( ( ! tem.empty( ) ) && ( tem.back( ) == s ) )
						{
							tem.pop_back( );
							++s;
						}
						ret.push_back( std::move( s ) );
					}
					obegin = std::transform(
								boost::counting_iterator< size_t >( 0 ),
								boost::counting_iterator< size_t >( std::distance( ibegin, iend ) - ret.size( ) ),
								obegin,
								[]( size_t ){ return square( ); } );
					std::copy( ret.rbegin( ), ret.rend( ), obegin );
			}
		}
		square( ) : square( 0 ) { }
		explicit square( size_t num ) : num( num ) { }
	};
	template< typename Data >
	auto srange( Data & data )
	{
		return
				boost::range::join
				(
					boost::range::join
					(
						boost::make_iterator_range( data[0].begin( ), data[0].end( ) ),
						boost::make_iterator_range( data[1].begin( ), data[1].end( ) )
					),
					boost::range::join
					(
						boost::make_iterator_range( data[2].begin( ), data[2].end( ) ),
						boost::make_iterator_range( data[3].begin( ), data[3].end( ) )
					)
				);
	}//GCC Workaround
	auto dleft_to_right( std::array< std::array< square, 4 >, 4 > & data )
	{
		std::vector< std::pair< decltype( data[0].begin( ) ), decltype( data[0].begin( ) ) > > ret;
		std::transform(
					data.begin( ),
					data.end( ),
					std::back_inserter( ret ),
					[]( decltype( data[0] ) data ){ return std::make_pair( data.begin( ), data.end( ) ); } );
		return ret;
	}//GCC Workaround
	auto dcleft_to_right( const std::array< std::array< square, 4 >, 4 > & data )
	{
		std::vector< std::pair< decltype( data[0].cbegin( ) ), decltype( data[0].cbegin( ) ) > > ret;
		std::transform(
					data.cbegin( ),
					data.cend( ),
					std::back_inserter( ret ),
					[]( decltype( data[0] ) data ){ return std::make_pair( data.cbegin( ), data.cend( ) ); } );
		return ret;
	}//GCC Workaround
	auto dright_to_left( std::array< std::array< square, 4 >, 4 > & data )
	{
		std::vector< std::pair< decltype( data[0].rbegin( ) ), decltype( data[0].rbegin( ) ) > > ret;
		std::transform(
					data.begin( ),
					data.end( ),
					std::back_inserter( ret ),
					[]( decltype( data[0] ) data ){ return std::make_pair( data.rbegin( ), data.rend( ) ); } );
		return ret;
	}//GCC Workaround
	auto dcright_to_left( const std::array< std::array< square, 4 >, 4 > & data )
	{
		std::vector< std::pair< decltype( data[0].crbegin( ) ), decltype( data[0].crbegin( ) ) > > ret;
		std::transform(
					data.begin( ),
					data.end( ),
					std::back_inserter( ret ),
					[]( decltype( data[0] ) data ){ return std::make_pair( data.crbegin( ), data.crend( ) ); } );
		return ret;
	}//GCC Workaround
	auto dup_to_down( std::array< std::array< square, 4 >, 4 > & data )
	{
		typedef skipping_iterator< decltype( srange( data ).begin( ) ), 4 > tem;
		std::vector< std::pair< tem, tem > > after_split;
		split
		<
			decltype( srange( data ).begin( ) ),
			decltype( std::back_inserter( after_split ) ),
			4
		>
		(
			srange( data ).begin( ),
			srange( data ).end( ),
			std::back_inserter( after_split )
		);
		return after_split;
	}//GCC Workaround
	auto dcup_to_down( const std::array< std::array< square, 4 >, 4 > & data )
	{
		typedef skipping_iterator< decltype( srange( data ).begin( ) ), 4 > tem;
		std::vector< std::pair< tem, tem > > after_split;
		split
		<
			decltype( srange( data ).begin( ) ),
			decltype( std::back_inserter( after_split ) ),
			4
		>
		(
			srange( data ).begin( ),
			srange( data ).end( ),
			std::back_inserter( after_split )
		);
		return after_split;
	}//GCC Workaround
	auto ddown_to_up( std::array< std::array< square, 4 >, 4 > & data )
	{
		typedef skipping_iterator< decltype( boost::rbegin( srange( data ) ) ), 4 > tem;
		std::vector< std::pair< tem, tem > > after_split;
		split
		<
			decltype( boost::rbegin( srange( data ) ) ),
			decltype( std::back_inserter( after_split ) ),
			4
		>
		(
			boost::rbegin( srange( data ) ),
			boost::rend( srange( data ) ),
			std::back_inserter( after_split )
		);
		return after_split;
	}//GCC Workaround
	auto dcdown_to_up( const std::array< std::array< square, 4 >, 4 > & data )
	{
		typedef skipping_iterator< decltype( boost::rbegin( srange( data ) ) ), 4 > tem;
		std::vector< std::pair< tem, tem > > after_split;
		split
		<
			decltype( boost::rbegin( srange( data ) ) ),
			decltype( std::back_inserter( after_split ) ),
			4
		>
		(
			boost::rbegin( srange( data ) ),
			boost::rend( srange( data ) ),
			std::back_inserter( after_split )
		);
		return after_split;
	}//GCC Workaround
	struct game_2048
	{
		game_2048( ) { }
		std::array< std::array< square, 4 >, 4 > data;
		decltype( srange( data ) ) range( ) { return srange( data ); }
		decltype( srange( data ).begin( ) ) begin( ) { return range( ).begin( ); }
		decltype( srange( data ).end( ) ) end( ) { return range( ).end( ); }
		decltype( boost::rbegin( srange( data ) ) ) rbegin( ) { return boost::rbegin( range( ) ); }
		decltype( boost::rend( srange( data ) ) ) rend( ) { return boost::rend( range( ) ); }
		enum direction { left, right, up, down };
		void random_add( )
		{
			auto pred = []( square & s ){ return s.empty( ); };
			auto i_begin = boost::make_filter_iterator( pred, begin( ), end( ) );
			auto i_end = boost::make_filter_iterator( pred, end( ), end( ) );
			std::random_device rd;
			for ( auto distance = std::distance( i_begin, i_end );i_begin != i_end; ++i_begin )
			{
				if ( std::uniform_real_distribution<>( )( rd ) <= 1 / static_cast< double >( distance ) )
				{
					*i_begin = square( 1 );
					break;
				}
				--distance;
			}
		}
		decltype( dleft_to_right( data ) ) left_to_right( ) { return dleft_to_right( data ); }
		decltype( dcleft_to_right( data ) ) left_to_right( ) const { return dcleft_to_right( data ); }
		decltype( dright_to_left( data ) ) right_to_left( ) { return dright_to_left( data ); }
		decltype( dcright_to_left( data ) ) right_to_left( ) const { return dcright_to_left( data ); }
		decltype( dup_to_down( data ) ) up_to_down( ) { return dup_to_down( data ); }
		decltype( dcup_to_down( data ) ) up_to_down( ) const { return dcup_to_down( data ); }
		decltype( ddown_to_up( data ) ) down_to_up( ) { return ddown_to_up( data ); }
		decltype( dcdown_to_up( data ) ) down_to_up( ) const { return dcdown_to_up( data ); }
		void move( direction dir, bool add_new = true )
		{
			if ( dir == left )
			{
				auto d = right_to_left( );
				std::for_each(
							d.begin( ),
							d.end( ),
							[]( decltype( d[0] ) data ){ square::merge( data.first, data.second, data.first ); } );
			}
			else if ( dir == right )
			{
				auto d = left_to_right( );
				std::for_each(
							d.begin( ),
							d.end( ),
							[]( decltype( d[0] ) data ){ square::merge( data.first, data.second, data.first ); } );
			}
			else if ( dir == up )
			{
				auto d = down_to_up( );
				std::for_each(
							d.begin( ),
							d.end( ),
							[]( decltype( d[0] ) data ){ square::merge( data.first, data.second, data.first ); } );
			}
			else
			{
				assert( dir ==  down );
				auto d = up_to_down( );
				std::for_each(
							d.begin( ),
							d.end( ),
							[]( decltype( d[0] ) data ){ square::merge( data.first, data.second, data.first ); } );
			}
			if ( add_new ) { random_add( ); }
		}
		bool can_move( ) const { return can_move( up ) || can_move( down ) || can_move( left ) || can_move( right ); }
		bool can_move( direction dir ) const
		{
			if ( dir == left )
			{
				auto d = right_to_left( );
				return std::find_if(
							d.begin( ),
							d.end( ),
							[]( decltype( d[0] ) data ){ return square::can_merge( data.first, data.second ); } ) != d.end( );
			}
			else if ( dir == right )
			{
				auto d = left_to_right( );
				return std::find_if(
							d.begin( ),
							d.end( ),
							[]( decltype( d[0] ) data ){ return square::can_merge( data.first, data.second ); } ) != d.end( );
			}
			else if ( dir == up )
			{
				auto d = down_to_up( );
				return std::find_if(
							d.begin( ),
							d.end( ),
							[]( decltype( d[0] ) data ){ return square::can_merge( data.first, data.second ); } ) != d.end( );
			}
			else
			{
				assert( dir ==  down );
				auto d = up_to_down( );
				return std::find_if(
							d.begin( ),
							d.end( ),
							[]( decltype( d[0] ) data ){ return square::can_merge( data.first, data.second ); } ) != d.end( );
			}
		}
		template< typename O >
		friend O & operator << ( O & o, const game_2048 & s )
		{
			o
					<< s.data[0][0] <<  "|" << s.data[0][1] << "|" << s.data[0][2] << "|" << s.data[0][3] << std::endl
					<< s.data[1][0] <<  "|" << s.data[1][1] << "|" << s.data[1][2] << "|" << s.data[1][3] << std::endl
					<< s.data[2][0] <<  "|" << s.data[2][1] << "|" << s.data[2][2] << "|" << s.data[2][3] << std::endl
					<< s.data[3][0] <<  "|" << s.data[3][1] << "|" << s.data[3][2] << "|" << s.data[3][3] << std::endl;
			return o;
		}
	};
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
}
#endif //MISC_HPP
