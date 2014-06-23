#ifndef REGEX_H_INCLUDED
#define REGEX_H_INCLUDED

#include <boost/regex.hpp>

#include "unicode.h"

#include "depends/pcre/pcre.h"

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
    Regex(const unicode& str);
    Regex(const Regex& rhs);

    const Regex& operator=(const Regex& rhs) {
        if(&rhs == this) {
            return *this;
        }

        original_ = rhs.original_;
        init();
        return *this;
    }

    ~Regex();

    REMatch match(const unicode& str, uint32_t start=0) const;
    std::vector<REMatch> search(const unicode& str) const;

private:
    unicode original_;
    pcre* _impl = nullptr;

    void init();
};

unicode escape(const unicode& string);

}

#endif // REGEX_H_INCLUDED
