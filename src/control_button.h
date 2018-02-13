#ifndef CONTROL_BUTTON_H
#define CONTROL_BUTTON_H

#include <gtkmm/button.h>
#include <glib.h>

class ControlButton: public Gtk::Button
{
    public:
        ControlButton(const Glib::ustring &label);
        ~ControlButton();
};

#endif // CONTROL_BUTTON_H
