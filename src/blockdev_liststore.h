#ifndef BD_LIST
#define BD_LIST

#include <gtkmm/treemodelcolumn.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treeiter.h>

#include <vector>

#include "device.h"

class BlockdevCols : public Gtk::TreeModel::ColumnRecord {
    public:
        BlockdevCols();
        ~BlockdevCols();
        Gtk::TreeModelColumn<Glib::ustring> device_name;
        Gtk::TreeModelColumn<Glib::ustring> device_model;
        Gtk::TreeModelColumn<Glib::ustring> device_size;
        Gtk::TreeModelColumn<size_t>        numparts;
        Gtk::TreeModelColumn<Glib::ustring> partlabels;
        Gtk::TreeModelColumn<Glib::ustring> removable;

        // These weren't shown in my original one, they should probably jlust be part of some object
        Gtk::TreeModelColumn<size_t>        is_target;
        Gtk::TreeModelColumn<Glib::ustring> fspath;
};

class BlockdevListStore : public Gtk::ListStore
{
    public:
        ~BlockdevListStore();
        static Glib::RefPtr<BlockdevListStore> create();

        BlockdevCols cols;

    protected:
        BlockdevListStore(bool hide_internal);
        std::vector<Device*> blockdev_info;
};

#endif /* BD_LIST */
