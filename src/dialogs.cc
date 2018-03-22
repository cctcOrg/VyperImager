#include <gtkmm/label.h>
#include "dialogs.h"

_Dialog::_Dialog(const std::string &msg)
    : Gtk::Dialog(msg, GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT)
{
    dialog_area = get_content_area();
    add_button("OK", Gtk::RESPONSE_ACCEPT); 
}

void _Dialog::confirm()
{
    show_all();
    run();
    hide();
}

_Dialog::~_Dialog()
{
}

ErrorDialog::ErrorDialog(const std::string &msg)
    : _Dialog("Error!")
{
    dialog_area->add(*Gtk::manage(new Gtk::Label(msg)));
}

ErrorDialog::~ErrorDialog()
{
}

ConfirmDialog::ConfirmDialog(const std::string &msg)
    : _Dialog("Confirm")
{
    dialog_area->add(*Gtk::manage(new Gtk::Label(msg)));
}

ConfirmDialog::~ConfirmDialog()
{
}

ProgSpinnerDialog::ProgSpinnerDialog()
    : _Dialog("Loading")
{
}

ProgSpinnerDialog::~ProgSpinnerDialog()
{
}

ProgBarDialog::ProgBarDialog()
    : _Dialog("Loading")
{
}

ProgBarDialog::~ProgBarDialog()
{
}
