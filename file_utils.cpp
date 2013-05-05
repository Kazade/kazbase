#include <string>
#include <fstream>
#include <streambuf>

#include "exceptions.h"
#include "file_utils.h"

namespace file_utils {

std::string read(const std::string& filename) {

    std::ifstream t(filename.c_str());
    if(!t) {
        throw IOError("Unable to load file" + filename);
    }

    std::string str((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());

    return str;
}

}
