#include <string>
#include <fstream>
#include <streambuf>

#include "exceptions.h"
#include "file_utils.h"

namespace file_utils {

std::vector<unicode> read_lines(const unicode& filename) {
    unicode contents = read(filename);

    return unicode(contents).split("\n");
}

unicode read(const unicode& filename) {

    std::ifstream t(filename.encode().c_str());
    if(!t) {
        throw IOError(_u("Unable to load file") + filename);
    }

    std::string str((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());

    return unicode(str);
}

}
