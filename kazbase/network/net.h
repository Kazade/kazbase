#ifndef NET_H_INCLUDED
#define NET_H_INCLUDED

#include <future>

namespace network {
namespace net {
    bool is_connected();
    std::shared_future<bool> is_connected_async();
}
}

#endif // NET_H_INCLUDED
