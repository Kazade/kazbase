#include <sys/stat.h>
#include "stat.h"

namespace os {

struct ::stat lstat(const unicode& path) {
    struct ::stat result;
    if(::lstat(path.encode().c_str(), &result) == -1) {
        throw os::Error(errno);
    }
    return result;
}

namespace stat {

datetime::DateTime modified_time(const std::string& file_path) {
    struct ::stat st = lstat(file_path);
    return boost::posix_time::from_time_t(st.st_mtime);
}

}
}

