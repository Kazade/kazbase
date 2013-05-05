#ifndef APPLICATIONS_H_INCLUDED
#define APPLICATIONS_H_INCLUDED

#include <vector>
#include <string>
#include <stdexcept>
#include <tr1/memory>

#include "../config/config_reader.h"

namespace fdo {
namespace apps {

class DesktopFile {
public:
    DesktopFile(const std::string& path);
    void save();
    void save_as(const std::string& filename);

    config::ConfigReader& get_config() { return *config_; }
    std::string get_file_path() const { return path_; }
private:
    std::string path_;
    std::shared_ptr<config::ConfigReader> config_;
};

std::vector<std::string> get_app_names();
std::vector<DesktopFile> get_desktop_files();
DesktopFile find_desktop_file(const std::string& app);

}
}

#endif // APPLICATIONS_H_INCLUDED

