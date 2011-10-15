#include <boost/regex.hpp>
#include "kazbase/logging/logging.h"
#include "regex.h"

namespace regex {

Match find(const Regex& regex,
             const std::string& text,
             const unsigned int start) {

    boost::regex ex(regex);
    boost::match_results<std::string::const_iterator> what;

    if(boost::regex_search(text.begin() + start, text.end(), what, ex, boost::match_default)) {
        Match new_match;
        new_match.start_pos = what[0].first - text.begin();
        new_match.end_pos = what[0].second - text.begin();
        std::string full_match(what[0].first, what[0].second);
        new_match.groups.push_back(full_match);
        return new_match;
    }
    return Match();
}

std::vector<Match> find_all(const Regex& regex, const std::string& text) {
    unsigned int start = 0;
    std::vector<Match> results;
    while(start < text.size()) {
        Match m = find(regex, text, start);
        if(!m) {
            break;
        }
        results.push_back(m);
        start = m.end_pos;
    }

    return results;
}

}
