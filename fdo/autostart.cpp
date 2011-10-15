#include <iostream>
#include "autostart.h"
#include "kazbase/os/path.h"
#include "kazbase/fdo/base_directory.h"
#include "kazbase/exceptions.h"
#include "kazbase/fdo/applications.h"

ApplicationAutostart::ApplicationAutostart(const std::string& name):
    desktop_file_(fdo::apps::find_desktop_file(name)) {
}

static std::string find_user_autostart(fdo::apps::DesktopFile desktop_file) {
    std::string desktop_file_name = os::path::split(desktop_file.get_file_path()).second;
    std::string rel_path = os::path::join("autostart", desktop_file_name);
    std::string file_path = fdo::xdg::find_user_config_file(rel_path);
    return file_path;
}

bool ApplicationAutostart::is_enabled_for_user() const {
    std::string file_path;
    try {
        file_path = find_user_autostart(desktop_file_);
    } catch(FileNotFoundError& e) {
        return false;
    }

    //Read the found autostart desktop file
    fdo::apps::DesktopFile auto_start_file(file_path);
    //Return true if the Hidden entry does not exist, or it exists but is false
    return !auto_start_file.get_config().get_setting<bool>("Desktop Entry", "Hidden");
}

bool ApplicationAutostart::is_enabled() const {
    std::string file_path;
    std::string desktop_file_name = os::path::split(desktop_file_.get_file_path()).second;
    std::string rel_path = os::path::join("autostart", desktop_file_name);
    try {
        file_path = fdo::xdg::find_config_file(rel_path);
    } catch(FileNotFoundError& e) {
        try {
            file_path = fdo::xdg::find_user_config_file(rel_path);
        } catch (FileNotFoundError& e) {
            return false;
        }
    }

    //Read the found autostart desktop file
    fdo::apps::DesktopFile auto_start_file(file_path);
    try {
        //Return true if the Hidden entry does not exist, or it exists but is false
        return !auto_start_file.get_config().get_setting<bool>("Desktop Entry", "Hidden");
    } catch(config::InvalidSettingError& e) {
        return false;
    }
}

void ApplicationAutostart::enable() {
    if(is_enabled()) {
        return;
    }

    std::string user_autostart = os::path::join(fdo::xdg::get_config_home(),
                                                os::path::join("autostart",
                                                    os::path::split(desktop_file_.get_file_path()).second
                                                ));

    //If the autostart desktop file doesn't exist then copy the existing one
    if(!os::path::exists(user_autostart)) {
        desktop_file_.save_as(user_autostart);
    }

    //Read it, and set the Hidden value to false
    fdo::apps::DesktopFile df(user_autostart);
    df.get_config().set_setting("Desktop Entry", "Hidden", false);
    df.save();
}

void ApplicationAutostart::disable() {
    if(!is_enabled()) {
        return;
    }

    std::string user_autostart = os::path::join(fdo::xdg::get_config_home(),
                                                os::path::join("autostart",
                                                    os::path::split(desktop_file_.get_file_path()).second
                                                ));

    //If the autostart desktop file doesn't exist then copy the existing one
    if(!os::path::exists(user_autostart)) {
        desktop_file_.save_as(user_autostart);
    }

    //Read it, and set the Hidden value to true
    fdo::apps::DesktopFile df(user_autostart);
    df.get_config().set_setting("Desktop Entry", "Hidden", true);
    df.save();
}

