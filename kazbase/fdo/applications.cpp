#include <cassert>

#include "applications.h"
#include "../config/config_reader.h"
#include "base_directory.h"
#include "../os/path.h"

namespace fdo {
namespace apps {

DesktopFile::DesktopFile(const unicode& filename):
    path_(filename),
    config_(new config::ConfigReader(filename)) {

}

void DesktopFile::save() {
    config_->save(path_);
}

void DesktopFile::save_as(const unicode& path) {
    config_->save(path);
}

std::vector<unicode> get_app_names() {
    assert(0);
}

std::vector<DesktopFile> get_desktop_files() {
    assert(0);
}

DesktopFile find_desktop_file(const unicode& app) {
    //First, look in the current directory
    if(os::path::exists(app + ".desktop")) {
        return DesktopFile(os::path::abs_path(_u("./") + app + _u(".desktop")));
    }

    unicode rel_path = os::path::join("applications", app + ".desktop");

    //Let FileNotFoundError propagate
    unicode desktop_file = fdo::xdg::find_data_file(rel_path.encode());

    return DesktopFile(desktop_file);
}

}
}

