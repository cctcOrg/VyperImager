#ifndef BD_TREEVIEW
#define BD_TREEVIEW

#include <gtkmm/treeview.h>
#include <gtkmm/treeviewcolumn.h>

#include "blockdev_liststore.h"

class BlockdevTreeView : public Gtk::TreeView
{
    public:
        BlockdevTreeView(bool hide_internal);
        ~BlockdevTreeView();

    protected:
        BlockdevListStore store;
        Glib::RefPtr<BlockdevListStore> model;

        Gtk::TreeView::Column devcol;
        Gtk::TreeView::Column modcol;
        Gtk::TreeView::Column sizcol;
        Gtk::TreeView::Column nptcol;
        Gtk::TreeView::Column ptlcol;
        Gtk::TreeView::Column remcol;
};


#endif /* BD_TREEVIEW */
