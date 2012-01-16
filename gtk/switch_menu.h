#ifndef SWITCH_MENU_H_INCLUDED
#define SWITCH_MENU_H_INCLUDED

#include <gtkmm.h>

class GtkSwitchMenu : public Gtk::EventBox {
public:
    GtkSwitchMenu(const std::string& initial_title);
    void append(Gtk::MenuItem& menu_item, bool switch_label=true);
    void append_separator();
    void insert(Gtk::MenuItem& menu_item, int index, bool switch_label=true);
    void remove_by_label(const std::string& label);

    void on_child_item_activate(Gtk::MenuItem* item);

    bool on_button_click(GdkEventButton* evt);
    void button_menu_position(int& x, int& y, bool& push_in);
    void on_menu_deactivate();
private:
    bool pressed_;
    Gtk::Menu menu_;
    Gtk::Label label_;
    Gtk::Alignment align_;
};


#endif // SWITCH_MENU_H_INCLUDED
