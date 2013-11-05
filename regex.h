#ifndef REGEX_H_INCLUDED
#define REGEX_H_INCLUDED

#include <boost/regex.hpp>

#include "unicode.h"

namespace regex {

typedef boost::regex Regex;
typedef boost::smatch Match;

Regex compile(const unicode& pattern);
Match match(const Regex& re, const unicode& string);
Match search(const Regex& re, const unicode& string);
unicode escape(const unicode& string);

}

#endif // REGEX_H_INCLUDED
