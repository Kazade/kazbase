#include <boost/algorithm/string.hpp>

#include "string.h"

namespace str {

std::string strip(const std::string& s, const std::string& what) {
    std::string result = s;
    boost::trim_if(result, boost::is_any_of(what));
    return result;
}

std::string lower(const std::string& s) {
    std::string result = s;
    boost::to_lower(result);
    return result;
}

std::string upper(const std::string& s) {
    std::string result = s;
    boost::to_upper(result);
    return result;
}


std::vector<std::string> split(const std::string& s, const std::string& delims, const int num_splits) {
    assert(num_splits == -1 || num_splits > 0);

    std::vector<std::string> result;

    boost::split(result, s, boost::is_any_of(delims), boost::token_compress_on);
    if(num_splits == -1) {
        return result;
    }

    std::vector<std::string> r2;
    for(int i = 0; (i < num_splits + 1 && i < result.size()); ++i) {
        r2.push_back(result.at(i));
    }
    return r2;
}

std::string join(std::set<std::string> parts, std::string joiner) {
    if(parts.empty()) {
        return "";
    }

    std::string final_string = "";
    for(std::string p: parts) {
        final_string += p;
        final_string += joiner;
    }

    return std::string(final_string.begin(), final_string.begin() + (final_string.length() - joiner.length()));
}

std::string join(std::vector<std::string> parts, std::string joiner) {
    if(parts.empty()) {
        return "";
    }

    std::string final_string = "";
    for(std::string p: parts) {
        final_string += p;
        final_string += joiner;
    }

    return std::string(final_string.begin(), final_string.begin() + (final_string.length() - joiner.length()));
}

std::string slice(const std::string& s, const int start_index, const int end_index) {
    int si = start_index;
    if(si < 0) {
        si = s.length() + si; //Negate si
    }

    int ei = end_index;
    if(ei < 0) {
        ei = s.length() + ei; //Negate ei
    }

    assert(si < ei && "Start index is not less than end index");
    assert(si < (int) s.length() && si >= 0);
    assert(ei < (int) s.length() && ei >= 0);

    return std::string(s.begin() + si, s.begin() + ei);
}

bool starts_with(const std::string& s, const std::string& what) {
    return boost::starts_with(s, what);
}

bool ends_with(const std::string& s, const std::string& what) {
    return boost::ends_with(s, what);
}

bool contains(const std::string& s, const std::string& what) {
    return s.find(what) != std::string::npos;
}

std::string replace(const std::string& s, const std::string& to_find, const std::string& to_replace) {
    std::string final_s(s);
    boost::replace_all(final_s, std::string(to_find), std::string(to_replace));
    return final_s;
}

}

