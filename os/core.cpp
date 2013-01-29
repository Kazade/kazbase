#include <boost/filesystem.hpp>
#include <fstream>

#include "core.h"
#include "../exceptions.h"
#include "path.h"

namespace os {

void touch(const std::string& path) {
    if(!os::path::exists(path)) {
        make_dirs(os::path::dir_name(path));

        std::ofstream file(path.c_str());
        file.close();
    }

    std::time_t n = std::time(0);
    boost::filesystem::last_write_time(path, n);
}

void make_dirs(const std::string& path) {
    boost::filesystem::create_directories(path);
}

bool delete_path(const std::string& path, bool recursive, bool fail_silently) {
    if(!os::path::exists(path)) {
        if(!fail_silently) {
            throw IOError("Tried to remove non-existent path");
        }
        return false;
    }

    //If we are not deleting recursively, we need to bail if this is a
    //directory with existing contents
    if(!recursive && os::path::is_dir(path)) {
        if(!os::path::list_dir(path).empty()) {
            if(!fail_silently) {
                throw IOError("Cannot remove directory as it is not empty");
            }
            return false;
        }
    }

    boost::filesystem::remove_all(path);

    return true;
}

std::string temp_dir() {
#ifdef WIN32
    assert(0 && "NotImplemented");
#endif
    return "/tmp";
}

}

