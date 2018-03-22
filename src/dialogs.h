#include <string>

#include <gtkmm/box.h>
#include <gtkmm/dialog.h>
#include <gtkmm/spinner.h>
#include <gtkmm/progressbar.h>

class _Dialog : public Gtk::Dialog
{
    public:
        _Dialog(const std::string &msg);
        ~_Dialog();
        void confirm();
        Gtk::Box *dialog_area;
        Gtk::Button accept_button;
};

class ErrorDialog: public _Dialog
{
    public:
        ErrorDialog(const std::string &msg);
        ~ErrorDialog();
};

class ConfirmDialog: public _Dialog
{
    public:
        ConfirmDialog(const std::string &msg);
        ~ConfirmDialog();
};

class ProgSpinnerDialog : public _Dialog 
{
    public:
        ProgSpinnerDialog();
        ~ProgSpinnerDialog();
        Gtk::Spinner spinner;
};

class ProgBarDialog : public _Dialog
{
    public:
        ProgBarDialog();
        ~ProgBarDialog();
        Gtk::ProgressBar prog_bar;
};
