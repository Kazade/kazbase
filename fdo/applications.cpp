
#include "applications.h"
#include "kazbase/config/config_reader.h"
#include "kazbase/fdo/base_directory.h"
#include "kazbase/os/path.h"

namespace fdo {
namespace apps {

DesktopFile::DesktopFile(const std::string& filename):
    path_(filename),
    config_(new config::ConfigReader(filename)) {

}

void DesktopFile::save() {
    config_->save(path_);
}

void DesktopFile::save_as(const std::string& path) {
    config_->save(path);
}

std::vector<std::string> get_app_names() {
    assert(0);
}

std::vector<DesktopFile> get_desktop_files() {
    assert(0);
}

DesktopFile find_desktop_file(const std::string& app) {
    //First, look in the current directory
    if(os::path::exists(app + ".desktop")) {
        return DesktopFile(os::path::abs_path("./" + app + ".desktop"));
    }

    std::string rel_path = os::path::join("applications", app + ".desktop");

    //Let FileNotFoundError propagate
    std::string desktop_file = fdo::xdg::find_data_file(rel_path);

    return DesktopFile(desktop_file);
}

}
}

