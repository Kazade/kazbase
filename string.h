
#ifndef STR_H
#define STR_H

#include <limits>
#include <vector>
#include <string>

namespace str {

std::string strip(const std::string& s);
std::string lower(const std::string& s);
std::string upper(const std::string& s);

std::string join(std::vector<std::string> parts, std::string joiner);
std::vector<std::string> split(const std::string& s, const std::string& delims=" ", const int num_splits=-1);
std::string slice(const std::string& s, const int start_index=0, const int end_index=std::numeric_limits<int>::max());

bool starts_with(const std::string& s, const std::string& what);
bool ends_with(const std::string& s, const std::string& what);
bool contains(const std::string& s, const std::string& what);

std::string replace(const std::string& s, const std::string& old, const std::string& new_s);

}

#endif

