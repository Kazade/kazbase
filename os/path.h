#ifndef KAZBASE_PATH_H
#define KAZBASE_PATH_H

#include <vector>
#include <utility>
#include "../unicode.h"

namespace os {

#ifdef WIN32
    #define OS_SEP _u("\\")
#else
    #define OS_SEP _u("/")
#endif

namespace path {

unicode join(const unicode& p1, const unicode& p2);

unicode abs_path(const unicode& p);
unicode norm_path(const unicode& path);
std::pair<unicode, unicode> split(const unicode &path);
bool exists(const unicode& path);
unicode dir_name(const unicode &path);
bool is_absolute(const unicode& path);
bool is_dir(const unicode& path);
bool is_file(const unicode& path);
bool is_link(const unicode& path);

unicode real_path(const unicode& path);

void hide_dir(const unicode& path);
std::pair<unicode, unicode> split_ext(const unicode& path);
unicode rel_path(const unicode& path, const unicode& start=unicode());
unicode expand_user(const unicode& path);
std::vector<unicode> list_dir(const unicode& path);

std::string read_file_contents(const unicode& path);
unicode exe_path();
unicode exe_dirname();
unicode get_cwd();
std::pair<unicode, unicode> split_ext(const unicode& path);

}
}

#endif
