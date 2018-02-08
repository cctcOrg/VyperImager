#ifndef APPWIN_H 
#define APPWIN_H 

#include <gtkmm/notebook.h>
#include <gtkmm/button.h>
#include <gtkmm/headerbar.h>
#include <gtkmm/box.h>
#include <gtkmm/window.h>

#include "appbook.h"

class  AppWin: public Gtk::Window
{

    public:
        AppWin(Glib::RefPtr<Gtk::Application> a);
        virtual ~AppWin();

    protected:
        //Signal handlers:
        void on_quit();

        //void on_button_clicked();

        //Member widgets:
        Gtk::ButtonBox* bbox;

        Glib::RefPtr<Gtk::Application> app;
        AppBook* notebook;
        Gtk::HeaderBar hb;
        Gtk::Box app_box;
        Gtk::Button quit_button;
};

#endif // APPWIN_H 
