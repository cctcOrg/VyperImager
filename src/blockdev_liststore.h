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
        BlockdevListStore(bool hide_internal, string evid);
        ~BlockdevListStore();

    protected:
        BlockdevCols cols;

        bool is_eviddev;
        size_t num_blockdevs;
        
        std::vector<Device*> blockdev_info;
};

#endif /* BD_LIST */
