#include <boost/filesystem.hpp>

#include "stat.h"

namespace os {
namespace stat {

datetime::DateTime modified_time(const std::string& file_path) {
    std::time_t modified = boost::filesystem::last_write_time(file_path);
    return boost::posix_time::from_time_t(modified);
}

}
}

