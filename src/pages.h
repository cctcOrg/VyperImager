#ifndef PAGES_H
#define PAGES_H

#include <glib.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/buttonbox.h>

#include <gtkmm/checkbutton.h>

#include <gtkmm/grid.h>
#include <gtkmm/entry.h>
#include <gtkmm/frame.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/textview.h>

#include <gtkmm/filechooserbutton.h>

#include <gtkmm/comboboxtext.h>

#include "infocont.h"
#include "blockdev_treeview.h"

class Page : public Gtk::Box {
    public:
        Page(const Glib::ustring &t);
        ~Page();
        const Glib::ustring title;
        size_t next_page;

        // To allow pages to accumulate information
        static InfoCont info;
        virtual void update_info() = 0;
};


class WelcomePage : public Page 
{
    public:
        WelcomePage();
        ~WelcomePage();
        void update_info();

    protected:
        Gtk::Label welcome_label;
};

class EvidPage : public Page 
{
    public:
        EvidPage();
        ~EvidPage();
        void update_info();

    protected:
        Gtk::Label evid_prompt;
        BlockdevTreeView evid_device_tv;
};

class TargPage : public Page 
{
    public:
        TargPage();
        ~TargPage();
        void update_info();

    protected:
        Gtk::Label targ_prompt;
        BlockdevTreeView targ_device_tv;
        Gtk::CheckButton format_button;
};

class FormatPage : public Page 
{
    public:
        FormatPage();
        ~FormatPage();
        void update_info();

    protected:
        Gtk::Box os_box;
        Gtk::Label format_prompt;
        Gtk::ButtonBox os_button_box;
};

class TargetLocPage : public Page
{
    public:
        TargetLocPage();
        ~TargetLocPage();
        void update_info();

    protected:
        Gtk::Grid grid;
        Gtk::Entry filename;
        Gtk::FileChooserButton directory;
};

class CaseMetadataPage : public Page 
{
    public:
        CaseMetadataPage();
        ~CaseMetadataPage();
        void update_info();

    protected:
        Gtk::Grid grid;
        Gtk::Frame frame;
        Gtk::ScrolledWindow swindow;

        // Entries
        Gtk::Entry casenum; 
        Gtk::Entry itemnum;
        Gtk::Entry examiner;
        Gtk::TextView desc;
        Gtk::Entry notes;
};

class ImageMetadataPage : public Page 
{
    public:
        ImageMetadataPage();
        ~ImageMetadataPage();
        void update_info();

    protected:
        Gtk::Grid grid;
        Gtk::Label overview_label;

        Gtk::ComboBoxText type_cb;
        Gtk::ComboBoxText veri_cb;
        Gtk::ComboBoxText comp_cb;
};

class SummaryPage : public Page 
{
    public:
        SummaryPage();
        ~SummaryPage();
        void update_info();

    protected:
        Gtk::Grid grid;
};

#endif // PAGES_H
