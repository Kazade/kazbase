#include <map>

#include "regex.h"
#include "glob.h"
#include "os.h"

namespace glob {

static std::map<unicode, regex::Regex> _cache;
static const int _MAX_CACHE = 100;

bool match(const unicode& name, const unicode& pattern) {
    return match_cs(os::path::norm_case(name), os::path::norm_case(pattern));
}

unicode _translate(const unicode& pat) {
    int i = 0;
    int n = pat.length();

    unicode res;

    while(i < n) {
        auto c = pat[i];
        ++i;

        if(c == '*') {
            res += ".*";
        } else if(c == '?') {
            res += ".";
        } else if(c == '[') {
            auto j = i;
            if(j < n && pat[j] == '!') {
                j++;
            }
            if(j < n && pat[j] == ']') {
                j++;
            }
            while(j < n && pat[j] != ']') {
                j++;
            }
            if(j >= n) {
                res += "\\[";
            } else {
                unicode stuff = pat.slice(i, j).replace("\\", "\\\\");
                i = j + 1;
                if(stuff[0] == '!') {
                    stuff = _u("^") + stuff.slice(1, nullptr);
                } else if(stuff[0] == '^') {
                    stuff = _u("\\") + stuff;
                }

                res = _u("{0}[{1}]").format(res, stuff);
            }
        } else {
            res += regex::escape(_u(1, c));
        }
    }

    return res;
}

std::vector<unicode> filter(const std::vector<unicode>& names, const unicode& pat) {
    if(_cache.find(pat) == _cache.end()) {
        auto res = _translate(pat);
        if(_cache.size() >= _MAX_CACHE) {
            _cache.clear();
        }

        _cache[pat] = regex::compile(res);
    }

    auto re = _cache[pat];

    auto results = std::vector<unicode>();

    for(auto name: names) {
        auto match = regex::match(re, name);

        if(match.matched) {
            results.push_back(name);
        }
    }

    return results;
}

bool match_cs(const unicode& name, const unicode& pat) {
    if(_cache.find(pat) == _cache.end()) {
        auto res = _translate(pat);
        if(_cache.size() >= _MAX_CACHE) {
            _cache.clear();
        }
        _cache[pat] = regex::compile(res);
    }

    auto match = regex::match(_cache[pat], name);

    return match.matched;
}

}
