#include "binary_interface.h"
#include "appwin.h"
#include <iostream>
#include <typeinfo>
#include <gtkmm.h>

AppWin::AppWin(Glib::RefPtr<Gtk::Application> a)
    : app_box(Gtk::ORIENTATION_VERTICAL, 0),
    quit_button("Quit")
{
    app = a;

    fullscreen();

    bbox = Gtk::manage(new Gtk::ButtonBox()); 
    hb = Gtk::manage(new Gtk::HeaderBar());

    notebook = Gtk::manage(new AppBook(bbox, hb));

    bbox->set_layout(Gtk::BUTTONBOX_EDGE);
    bbox->set_size_request(-1, 100);
    bbox->pack_start(quit_button, FALSE, FALSE, 0);
    bbox->set_margin_start(30);
    bbox->set_margin_end(30);
    bbox->set_margin_bottom(50);
    bbox->set_margin_top(20);

    add(app_box);

    app_box.pack_start(*hb, FALSE, FALSE, 0);
    app_box.pack_start(*notebook, TRUE, TRUE, 0);
    app_box.pack_end(*bbox, FALSE, FALSE, 0);
    
    quit_button.signal_clicked().connect(sigc::mem_fun(*this,
    &AppWin::on_quit));

    show_all();
}

AppWin::~AppWin()
{
}

void AppWin::on_quit()
{
    bint::unmount_target();
    app->quit();
}
