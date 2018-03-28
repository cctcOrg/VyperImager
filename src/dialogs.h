#include <string>

#include <gtkmm/box.h>
#include <gtkmm/dialog.h>
#include <gtkmm/button.h>
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
        void complete();

        Gtk::Spinner spinner;

    protected:
        Gtk::Button *button;
};

class ProgBarDialog : public _Dialog
{
    public:
        ProgBarDialog();
        ~ProgBarDialog();
        
        // Control the progress bar
        void set_progress(double val);
        void complete();

    protected:
        Gtk::Button *button;
        Gtk::ProgressBar prog_bar;
};
