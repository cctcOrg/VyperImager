#ifndef PAGES_H
#define PAGES_H

#include <glib.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/checkbutton.h>

#include "blockdev_treeview.h"

class Page : public Gtk::Box {
    public:
        Page(const Glib::ustring &t);
        ~Page();
        const Glib::ustring title;
        size_t next_page;
};


class WelcomePage : public Page 
{
    public:
        WelcomePage();
        ~WelcomePage();

    protected:
        Gtk::Label welcome_label;
};

class EvidPage : public Page 
{
    public:
        EvidPage();
        ~EvidPage();

    protected:
        Gtk::Label evid_prompt;
        BlockdevTreeView evid_device_tv;
};

class TargPage : public Page 
{
    public:
        TargPage();
        ~TargPage();

    protected:
        Gtk::Label targ_prompt;
        BlockdevTreeView targ_device_tv;
        Gtk::CheckButton format_button;
};
#endif // PAGES_H
