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
    set_column_types(cols);
    blockdev_info = get_blockdev_info(&num_blockdevs);

    for (int i=0; i<num_blockdevs; i++) {
        partlabels = "";
        dev = blockdev_info[i];
 
        is_eviddev = ( evid != NULL  &&  (strcmp(evid, dev->name) == 0) ); 
        /*if (evid != NULL) printf("%s\n", evid);*/
        
        for (int j = 0; j < dev->numparts; j++) {
            label = dev->labels[j];
            if (label != NULL) {
                if (is_first) {
                    partlabels = malloc(strlen(label)+1);
                    strcpy(partlabels, label);
                    is_first = false;
                } 
                else {
                    /* Need to allocate 2 extra spaces for the ", " part and
                     * and extra byte for the terminating null byte */
                    partlabels = realloc(partlabels, 
                            (strlen(partlabels) + strlen(label) + 3));
                    strcat(partlabels, ", ");
                    strcat(partlabels, label);
                }
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
        row[cols.partlabels] = partlabels;
        row[cols.removable] = (dev->removable) ? "Yes" : "No";
        row[cols.is_target] = dev->is_target;
        row[cols.fspath] = dev->fspath;
    }

}

BlockdevListStore::~BlockdevListStore()
{
}
