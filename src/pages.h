#ifndef PAGES_H
#define PAGES_H

#include <glib.h>
#include <gtkmm/box.h>

class Page : public Gtk::Box {
    public:
        Page(const Glib::ustring &t);
        ~Page();
        const Glib::ustring title;
};


class WelcomePage : public Page 
{
    public:
        WelcomePage();
        ~WelcomePage();
};

class EvidPage : public Page 
{
    public:
        EvidPage();
        ~EvidPage();
};

#endif // PAGES_H
