#ifndef APPLICATIONS_H_INCLUDED
#define APPLICATIONS_H_INCLUDED

#include <vector>
#include <string>
#include <stdexcept>
#include <memory>

#include "../config/config_reader.h"

namespace fdo {
namespace apps {

class DesktopFile {
public:
    DesktopFile(const unicode& path);
    void save();
    void save_as(const unicode& filename);

    config::ConfigReader& get_config() { return *config_; }
    unicode get_file_path() const { return path_; }
private:
    unicode path_;
    std::shared_ptr<config::ConfigReader> config_;
};

std::vector<unicode> get_app_names();
std::vector<DesktopFile> get_desktop_files();
DesktopFile find_desktop_file(const unicode& app);

}
}

#endif // APPLICATIONS_H_INCLUDED

