#define BOOST_FILESYSTEM_VERSION 3

#define USE_FILESYSTEM_THREE

#include <boost/filesystem.hpp>
#include <fstream>
#include "path.h"

#include "../exceptions.h"
#include "../string.h"

namespace os {
namespace path {

unicode join(const unicode &p1, const unicode &p2) {
    return p1 + OS_SEP + p2;
}

unicode abs_path(const unicode& p) {
    return abs_path(p.encode());
}

std::string abs_path(const std::string& p) {
    return boost::filesystem::complete(p).normalize().string();
}

std::pair<std::string, std::string> split(const std::string& path) {
    boost::filesystem::path p(path);
#ifdef USE_FILESYSTEM_THREE
    return std::make_pair(p.parent_path().string(), p.filename().string());
#else
    return std::make_pair(p.branch_path().string(), p.leaf());
#endif
}

bool exists(const unicode &path) {
    return boost::filesystem::exists(path.encode());
}

std::string dir_name(const std::string& path) {
    return boost::filesystem::path(path).branch_path().string();
}

bool is_absolute(const std::string& path) {
    return boost::filesystem::path(path).is_complete();
}

bool is_dir(const std::string& path) {
    return boost::filesystem::is_directory(path);
}

bool is_file(const std::string& path) {
    return boost::filesystem::is_regular_file(path);
}

bool is_link(const std::string& path) {
    return boost::filesystem::is_symlink(path);
}

std::pair<std::string, std::string> split_ext(const std::string& path) {
    boost::filesystem::path bp(path);
    std::string name = bp.stem().string();
    std::string ext = bp.extension().string();
    return std::make_pair(name, ext);
}

std::string rel_path(const std::string& path, const std::string& start) {
    assert(0);
}

static std::string get_env_var(const std::string& name) {
    char* env = getenv(name.c_str());
    if(env) {
        return std::string(env);
    }

    return std::string();
}

std::string expand_user(const std::string& path) {
    std::string cp = path;

    if(!str::starts_with(path, "~")) {
        return path;
    }

    std::string home = get_env_var("HOME");

    if(home.empty()) {
        return path;
    }

    return str::replace(cp, "~", home);
}

void hide_dir(const std::string& path) {
#ifdef WIN32
    assert(0 && "Not Implemented");
#else
    //On Unix systems, prefix with a dot
    std::pair<std::string, std::string> parts = os::path::split(path);
    std::string final = parts.first + "." + parts.second;
    boost::filesystem::rename(path, final);
#endif

}

std::vector<std::string> list_dir(const std::string& path) {
    std::vector<std::string> results;

    boost::filesystem::directory_iterator end;
    for(boost::filesystem::directory_iterator it(path); it != end; ++it) {
#ifdef USE_FILESYSTEM_THREE
        results.push_back((*it).path().filename().string());
#else
        results.push_back((*it).path().filename());
#endif
    }

    return results;
}

std::string read_file_contents(const std::string& path) {
    std::ifstream t(path);
    std::string str((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());

    return str;
}

std::string exe_path() {
#ifdef WIN32
    assert(0 && "Not implemented");
#else
    char buff[1024];
    ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff)-1);
    if (len != -1) {
        buff[len] = '\0';
        return os::path::dir_name(std::string(buff));
    } else {
        throw RuntimeError("Unable to work out the program path");
    }
   
#endif 
}

std::string working_directory() {
    char buff[PATH_MAX];
    getcwd(buff, PATH_MAX);
    std::string cwd(buff);
    return cwd;
}

std::pair<unicode, unicode> split_ext(const unicode& path) {
    size_t sep_index = path.rfind(OS_SEP);
    size_t dot_index = path.rfind(".");

    if(dot_index > sep_index) {
        size_t filename_index = sep_index + 1;
        while(filename_index < dot_index) {
            if(path[filename_index] != '.') {
                return std::make_pair(
                    path.slice(nullptr, dot_index),
                    path.slice(dot_index, nullptr)
                );
            }
        }
    }

    return std::make_pair(path, _u(""));
}

}
}

