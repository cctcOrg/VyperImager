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

#include "dialogs.h"
#include "infocont.h"

#include "blockdev_treeview.h"
#include <gtkmm/treeselection.h>

enum {
    WELCOME_PAGE = 0,
    EVID_PAGE,
    TGT_PAGE,
    TGTF_PAGE,
    LOC_PAGE,
    CASE_PAGE,
    IMGI_PAGE,
    SUMM_PAGE
};

class Page : public Gtk::Box {
    public:
        Page(const Glib::ustring &t);
        ~Page();
        const Glib::ustring title;
        size_t next_page;

        // To allow pages to accumulate information
        static InfoCont info;
        virtual bool update_info() = 0;
};


class WelcomePage : public Page 
{
    public:
        WelcomePage();
        ~WelcomePage();
        bool update_info();

    protected:
        Gtk::Label welcome_label;
};

class EvidPage : public Page 
{
    public:
        EvidPage();
        ~EvidPage();
        bool update_info();

    protected:
        ErrorDialog no_device_dialog; 
        Gtk::Label evid_prompt;
        BlockdevTreeView evid_device_tv;
};

class TargPage : public Page 
{
    public:
        TargPage();
        ~TargPage();
        bool update_info();

    protected:
        ErrorDialog no_device_dialog; 
        Gtk::Label targ_prompt;
        BlockdevTreeView targ_device_tv;
        Gtk::CheckButton format_button;
};

class FormatPage : public Page 
{
    public:
        FormatPage();
        ~FormatPage();
        bool update_info();

    protected:
        ErrorDialog no_format_dialog; 
        ConfirmDialog confirm_dialog; 

        Gtk::Box os_box;
        Gtk::Label format_prompt;
        Gtk::ButtonBox os_button_box;
};

class TargetLocPage : public Page
{
    public:
        TargetLocPage();
        ~TargetLocPage();
        bool update_info();

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
        bool update_info();

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
        bool update_info();

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
        bool update_info();
        bool image();

    protected:
        static void on_status_read(GObject *o, GAsyncResult *r, gpointer udata);
        static void imaging_done(GObject *o, GAsyncResult *r, gpointer udata);
        void app_done(Gtk::Dialog *d, gint i, gpointer udata);

        Gtk::Grid grid;
        ProgBarDialog prog_diag;
        GSubprocess *subp;

        Gtk::Label eviddev;
        Gtk::Label targdev;
        Gtk::Label targfisys;
        Gtk::Label targdir;
        Gtk::Label targfiname;
        Gtk::Label casenum;
        Gtk::Label itemnum;
        Gtk::Label examiner;
        Gtk::Label desc;
        Gtk::Label notes;
        Gtk::Label devtype;
        Gtk::Label comptype;
        Gtk::Label hashtype;
};

#endif // PAGES_H
