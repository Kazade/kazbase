#define BOOST_FILESYSTEM_VERSION 3

#define USE_FILESYSTEM_THREE

#include <boost/filesystem.hpp>
#include <fstream>
#include "path.h"

#include "kazbase/exceptions.h"
#include "kazbase/string.h"

namespace os {
namespace path {

std::string join(const std::string& p1, const std::string& p2) {
    return p1 + OS_SEP + p2;
}

std::string abs_path(const std::string& p) {
#ifdef USE_FILESYSTEM_THREE
        return boost::filesystem3::complete(p).normalize().string();
#else
        return boost::filesystem::complete(p).normalize().string();
#endif
}

std::pair<std::string, std::string> split(const std::string& path) {
    boost::filesystem::path p(path);
#ifdef USE_FILESYSTEM_THREE
    return std::make_pair(p.parent_path().string(), p.filename().string());
#else
    return std::make_pair(p.branch_path().string(), p.leaf());
#endif
}

bool exists(const std::string& path) {
    return boost::filesystem::exists(path);
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


}
}

