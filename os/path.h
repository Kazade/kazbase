#ifndef KAZBASE_PATH_H
#define KAZBASE_PATH_H

#include <vector>
#include <string>
#include <utility>

namespace os {

#ifdef WIN32
    #define OS_SEP "\\"
#else
    #define OS_SEP "/"
#endif

namespace path {

std::string join(const std::string& p1, const std::string& p2);
std::string abs_path(const std::string& p);
std::pair<std::string, std::string> split(const std::string& path);
bool exists(const std::string& path);
std::string dir_name(const std::string& path);
bool is_absolute(const std::string& path);
bool is_dir(const std::string& path);
bool is_file(const std::string& path);
bool is_link(const std::string& path);
void hide_dir(const std::string& path);
std::pair<std::string, std::string> split_ext(const std::string& path);
std::string rel_path(const std::string& path, const std::string& start=std::string());
std::string expand_user(const std::string& path);
std::vector<std::string> list_dir(const std::string& path);

std::string read_file_contents(const std::string& path);

}
}

#endif
