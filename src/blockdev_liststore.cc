#include <string>
#include <sstream>

#include "blockdev_liststore.h"

BlockdevCols::BlockdevCols()
{
    add(device_name);
    add(device_model);
    add(device_size);
    add(numparts);
    add(partlabels);
    add(removable);

    // Want this info to go somewhere else
    add(is_target);
    add(fspath);
}

BlockdevCols::~BlockdevCols()
{
}

BlockdevListStore::BlockdevListStore(bool hide_internal)
{
    Device *dev;
    bool is_first = true;

    std::stringstream partlabels;
    set_column_types(cols);
    blockdev_info = get_blockdev_info();

    for (size_t i=0; i<blockdev_info.size(); i++) {
        dev = blockdev_info[i];
 
        for (size_t j = 0; j < dev->labels.size(); j++) {
            if (is_first)
            {
                partlabels << dev->labels.at(j);
                is_first = false;
            }
            else
            {
                partlabels << ", " << dev->labels.at(j);
            }
        }
        is_first = true;

        if (hide_internal && !dev->removable)
            continue;

        auto row = *append();
        row[cols.device_name] = dev->name;
        row[cols.device_model] = dev->model;
        row[cols.device_size] = dev->size;
        row[cols.numparts] = dev->numparts;
        row[cols.partlabels] = partlabels.str();
        row[cols.removable] = (dev->removable) ? "Yes" : "No";
        row[cols.is_target] = dev->is_target;
        row[cols.fspath] = dev->fspath;

        partlabels.str("");
    }

}

Glib::RefPtr<BlockdevListStore> BlockdevListStore::create()
{
  return Glib::RefPtr<BlockdevListStore>( new BlockdevListStore(false) );
}


BlockdevListStore::~BlockdevListStore()
{
}

