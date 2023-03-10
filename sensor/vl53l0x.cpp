#include <iostream>
#include <fstream>
#include <sstream>
#include <assert.h>

#include "vl53l0x.h"
#include "interface.h"
#include "utils.h"

Vl53l0x::Vl53l0x(std::string device) 
{
    device_ = Utils::ScanIioDevice(device);
    assert(device_ != "");
    std::cout << BOLDGREEN << "Iio bus path " << device_ << std::endl;
}

Vl53l0x::~Vl53l0x() {
    std::cout << BOLDGREEN << "Close vl53l0x device!" << std::endl;
}

int Vl53l0x::GetDistance()
{
    std::string distance_str = device_ + "in_distance_raw";
    std::string buf = Utils::ReadFileIntoString(device_);
    int val = atoi(buf.c_str());

    return val;
}