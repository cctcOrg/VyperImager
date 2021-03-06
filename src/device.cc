#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iterator>
#include <iomanip>

extern "C" {
#include<stdio.h>
#include<string.h>
#include<error.h>
#include<dirent.h>
#include<stdlib.h>
#include<sys/stat.h>

#include<parted/device.h>
#include<blkid/blkid.h>
}

#include "device.h"

#define NUM_BLOCKDEVS 5
#define SIZE_SIZE 15
#define PATH_SIZE 40
#define MODEL_SIZE 60
#define REM_SIZE 10

using std::vector;
using std::string;
using std::stringstream;

Device::Device(string n, string m, string s, string fsp, vector<string> ls, size_t nps, bool r, bool it, PedDevice *p)
{
    name = n;
    model = m;
    size = s;
    fspath = fsp;
    labels = ls;
    numparts = nps;
    removable = r;
    is_target = it;
    dev = p;
}
Device::~Device() {}

bool Device::is_evid(string& evid)
{
    std::cout << name + " vs " + evid << std::endl;
    return name.compare(evid) == 0;
}

/* Takes a path, resolving any symbolic links, and strips away everything before
 * and including the second '/' found -- for example /dev/mmcblk0 -> mmcblk0.
 * Note that this function will mangle the given filename */
static string get_target_dev(string filename) {
    char *actualpath;
    char *token;
    
    actualpath = realpath(filename.c_str(), NULL);
    if (actualpath) {
        token = strtok(actualpath, "/"); // Should be "dev"
        token = strtok(NULL, "/"); // should be block device
    
        /* Chop the partition number off so we just have the device */
        token[strlen(token)-1] = 0;
        /* Since token points to data in actualpath, which is dynamically 
         * allocated, this should work */
        return token;
    }
    
    std::cerr << "[WARNING] Path does not exist: " << filename << std::endl;
    return string();
}

vector<Device*> get_blockdev_info() {
    PedDevice *ped = NULL;
    string target_device;

    Device *dev;
    
    vector<Device*> device_info;

    target_device = get_target_dev("/dev/disk/by-label/EVID_TARGET");
    
    ped_device_probe_all();

    while ( (ped = ped_device_get_next(ped)) ) {
        dev = get_blockdev_struct(ped, target_device);
        device_info.push_back(dev);
    }

    return device_info;
}

Device *get_blockdev_struct(PedDevice *dev, string& td) {
    FILE *info_file;
    char *tmp;
    const char *tmpc;
    blkid_partlist ls;
    char rem[REM_SIZE]; 
    string wholedev_label;
    vector<string> labels;
    int numparts = 0;
    int i;
    int err;

    PedDisk *disk = ped_disk_new(dev);
    PedPartition *curr_part;

    stringstream size;
    //size = malloc(SIZE_SIZE*sizeof(char));
    float size_value;

    /* Literally just get /dev/<x> */
    char *dev_path = new char[strlen(dev->path)+1];
    strcpy(dev_path, dev->path);
    strtok(dev_path, "/"); // "dev"
    tmp = strtok(NULL, "/"); // <name>
    string devname(tmp);
    delete[] dev_path;

    /* Find the size */
    blkid_probe pr = blkid_new_probe_from_filename(dev->path);
    if (pr == NULL)
    {
        perror(dev->path);
        exit(1);
    }

    size_value = blkid_probe_get_size(pr);
    size_value /= 1000000000;

    /*printf("%s: %f\n", dev->path, size_value);*/

    /* Let's see if the whole device has a label */
    blkid_do_probe(pr);
    err = blkid_probe_lookup_value(pr, "LABEL", &tmpc, NULL);

    if (err == 0) {
        wholedev_label = string(tmpc);
    }

    size << std::fixed << std::setprecision(1) << size_value << " G";
    //sprintf(size, "%.2f", size_value);
    //strcat(size, " G");

    /* See what labels we can find */
    ls = blkid_probe_get_partitions(pr);
    if (ls != NULL) {
        i = 0;
        numparts = blkid_partlist_numof_partitions(ls);

        if (! wholedev_label.empty()) {
            labels.push_back(wholedev_label);
            i++;
        }

        curr_part = NULL;



        // Iterate through the partitions to get the labels
        while ( (curr_part = ped_disk_next_partition(disk, curr_part)) != NULL) 
        {
            // I'm not sure where this comes from, but oh well
            if  (curr_part->num < 1)
                continue;

            pr = blkid_new_probe_from_filename(ped_partition_get_path(curr_part));
            //std::cout << "Probing " << pr << std::endl;

            blkid_do_probe(pr);
            err = blkid_probe_lookup_value(pr, "LABEL", &tmpc, NULL);

            if (err == 0) {
                labels.push_back(string(tmpc));
            }

            blkid_free_probe(pr);
        }
    }

    /* Removable */
    stringstream info_stream;
    info_stream << "/sys/block/" << devname << "/removable";
    info_file = fopen(info_stream.str().c_str(), "r");
    fgets(rem, REM_SIZE, info_file);
    fclose(info_file);
    
    Device *devinfo = new Device(
        devname,
        string(dev->model), size.str(), string(dev_path),
        labels, numparts,
        (rem[0] == '0') ? 0 : 1, td.compare(devname), dev
    );

    return devinfo;
}

