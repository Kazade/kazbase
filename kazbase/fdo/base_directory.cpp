#include <cstdlib>
#include "base_directory.h"
#include "../os/core.h"
#include "../os/path.h"
#include "../exceptions.h"

namespace fdo {
namespace xdg {

static unicode get_user_home() {
    return os::path::expand_user("~");
}

static unicode get_env_var(const unicode& name) {
    char* env = getenv(name.encode().c_str());
    if(env) {
        return unicode(env);
    }

    return unicode();
}

unicode get_data_home() {
    using os::path::join;

    unicode home = get_env_var("XDG_DATA_HOME");
    if(!home.empty()) {
        return os::path::expand_user(home);
    }

    return join(get_user_home(), join(".local", "share")).encode();
}

/**
    Make a folder in the data home directory, creating the parent
    directories if they don't exist already. Return the full
    final path of the directory.
*/
unicode make_dir_in_data_home(const unicode& folder_name) {
    using os::path::join;
    unicode dest_path = get_data_home();

    //Make the data home folder if it doesn't exist
    //and make it hidden if necessary
    if(!os::path::exists(dest_path)) {
        os::make_dirs(dest_path);
        os::path::hide_dir(dest_path);
    }

    unicode final_path = join(dest_path, folder_name);
    os::make_dirs(final_path);
    return final_path.encode();
}

unicode get_config_home() {
    using os::path::join;

    unicode home = get_env_var("XDG_CONFIG_HOME");
    if(!home.empty()) {
        return os::path::expand_user(home);
    }

    return join(get_user_home(), ".config").encode();
}

/**
    Make a folder in the config home directory, creating the parent
    directories if they don't exist already. Return the full
    final path of the directory.
*/
unicode make_dir_in_config_home(const unicode& folder_name) {
    using os::path::join;
    unicode dest_path = get_config_home();

    //Make the data home folder if it doesn't exist
    //and make it hidden if necessary
    if(!os::path::exists(dest_path)) {
        os::make_dirs(dest_path);
        os::path::hide_dir(dest_path);
    }

    unicode final_path = join(dest_path, folder_name);
    os::make_dirs(final_path);
    return final_path.encode();
}

unicode get_cache_home() {
    using os::path::join;

    unicode home = get_env_var("XDG_CACHE_HOME");
    if(!home.empty()) {
        return os::path::expand_user(home);
    }

    return join(get_user_home(), ".cache").encode();
}

std::vector<unicode> get_data_dirs() {
    unicode data_dir_s = get_env_var("XDG_DATA_DIRS");
    if(data_dir_s.empty()) {
        data_dir_s = "/usr/local/share/:/usr/share/";
    }

    std::vector<unicode> data_dirs = data_dir_s.split(":");
    return data_dirs;
}

std::vector<unicode> get_config_dirs() {
    unicode data_dir_s = get_env_var("XDG_CONFIG_DIRS");
    if(data_dir_s.empty()) {
        data_dir_s = "/etc/xdg";
    }

    std::vector<unicode> data_dirs = data_dir_s.split(":");
    return data_dirs;
}

unicode find_data_file(const unicode& relative_path) {
    std::vector<unicode> data_dirs = get_data_dirs();

    if(os::path::exists(relative_path)) {
        return os::path::abs_path(relative_path);
    }

    for(std::vector<unicode>::iterator it = data_dirs.begin(); it != data_dirs.end(); ++it) {
        unicode final_path = os::path::join((*it), relative_path);
        if(os::path::exists(final_path)) {
            return final_path.encode();
        }
    }

    throw FileNotFoundError(relative_path);
}

unicode find_config_file(const unicode& relative_path) {
    std::vector<unicode> config_dirs = get_config_dirs();
    for(std::vector<unicode>::iterator it = config_dirs.begin(); it != config_dirs.end(); ++it) {
        unicode final = os::path::join((*it), relative_path);
        if(os::path::exists(final)) {
            return final.encode();
        }
    }

    throw FileNotFoundError(relative_path);
}


unicode find_user_data_file(const unicode &relative_path) {
    unicode data_home = get_data_home();
    unicode final = os::path::join(data_home, relative_path);
    if(os::path::exists(final)) {
        return final.encode();
    }

    throw FileNotFoundError(relative_path);
}

unicode find_user_config_file(const unicode& relative_path) {
    unicode config_home = get_config_home();
    unicode final = os::path::join(config_home, relative_path);
    if(os::path::exists(final)) {
        return final.encode();
    }

    throw FileNotFoundError(relative_path);
}

unicode get_or_create_program_cache_path(const unicode& program_name) {
    unicode path = os::path::join(get_cache_home(), program_name);
    if(!os::path::exists(path)) {
        os::make_dirs(path);
    }
    return path.encode();
}

}
}

