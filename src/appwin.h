#ifndef APPWIN_H 
#define APPWIN_H 

#include <gtkmm/notebook.h>
#include <gtkmm/button.h>
#include <gtkmm/headerbar.h>
#include <gtkmm/box.h>
#include <gtkmm/window.h>

#include "appbook.h"
#include "control_button.h"

class AppWin: public Gtk::Window
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
        Gtk::HeaderBar* hb;
        AppBook* notebook;

        Glib::RefPtr<Gtk::Application> app;
        Gtk::Box app_box;
        ControlButton quit_button;
};

#endif // APPWIN_H 
