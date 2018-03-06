#include "blockdev_treeview.h"

BlockdevTreeView::BlockdevTreeView(bool hide_internal)
    : store(hide_internal, "test"), 
    devcol("Device"), modcol("Model"), sizcol("Size"),
    nptcol("Number of Partitions"), ptlcol("Partition Labels"),
    remcol("Removable")
{
    // Getting a segfault on quit around here
    Glib::RefPtr<BlockdevListStore> model(&store);
    set_model(model);

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
