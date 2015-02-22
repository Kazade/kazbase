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

    std::ifstream in(filename.encode().c_str());
    if(!in) {
        throw IOError(_u("Unable to load file") + filename);
    }

    auto str = [&in]{
      std::ostringstream ss{};
      ss << in.rdbuf();
      return ss.str();
    }();

    return unicode(str);
}

}
