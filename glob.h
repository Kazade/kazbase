#ifndef FNMATCH_H
#define FNMATCH_H

#include <vector>
#include "unicode.h"

namespace glob {

bool match(const unicode& name, const unicode& pattern);
std::vector<unicode> filter(const std::vector<unicode>& names, const unicode& pattern);
bool match_cs(const unicode& name, const unicode& pattern);

}

#endif // FNMATCH_H
