#ifndef DEVICE_H
#define DEVICE_H

#define HIDE_INT 1
#define SHOW_INT 0

#include<parted/device.h>

#include<iostream>
#include <string>
#include <vector>

using std::string;
using std::vector;

class Device {
    public:
        Device(string name, string model, string size, string fspath, vector<string> labels, size_t numparts, bool removable, bool is_target, PedDevice *p);
        ~Device();

    protected:
        string name;
        string model;
        string size;
        string fspath;
        vector<string> labels;
        size_t  numparts;
        bool  removable;
        bool  is_target;
        PedDevice *dev;
};

/* Will use parted to probe for all block devices and then construct Device
 * structures for each one, storing the number of found block devices in the
 * given num_blockdevs output parameter. Returns a newly allocated array of 
 * Devices of the size indicated by the retuned num_blockdevs*/
vector<Device*> get_blockdev_info(size_t *num_blockdevs);

/* Uses a combination of blkid and parted to probe a given device for information
 * so that the Device struct can be populated. Returns a newly allocated Device
 * struct */
Device *get_blockdev_struct(PedDevice *dev, string& td);

#endif /* DEVICE_H */
