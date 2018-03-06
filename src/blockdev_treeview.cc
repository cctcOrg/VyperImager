#include "blockdev_liststore.h"
#include "blockdev_treeview.h"

BlockdevTreeView::BlockdevTreeView(bool hide_internal)
{
    model = BlockdevListStore::create();
    set_model(model);

    devcol = Gtk::TreeView::Column("Device", model->cols.device_name);
    modcol = Gtk::TreeView::Column("Model", model->cols.device_model);
    sizcol = Gtk::TreeView::Column("Size", model->cols.device_size);
    nptcol = Gtk::TreeView::Column("Number of Partitions", model->cols.numparts);
    ptlcol = Gtk::TreeView::Column("Partition Labels", model->cols.partlabels);
    remcol = Gtk::TreeView::Column("Removable", model->cols.removable);

    append_column(devcol);
    append_column(modcol);
    append_column(sizcol);
    append_column(nptcol);
    append_column(ptlcol);

    if (!hide_internal)
        append_column(remcol);
}

BlockdevTreeView::~BlockdevTreeView()
{
}
