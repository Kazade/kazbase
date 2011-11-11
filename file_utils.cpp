#include <string>
#include <fstream>
#include <streambuf>

#include "file_utils.h"

namespace file_utils {

std::string read_contents(const std::string& filename) {

    std::ifstream t(filename.c_str());
    std::string str((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());

    return str;
}

}
