#ifndef APPBOOK_H 
#define APPBOOK_H 

#include <gtkmm/notebook.h>
#include <gtkmm/button.h>
#include <gtkmm/headerbar.h>
#include <gtkmm/box.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/window.h>
#include <stack>

#include "control_button.h"
#include "pages.h"

class AppBook: public Gtk::Notebook
{
    public:
        AppBook(Gtk::ButtonBox* b, Gtk::HeaderBar* hb);
        virtual ~AppBook();

    protected:
        // Signal handlers:
        void on_prev();
        void on_next();
        void on_page_switch(Gtk::Widget *w, guint pn);
        int append_page_obj(Page& p);
        
        // Member widgets:
        Gtk::ButtonBox* bbox;
        Gtk::HeaderBar* header;
        ControlButton prev_button;
        ControlButton next_button;

        // Pages stack
        size_t current_page;
        std::stack<size_t> pg_stack;

        // Pages
        WelcomePage wp;
        EvidPage ep;
        TargPage tp;
        FormatPage fp;
        TargetLocPage tlp;
        CaseMetadataPage cmdp;
        ImageMetadataPage imdp;
        SummaryPage sp;
};

#endif // APPWIN_H 
