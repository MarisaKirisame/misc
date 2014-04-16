#ifndef MISC_HPP
#define MISC_HPP
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
		double operator - ( const skipping_iterator & rhs ) const { return static_cast< double >( distance_to_end - rhs.distance_to_end ) / skip_size; }
		decltype( * current ) operator [ ]( size_t n ) const { return *( *this + n ); }
		bool operator < ( const skipping_iterator & comp ) { return ( comp - *this ) > 0; }
		bool operator > ( const skipping_iterator & comp ) { return comp < *this; }
		bool operator >= ( const skipping_iterator & comp ) { return !( *this < comp ); }
		bool operator <= ( const skipping_iterator & comp ) { return !( *this > comp ); }
		skipping_iterator( iterator current, iterator end ) : current( current ), distance_to_end( std::distance( current, end ) ) { }
	};
	template< typename it, typename op, size_t n >
	auto split( it begin, it end, op out )
	{
		std::transform( boost::counting_iterator< size_t >( 0 ),
										boost::counting_iterator< size_t >( n ),
										out,
										[&](size_t)
		{
			auto ret = skipping_iterator< it, n >( begin, end );
			++begin;
			return ret;
		} );
		return skipping_iterator< it, n >( begin, begin );
	}
	struct square
	{
		square & operator ++ ( ) { ++num; return * this; }
		size_t num;
		bool empty( ) const { return num == 0; }
		template< typename O >
		friend O & operator << ( O & o, const square & s ) { return o << s.num == 0 ? 0 : std::pow( 2, s.num ); }
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
				if ( std::distance( ibegin, iend ) == 2 )
				{
					if ( ( ibegin->num != 0 && ibegin->num == second->num ) )
					{
						*obegin = square( 0 );
						++obegin;
						++( *obegin );
					}
					else if ( ibegin->num == 0 && second->num != 0 )
					{
						*obegin = *second;
						++obegin;
						*obegin = *ibegin;
					}
				}
				else
				{
					std::vector< square > tem;
					std::for_each(
								ibegin,
								iend,
								[&]( const square & s )
					{
						if ( ! s.empty( ) )
						{ tem.push_back( s ); }
					} );
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
					std::transform(
								boost::counting_iterator< size_t >( 0 ),
								boost::counting_iterator< size_t >( std::distance( ibegin, iend ) - ret.size( ) ),
								obegin,
								[]( size_t ){ return square( ); } );
					std::copy( ret.begin( ), ret.end( ), obegin );
				}
			}
		}
		square( ) : square( 0 ) { }
		square( size_t num ) : num( num ) { }
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
	}
	auto dright_to_left( std::array< std::array< square, 4 >, 4 > & data )
	{
		std::vector< std::pair< decltype( data[0].rbegin( ) ), decltype( data[0].rbegin( ) ) > > ret;
		std::transform(
					data.begin( ),
					data.end( ),
					std::back_inserter( ret ),
					[]( decltype( data[0] ) data ){ return std::make_pair( data.rbegin( ), data.rend( ) ); } );
		return ret;
	}
	auto dup_to_down( std::array< std::array< square, 4 >, 4 > & data )
	{
		typedef skipping_iterator< decltype( srange( data ).begin( ) ), 4 > tem;
		std::vector< tem > after_split;
		auto
				p_end =
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
		std::vector< std::pair< tem, tem > > ret;
		std::transform(
					after_split.begin( ),
					after_split.end( ),
					std::back_inserter( ret ),
					[&]( decltype( * after_split.begin( ) ) b ){ return std::make_pair( b, p_end ); } );
		return ret;
	}
	auto ddown_to_up( std::array< std::array< square, 4 >, 4 > & data )
	{
		typedef skipping_iterator< decltype( boost::rbegin( srange( data ) ) ), 4 > tem;
		std::vector< tem > after_split;
		auto
				p_end =
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
		std::vector< std::pair< tem, tem > > ret;
		std::transform(
					after_split.begin( ),
					after_split.end( ),
					std::back_inserter( ret ),
					[&]( decltype( * after_split.begin( ) ) b ){ return std::make_pair( b, p_end ); } );
		return ret;
	}
	struct game_2048
	{
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
		decltype( dright_to_left( data ) ) right_to_left( ) { return dright_to_left( data ); }
		decltype( dup_to_down( data ) ) up_to_down( ) { return dup_to_down( data ); }
		decltype( ddown_to_up( data ) ) down_to_up( ) { return ddown_to_up( data ); }
		void move( direction dir )
		{
			if ( dir == left )
			{
				auto d = dleft_to_right( data );
				std::for_each(
							d.begin( ),
							d.end( ),
							[]( decltype( d[0] ) data ){ square::merge( data.first, data.second, data.first ); } );
			}
			else if ( dir == right )
			{
				auto d = dright_to_left( data );
				std::for_each(
							d.begin( ),
							d.end( ),
							[]( decltype( d[0] ) data ){ square::merge( data.first, data.second, data.first ); } );
			}
			else if ( dir == up )
			{
				auto d = dup_to_down( data );
				std::for_each(
							d.begin( ),
							d.end( ),
							[]( decltype( d[0] ) data ){ square::merge( data.first, data.second, data.first ); } );
			}
			else
			{
				assert( dir ==  down );
				auto d = ddown_to_up( data );
				std::for_each(
							d.begin( ),
							d.end( ),
							[]( decltype( d[0] ) data ){ square::merge( data.first, data.second, data.first ); } );
			}
			random_add( );
		}
		bool can_move( ) { return can_move( up ) || can_move( down ) || can_move( left ) || can_move( right ); }
		bool can_move( direction dir )
		{
			if ( dir == left )
			{
				auto d = dleft_to_right( data );
				return std::find_if(
							d.begin( ),
							d.end( ),
							[]( decltype( d[0] ) data ){ return square::can_merge( data.first, data.second ); } ) != d.end( );
			}
			else if ( dir == right )
			{
				auto d = dleft_to_right( data );
				return std::find_if(
							d.begin( ),
							d.end( ),
							[]( decltype( d[0] ) data ){ return square::can_merge( data.first, data.second ); } ) != d.end( );
			}
			else if ( dir == up )
			{
				auto d = dleft_to_right( data );
				return std::find_if(
							d.begin( ),
							d.end( ),
							[]( decltype( d[0] ) data ){ return square::can_merge( data.first, data.second ); } ) != d.end( );
			}
			else
			{
				assert( dir ==  down );
				auto d = dleft_to_right( data );
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
					<< s.data[0][0] << s.data[0][1] << s.data[0][2] << s.data[0][3] << std::endl
					<< s.data[1][0] << s.data[1][1] << s.data[1][2] << s.data[1][3] << std::endl
					<< s.data[2][0] << s.data[2][1] << s.data[2][2] << s.data[2][3] << std::endl
					<< s.data[3][0] << s.data[3][1] << s.data[3][2] << s.data[3][3] << std::endl;
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
	struct multiply_type
	{
		typedef typename
		boost::mpl::transform
		<
			t1,
			t2,
			boost::mpl::plus< _1, _2 >
		>::type type;
	};
	template< typename t1, typename t2 >
	struct divide_type
	{
		typedef typename boost::mpl::transform
		<
			t1,
			t2,
			boost::mpl::minus< _1, _2 >
		>::type type;
	};
#define DEFINE_MULTIPLY_UNIT( t1, t2, t3 ) typedef multiply_type< t1, t2 >::type t3
#define DEFINE_DIVIDE_UNIT( t1, t2, t3 ) typedef divide_type< t1, t2 >::type t3
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
	typedef divide_type< divide_type< work, mass >::type, temperature >::type specific_heat;
}
#endif //MISC_HPP
