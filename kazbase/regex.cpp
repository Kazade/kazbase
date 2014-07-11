#include "regex.h"
#include "unicode.h"
#include "exceptions.h"

//Temporarily using PCRE until the Android NDK, GCC and Clang support std::regex
#include <pcre.h>

const int OVECCOUNT = 30;

namespace regex {

Regex::Regex(const unicode& str):
    original_(str),
    _impl(nullptr){

    init();
}

Regex::Regex(const Regex &rhs):
    original_(rhs.original_),
    _impl(nullptr) {

    init();
}

void Regex::init() {
    const char* error = nullptr;
    int error_offset;

    _impl = pcre_compile(
        original_.encode().c_str(),
        0,
        &error,
        &error_offset,
        nullptr
    );


    if(!_impl) {
        throw ValueError(_u("Unable to compile regular expression. {}").format(error));
    }
}

Regex::~Regex() {
    if(_impl) {
        pcre_free(_impl);
    }
    _impl = nullptr;
}

Regex compile(const unicode& pattern) {
    return Regex(pattern.encode());
}

REMatch Regex::match(const unicode &str, uint32_t start_pos) const {
    int ovector[OVECCOUNT];
    std::string s = str.encode();
    const char* source = s.c_str();
    int rc = pcre_exec(_impl, nullptr, source, s.length(), start_pos, 0, ovector, OVECCOUNT);

    REMatch result;

    if(rc < 0) {
        switch(rc) {
            case PCRE_ERROR_NOMATCH:
                return result;
            default:
                throw ValueError("Error matching pattern");
        }
    }

    for(int i = 0; i < rc; ++i) {
        int start = ovector[2 * i];
        int end = ovector[2 * i + 1];

        const char* substr_start = source + start;
        int substr_length = end - start;

        if(!substr_length) {
            //I'm not sure this is correct. This shouldn't happen should it?
            continue;
        }

        unicode match(substr_start, substr_start + substr_length);

        result.groups_.push_back(
            REMatch::Group({
               match,
               start,
               end
            })
        );
    }

    return result;
}

std::vector<REMatch> Regex::search(const unicode& str) const {
    std::vector<REMatch> matches;

    uint32_t start = 0;
    while(REMatch m = match(str, start)) {
        matches.push_back(m);
        start = (uint32_t) m.end();
    }

    return matches;
}

int REMatch::start(int group) {
    return groups_.at(group).start;
}

int REMatch::end(int group) {
    return groups_.at(group).end;
}

std::pair<int, int> REMatch::span(int group) {
    return std::make_pair(start(group), end(group));
}

std::vector<unicode> REMatch::groups() const {
    std::vector<unicode> ret;
    for(auto g: groups_) {
        ret.push_back(g.str);
    }
    return ret;
}

unicode REMatch::group(uint32_t i) const {
    return groups_.at(i).str;
}

unicode escape(const unicode& string) {
    std::vector<unicode> to_replace = { "\\", "{", "}", "^", "$", "|", "(", ")", "[", "]", "*", "+", "?", "." };

    //Could be faster!
    unicode result = string;
    for(auto u: to_replace) {
        result = result.replace(u, _u("\\") + u);
    }

    return result;
}

}
