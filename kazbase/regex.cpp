#include "regex.h"
#include "unicode.h"

namespace regex {

Regex compile(const unicode& pattern) {
    return Regex(pattern.encode());
}

REMatch Regex::match(const unicode &str, uint32_t start) const {
    REMatch result;

    std::locale old;
    std::locale::global(std::locale("en_US.UTF-8"));

    std::string encoded = str.encode();

    boost::match_results<std::string::iterator> matches;

    boost::regex_match(encoded.begin(), encoded.end(), matches, _impl);

    for(auto match: matches) {
        unicode match_string(match.first, match.second);

        result.groups_.push_back(REMatch::Group({
            match_string,
            std::distance(encoded.begin(), match.first),
            std::distance(encoded.begin(), match.second)
        }));
    }

    std::locale::global(old);
    return result;
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
