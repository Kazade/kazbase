#ifndef AUTOSTART_H_INCLUDED
#define AUTOSTART_H_INCLUDED

#include <stdexcept>
#include <string>

#include "applications.h"

class ApplicationAutostart {
public:
    ApplicationAutostart(const std::string& name);

    bool is_enabled_for_user() const;
    bool is_enabled() const;

    void enable();
    void disable();

private:
    fdo::apps::DesktopFile desktop_file_;

};


#endif // AUTOSTART_H_INCLUDED

