#include "regex.h"
#include "unicode.h"

namespace regex {

Regex compile(const unicode& pattern) {
    return Regex(pattern.encode());
}

Match match(const Regex& re, const unicode& string) {
    Match result;

    std::locale old;
    std::locale::global(std::locale("en_US.UTF-8"));

    std::string encoded = string.encode();

    boost::match_results<std::string::const_iterator> matches;

    result.matched = boost::regex_match(encoded, matches, re._impl);

    for(auto match: matches) {
        unicode match_string(match.first, match.second);
        std::cout << "SM: " << match_string << std::endl;
        result.groups.push_back(match_string);
        result.matched = true;
    }

    std::locale::global(old);

    return result;
}

std::vector<Match> search(const Regex& re, const unicode& string) {
    std::vector<Match> results;

    std::locale old;
    std::locale::global(std::locale("en_US.UTF-8"));

    auto to_search = string.encode();

    std::string::const_iterator start, end;
    start = to_search.begin();
    end = to_search.end();

    boost::match_results<std::string::const_iterator> matches;

    while(boost::regex_search(start, end, matches, re._impl)) {
        Match new_match;
        for(auto match: matches) {
            std::string match_string(match.first, match.second);
            unicode group(match_string);
            new_match.groups.push_back(group);
            new_match.matched = true;
        }

        start = matches[0].second;

        results.push_back(new_match);
    }

    std::locale::global(old);

    return results;
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
