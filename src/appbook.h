#ifndef APPBOOK_H 
#define APPBOOK_H 

#include <gtkmm/notebook.h>
#include <gtkmm/button.h>
#include <gtkmm/headerbar.h>
#include <gtkmm/box.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/window.h>
#include <stack>

class AppBook: public Gtk::Notebook
{
    public:
        AppBook(Gtk::ButtonBox *b);
        virtual ~AppBook();

    protected:
        // Signal handlers:
        void on_prev();
        void on_next();
        
        // Member widgets:
        Gtk::ButtonBox *bbox;
        Gtk::Button prev_button;
        Gtk::Button next_button;

        // Pages stack
        size_t current_page;
        std::stack<size_t> pg_stack;
};

#endif // APPWIN_H 
