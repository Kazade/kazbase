#include <iostream>
#include "autostart.h"
#include "../os/path.h"
#include "base_directory.h"
#include "../exceptions.h"
#include "applications.h"

ApplicationAutostart::ApplicationAutostart(const unicode& name):
    desktop_file_(fdo::apps::find_desktop_file(name)) {
}

static unicode find_user_autostart(fdo::apps::DesktopFile desktop_file) {
    unicode desktop_file_name = os::path::split(desktop_file.get_file_path()).second;
    unicode rel_path = os::path::join("autostart", desktop_file_name);
    unicode file_path = fdo::xdg::find_user_config_file(rel_path);
    return file_path;
}

bool ApplicationAutostart::is_enabled_for_user() const {
    unicode file_path;
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
    unicode file_path;
    unicode desktop_file_name = os::path::split(desktop_file_.get_file_path()).second;
    unicode rel_path = os::path::join("autostart", desktop_file_name);
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

    unicode user_autostart = os::path::join(fdo::xdg::get_config_home(),
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

    unicode user_autostart = os::path::join(fdo::xdg::get_config_home(),
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

