#ifndef AsioHacksFwd_h__
#define AsioHacksFwd_h__

#include <boost/version.hpp>

/**
  Collection of forward declarations to improve compile time
 */
namespace boost
{
    namespace posix_time
    {
        class ptime;
    }

    namespace asio
    {
        template <typename Time>
        struct time_traits;

        namespace ip
        {
            class address;

            class tcp;

            template <typename InternetProtocol>
            class basic_endpoint;

            typedef basic_endpoint<tcp> tcp_endpoint;
        }

#if BOOST_VERSION >= 106600
        template <typename Time, typename TimeTraits>
        class basic_deadline_timer;

        typedef basic_deadline_timer<posix_time::ptime, time_traits<posix_time::ptime>> deadline_timer;

        namespace ip
        {
            template <typename InternetProtocol>
            class basic_resolver;

            typedef basic_resolver<tcp> tcp_resolver;
    }
#else
        template <typename TimeType, typename TimeTraits>
        class deadline_timer_service;

        template <typename Time, typename TimeTraits, typename TimerService>
        class basic_deadline_timer;

        typedef basic_deadline_timer<posix_time::ptime, time_traits<posix_time::ptime>, deadline_timer_service<posix_time::ptime, time_traits<posix_time::ptime>>> deadline_timer;

        namespace ip
        {
            template <typename InternetProtocol>
            class resolver_service;

            template <typename InternetProtocol, typename ResolverService>
            class basic_resolver;

            typedef basic_resolver<tcp, resolver_service<tcp>> tcp_resolver;
        }
#endif
    }
}

    namespace Asio
    {
        class Strand;
    }


#endif // AsioHacksFwd_h__
