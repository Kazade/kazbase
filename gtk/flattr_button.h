#ifndef FLATTR_BUTTON_H_INCLUDED
#define FLATTR_BUTTON_H_INCLUDED

#include <gtkmm.h>
#include <string>

#include "frontend/stream/gtk_clickable_remote_image.h"

class FlattrButton : public Gtk::Container {
public:
    FlattrButton(LockingImageStore* store);

private:
    Gtk::Button button_;
    GtkClickableRemoteImage image_;
};

#endif // FLATTR_BUTTON_H_INCLUDED
