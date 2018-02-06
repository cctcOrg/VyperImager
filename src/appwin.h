#ifndef APPWIN_H 
#define APPWIN_H 

#include <gtkmm/notebook.h>
#include <gtkmm/button.h>
#include <gtkmm/headerbar.h>
#include <gtkmm/box.h>
#include <gtkmm/window.h>

class  AppWin: public Gtk::Window
{

    public:
        AppWin();
        virtual ~AppWin();

    protected:
        //Signal handlers:
        void on_button_clicked();

        //Member widgets:
        Gtk::Notebook notebook;
        Gtk::HeaderBar hb;
        Gtk::Box app_box;
};

#endif // APPWIN_H 
