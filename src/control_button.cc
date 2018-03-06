#include "control_button.h"

ControlButton::ControlButton(const Glib::ustring &label)
{
    set_label(label);
    set_size_request(-1, 100);
}

ControlButton::~ControlButton()
{
}
