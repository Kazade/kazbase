#include <fstream>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>

#include "path.h"

#include "../exceptions.h"
#include "stat.h"

namespace os {
namespace path {

unicode join(const unicode &p1, const unicode &p2) {
    return p1 + OS_SEP + p2;
}

unicode join(const std::vector<unicode>& parts) {
    return unicode(OS_SEP).join(parts);
}

unicode get_cwd(){
    char buf[FILENAME_MAX];
    char* succ = getcwd(buf, FILENAME_MAX);

    if(succ) {
        return unicode(succ);
    }

    throw std::runtime_error("Unable to get the current working directory");
}

unicode abs_path(const unicode& p) {
    unicode path = p;
    if(!is_absolute(p)) {
        unicode cwd = get_cwd();
        path = join(cwd, p);
    }

    return norm_path(path);
}

unicode norm_case(const unicode& path) {
#ifdef __WIN32__
    assert(0);
#endif
    return path;
}

unicode norm_path(const unicode& path) {
    unicode slash = "/";
    unicode dot = ".";

    if(path.empty()) {
        return dot;
    }

    int32_t initial_slashes = path.starts_with(slash) ? 1: 0;
    //POSIX treats 3 or more slashes as a single one
    if(initial_slashes && path.starts_with("//") && !path.starts_with("///")) {
        initial_slashes = 2;
    }

    std::vector<unicode> comps = path.split(slash);
    std::vector<unicode> new_comps;

    for(unicode comp: comps) {
        if(comp.empty() || comp == dot) {
            continue;
        }

        if(comp != _u("..") ||
            (initial_slashes == 0 && new_comps.empty()) ||
            (!new_comps.empty() && new_comps.back() == _u(".."))
            ) {
            new_comps.push_back(comp);
        } else if(!new_comps.empty()) {
            new_comps.pop_back();
        }
    }

    comps = new_comps;
    unicode final_path = slash.join(comps);
    if(initial_slashes) {
        final_path = slash * initial_slashes + final_path;
    }

    return final_path.empty() ? dot : final_path;
}

std::pair<unicode, unicode> split(const unicode& path) {
    unicode::size_type i = path.rfind(OS_SEP) + 1;

    unicode head = path.slice(nullptr, i);
    unicode tail = path.slice(i, nullptr);

    if(!head.empty() && head != OS_SEP * head.length()) {
        head = head.rstrip(OS_SEP);
    }

    return std::make_pair(head, tail);
}

bool exists(const unicode &path) {
    try {
        os::lstat(path);
    } catch(os::Error& e) {
        return false;
    }

    return true;
}

unicode dir_name(const unicode& path) {
    unicode::size_type i = path.rfind(OS_SEP) + 1;
    unicode head = path.slice(nullptr, i);
    if(!head.empty() && head != OS_SEP * head.length()) {
        head = head.rstrip(OS_SEP);
    }
    return head;
}

bool is_absolute(const unicode& path) {
    return path.starts_with("/");
}

bool is_dir(const unicode& path) {
    struct stat st;
    try {
        st = os::lstat(path);
    } catch(os::Error& e) {
        return false;
    }

    return S_ISDIR(st.st_mode);
}

bool is_file(const unicode& path) {
    struct stat st;
    try {
        st = os::lstat(path);
    } catch(os::Error& e) {
        return false;
    }

    return S_ISREG(st.st_mode);
}

bool is_link(const unicode& path) {
    struct stat st;
    try {
        st = os::lstat(path);
    } catch(os::Error& e) {
        return false;
    }

    return S_ISLNK(st.st_mode);
}

unicode real_path(const unicode& path) {
    char *real_path = realpath(path.encode().c_str(), NULL);
    if(!real_path) {
        return unicode();
    }
    unicode result(real_path);
    free(real_path);
    return result;
}

unicode rel_path(const unicode& path, const unicode& start) {
    if(path.empty()) {
        throw ValueError("No path specified");
    }

    auto start_list = os::path::abs_path(start).split(OS_SEP, -1, false);
    auto path_list = os::path::abs_path(path).split(OS_SEP, -1, false);

    int i = common_prefix(start_list, path_list).size();

    unicode pardir = "..";

    std::vector<unicode> result;
    for(int j = 0; j < (start_list.size() - i); ++j) {
        result.push_back(pardir);
    }

    result.insert(result.end(), path_list.begin() + i, path_list.end());
    if(result.empty()) {
        return ".";
    }

    return os::path::join(result);
}

static unicode get_env_var(const unicode& name) {
    char* env = getenv(name.encode().c_str());
    if(env) {
        return unicode(env);
    }

    return unicode();
}

unicode expand_user(const unicode& path) {
    unicode cp = path;

    if(!path.starts_with("~")) {
        return path;
    }

    unicode home = get_env_var("HOME");

    if(home.empty()) {
        return path;
    }

    return cp.replace("~", home);
}

void hide_dir(const unicode &path) {
#ifdef WIN32
    assert(0 && "Not Implemented");
#else
    //On Unix systems, prefix with a dot
    std::pair<unicode, unicode> parts = os::path::split(path);
    unicode final = parts.first + "." + parts.second;
    if(::rename(path.encode().c_str(), final.encode().c_str()) != 0) {
        throw os::Error(errno);
    }
#endif

}

std::vector<unicode> list_dir(const unicode& path) {
    std::vector<unicode> result;
#ifdef WIN32
    assert(0 && "Not implemented");
#else
    if(!is_dir(path)) {
        throw os::Error(errno);
    }

    DIR* dirp = opendir(path.encode().c_str());
    dirent* dp = nullptr;

    while((dp = readdir(dirp)) != nullptr) {
        result.push_back(dp->d_name);
    }
    closedir(dirp);
#endif
    return result;
}

std::string read_file_contents(const unicode& path) {
    std::ifstream t(path.encode());
    std::string str((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());

    return str;
}

unicode exe_path() {
#ifdef WIN32
    assert(0 && "Not implemented");
#else
    char buff[1024];
    ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff)-1);
    if(len != -1) {
        buff[len] = '\0';
        return unicode(buff);
    }

    throw RuntimeError("Unable to work out the program filename");
#endif
}

unicode exe_dirname() {
    unicode path = exe_path();
    return os::path::dir_name(path);
}

std::pair<unicode, unicode> split_ext(const unicode& path) {
    int64_t sep_index = (int64_t) path.rfind(OS_SEP);
    int64_t dot_index = (int64_t) path.rfind(".");

    if(sep_index == ustring::npos) {
        sep_index = -1;
    }

    if(dot_index > sep_index) {
        auto filename_index = sep_index + 1;

        while(filename_index < dot_index) {
            if(path[filename_index] != '.') {
                return std::make_pair(
                    path.slice(nullptr, dot_index),
                    path.slice(dot_index, nullptr)
                );
            }
            filename_index += 1;
        }
    }

    return std::make_pair(path, _u(""));
}


}
}

