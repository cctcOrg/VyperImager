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
    notebook = Gtk::manage(new AppBook(bbox));
    bbox->set_layout(Gtk::BUTTONBOX_EDGE);
    bbox->set_size_request(-1, 100);

    bbox->pack_end(quit_button, FALSE, FALSE, 0);

    add(app_box);

    app_box.pack_start(hb, FALSE, FALSE, 0);
    app_box.pack_start(*notebook, TRUE, TRUE, 0);
    app_box.pack_end(*bbox, FALSE, FALSE, 0);
    
    hb.set_title("Evidence and Target Devices");

    quit_button.signal_clicked().connect(sigc::mem_fun(*this,
    &AppWin::on_quit));

    show_all();
}

AppWin::~AppWin()
{
}

void AppWin::on_quit()
{
    app->quit();
}
