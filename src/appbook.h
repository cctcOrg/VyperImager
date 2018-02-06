#ifndef APPBOOK_H 
#define APPBOOK_H 

#include <gtkmm/notebook.h>
#include <gtkmm/button.h>
#include <gtkmm/headerbar.h>
#include <gtkmm/box.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/window.h>

class AppBook: public Gtk::Notebook
{
    public:
        AppBook();
        virtual ~AppBook();

    protected:
        //Signal handlers:
        void on_prev();
        void on_next();
        void on_quit();

        //Member widgets:
        Gtk::ButtonBox bbox;
        Gtk::Button prev_button;
        Gtk::Button next_button;
        Gtk::Button quit_button;
};

#endif // APPWIN_H 
