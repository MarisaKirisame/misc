#ifndef CAN_CALL_HPP
#define CAN_CALL_HPP
#include <boost/mpl/bool.hpp>
namespace misc
{
    template< typename T, typename ... R >
    struct can_call
    {
        template< typename t >
        static constexpr boost::mpl::true_ SFINAE( t, typename std::remove_reference< decltype( std::declval< t >( )( std::declval< R >( )... ) ) >::type * = nullptr )
        { return boost::mpl::true_( ); }
        static constexpr boost::mpl::false_ SFINAE( ... ) { return boost::mpl::false_( ); }
        static constexpr bool value = decltype( SFINAE( std::declval< T >( ) ) )::value;
    };
}
#endif // CAN_CALL_HPP
