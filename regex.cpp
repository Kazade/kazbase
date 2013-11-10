#include <regex>
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

    result.matched = boost::regex_match(encoded, result.matches, re);

    std::locale::global(old);

    return result;
}

Match search(const Regex& re, const unicode& string) {
    Match result;

    std::locale old;
    std::locale::global(std::locale("en_US.UTF-8"));

    result.matched = boost::regex_search(string.encode(), result.matches, re);

    std::locale::global(old);

    return result;
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
