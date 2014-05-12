#ifndef REGEX_H_INCLUDED
#define REGEX_H_INCLUDED

#include <boost/regex.hpp>

#include "unicode.h"

namespace regex {

class Regex;

struct REMatch {
    explicit operator bool() const {
        return !groups_.empty();
    }

    int start(int group=0);
    int end(int group=0);
    std::pair<int, int> span(int group=0);

    std::vector<unicode> groups() const;
    unicode group(uint32_t i=0) const;

private:
    struct Group {
        unicode str;
        int start;
        int end;
    };

    std::vector<Group> groups_;

    friend class Regex;
};

struct Regex {
public:
    Regex() = default;

    Regex(const unicode& str) {
        _impl = boost::regex(str.encode());
    }

    boost::regex _impl;    

    REMatch match(const unicode& str, uint32_t start=0) const;
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
