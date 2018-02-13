#include <gtkmm/liststore.h>

class BlockdevListStore : Gtk::ListStore
{
    public:
        BlockdevListStore();
        ~BlockdevListStore();

    protected:
        bool is_eviddev;
};
