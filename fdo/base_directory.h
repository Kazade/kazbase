#ifndef BASE_DIRECTORY_H_INCLUDED
#define BASE_DIRECTORY_H_INCLUDED

#include <stdexcept>
#include <string>
#include <vector>

namespace fdo {
namespace xdg {

std::string get_data_home();
std::string get_config_home();
std::string get_cache_home();

std::vector<std::string> get_data_dirs();
std::vector<std::string> get_config_dirs();

std::string find_data_file(const std::string& relative_path);
std::string find_config_file(const std::string& relative_path);
std::string find_user_data_file(const std::string& relative_path);
std::string find_user_config_file(const std::string& relative_path);

std::string make_dir_in_data_home(const std::string& folder_name);
std::string make_dir_in_config_home(const std::string& folder_name);

std::string get_or_create_program_cache_path(const std::string& program_name);
std::string get_or_create_program_data_path(const std::string& program_name);

}
}

#endif // BASE_DIRECTORY_H_INCLUDED

