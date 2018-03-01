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

BlockdevListStore::BlockdevListStore(bool hide_internal, char *evid)
{
    Device *dev;
    bool is_first;

    std::stringstream partlabels;
    set_column_types(cols);
    blockdev_info = get_blockdev_info(&num_blockdevs);

    for (size_t i=0; i<num_blockdevs; i++) {
        dev = blockdev_info[i];
 
        is_eviddev = ( evid != NULL  &&  dev->is_evid(evid) ); 
        /*if (evid != NULL) printf("%s\n", evid);*/
        
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

        if (hide_internal && (!dev->removable || is_eviddev))
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
    }

}

BlockdevListStore::~BlockdevListStore()
{
}
