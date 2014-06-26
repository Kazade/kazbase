#include <fstream>
#include <ctime>
#include <utime.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

#include "core.h"
#include "../exceptions.h"
#include "path.h"


namespace os {

void touch(const unicode& path) {
    if(!os::path::exists(path)) {
        make_dirs(os::path::dir_name(path));

        std::ofstream file(path.encode().c_str());
        file.close();
    }

    struct utimbuf new_times;
    struct stat st = os::lstat(path);

    new_times.actime = st.st_atime;
    new_times.modtime = time(NULL);
    utime(path.encode().c_str(), &new_times);
}

void make_dir(const unicode& path, mode_t mode) {
    if(os::path::exists(path)) {
        throw os::Error(EEXIST);
    } else {
        if(mkdir(path.encode().c_str(), mode) != 0) {
            throw os::Error(errno);
        }
    }
}

void make_dirs(const unicode &path, mode_t mode) {
    std::pair<unicode, unicode> res = os::path::split(path);

    unicode head = res.first;
    unicode tail = res.second;

    if(tail.empty()) {
        res = os::path::split(head);
        head = res.first;
        tail = res.second;
    }

    if(!head.empty() && !tail.empty() && !os::path::exists(head)) {
        try {
            make_dirs(head, mode);
        } catch(os::Error& e) {
            if(e.err != EEXIST) {
                //Someone already created the directory
                return;
            }
        }
        if(tail == ".") {
            return;
        }
    }

    make_dir(path, mode);
}

void remove(const unicode& path) {
    if(os::path::exists(path)) {
        if(os::path::is_dir(path)) {
            throw IOError("Tried to remove a folder, use remove_dir instead");
        } else {
            ::remove(path.encode().c_str());
        }
    }
}

void remove_dir(const unicode& path) {
    if(!os::path::exists(path)) {
        throw IOError("Tried to remove a non-existent path");
    }

    if(os::path::list_dir(path).empty()) {
        if(rmdir(path.encode().c_str()) != 0) {
            throw os::Error(errno);
        }
    } else {
        throw IOError("Tried to remove a non-empty directory");
    }
}

void remove_dirs(const unicode& path) {
    if(!os::path::exists(path)) {
        throw IOError("Tried to remove a non-existent path");
    }

    for(unicode f: os::path::list_dir(path)) {
        unicode full = os::path::join(path, f);
        if(os::path::is_dir(full)) {
            remove_dirs(full);
            remove_dir(full);
        } else {
            remove(full);
        }
    }
}

void rename(const unicode& old, const unicode& new_path) {
    if(::rename(old.encode().c_str(), new_path.encode().c_str()) != 0) {
        throw os::Error(errno);
    }
}

std::string temp_dir() {
#ifdef WIN32
    assert(0 && "NotImplemented");
#endif
    return "/tmp";
}

}

