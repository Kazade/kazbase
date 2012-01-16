#include "kazbase/logging/logging.h"
#include "kazbase/exceptions.h"
#include "switch_menu.h"

GtkSwitchMenu::GtkSwitchMenu(const std::string& initial_title):
    pressed_(false) {

    //set_submenu(menu_);
    align_.add(label_);
    align_.set_padding(5, 5, 5, 5);
    add(align_);
//    label_.set_spacing(5);
    label_.set_use_markup(true);
    label_.set_markup("<b>" + initial_title + "</b>");
    label_.show();

    Gtk::MenuItem sample;

    Glib::RefPtr<Gtk::StyleContext> style = sample.get_style_context();

    override_background_color(style->get_background_color(Gtk::STATE_FLAG_PRELIGHT), Gtk::STATE_FLAG_PRELIGHT);

    show();
    set_events(Gdk::BUTTON_PRESS_MASK);
    signal_button_press_event().connect(sigc::mem_fun(this, &GtkSwitchMenu::on_button_click));
    menu_.signal_deactivate().connect(sigc::mem_fun(this, &GtkSwitchMenu::on_menu_deactivate));

}

void GtkSwitchMenu::insert(Gtk::MenuItem& menu_item, int index, bool switch_label) {
    if(index < 0) {
        index = (menu_.get_children().size() + index);
        index = std::max(index, 0);
    }

    menu_.insert(menu_item, index);
    if(switch_label) {
        menu_item.signal_activate().connect(sigc::bind<0>(sigc::mem_fun(this, &GtkSwitchMenu::on_child_item_activate), &menu_item));
    }

    menu_.show_all();
}

void GtkSwitchMenu::append(Gtk::MenuItem& menu_item, bool switch_label) {
    menu_.append(menu_item);

    if(switch_label) {
        menu_item.signal_activate().connect(sigc::bind<0>(sigc::mem_fun(this, &GtkSwitchMenu::on_child_item_activate), &menu_item));
    }

    menu_.show_all();
}

void GtkSwitchMenu::append_separator() {
    Gtk::SeparatorMenuItem* new_item = Gtk::manage(new Gtk::SeparatorMenuItem());
    menu_.append(*new_item);
}

void GtkSwitchMenu::remove_by_label(const std::string& label) {
    throw NotImplementedError(__FILE__, __LINE__);
}

void GtkSwitchMenu::on_child_item_activate(Gtk::MenuItem* item) {
    L_DEBUG("Trying to switch menu");
    label_.set_markup("<b>" + item->get_label() + "</b>");
}

bool GtkSwitchMenu::on_button_click(GdkEventButton* evt) {
    if(evt->type == GDK_BUTTON_PRESS) {
        L_DEBUG("Clicked");
        pressed_ = true;
        menu_.popup(sigc::mem_fun(this, &GtkSwitchMenu::button_menu_position), 1, evt->time);
    }

    return true;
}

void GtkSwitchMenu::on_menu_deactivate() {
    pressed_ = false;
}

void GtkSwitchMenu::button_menu_position(int& x, int& y, bool& push_in) {
    /* Position the menu right below the widget */
    Gtk::Allocation allocation = align_.get_allocation();
    int x_pos, y_pos;
    get_window()->get_origin(x_pos, y_pos);

    int widget_x = x_pos + allocation.get_x();
    int widget_y = y_pos + allocation.get_y();
    //label_.get_window()->get_position(widget_x, widget_y);

    x = widget_x;
    y = widget_y + allocation.get_height();
    push_in = true;
}
