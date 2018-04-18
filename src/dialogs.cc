#include <gtkmm/label.h>
#include "dialogs.h"

_Dialog::_Dialog(const std::string &msg)
    : Gtk::Dialog(msg, GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT)
{
    dialog_area = get_content_area();
    add_button("OK", Gtk::RESPONSE_ACCEPT); 
    set_keep_above(true);
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
    button = dynamic_cast<Gtk::Button*>(get_widget_for_response(GTK_RESPONSE_ACCEPT));
    button->set_sensitive(false);

    set_size_request(200, 200);

    dialog_area->pack_start(*Gtk::manage(new Gtk::Label("Working...")));
    dialog_area->pack_start(spinner, true, true, 30);
    dialog_area->set_margin_start(30);
    dialog_area->set_margin_end(30);
    dialog_area->set_margin_top(30);
    dialog_area->set_margin_bottom(30);

    spinner.start();
}

void ProgSpinnerDialog::complete()
{
    button->set_sensitive(true);
}

ProgSpinnerDialog::~ProgSpinnerDialog()
{
}

ProgBarDialog::ProgBarDialog()
    : _Dialog("Loading")
{
    button = dynamic_cast<Gtk::Button*>(get_widget_for_response(GTK_RESPONSE_ACCEPT));

    set_size_request(200, 200);

    button->set_sensitive(false);
    
    dialog_area->pack_start(*Gtk::manage(new Gtk::Label("Working...")));
    dialog_area->pack_start(prog_bar, true, true, 30);

    dialog_area->set_margin_start(30);
    dialog_area->set_margin_end(30);
    dialog_area->set_margin_top(30);
    dialog_area->set_margin_bottom(30);

    prog_bar.set_show_text(true);
}

void ProgBarDialog::set_progress(double val)
{
    prog_bar.set_fraction(val);
}

void ProgBarDialog::complete()
{
    button->set_sensitive(true);
}

ProgBarDialog::~ProgBarDialog()
{
}
