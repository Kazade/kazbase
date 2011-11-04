#ifndef KAZBASE_STAT_H
#define KAZBASE_STAT_H

#include "kazbase/datetime.h"

namespace os {
namespace stat {

datetime::DateTime modified_time(const std::string& file_path);

}
}

#endif
