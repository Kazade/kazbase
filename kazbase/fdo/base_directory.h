#ifndef BASE_DIRECTORY_H_INCLUDED
#define BASE_DIRECTORY_H_INCLUDED

#include <stdexcept>
#include <vector>

#include "../unicode.h"

namespace fdo {
namespace xdg {

unicode get_data_home();
unicode get_config_home();
unicode get_cache_home();

std::vector<unicode> get_data_dirs();
std::vector<unicode> get_config_dirs();

unicode find_data_file(const unicode& relative_path);
unicode find_config_file(const unicode& relative_path);
unicode find_user_data_file(const unicode& relative_path);
unicode find_user_config_file(const unicode& relative_path);

unicode make_dir_in_data_home(const unicode& folder_name);
unicode make_dir_in_config_home(const unicode& folder_name);

unicode get_or_create_program_cache_path(const unicode& program_name);
unicode get_or_create_program_data_path(const unicode& program_name);

}
}

#endif // BASE_DIRECTORY_H_INCLUDED

