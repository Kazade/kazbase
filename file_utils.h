#ifndef KAZBASE_FILE_UTILS_H
#define KAZBASE_FILE_UTILS_H

#include "unicode.h"

namespace file_utils {

unicode read(const unicode& filename);
std::vector<unicode> read_lines(const unicode& filename);

}

#endif
