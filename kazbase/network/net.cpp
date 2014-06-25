#include "../logging.h"
#include "../thread/future.h"

namespace network {
namespace net {
/*
    Look for Google, if there is no reply then we have no internet,
    or the DNS isn't working or something. (or, of course Google is down...)
*/
bool is_connected() {
    L_DEBUG("Checking internet connection");

    int failed = system("ping -c 1 www.google.com");
    if(failed) {
        L_WARN("Internet is not connected");
        return false;
    }

    return true;
}

std::shared_future<bool> is_connected_async() {
    return thread::submit_task(&is_connected);
}

}
}
