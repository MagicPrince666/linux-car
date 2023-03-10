#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <map>
#include <sys/stat.h>
#include <assert.h>

#include "srf04.h"
#include "utils.h"
#include "interface.h"
#include "xinotify.h"

Srf04::Srf04(std::string dev) 
{
    device_dir_ = Utils::ScanIioDevice(dev);
    assert(device_dir_ != "");
    std::cout << BOLDGREEN << "Iio bus path " << device_dir_ << std::endl;
    // MY_INOTIFY.AddFileWatch(device_dir_  + "in_distance_raw", std::bind(&Srf04::Srf04Distance, this));
}

Srf04::~Srf04() {
    // MY_INOTIFY.DelFileWatch(device_dir_  + "in_distance_raw");
    std::cout << BOLDGREEN << "Close srf04 device!" << std::endl;
}

int Srf04::Srf04Distance()
{
    std::string distance_str = device_dir_ + "in_distance_raw";
    std::string buf = Utils::ReadFileIntoString(distance_str);
    int distance = atoi(buf.c_str());

    return distance;
}

