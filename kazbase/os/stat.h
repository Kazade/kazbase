#ifndef KAZBASE_STAT_H
#define KAZBASE_STAT_H

#include <sys/stat.h>
#include <stdexcept>

#include "../unicode.h"
#include "../datetime.h"

namespace os {

class Error : public std::runtime_error {
public:
    Error(int32_t err):
        std::runtime_error(_u("System error: {0}").format(err).encode()),
        err(err) {

    }

    int32_t err;
};

namespace stat {

datetime::DateTime modified_time(const std::string& file_path);

}

struct ::stat lstat(const unicode& path);

}

#endif
