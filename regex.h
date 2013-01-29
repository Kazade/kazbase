#ifndef REGEX_H_INCLUDED
#define REGEX_H_INCLUDED

#include <utility>
#include <string>

namespace regex {

class Match {
public:
    std::string::size_type start_pos;
    std::string::size_type end_pos;
    std::vector<std::string> groups;

    Match():
        start_pos(std::string::npos),
        end_pos(std::string::npos) {}

    bool operator==(const Match& rhs) const {
        return start_pos == rhs.start_pos &&
               end_pos == rhs.end_pos &&
               groups == rhs.groups;
    }

    bool operator!=(const Match& rhs) const {
        return !(*this == rhs);
    }

    operator bool() const {
        return start_pos != std::string::npos || end_pos != std::string::npos;
    }
};


typedef std::string Regex;

Match find(const Regex& regex,
                const std::string& text,
                const unsigned int start=0);

std::vector<Match> find_all(const Regex& regex, const std::string& text);

}

#endif // REGEX_H_INCLUDED
