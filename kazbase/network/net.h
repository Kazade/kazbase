#ifndef NET_H_INCLUDED
#define NET_H_INCLUDED

#include <boost/thread/future.hpp>

namespace network {
namespace net {
    bool is_connected();
    boost::shared_future<bool> is_connected_async();
}
}

#endif // NET_H_INCLUDED
