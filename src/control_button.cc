#include "control_button.h"

ControlButton::ControlButton(const Glib::ustring &label)
{
    set_label(label);

    set_size_request(-1, 100);
    set_margin_start(30);
    set_margin_end(30);
    set_margin_bottom(50);
    set_margin_top(20);
}

ControlButton::~ControlButton()
{
}
