#ifndef REGEX_H_INCLUDED
#define REGEX_H_INCLUDED

#include <boost/regex.hpp>

#include "unicode.h"

namespace regex {

struct Regex {
public:
    Regex() = default;

    Regex(const unicode& str) {
        _impl = boost::regex(str.encode());
    }

    boost::regex _impl;
};

class Match {
public:
    std::vector<unicode> groups;
    bool matched;

    unicode group(const int i) {
        if(i >= groups.size()) {
            return unicode();
        }

        auto match = groups[i];
        return match;
    }
};


Regex compile(const unicode& pattern);
Match match(const Regex& re, const unicode& string);
std::vector<Match> search(const Regex& re, const unicode& string);
unicode escape(const unicode& string);

}

#endif // REGEX_H_INCLUDED
